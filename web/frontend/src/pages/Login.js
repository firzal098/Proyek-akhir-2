import React, { useState } from 'react';
import { useNavigate } from 'react-router-dom';
import {
  Container,
  Paper,
  TextField,
  Button,
  Typography,
  Box,
  Tabs,
  Tab,
  Alert,
  CircularProgress
} from '@mui/material';
import { useAuth } from '../contexts/AuthContext';

function TabPanel({ children, value, index }) {
  return (
    <div role="tabpanel" hidden={value !== index}>
      {value === index && <Box sx={{ p: 3 }}>{children}</Box>}
    </div>
  );
}

function Login() {
  const [tab, setTab] = useState(0);
  const [loginData, setLoginData] = useState({ id: '', password: '' });
  const [registerData, setRegisterData] = useState({
    id: '',
    nama: '',
    nomor_telepon: '',
    email: '',
    password: '',
    confirmPassword: ''
  });
  const [error, setError] = useState('');
  const [loading, setLoading] = useState(false);
  const { login, register } = useAuth();
  const navigate = useNavigate();

  const handleLogin = async (e) => {
    e.preventDefault();
    setError('');
    setLoading(true);

    const result = await login(loginData.id, loginData.password);
    setLoading(false);

    if (result.success) {
      const user = JSON.parse(localStorage.getItem('user'));
      if (user.role === 'admin') {
        navigate('/admin');
      } else {
        navigate('/customer');
      }
    } else {
      setError(result.error);
    }
  };

  const handleRegister = async (e) => {
    e.preventDefault();
    setError('');

    if (registerData.password !== registerData.confirmPassword) {
      setError('Passwords do not match');
      return;
    }

    if (registerData.password.length < 6) {
      setError('Password must be at least 6 characters');
      return;
    }

    setLoading(true);
    const { confirmPassword, ...dataToSend } = registerData;
    const result = await register(dataToSend);
    setLoading(false);

    if (result.success) {
      setError('');
      setTab(0);
      setLoginData({ id: registerData.id, password: '' });
      alert('Registration successful! Please login.');
    } else {
      setError(result.error);
    }
  };

  return (
    <Container maxWidth="sm" sx={{ mt: 8 }}>
      <Paper elevation={3} sx={{ p: 4 }}>
        <Typography variant="h4" component="h1" gutterBottom align="center">
          Parking System
        </Typography>
        <Typography variant="subtitle1" align="center" color="text.secondary" sx={{ mb: 3 }}>
          Login or Register to continue
        </Typography>

        <Box sx={{ borderBottom: 1, borderColor: 'divider' }}>
          <Tabs value={tab} onChange={(e, newValue) => setTab(newValue)}>
            <Tab label="Login" />
            <Tab label="Register" />
          </Tabs>
        </Box>

        {error && (
          <Alert severity="error" sx={{ mt: 2 }}>
            {error}
          </Alert>
        )}

        <TabPanel value={tab} index={0}>
          <form onSubmit={handleLogin}>
            <TextField
              fullWidth
              label="ID"
              margin="normal"
              value={loginData.id}
              onChange={(e) => setLoginData({ ...loginData, id: e.target.value })}
              required
            />
            <TextField
              fullWidth
              label="Password"
              type="password"
              margin="normal"
              value={loginData.password}
              onChange={(e) => setLoginData({ ...loginData, password: e.target.value })}
              required
            />
            <Button
              type="submit"
              fullWidth
              variant="contained"
              sx={{ mt: 3, mb: 2 }}
              disabled={loading}
            >
              {loading ? <CircularProgress size={24} /> : 'Login'}
            </Button>
            <Typography variant="body2" color="text.secondary" align="center">
              Admin: admin / admin123
            </Typography>
          </form>
        </TabPanel>

        <TabPanel value={tab} index={1}>
          <form onSubmit={handleRegister}>
            <TextField
              fullWidth
              label="Customer ID"
              margin="normal"
              value={registerData.id}
              onChange={(e) => setRegisterData({ ...registerData, id: e.target.value })}
              required
            />
            <TextField
              fullWidth
              label="Full Name"
              margin="normal"
              value={registerData.nama}
              onChange={(e) => setRegisterData({ ...registerData, nama: e.target.value })}
              required
            />
            <TextField
              fullWidth
              label="Phone Number"
              margin="normal"
              value={registerData.nomor_telepon}
              onChange={(e) => setRegisterData({ ...registerData, nomor_telepon: e.target.value })}
              required
            />
            <TextField
              fullWidth
              label="Email"
              type="email"
              margin="normal"
              value={registerData.email}
              onChange={(e) => setRegisterData({ ...registerData, email: e.target.value })}
              required
            />
            <TextField
              fullWidth
              label="Password"
              type="password"
              margin="normal"
              value={registerData.password}
              onChange={(e) => setRegisterData({ ...registerData, password: e.target.value })}
              required
            />
            <TextField
              fullWidth
              label="Confirm Password"
              type="password"
              margin="normal"
              value={registerData.confirmPassword}
              onChange={(e) => setRegisterData({ ...registerData, confirmPassword: e.target.value })}
              required
            />
            <Button
              type="submit"
              fullWidth
              variant="contained"
              sx={{ mt: 3, mb: 2 }}
              disabled={loading}
            >
              {loading ? <CircularProgress size={24} /> : 'Register'}
            </Button>
          </form>
        </TabPanel>
      </Paper>
    </Container>
  );
}

export default Login;

