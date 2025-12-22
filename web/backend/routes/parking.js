const express = require('express');
const { getDb } = require('../config/database');
const { authenticateToken, isAdmin } = require('../middleware/auth');

const router = express.Router();

// Get parking map (5x5 grid)
router.get('/map', (req, res) => {
  try {
    const db = getDb();
    const activeTickets = db.prepare(`
      SELECT slot_parkir FROM parking_tickets 
      WHERE status = 'AKTIF'
    `).all();

    const occupiedSlots = new Set(activeTickets.map(t => t.slot_parkir));

    // Generate 5x5 grid
    const grid = [];
    for (let i = 0; i < 5; i++) {
      const row = [];
      for (let j = 0; j < 5; j++) {
        const slotId = String.fromCharCode(65 + i) + (j + 1); // A1, A2, ..., E5
        row.push({
          id: slotId,
          occupied: occupiedSlots.has(slotId)
        });
      }
      grid.push(row);
    }

    res.json({ grid });
  } catch (error) {
    console.error('Get parking map error:', error);
    res.status(500).json({ error: 'Internal server error' });
  }
});

// Check-in (parking entry)
router.post('/check-in', authenticateToken, (req, res) => {
  try {
    const { nomor_polisi, slot_parkir } = req.body;
    const customerId = req.user.role === 'admin' ? req.body.customer_id : req.user.id;

    if (!nomor_polisi || !slot_parkir) {
      return res.status(400).json({ error: 'License plate and parking slot are required' });
    }

    // Validate slot format (A1-E5)
    if (!/^[A-E][1-5]$/.test(slot_parkir)) {
      return res.status(400).json({ error: 'Invalid slot format. Use A1-E5' });
    }

    const db = getDb();

    // Check if slot is already occupied
    const existingTicket = db.prepare(`
      SELECT id FROM parking_tickets 
      WHERE slot_parkir = ? AND status = 'AKTIF'
    `).get(slot_parkir);

    if (existingTicket) {
      return res.status(400).json({ error: 'Parking slot is already occupied' });
    }

    // Check if customer exists
    const customer = db.prepare('SELECT id FROM customers WHERE id = ?').get(customerId);
    if (!customer) {
      return res.status(404).json({ error: 'Customer not found' });
    }

    // Generate ticket ID
    const ticketId = 'TKT-' + Date.now();

    // Create parking ticket
    const waktuMasuk = Math.floor(Date.now() / 1000);
    const stmt = db.prepare(`
      INSERT INTO parking_tickets 
      (id, customer_id, nomor_polisi, slot_parkir, waktu_masuk, status)
      VALUES (?, ?, ?, ?, ?, 'AKTIF')
    `);
    stmt.run(ticketId, customerId, nomor_polisi, slot_parkir, waktuMasuk);

    res.status(201).json({
      message: 'Check-in successful',
      ticket: {
        id: ticketId,
        nomor_polisi,
        slot_parkir,
        waktu_masuk: waktuMasuk
      }
    });
  } catch (error) {
    console.error('Check-in error:', error);
    res.status(500).json({ error: 'Internal server error' });
  }
});

// Check-out (parking exit)
router.post('/check-out', authenticateToken, (req, res) => {
  try {
    const { ticket_id } = req.body;

    if (!ticket_id) {
      return res.status(400).json({ error: 'Ticket ID is required' });
    }

    const db = getDb();

    // Get active ticket
    const ticket = db.prepare(`
      SELECT * FROM parking_tickets 
      WHERE id = ? AND status = 'AKTIF'
    `).get(ticket_id);

    if (!ticket) {
      return res.status(404).json({ error: 'Active ticket not found' });
    }

    // Calculate parking fee
    const waktuKeluar = Math.floor(Date.now() / 1000);
    const durasi = waktuKeluar - ticket.waktu_masuk;
    const biaya = Math.max(1, durasi) * 2000; // Rp2000 per second (minimum 1 second)

    // Update ticket
    db.prepare(`
      UPDATE parking_tickets 
      SET waktu_keluar = ?, biaya = ?, status = 'DIBAYAR'
      WHERE id = ?
    `).run(waktuKeluar, biaya, ticket_id);

    // Update loyalty points
    const poinDidapat = Math.floor(biaya / 1000);
    if (poinDidapat > 0) {
      const existingLoyalty = db.prepare('SELECT * FROM loyalty_points WHERE nomor_polisi = ?')
        .get(ticket.nomor_polisi);

      if (existingLoyalty) {
        db.prepare('UPDATE loyalty_points SET poin = poin + ?, updated_at = CURRENT_TIMESTAMP WHERE nomor_polisi = ?')
          .run(poinDidapat, ticket.nomor_polisi);
      } else {
        db.prepare('INSERT INTO loyalty_points (nomor_polisi, poin) VALUES (?, ?)')
          .run(ticket.nomor_polisi, poinDidapat);
      }
    }

    res.json({
      message: 'Check-out successful',
      ticket: {
        id: ticket.id,
        nomor_polisi: ticket.nomor_polisi,
        slot_parkir: ticket.slot_parkir,
        durasi_detik: durasi,
        biaya: biaya,
        poin_didapat: poinDidapat
      }
    });
  } catch (error) {
    console.error('Check-out error:', error);
    res.status(500).json({ error: 'Internal server error' });
  }
});

// Get active tickets
router.get('/active', authenticateToken, (req, res) => {
  try {
    const db = getDb();
    let tickets;

    if (req.user.role === 'admin') {
      // Admin can see all active tickets
      tickets = db.prepare(`
        SELECT pt.*, c.nama as customer_nama
        FROM parking_tickets pt
        JOIN customers c ON pt.customer_id = c.id
        WHERE pt.status = 'AKTIF'
        ORDER BY pt.waktu_masuk DESC
      `).all();
    } else {
      // Customer can only see their own tickets
      tickets = db.prepare(`
        SELECT * FROM parking_tickets 
        WHERE customer_id = ? AND status = 'AKTIF'
        ORDER BY waktu_masuk DESC
      `).all(req.user.id);
    }

    res.json(tickets);
  } catch (error) {
    console.error('Get active tickets error:', error);
    res.status(500).json({ error: 'Internal server error' });
  }
});

// Get parking history
router.get('/history', authenticateToken, (req, res) => {
  try {
    const db = getDb();
    let tickets;

    if (req.user.role === 'admin') {
      tickets = db.prepare(`
        SELECT pt.*, c.nama as customer_nama
        FROM parking_tickets pt
        JOIN customers c ON pt.customer_id = c.id
        WHERE pt.status = 'DIBAYAR'
        ORDER BY pt.waktu_keluar DESC
        LIMIT 100
      `).all();
    } else {
      tickets = db.prepare(`
        SELECT * FROM parking_tickets 
        WHERE customer_id = ? AND status = 'DIBAYAR'
        ORDER BY waktu_keluar DESC
        LIMIT 50
      `).all(req.user.id);
    }

    res.json(tickets);
  } catch (error) {
    console.error('Get parking history error:', error);
    res.status(500).json({ error: 'Internal server error' });
  }
});

// Get loyalty points by license plate
router.get('/loyalty/:nomor_polisi', (req, res) => {
  try {
    const db = getDb();
    const loyalty = db.prepare('SELECT * FROM loyalty_points WHERE nomor_polisi = ?')
      .get(req.params.nomor_polisi);

    if (!loyalty) {
      return res.json({ nomor_polisi: req.params.nomor_polisi, poin: 0 });
    }

    res.json(loyalty);
  } catch (error) {
    console.error('Get loyalty points error:', error);
    res.status(500).json({ error: 'Internal server error' });
  }
});

module.exports = router;

