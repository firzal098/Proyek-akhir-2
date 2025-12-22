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

// Routes
app.use('/auth', require('../routes/auth'));
app.use('/customers', require('../routes/customers'));
app.use('/vendors', require('../routes/vendors'));
app.use('/parking', require('../routes/parking'));

// Health check
app.get('/health', (req, res) => {
  res.json({ status: 'OK', message: 'Server is running' });
});

// Export for Vercel serverless
module.exports = app;

