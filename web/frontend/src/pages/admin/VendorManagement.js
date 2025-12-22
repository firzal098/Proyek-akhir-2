import React, { useState, useEffect } from 'react';
import {
  Paper,
  Typography,
  Button,
  TextField,
  Grid,
  Card,
  CardContent,
  CardActions,
  Dialog,
  DialogTitle,
  DialogContent,
  DialogActions,
  Alert,
  Box,
  Table,
  TableBody,
  TableCell,
  TableContainer,
  TableHead,
  TableRow,
  IconButton
} from '@mui/material';
import { Add, Delete } from '@mui/icons-material';
import axios from 'axios';

const API_URL = process.env.REACT_APP_API_URL || 
  (process.env.NODE_ENV === 'production' ? '/api' : 'http://localhost:5000/api');

function VendorManagement() {
  const [vendors, setVendors] = useState([]);
  const [selectedVendor, setSelectedVendor] = useState(null);
  const [products, setProducts] = useState([]);
  const [vendorDialogOpen, setVendorDialogOpen] = useState(false);
  const [productDialogOpen, setProductDialogOpen] = useState(false);
  const [deleteDialogOpen, setDeleteDialogOpen] = useState(false);
  const [vendorToDelete, setVendorToDelete] = useState(null);
  const [vendorForm, setVendorForm] = useState({ nama: '', produk_utama: '', kategori: '' });
  const [productForm, setProductForm] = useState({ nama: '', harga: '', stok: '' });
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
      setMessage({ type: 'error', text: 'Failed to load vendors' });
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

  const handleCreateVendor = async () => {
    try {
      await axios.post(`${API_URL}/vendors`, vendorForm);
      setMessage({ type: 'success', text: 'Vendor created successfully!' });
      setVendorDialogOpen(false);
      setVendorForm({ nama: '', produk_utama: '', kategori: '' });
      loadVendors();
    } catch (error) {
      setMessage({ type: 'error', text: error.response?.data?.error || 'Failed to create vendor' });
    }
  };

  const handleDeleteVendor = async () => {
    try {
      await axios.delete(`${API_URL}/vendors/${vendorToDelete.id}`);
      setMessage({ type: 'success', text: 'Vendor deleted successfully!' });
      setDeleteDialogOpen(false);
      setVendorToDelete(null);
      setSelectedVendor(null);
      loadVendors();
    } catch (error) {
      setMessage({ type: 'error', text: error.response?.data?.error || 'Failed to delete vendor' });
    }
  };

  const handleAddProduct = async () => {
    try {
      await axios.post(`${API_URL}/vendors/${selectedVendor.id}/products`, {
        nama: productForm.nama,
        harga: parseFloat(productForm.harga),
        stok: parseInt(productForm.stok) || 0
      });
      setMessage({ type: 'success', text: 'Product added successfully!' });
      setProductDialogOpen(false);
      setProductForm({ nama: '', harga: '', stok: '' });
      loadVendorDetails(selectedVendor.id);
    } catch (error) {
      setMessage({ type: 'error', text: error.response?.data?.error || 'Failed to add product' });
    }
  };

  const handleDeleteProduct = async (productId) => {
    try {
      await axios.delete(`${API_URL}/vendors/${selectedVendor.id}/products/${productId}`);
      setMessage({ type: 'success', text: 'Product deleted successfully!' });
      loadVendorDetails(selectedVendor.id);
    } catch (error) {
      setMessage({ type: 'error', text: error.response?.data?.error || 'Failed to delete product' });
    }
  };

  return (
    <Box>
      <Box sx={{ display: 'flex', justifyContent: 'space-between', alignItems: 'center', mb: 3 }}>
        <Typography variant="h4">Vendor Management</Typography>
        <Button
          variant="contained"
          startIcon={<Add />}
          onClick={() => setVendorDialogOpen(true)}
        >
          Add Vendor
        </Button>
      </Box>

      {message.text && (
        <Alert severity={message.type} sx={{ mb: 2 }} onClose={() => setMessage({ type: '', text: '' })}>
          {message.text}
        </Alert>
      )}

      {selectedVendor ? (
        <Box>
          <Button onClick={() => setSelectedVendor(null)} sx={{ mb: 2 }}>
            ← Back to Vendors
          </Button>
          <Paper sx={{ p: 3 }}>
            <Box sx={{ display: 'flex', justifyContent: 'space-between', alignItems: 'center', mb: 2 }}>
              <Box>
                <Typography variant="h5">{selectedVendor.nama}</Typography>
                <Typography variant="body2" color="text.secondary">
                  Category: {selectedVendor.kategori || 'N/A'} | Main Product: {selectedVendor.produk_utama || 'N/A'}
                </Typography>
              </Box>
              <Button
                variant="outlined"
                color="error"
                onClick={() => {
                  setVendorToDelete(selectedVendor);
                  setDeleteDialogOpen(true);
                }}
              >
                Delete Vendor
              </Button>
            </Box>
            <Box sx={{ display: 'flex', justifyContent: 'space-between', alignItems: 'center', mb: 2 }}>
              <Typography variant="h6">Products</Typography>
              <Button
                variant="contained"
                startIcon={<Add />}
                onClick={() => setProductDialogOpen(true)}
              >
                Add Product
              </Button>
            </Box>
            <TableContainer>
              <Table>
                <TableHead>
                  <TableRow>
                    <TableCell>Name</TableCell>
                    <TableCell>Price</TableCell>
                    <TableCell>Stock</TableCell>
                    <TableCell>Actions</TableCell>
                  </TableRow>
                </TableHead>
                <TableBody>
                  {products.map((product) => (
                    <TableRow key={product.id}>
                      <TableCell>{product.nama}</TableCell>
                      <TableCell>Rp{product.harga.toLocaleString()}</TableCell>
                      <TableCell>{product.stok}</TableCell>
                      <TableCell>
                        <IconButton
                          color="error"
                          size="small"
                          onClick={() => handleDeleteProduct(product.id)}
                        >
                          <Delete />
                        </IconButton>
                      </TableCell>
                    </TableRow>
                  ))}
                  {products.length === 0 && (
                    <TableRow>
                      <TableCell colSpan={4} align="center">
                        <Typography color="text.secondary">No products</Typography>
                      </TableCell>
                    </TableRow>
                  )}
                </TableBody>
              </Table>
            </TableContainer>
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
                    Manage Products
                  </Button>
                </CardActions>
              </Card>
            </Grid>
          ))}
        </Grid>
      )}

      {/* Create Vendor Dialog */}
      <Dialog open={vendorDialogOpen} onClose={() => setVendorDialogOpen(false)}>
        <DialogTitle>Create Vendor</DialogTitle>
        <DialogContent>
          <TextField
            fullWidth
            label="Vendor Name"
            value={vendorForm.nama}
            onChange={(e) => setVendorForm({ ...vendorForm, nama: e.target.value })}
            sx={{ mt: 2 }}
            required
          />
          <TextField
            fullWidth
            label="Main Product"
            value={vendorForm.produk_utama}
            onChange={(e) => setVendorForm({ ...vendorForm, produk_utama: e.target.value })}
            sx={{ mt: 2 }}
          />
          <TextField
            fullWidth
            label="Category"
            value={vendorForm.kategori}
            onChange={(e) => setVendorForm({ ...vendorForm, kategori: e.target.value })}
            sx={{ mt: 2 }}
          />
        </DialogContent>
        <DialogActions>
          <Button onClick={() => setVendorDialogOpen(false)}>Cancel</Button>
          <Button onClick={handleCreateVendor} variant="contained">Create</Button>
        </DialogActions>
      </Dialog>

      {/* Add Product Dialog */}
      <Dialog open={productDialogOpen} onClose={() => setProductDialogOpen(false)}>
        <DialogTitle>Add Product</DialogTitle>
        <DialogContent>
          <TextField
            fullWidth
            label="Product Name"
            value={productForm.nama}
            onChange={(e) => setProductForm({ ...productForm, nama: e.target.value })}
            sx={{ mt: 2 }}
            required
          />
          <TextField
            fullWidth
            type="number"
            label="Price"
            value={productForm.harga}
            onChange={(e) => setProductForm({ ...productForm, harga: e.target.value })}
            sx={{ mt: 2 }}
            inputProps={{ min: 0, step: 0.01 }}
            required
          />
          <TextField
            fullWidth
            type="number"
            label="Stock"
            value={productForm.stok}
            onChange={(e) => setProductForm({ ...productForm, stok: e.target.value })}
            sx={{ mt: 2 }}
            inputProps={{ min: 0 }}
          />
        </DialogContent>
        <DialogActions>
          <Button onClick={() => setProductDialogOpen(false)}>Cancel</Button>
          <Button onClick={handleAddProduct} variant="contained">Add</Button>
        </DialogActions>
      </Dialog>

      {/* Delete Vendor Dialog */}
      <Dialog open={deleteDialogOpen} onClose={() => setDeleteDialogOpen(false)}>
        <DialogTitle>Delete Vendor</DialogTitle>
        <DialogContent>
          <Typography>
            Are you sure you want to delete vendor "{vendorToDelete?.nama}"? This will also delete all products.
          </Typography>
        </DialogContent>
        <DialogActions>
          <Button onClick={() => setDeleteDialogOpen(false)}>Cancel</Button>
          <Button onClick={handleDeleteVendor} variant="contained" color="error">
            Delete
          </Button>
        </DialogActions>
      </Dialog>
    </Box>
  );
}

export default VendorManagement;

