const express = require('express');
const { getDb } = require('../config/database');
const { authenticateToken, isAdmin } = require('../middleware/auth');

const router = express.Router();

// Get all vendors
router.get('/', (req, res) => {
  try {
    const db = getDb();
    const vendors = db.prepare(`
      SELECT v.*, 
             COUNT(p.id) as jumlah_produk
      FROM vendors v
      LEFT JOIN products p ON v.id = p.vendor_id
      GROUP BY v.id
      ORDER BY v.id
    `).all();
    res.json(vendors);
  } catch (error) {
    console.error('Get vendors error:', error);
    res.status(500).json({ error: 'Internal server error' });
  }
});

// Get vendor by ID with products
router.get('/:id', (req, res) => {
  try {
    const db = getDb();
    const vendor = db.prepare('SELECT * FROM vendors WHERE id = ?').get(req.params.id);
    
    if (!vendor) {
      return res.status(404).json({ error: 'Vendor not found' });
    }

    const products = db.prepare('SELECT * FROM products WHERE vendor_id = ?').all(req.params.id);
    vendor.products = products;

    res.json(vendor);
  } catch (error) {
    console.error('Get vendor error:', error);
    res.status(500).json({ error: 'Internal server error' });
  }
});

// Create vendor (Admin only)
router.post('/', authenticateToken, isAdmin, (req, res) => {
  try {
    const { nama, produk_utama, kategori } = req.body;

    if (!nama) {
      return res.status(400).json({ error: 'Vendor name is required' });
    }

    const db = getDb();
    const stmt = db.prepare('INSERT INTO vendors (nama, produk_utama, kategori) VALUES (?, ?, ?)');
    const result = stmt.run(nama, produk_utama || null, kategori || null);

    res.status(201).json({
      id: result.lastInsertRowid,
      nama,
      produk_utama,
      kategori
    });
  } catch (error) {
    console.error('Create vendor error:', error);
    res.status(500).json({ error: 'Internal server error' });
  }
});

// Update vendor (Admin only)
router.put('/:id', authenticateToken, isAdmin, (req, res) => {
  try {
    const { nama, produk_utama, kategori } = req.body;
    const db = getDb();

    const vendor = db.prepare('SELECT * FROM vendors WHERE id = ?').get(req.params.id);
    if (!vendor) {
      return res.status(404).json({ error: 'Vendor not found' });
    }

    const stmt = db.prepare('UPDATE vendors SET nama = ?, produk_utama = ?, kategori = ? WHERE id = ?');
    stmt.run(nama || vendor.nama, produk_utama || vendor.produk_utama, kategori || vendor.kategori, req.params.id);

    res.json({ message: 'Vendor updated successfully' });
  } catch (error) {
    console.error('Update vendor error:', error);
    res.status(500).json({ error: 'Internal server error' });
  }
});

// Delete vendor (Admin only)
router.delete('/:id', authenticateToken, isAdmin, (req, res) => {
  try {
    const db = getDb();
    const vendor = db.prepare('SELECT * FROM vendors WHERE id = ?').get(req.params.id);
    
    if (!vendor) {
      return res.status(404).json({ error: 'Vendor not found' });
    }

    db.prepare('DELETE FROM vendors WHERE id = ?').run(req.params.id);
    res.json({ message: 'Vendor deleted successfully' });
  } catch (error) {
    console.error('Delete vendor error:', error);
    res.status(500).json({ error: 'Internal server error' });
  }
});

// Search vendors
router.get('/search/:keyword', (req, res) => {
  try {
    const keyword = `%${req.params.keyword.toLowerCase()}%`;
    const db = getDb();
    const vendors = db.prepare(`
      SELECT v.*, COUNT(p.id) as jumlah_produk
      FROM vendors v
      LEFT JOIN products p ON v.id = p.vendor_id
      WHERE LOWER(v.nama) LIKE ? OR LOWER(v.kategori) LIKE ?
      GROUP BY v.id
    `).all(keyword, keyword);
    res.json(vendors);
  } catch (error) {
    console.error('Search vendors error:', error);
    res.status(500).json({ error: 'Internal server error' });
  }
});

// Add product to vendor (Admin only)
router.post('/:id/products', authenticateToken, isAdmin, (req, res) => {
  try {
    const { nama, harga, stok } = req.body;
    const vendorId = req.params.id;

    if (!nama || !harga || harga < 0) {
      return res.status(400).json({ error: 'Product name and valid price are required' });
    }

    const db = getDb();
    
    // Check if vendor exists
    const vendor = db.prepare('SELECT * FROM vendors WHERE id = ?').get(vendorId);
    if (!vendor) {
      return res.status(404).json({ error: 'Vendor not found' });
    }

    const stmt = db.prepare('INSERT INTO products (vendor_id, nama, harga, stok) VALUES (?, ?, ?, ?)');
    const result = stmt.run(vendorId, nama, harga, stok || 0);

    res.status(201).json({
      id: result.lastInsertRowid,
      vendor_id: vendorId,
      nama,
      harga,
      stok: stok || 0
    });
  } catch (error) {
    console.error('Add product error:', error);
    res.status(500).json({ error: 'Internal server error' });
  }
});

// Update product (Admin only)
router.put('/:vendorId/products/:productId', authenticateToken, isAdmin, (req, res) => {
  try {
    const { nama, harga, stok } = req.body;
    const db = getDb();

    const product = db.prepare('SELECT * FROM products WHERE id = ? AND vendor_id = ?')
      .get(req.params.productId, req.params.vendorId);
    
    if (!product) {
      return res.status(404).json({ error: 'Product not found' });
    }

    const stmt = db.prepare('UPDATE products SET nama = ?, harga = ?, stok = ? WHERE id = ?');
    stmt.run(
      nama || product.nama,
      harga !== undefined ? harga : product.harga,
      stok !== undefined ? stok : product.stok,
      req.params.productId
    );

    res.json({ message: 'Product updated successfully' });
  } catch (error) {
    console.error('Update product error:', error);
    res.status(500).json({ error: 'Internal server error' });
  }
});

// Delete product (Admin only)
router.delete('/:vendorId/products/:productId', authenticateToken, isAdmin, (req, res) => {
  try {
    const db = getDb();
    db.prepare('DELETE FROM products WHERE id = ? AND vendor_id = ?')
      .run(req.params.productId, req.params.vendorId);
    res.json({ message: 'Product deleted successfully' });
  } catch (error) {
    console.error('Delete product error:', error);
    res.status(500).json({ error: 'Internal server error' });
  }
});

// Purchase product (Customer)
router.post('/:vendorId/products/:productId/purchase', authenticateToken, (req, res) => {
  try {
    const { quantity } = req.body;
    const customerId = req.user.id;
    const vendorId = req.params.vendorId;
    const productId = req.params.productId;

    if (!quantity || quantity <= 0) {
      return res.status(400).json({ error: 'Valid quantity is required' });
    }

    const db = getDb();

    // Get customer
    const customer = db.prepare('SELECT * FROM customers WHERE id = ?').get(customerId);
    if (!customer) {
      return res.status(404).json({ error: 'Customer not found' });
    }

    // Get product
    const product = db.prepare('SELECT * FROM products WHERE id = ? AND vendor_id = ?')
      .get(productId, vendorId);
    
    if (!product) {
      return res.status(404).json({ error: 'Product not found' });
    }

    // Check stock
    if (product.stok < quantity) {
      return res.status(400).json({ error: 'Insufficient stock' });
    }

    // Calculate total price
    const totalHarga = product.harga * quantity;

    // Check balance
    if (customer.saldo < totalHarga) {
      return res.status(400).json({ error: 'Insufficient balance' });
    }

    // Start transaction
    const transaction = db.transaction(() => {
      // Update customer balance
      db.prepare('UPDATE customers SET saldo = saldo - ? WHERE id = ?')
        .run(totalHarga, customerId);

      // Update product stock
      db.prepare('UPDATE products SET stok = stok - ? WHERE id = ?')
        .run(quantity, productId);

      // Record transaction
      db.prepare(`
        INSERT INTO transactions (customer_id, vendor_id, product_id, product_name, quantity, harga_satuan, total_harga)
        VALUES (?, ?, ?, ?, ?, ?, ?)
      `).run(customerId, vendorId, productId, product.nama, quantity, product.harga, totalHarga);
    });

    transaction();

    // Get updated customer
    const updatedCustomer = db.prepare('SELECT saldo FROM customers WHERE id = ?').get(customerId);
    const updatedProduct = db.prepare('SELECT stok FROM products WHERE id = ?').get(productId);

    res.json({
      message: 'Purchase successful',
      transaction: {
        product_name: product.nama,
        quantity,
        total_harga: totalHarga
      },
      remaining_balance: updatedCustomer.saldo,
      remaining_stock: updatedProduct.stok
    });
  } catch (error) {
    console.error('Purchase error:', error);
    res.status(500).json({ error: 'Internal server error' });
  }
});

module.exports = router;

