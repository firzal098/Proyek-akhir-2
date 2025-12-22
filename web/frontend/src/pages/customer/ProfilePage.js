import React, { useState, useEffect } from 'react';
import {
  Paper,
  Typography,
  Box,
  Grid,
  Table,
  TableBody,
  TableCell,
  TableContainer,
  TableHead,
  TableRow
} from '@mui/material';
import axios from 'axios';
import { useAuth } from '../../contexts/AuthContext';

const API_URL = process.env.REACT_APP_API_URL || 
  (process.env.NODE_ENV === 'production' ? '/api' : 'http://localhost:5000/api');

function ProfilePage() {
  const { user } = useAuth();
  const [customerData, setCustomerData] = useState(null);
  const [parkingHistory, setParkingHistory] = useState([]);

  const loadCustomerData = async () => {
    try {
      const response = await axios.get(`${API_URL}/customers/${user.id}`);
      setCustomerData(response.data);
    } catch (error) {
      console.error('Error loading customer data:', error);
    }
  };

  useEffect(() => {
    loadCustomerData();
    loadParkingHistory();
    // eslint-disable-next-line react-hooks/exhaustive-deps
  }, [user.id]);

  const loadParkingHistory = async () => {
    try {
      const response = await axios.get(`${API_URL}/parking/history`);
      setParkingHistory(response.data);
    } catch (error) {
      console.error('Error loading parking history:', error);
    }
  };

  const formatDate = (timestamp) => {
    if (!timestamp) return 'N/A';
    return new Date(timestamp * 1000).toLocaleString();
  };

  return (
    <Box>
      <Typography variant="h4" gutterBottom>
        My Profile
      </Typography>

      <Grid container spacing={3}>
        <Grid item xs={12} md={6}>
          <Paper sx={{ p: 3 }}>
            <Typography variant="h6" gutterBottom>
              Customer Information
            </Typography>
            {customerData && (
              <Box>
                <Typography><strong>ID:</strong> {customerData.id}</Typography>
                <Typography><strong>Name:</strong> {customerData.nama}</Typography>
                <Typography><strong>Phone:</strong> {customerData.nomor_telepon}</Typography>
                <Typography><strong>Email:</strong> {customerData.email}</Typography>
                <Typography variant="h6" sx={{ mt: 2 }}>
                  <strong>Balance:</strong> Rp{customerData.saldo.toLocaleString()}
                </Typography>
              </Box>
            )}
          </Paper>
        </Grid>

        <Grid item xs={12} md={6}>
          <Paper sx={{ p: 3 }}>
            <Typography variant="h6" gutterBottom>
              Parking History
            </Typography>
            {parkingHistory.length === 0 ? (
              <Typography color="text.secondary">No parking history</Typography>
            ) : (
              <TableContainer>
                <Table size="small">
                  <TableHead>
                    <TableRow>
                      <TableCell>Ticket ID</TableCell>
                      <TableCell>Plate</TableCell>
                      <TableCell>Slot</TableCell>
                      <TableCell>Check Out</TableCell>
                      <TableCell>Fee</TableCell>
                    </TableRow>
                  </TableHead>
                  <TableBody>
                    {parkingHistory.slice(0, 10).map((ticket) => (
                      <TableRow key={ticket.id}>
                        <TableCell>{ticket.id}</TableCell>
                        <TableCell>{ticket.nomor_polisi}</TableCell>
                        <TableCell>{ticket.slot_parkir}</TableCell>
                        <TableCell>{formatDate(ticket.waktu_keluar)}</TableCell>
                        <TableCell>Rp{ticket.biaya.toLocaleString()}</TableCell>
                      </TableRow>
                    ))}
                  </TableBody>
                </Table>
              </TableContainer>
            )}
          </Paper>
        </Grid>
      </Grid>
    </Box>
  );
}

export default ProfilePage;

