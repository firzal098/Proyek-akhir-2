# Parking System Web Application

A full-stack web application for managing parking, customers, and vendors. This is a web version of the C++ parking system project.

## Project Structure

```
web/
├── backend/          # Node.js/Express backend
│   ├── config/       # Database configuration
│   ├── middleware/   # Authentication middleware
│   ├── routes/       # API routes
│   └── server.js     # Entry point
└── frontend/         # React frontend
    ├── public/       # Static files
    └── src/          # React source code
        ├── components/  # Reusable components
        ├── contexts/    # React contexts
        └── pages/       # Page components
```

## Features

### Customer Features
- User registration and login
- Parking check-in/check-out
- View parking map (5x5 grid)
- Browse and purchase from vendors
- View profile and parking history
- Check loyalty points

### Admin Features
- Customer management (view all customers, add balance)
- Vendor management (create, update, delete vendors and products)
- Parking management (check-in/check-out, view active tickets and history)
- View parking map

## Prerequisites

- Node.js (v14 or higher)
- npm or yarn

## Installation

### Backend Setup

1. Navigate to the backend directory:
```bash
cd web/backend
```

2. Install dependencies:
```bash
npm install
```

3. Create a `.env` file (copy from `.env.example`):
```bash
PORT=5000
JWT_SECRET=your-secret-key-change-this-in-production
DB_PATH=./database.db
```

4. Start the backend server:
```bash
npm start
# or for development with auto-reload:
npm run dev
```

The backend will run on `http://localhost:5000`

### Frontend Setup

1. Navigate to the frontend directory:
```bash
cd web/frontend
```

2. Install dependencies:
```bash
npm install
```

3. Create a `.env` file (optional, defaults to localhost:5000):
```bash
REACT_APP_API_URL=http://localhost:5000/api
```

4. Start the frontend development server:
```bash
npm start
```

The frontend will run on `http://localhost:3000`

## Default Credentials

- **Admin**: 
  - ID: `admin`
  - Password: `admin123`

## API Endpoints

### Authentication
- `POST /api/auth/register` - Register new customer
- `POST /api/auth/login` - Login (customer or admin)

### Customers
- `GET /api/customers` - Get all customers (admin only)
- `GET /api/customers/:id` - Get customer by ID
- `POST /api/customers/:id/add-balance` - Add balance to customer (admin only)

### Vendors
- `GET /api/vendors` - Get all vendors
- `GET /api/vendors/:id` - Get vendor with products
- `POST /api/vendors` - Create vendor (admin only)
- `PUT /api/vendors/:id` - Update vendor (admin only)
- `DELETE /api/vendors/:id` - Delete vendor (admin only)
- `GET /api/vendors/search/:keyword` - Search vendors
- `POST /api/vendors/:id/products` - Add product to vendor (admin only)
- `POST /api/vendors/:vendorId/products/:productId/purchase` - Purchase product (customer)

### Parking
- `GET /api/parking/map` - Get parking map (5x5 grid)
- `POST /api/parking/check-in` - Check in (parking entry)
- `POST /api/parking/check-out` - Check out (parking exit)
- `GET /api/parking/active` - Get active tickets
- `GET /api/parking/history` - Get parking history
- `GET /api/parking/loyalty/:nomor_polisi` - Get loyalty points

## Database Schema

The application uses SQLite database with the following tables:

- `customers` - Customer information
- `vendors` - Vendor information
- `products` - Product information (linked to vendors)
- `parking_tickets` - Parking ticket records
- `loyalty_points` - Loyalty points by license plate
- `transactions` - Purchase transaction records

## Technologies Used

### Backend
- Node.js
- Express.js
- SQLite (better-sqlite3)
- JWT for authentication
- bcryptjs for password hashing

### Frontend
- React
- React Router
- Material-UI (MUI)
- Axios for API calls

## Development

### Running Both Servers

You can run both servers simultaneously:

**Terminal 1 (Backend):**
```bash
cd web/backend
npm run dev
```

**Terminal 2 (Frontend):**
```bash
cd web/frontend
npm start
```

## Building for Production

### Backend
The backend is ready to run with Node.js. For production, consider:
- Using environment variables for sensitive data
- Setting up proper logging
- Using a process manager like PM2

### Frontend
Build the React app:
```bash
cd web/frontend
npm run build
```

The built files will be in the `build/` directory.

## Notes

- The database file (`database.db`) will be created automatically on first run
- Admin credentials are hardcoded (as in the original C++ project): admin/admin123
- Parking fee calculation: Rp2000 per second (minimum 1 second)
- Loyalty points: 1 point per Rp1000 parking fee

