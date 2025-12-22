import React, { useState, useEffect } from 'react';
import {
  Paper,
  Typography,
  Button,
  TextField,
  Table,
  TableBody,
  TableCell,
  TableContainer,
  TableHead,
  TableRow,
  Dialog,
  DialogTitle,
  DialogContent,
  DialogActions,
  Alert,
  Box
} from '@mui/material';
import axios from 'axios';

const API_URL = process.env.REACT_APP_API_URL || 
  (process.env.NODE_ENV === 'production' ? '/api' : 'http://localhost:5000/api');

function CustomerManagement() {
  const [customers, setCustomers] = useState([]);
  const [addBalanceOpen, setAddBalanceOpen] = useState(false);
  const [selectedCustomer, setSelectedCustomer] = useState(null);
  const [balanceAmount, setBalanceAmount] = useState('');
  const [message, setMessage] = useState({ type: '', text: '' });

  useEffect(() => {
    loadCustomers();
  }, []);

  const loadCustomers = async () => {
    try {
      const response = await axios.get(`${API_URL}/customers`);
      setCustomers(response.data);
    } catch (error) {
      console.error('Error loading customers:', error);
      setMessage({ type: 'error', text: 'Failed to load customers' });
    }
  };

  const handleAddBalance = async () => {
    try {
      const amount = parseFloat(balanceAmount);
      if (isNaN(amount) || amount <= 0) {
        setMessage({ type: 'error', text: 'Please enter a valid amount' });
        return;
      }

      await axios.post(`${API_URL}/customers/${selectedCustomer.id}/add-balance`, {
        amount
      });
      setMessage({ type: 'success', text: `Balance added successfully!` });
      setAddBalanceOpen(false);
      setBalanceAmount('');
      setSelectedCustomer(null);
      loadCustomers();
    } catch (error) {
      setMessage({ type: 'error', text: error.response?.data?.error || 'Failed to add balance' });
    }
  };

  const openAddBalanceDialog = (customer) => {
    setSelectedCustomer(customer);
    setAddBalanceOpen(true);
  };

  return (
    <Box>
      <Typography variant="h4" gutterBottom>
        Customer Management
      </Typography>

      {message.text && (
        <Alert severity={message.type} sx={{ mb: 2 }} onClose={() => setMessage({ type: '', text: '' })}>
          {message.text}
        </Alert>
      )}

      <Paper sx={{ p: 3 }}>
        <TableContainer>
          <Table>
            <TableHead>
              <TableRow>
                <TableCell>ID</TableCell>
                <TableCell>Name</TableCell>
                <TableCell>Phone</TableCell>
                <TableCell>Email</TableCell>
                <TableCell>Balance</TableCell>
                <TableCell>Actions</TableCell>
              </TableRow>
            </TableHead>
            <TableBody>
              {customers.map((customer) => (
                <TableRow key={customer.id}>
                  <TableCell>{customer.id}</TableCell>
                  <TableCell>{customer.nama}</TableCell>
                  <TableCell>{customer.nomor_telepon}</TableCell>
                  <TableCell>{customer.email}</TableCell>
                  <TableCell>Rp{customer.saldo.toLocaleString()}</TableCell>
                  <TableCell>
                    <Button
                      size="small"
                      variant="outlined"
                      onClick={() => openAddBalanceDialog(customer)}
                    >
                      Add Balance
                    </Button>
                  </TableCell>
                </TableRow>
              ))}
              {customers.length === 0 && (
                <TableRow>
                  <TableCell colSpan={6} align="center">
                    <Typography color="text.secondary">No customers found</Typography>
                  </TableCell>
                </TableRow>
              )}
            </TableBody>
          </Table>
        </TableContainer>
      </Paper>

      {/* Add Balance Dialog */}
      <Dialog open={addBalanceOpen} onClose={() => setAddBalanceOpen(false)}>
        <DialogTitle>Add Balance</DialogTitle>
        <DialogContent>
          {selectedCustomer && (
            <Box>
              <Typography variant="body1" gutterBottom>
                Customer: {selectedCustomer.nama}
              </Typography>
              <Typography variant="body2" color="text.secondary" gutterBottom>
                Current Balance: Rp{selectedCustomer.saldo.toLocaleString()}
              </Typography>
              <TextField
                fullWidth
                type="number"
                label="Amount to Add"
                value={balanceAmount}
                onChange={(e) => setBalanceAmount(e.target.value)}
                sx={{ mt: 2 }}
                inputProps={{ min: 0, step: 0.01 }}
              />
            </Box>
          )}
        </DialogContent>
        <DialogActions>
          <Button onClick={() => setAddBalanceOpen(false)}>Cancel</Button>
          <Button onClick={handleAddBalance} variant="contained">Add Balance</Button>
        </DialogActions>
      </Dialog>
    </Box>
  );
}

export default CustomerManagement;

