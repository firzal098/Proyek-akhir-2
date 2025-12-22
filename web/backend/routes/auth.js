const express = require('express');
const bcrypt = require('bcryptjs');
const jwt = require('jsonwebtoken');
const { getDb } = require('../config/database');

const router = express.Router();

// Register new customer
router.post('/register', async (req, res) => {
  try {
    const { id, nama, nomor_telepon, email, password } = req.body;

    if (!id || !nama || !nomor_telepon || !email || !password) {
      return res.status(400).json({ error: 'All fields are required' });
    }

    const db = getDb();

    // Check if ID already exists
    const existingId = db.prepare('SELECT id FROM customers WHERE id = ?').get(id);
    if (existingId) {
      return res.status(400).json({ error: 'Customer ID already exists' });
    }

    // Check if email already exists
    const existingEmail = db.prepare('SELECT id FROM customers WHERE email = ?').get(email);
    if (existingEmail) {
      return res.status(400).json({ error: 'Email already registered' });
    }

    // Hash password
    const hashedPassword = await bcrypt.hash(password, 10);

    // Insert new customer
    const stmt = db.prepare(`
      INSERT INTO customers (id, nama, nomor_telepon, email, password, saldo)
      VALUES (?, ?, ?, ?, ?, 0.0)
    `);
    stmt.run(id, nama, nomor_telepon, email, hashedPassword);

    res.status(201).json({ message: 'Customer registered successfully' });
  } catch (error) {
    console.error('Register error:', error);
    res.status(500).json({ error: 'Internal server error' });
  }
});

// Login
router.post('/login', async (req, res) => {
  try {
    const { id, password } = req.body;

    if (!id || !password) {
      return res.status(400).json({ error: 'ID and password are required' });
    }

    // Check admin credentials (hardcoded as in original)
    if (id === 'admin' && password === 'admin123') {
      const token = jwt.sign(
        { id: 'admin', role: 'admin' },
        process.env.JWT_SECRET || 'your-secret-key',
        { expiresIn: '24h' }
      );
      return res.json({
        token,
        user: { id: 'admin', role: 'admin', nama: 'Administrator' }
      });
    }

    // Check customer credentials
    const db = getDb();
    const customer = db.prepare('SELECT * FROM customers WHERE id = ?').get(id);

    if (!customer) {
      return res.status(401).json({ error: 'Invalid credentials' });
    }

    const isValidPassword = await bcrypt.compare(password, customer.password);
    if (!isValidPassword) {
      return res.status(401).json({ error: 'Invalid credentials' });
    }

    const token = jwt.sign(
      { id: customer.id, role: 'customer' },
      process.env.JWT_SECRET || 'your-secret-key',
      { expiresIn: '24h' }
    );

    res.json({
      token,
      user: {
        id: customer.id,
        role: 'customer',
        nama: customer.nama,
        email: customer.email,
        saldo: customer.saldo
      }
    });
  } catch (error) {
    console.error('Login error:', error);
    res.status(500).json({ error: 'Internal server error' });
  }
});

module.exports = router;

