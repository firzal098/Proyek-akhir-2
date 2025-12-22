const express = require('express');
const { getDb } = require('../config/database');
const { authenticateToken, isAdmin } = require('../middleware/auth');

const router = express.Router();

// Get all customers (Admin only)
router.get('/', authenticateToken, isAdmin, (req, res) => {
  try {
    const db = getDb();
    const customers = db.prepare('SELECT id, nama, nomor_telepon, email, saldo, created_at FROM customers').all();
    res.json(customers);
  } catch (error) {
    console.error('Get customers error:', error);
    res.status(500).json({ error: 'Internal server error' });
  }
});

// Get customer by ID
router.get('/:id', authenticateToken, (req, res) => {
  try {
    const db = getDb();
    const customer = db.prepare('SELECT id, nama, nomor_telepon, email, saldo, created_at FROM customers WHERE id = ?').get(req.params.id);

    if (!customer) {
      return res.status(404).json({ error: 'Customer not found' });
    }

    // Only allow admin or the customer themselves to view
    if (req.user.role !== 'admin' && req.user.id !== req.params.id) {
      return res.status(403).json({ error: 'Access denied' });
    }

    res.json(customer);
  } catch (error) {
    console.error('Get customer error:', error);
    res.status(500).json({ error: 'Internal server error' });
  }
});

// Add balance to customer (Admin only)
router.post('/:id/add-balance', authenticateToken, isAdmin, (req, res) => {
  try {
    const { amount } = req.body;
    const customerId = req.params.id;

    if (!amount || amount <= 0) {
      return res.status(400).json({ error: 'Valid amount is required' });
    }

    const db = getDb();
    
    // Check if customer exists
    const customer = db.prepare('SELECT * FROM customers WHERE id = ?').get(customerId);
    if (!customer) {
      return res.status(404).json({ error: 'Customer not found' });
    }

    // Update balance
    const newBalance = customer.saldo + amount;
    const stmt = db.prepare('UPDATE customers SET saldo = ? WHERE id = ?');
    stmt.run(newBalance, customerId);

    res.json({
      message: 'Balance added successfully',
      customer: {
        id: customer.id,
        nama: customer.nama,
        saldo: newBalance
      }
    });
  } catch (error) {
    console.error('Add balance error:', error);
    res.status(500).json({ error: 'Internal server error' });
  }
});

module.exports = router;

