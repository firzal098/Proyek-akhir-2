# Quick Start Guide

## Step 1: Install Dependencies

### Backend
```bash
cd web/backend
npm install
```

### Frontend
```bash
cd web/frontend
npm install
```

## Step 2: Configure Backend

1. Copy the environment example file:
```bash
cd web/backend
copy env.example .env
```

2. Edit `.env` if needed (defaults are fine for development)

## Step 3: Start the Application

### Option A: Run in Separate Terminals

**Terminal 1 - Backend:**
```bash
cd web/backend
npm start
```

**Terminal 2 - Frontend:**
```bash
cd web/frontend
npm start
```

### Option B: Use Development Mode (Auto-reload)

**Terminal 1 - Backend:**
```bash
cd web/backend
npm run dev
```

**Terminal 2 - Frontend:**
```bash
cd web/frontend
npm start
```

## Step 4: Access the Application

- Frontend: http://localhost:3000
- Backend API: http://localhost:5000

## Step 5: Login

### Admin Account
- ID: `admin`
- Password: `admin123`

### Customer Account
Register a new account from the login page, or use an existing customer ID if you have one.

## Features to Test

### As Admin:
1. View all customers
2. Add balance to customers
3. Create vendors and add products
4. Manage parking (check-in/check-out)

### As Customer:
1. Check in/out parking
2. View parking map
3. Browse vendors and purchase products
4. View profile and parking history

## Troubleshooting

### Port Already in Use
If port 5000 or 3000 is already in use:
- Backend: Change `PORT` in `.env` file
- Frontend: React will prompt to use a different port

### Database Issues
- The database file (`database.db`) is created automatically
- Delete `database.db` to reset the database

### CORS Issues
- Make sure backend is running before starting frontend
- Check that `REACT_APP_API_URL` in frontend matches backend URL

