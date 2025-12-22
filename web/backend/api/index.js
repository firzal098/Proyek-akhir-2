const express = require('express');
const cors = require('cors');

const app = express();

// Middleware
app.use(cors());
app.use(express.json());
app.use(express.urlencoded({ extended: true }));

// Initialize database
const db = require('../config/database');
db.init();

// ✅ FIX: Add '/api' prefix to ALL routes so they match the URL coming from Frontend
app.use('/api/auth', require('../routes/auth'));
app.use('/api/customers', require('../routes/customers'));
app.use('/api/vendors', require('../routes/vendors'));
app.use('/api/parking', require('../routes/parking'));

// ✅ FIX: Update Health Check too
app.get('/api/health', (req, res) => {
  res.json({ status: 'OK', message: 'Server is running' });
});

// Export for Vercel serverless
module.exports = app;