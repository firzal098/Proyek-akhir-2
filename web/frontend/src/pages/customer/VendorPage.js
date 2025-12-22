import React, { useState, useEffect } from 'react';
import {
  Paper,
  Typography,
  Grid,
  Card,
  CardContent,
  CardActions,
  Button,
  TextField,
  Dialog,
  DialogTitle,
  DialogContent,
  DialogActions,
  Alert,
  Box,
  Chip
} from '@mui/material';
import axios from 'axios';
import { useAuth } from '../../contexts/AuthContext';

const API_URL = process.env.REACT_APP_API_URL || 
  (process.env.NODE_ENV === 'production' ? '/api' : 'http://localhost:5000/api');

function VendorPage() {
  const { user } = useAuth();
  const [vendors, setVendors] = useState([]);
  const [selectedVendor, setSelectedVendor] = useState(null);
  const [products, setProducts] = useState([]);
  const [productDialogOpen, setProductDialogOpen] = useState(false);
  const [selectedProduct, setSelectedProduct] = useState(null);
  const [quantity, setQuantity] = useState(1);
  const [searchKeyword, setSearchKeyword] = useState('');
  const [message, setMessage] = useState({ type: '', text: '' });

  useEffect(() => {
    loadVendors();
  }, []);

  const loadVendors = async () => {
    try {
      const response = await axios.get(`${API_URL}/vendors`);
      setVendors(response.data);
    } catch (error) {
      console.error('Error loading vendors:', error);
    }
  };

  const loadVendorDetails = async (vendorId) => {
    try {
      const response = await axios.get(`${API_URL}/vendors/${vendorId}`);
      setSelectedVendor(response.data);
      setProducts(response.data.products || []);
    } catch (error) {
      console.error('Error loading vendor details:', error);
    }
  };

  const searchVendors = async () => {
    if (!searchKeyword.trim()) {
      loadVendors();
      return;
    }
    try {
      const response = await axios.get(`${API_URL}/vendors/search/${searchKeyword}`);
      setVendors(response.data);
    } catch (error) {
      console.error('Error searching vendors:', error);
    }
  };

  const handlePurchase = async () => {
    try {
      await axios.post(
        `${API_URL}/vendors/${selectedVendor.id}/products/${selectedProduct.id}/purchase`,
        { quantity }
      );
      setMessage({ type: 'success', text: 'Purchase successful!' });
      setProductDialogOpen(false);
      setQuantity(1);
      // Reload user data to update balance
      window.location.reload();
      loadVendorDetails(selectedVendor.id);
    } catch (error) {
      setMessage({ type: 'error', text: error.response?.data?.error || 'Purchase failed' });
    }
  };

  const openProductDialog = (product) => {
    setSelectedProduct(product);
    setProductDialogOpen(true);
  };

  return (
    <Box>
      <Typography variant="h4" gutterBottom>
        Vendor Marketplace
      </Typography>

      {message.text && (
        <Alert severity={message.type} sx={{ mb: 2 }} onClose={() => setMessage({ type: '', text: '' })}>
          {message.text}
        </Alert>
      )}

      <Box sx={{ mb: 3, display: 'flex', gap: 2 }}>
        <TextField
          fullWidth
          label="Search vendors (name or category)"
          value={searchKeyword}
          onChange={(e) => setSearchKeyword(e.target.value)}
          onKeyPress={(e) => e.key === 'Enter' && searchVendors()}
        />
        <Button variant="contained" onClick={searchVendors}>
          Search
        </Button>
        <Button variant="outlined" onClick={loadVendors}>
          Show All
        </Button>
      </Box>

      {selectedVendor ? (
        <Box>
          <Button onClick={() => setSelectedVendor(null)} sx={{ mb: 2 }}>
            ← Back to Vendors
          </Button>
          <Paper sx={{ p: 3 }}>
            <Typography variant="h5" gutterBottom>
              {selectedVendor.nama}
            </Typography>
            <Typography variant="body2" color="text.secondary" gutterBottom>
              Category: {selectedVendor.kategori || 'N/A'}
            </Typography>
            <Typography variant="body2" color="text.secondary" gutterBottom>
              Main Product: {selectedVendor.produk_utama || 'N/A'}
            </Typography>
            <Typography variant="h6" sx={{ mt: 3, mb: 2 }}>
              Products
            </Typography>
            <Grid container spacing={2}>
              {products.map((product) => (
                <Grid item xs={12} sm={6} md={4} key={product.id}>
                  <Card>
                    <CardContent>
                      <Typography variant="h6">{product.nama}</Typography>
                      <Typography variant="body2" color="text.secondary">
                        Price: Rp{product.harga.toLocaleString()}
                      </Typography>
                      <Chip
                        label={product.stok > 0 ? `Stock: ${product.stok}` : 'Out of Stock'}
                        color={product.stok > 0 ? 'success' : 'error'}
                        size="small"
                        sx={{ mt: 1 }}
                      />
                    </CardContent>
                    <CardActions>
                      <Button
                        size="small"
                        variant="contained"
                        onClick={() => openProductDialog(product)}
                        disabled={product.stok === 0}
                      >
                        Buy
                      </Button>
                    </CardActions>
                  </Card>
                </Grid>
              ))}
              {products.length === 0 && (
                <Typography color="text.secondary">No products available</Typography>
              )}
            </Grid>
          </Paper>
        </Box>
      ) : (
        <Grid container spacing={3}>
          {vendors.map((vendor) => (
            <Grid item xs={12} sm={6} md={4} key={vendor.id}>
              <Card>
                <CardContent>
                  <Typography variant="h6">{vendor.nama}</Typography>
                  <Typography variant="body2" color="text.secondary">
                    Category: {vendor.kategori || 'N/A'}
                  </Typography>
                  <Typography variant="body2" color="text.secondary">
                    Products: {vendor.jumlah_produk || 0}
                  </Typography>
                </CardContent>
                <CardActions>
                  <Button size="small" onClick={() => loadVendorDetails(vendor.id)}>
                    View Products
                  </Button>
                </CardActions>
              </Card>
            </Grid>
          ))}
          {vendors.length === 0 && (
            <Typography color="text.secondary">No vendors found</Typography>
          )}
        </Grid>
      )}

      {/* Purchase Dialog */}
      <Dialog open={productDialogOpen} onClose={() => setProductDialogOpen(false)}>
        <DialogTitle>Purchase Product</DialogTitle>
        <DialogContent>
          {selectedProduct && (
            <Box>
              <Typography variant="h6">{selectedProduct.nama}</Typography>
              <Typography variant="body2" color="text.secondary">
                Price: Rp{selectedProduct.harga.toLocaleString()} each
              </Typography>
              <Typography variant="body2" color="text.secondary">
                Stock: {selectedProduct.stok}
              </Typography>
              <TextField
                fullWidth
                type="number"
                label="Quantity"
                value={quantity}
                onChange={(e) => setQuantity(parseInt(e.target.value) || 1)}
                inputProps={{ min: 1, max: selectedProduct.stok }}
                sx={{ mt: 2 }}
              />
              <Typography variant="h6" sx={{ mt: 2 }}>
                Total: Rp{(selectedProduct.harga * quantity).toLocaleString()}
              </Typography>
              <Typography variant="body2" color="text.secondary">
                Your Balance: Rp{user?.saldo?.toLocaleString() || '0'}
              </Typography>
            </Box>
          )}
        </DialogContent>
        <DialogActions>
          <Button onClick={() => setProductDialogOpen(false)}>Cancel</Button>
          <Button
            onClick={handlePurchase}
            variant="contained"
            disabled={!selectedProduct || quantity > selectedProduct.stok || (selectedProduct.harga * quantity) > user?.saldo}
          >
            Purchase
          </Button>
        </DialogActions>
      </Dialog>
    </Box>
  );
}

export default VendorPage;

