const Database = require('better-sqlite3');
const path = require('path');
const fs = require('fs');

// Use /tmp for Vercel serverless (persists during function execution)
// Note: /tmp resets on cold starts, so data won't persist between deployments
const dbPath = process.env.DB_PATH || 
  (process.env.VERCEL ? '/tmp/database.db' : path.join(__dirname, '../database.db'));

let db = null;

const init = () => {
  // Ensure directory exists
  const dbDir = path.dirname(dbPath);
  if (!fs.existsSync(dbDir) && dbDir !== '/tmp') {
    fs.mkdirSync(dbDir, { recursive: true });
  }

  // Create database file if it doesn't exist
  db = new Database(dbPath);
  
  // Enable foreign keys
  db.pragma('foreign_keys = ON');
  
  // Create tables
  createTables();
  
  // Insert default admin if not exists
  insertDefaultAdmin();
  
  console.log('Database initialized successfully at:', dbPath);
  return db;
};

const createTables = () => {
  // Customers table
  db.exec(`
    CREATE TABLE IF NOT EXISTS customers (
      id TEXT PRIMARY KEY,
      nama TEXT NOT NULL,
      nomor_telepon TEXT NOT NULL,
      email TEXT NOT NULL UNIQUE,
      password TEXT NOT NULL,
      saldo REAL DEFAULT 0.0,
      created_at DATETIME DEFAULT CURRENT_TIMESTAMP
    )
  `);

  // Vendors table
  db.exec(`
    CREATE TABLE IF NOT EXISTS vendors (
      id INTEGER PRIMARY KEY AUTOINCREMENT,
      nama TEXT NOT NULL,
      produk_utama TEXT,
      kategori TEXT,
      created_at DATETIME DEFAULT CURRENT_TIMESTAMP
    )
  `);

  // Products table
  db.exec(`
    CREATE TABLE IF NOT EXISTS products (
      id INTEGER PRIMARY KEY AUTOINCREMENT,
      vendor_id INTEGER NOT NULL,
      nama TEXT NOT NULL,
      harga REAL NOT NULL,
      stok INTEGER DEFAULT 0,
      created_at DATETIME DEFAULT CURRENT_TIMESTAMP,
      FOREIGN KEY (vendor_id) REFERENCES vendors(id) ON DELETE CASCADE
    )
  `);

  // Parking tickets table
  db.exec(`
    CREATE TABLE IF NOT EXISTS parking_tickets (
      id TEXT PRIMARY KEY,
      customer_id TEXT NOT NULL,
      nomor_polisi TEXT NOT NULL,
      slot_parkir TEXT NOT NULL,
      waktu_masuk INTEGER NOT NULL,
      waktu_keluar INTEGER,
      biaya REAL DEFAULT 0.0,
      status TEXT DEFAULT 'AKTIF',
      created_at DATETIME DEFAULT CURRENT_TIMESTAMP,
      FOREIGN KEY (customer_id) REFERENCES customers(id)
    )
  `);

  // Loyalty points table
  db.exec(`
    CREATE TABLE IF NOT EXISTS loyalty_points (
      id INTEGER PRIMARY KEY AUTOINCREMENT,
      nomor_polisi TEXT NOT NULL UNIQUE,
      poin INTEGER DEFAULT 0,
      created_at DATETIME DEFAULT CURRENT_TIMESTAMP,
      updated_at DATETIME DEFAULT CURRENT_TIMESTAMP
    )
  `);

  // Transactions table (for vendor purchases)
  db.exec(`
    CREATE TABLE IF NOT EXISTS transactions (
      id INTEGER PRIMARY KEY AUTOINCREMENT,
      customer_id TEXT NOT NULL,
      vendor_id INTEGER NOT NULL,
      product_id INTEGER NOT NULL,
      product_name TEXT NOT NULL,
      quantity INTEGER NOT NULL,
      harga_satuan REAL NOT NULL,
      total_harga REAL NOT NULL,
      created_at DATETIME DEFAULT CURRENT_TIMESTAMP,
      FOREIGN KEY (customer_id) REFERENCES customers(id),
      FOREIGN KEY (vendor_id) REFERENCES vendors(id),
      FOREIGN KEY (product_id) REFERENCES products(id)
    )
  `);
};

const insertDefaultAdmin = () => {
  // Admin is handled via hardcoded check in auth, no need to insert
};

const getDb = () => {
  if (!db) {
    db = init();
  }
  return db;
};

module.exports = {
  init,
  getDb
};

