import React, { useState, useEffect } from 'react';
import {
  Paper,
  Typography,
  Button,
  TextField,
  Grid,
  Box,
  Card,
  Alert,
  Dialog,
  DialogTitle,
  DialogContent,
  DialogActions,
  Table,
  TableBody,
  TableCell,
  TableContainer,
  TableHead,
  TableRow,
  Select,
  MenuItem,
  FormControl,
  InputLabel
} from '@mui/material';
import axios from 'axios';

const API_URL = process.env.REACT_APP_API_URL || 
  (process.env.NODE_ENV === 'production' ? '/api' : 'http://localhost:5000/api');

function ParkingManagement() {
  const [parkingMap, setParkingMap] = useState([]);
  const [activeTickets, setActiveTickets] = useState([]);
  const [parkingHistory, setParkingHistory] = useState([]);
  const [checkInOpen, setCheckInOpen] = useState(false);
  const [checkOutOpen, setCheckOutOpen] = useState(false);
  const [customers, setCustomers] = useState([]);
  const [checkInData, setCheckInData] = useState({
    customer_id: '',
    nomor_polisi: '',
    slot_parkir: ''
  });
  const [checkOutData, setCheckOutData] = useState({ ticket_id: '' });
  const [message, setMessage] = useState({ type: '', text: '' });

  useEffect(() => {
    loadParkingMap();
    loadActiveTickets();
    loadParkingHistory();
    loadCustomers();
  }, []);

  const loadParkingMap = async () => {
    try {
      const response = await axios.get(`${API_URL}/parking/map`);
      setParkingMap(response.data.grid);
    } catch (error) {
      console.error('Error loading parking map:', error);
    }
  };

  const loadActiveTickets = async () => {
    try {
      const response = await axios.get(`${API_URL}/parking/active`);
      setActiveTickets(response.data);
    } catch (error) {
      console.error('Error loading active tickets:', error);
    }
  };

  const loadParkingHistory = async () => {
    try {
      const response = await axios.get(`${API_URL}/parking/history`);
      setParkingHistory(response.data);
    } catch (error) {
      console.error('Error loading parking history:', error);
    }
  };

  const loadCustomers = async () => {
    try {
      const response = await axios.get(`${API_URL}/customers`);
      setCustomers(response.data);
    } catch (error) {
      console.error('Error loading customers:', error);
    }
  };

  const handleCheckIn = async () => {
    try {
      await axios.post(`${API_URL}/parking/check-in`, checkInData);
      setMessage({ type: 'success', text: 'Check-in successful!' });
      setCheckInOpen(false);
      setCheckInData({ customer_id: '', nomor_polisi: '', slot_parkir: '' });
      loadParkingMap();
      loadActiveTickets();
    } catch (error) {
      setMessage({ type: 'error', text: error.response?.data?.error || 'Check-in failed' });
    }
  };

  const handleCheckOut = async () => {
    try {
      const response = await axios.post(`${API_URL}/parking/check-out`, checkOutData);
      setMessage({ type: 'success', text: `Check-out successful! Fee: Rp${response.data.ticket.biaya.toLocaleString()}` });
      setCheckOutOpen(false);
      setCheckOutData({ ticket_id: '' });
      loadParkingMap();
      loadActiveTickets();
      loadParkingHistory();
    } catch (error) {
      setMessage({ type: 'error', text: error.response?.data?.error || 'Check-out failed' });
    }
  };

  const formatDate = (timestamp) => {
    if (!timestamp) return 'N/A';
    return new Date(timestamp * 1000).toLocaleString();
  };

  return (
    <Box>
      <Typography variant="h4" gutterBottom>
        Parking Management
      </Typography>

      {message.text && (
        <Alert severity={message.type} sx={{ mb: 2 }} onClose={() => setMessage({ type: '', text: '' })}>
          {message.text}
        </Alert>
      )}

      <Grid container spacing={3}>
        <Grid item xs={12} md={8}>
          <Paper sx={{ p: 3 }}>
            <Typography variant="h6" gutterBottom>
              Parking Map (5x5 Grid)
            </Typography>
            <Box sx={{ display: 'flex', flexDirection: 'column', gap: 1, mt: 2 }}>
              {parkingMap.map((row, rowIndex) => (
                <Box key={rowIndex} sx={{ display: 'flex', gap: 1, justifyContent: 'center' }}>
                  {row.map((slot) => (
                    <Card
                      key={slot.id}
                      sx={{
                        width: 60,
                        height: 60,
                        display: 'flex',
                        alignItems: 'center',
                        justifyContent: 'center',
                        bgcolor: slot.occupied ? 'error.light' : 'success.light',
                        color: slot.occupied ? 'error.contrastText' : 'success.contrastText',
                      }}
                    >
                      <Typography variant="body2" fontWeight="bold">
                        {slot.occupied ? 'XX' : slot.id}
                      </Typography>
                    </Card>
                  ))}
                </Box>
              ))}
            </Box>
            <Box sx={{ mt: 2, display: 'flex', gap: 2 }}>
              <Button variant="contained" onClick={() => setCheckInOpen(true)}>
                Check In
              </Button>
              <Button variant="outlined" onClick={() => setCheckOutOpen(true)}>
                Check Out
              </Button>
            </Box>
          </Paper>
        </Grid>

        <Grid item xs={12} md={4}>
          <Paper sx={{ p: 3 }}>
            <Typography variant="h6" gutterBottom>
              Active Tickets
            </Typography>
            {activeTickets.length === 0 ? (
              <Typography color="text.secondary">No active tickets</Typography>
            ) : (
              <TableContainer>
                <Table size="small">
                  <TableHead>
                    <TableRow>
                      <TableCell>Ticket ID</TableCell>
                      <TableCell>Customer</TableCell>
                      <TableCell>Plate</TableCell>
                      <TableCell>Slot</TableCell>
                    </TableRow>
                  </TableHead>
                  <TableBody>
                    {activeTickets.map((ticket) => (
                      <TableRow key={ticket.id}>
                        <TableCell>{ticket.id}</TableCell>
                        <TableCell>{ticket.customer_nama || ticket.customer_id}</TableCell>
                        <TableCell>{ticket.nomor_polisi}</TableCell>
                        <TableCell>{ticket.slot_parkir}</TableCell>
                      </TableRow>
                    ))}
                  </TableBody>
                </Table>
              </TableContainer>
            )}
          </Paper>
        </Grid>

        <Grid item xs={12}>
          <Paper sx={{ p: 3 }}>
            <Typography variant="h6" gutterBottom>
              Parking History
            </Typography>
            <TableContainer>
              <Table>
                <TableHead>
                  <TableRow>
                    <TableCell>Ticket ID</TableCell>
                    <TableCell>Customer</TableCell>
                    <TableCell>Plate</TableCell>
                    <TableCell>Slot</TableCell>
                    <TableCell>Check In</TableCell>
                    <TableCell>Check Out</TableCell>
                    <TableCell>Fee</TableCell>
                  </TableRow>
                </TableHead>
                <TableBody>
                  {parkingHistory.slice(0, 20).map((ticket) => (
                    <TableRow key={ticket.id}>
                      <TableCell>{ticket.id}</TableCell>
                      <TableCell>{ticket.customer_nama || ticket.customer_id}</TableCell>
                      <TableCell>{ticket.nomor_polisi}</TableCell>
                      <TableCell>{ticket.slot_parkir}</TableCell>
                      <TableCell>{formatDate(ticket.waktu_masuk)}</TableCell>
                      <TableCell>{formatDate(ticket.waktu_keluar)}</TableCell>
                      <TableCell>Rp{ticket.biaya.toLocaleString()}</TableCell>
                    </TableRow>
                  ))}
                  {parkingHistory.length === 0 && (
                    <TableRow>
                      <TableCell colSpan={7} align="center">
                        <Typography color="text.secondary">No parking history</Typography>
                      </TableCell>
                    </TableRow>
                  )}
                </TableBody>
              </Table>
            </TableContainer>
          </Paper>
        </Grid>
      </Grid>

      {/* Check In Dialog */}
      <Dialog open={checkInOpen} onClose={() => setCheckInOpen(false)}>
        <DialogTitle>Check In</DialogTitle>
        <DialogContent>
          <FormControl fullWidth sx={{ mt: 2 }}>
            <InputLabel>Customer</InputLabel>
            <Select
              value={checkInData.customer_id}
              onChange={(e) => setCheckInData({ ...checkInData, customer_id: e.target.value })}
              label="Customer"
            >
              {customers.map((customer) => (
                <MenuItem key={customer.id} value={customer.id}>
                  {customer.nama} ({customer.id})
                </MenuItem>
              ))}
            </Select>
          </FormControl>
          <TextField
            fullWidth
            label="License Plate"
            value={checkInData.nomor_polisi}
            onChange={(e) => setCheckInData({ ...checkInData, nomor_polisi: e.target.value })}
            sx={{ mt: 2 }}
          />
          <TextField
            fullWidth
            label="Parking Slot (e.g., A1)"
            value={checkInData.slot_parkir}
            onChange={(e) => setCheckInData({ ...checkInData, slot_parkir: e.target.value.toUpperCase() })}
            sx={{ mt: 2 }}
            helperText="Format: A1-E5"
          />
        </DialogContent>
        <DialogActions>
          <Button onClick={() => setCheckInOpen(false)}>Cancel</Button>
          <Button onClick={handleCheckIn} variant="contained">Check In</Button>
        </DialogActions>
      </Dialog>

      {/* Check Out Dialog */}
      <Dialog open={checkOutOpen} onClose={() => setCheckOutOpen(false)}>
        <DialogTitle>Check Out</DialogTitle>
        <DialogContent>
          <TextField
            fullWidth
            label="Ticket ID"
            value={checkOutData.ticket_id}
            onChange={(e) => setCheckOutData({ ...checkOutData, ticket_id: e.target.value })}
            sx={{ mt: 2 }}
          />
        </DialogContent>
        <DialogActions>
          <Button onClick={() => setCheckOutOpen(false)}>Cancel</Button>
          <Button onClick={handleCheckOut} variant="contained">Check Out</Button>
        </DialogActions>
      </Dialog>
    </Box>
  );
}

export default ParkingManagement;

