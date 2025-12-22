# Quick Deploy Guide to Vercel

## Prerequisites

1. **Install Vercel CLI:**
```bash
npm install -g vercel
```

2. **Login to Vercel:**
```bash
vercel login
```

## Deployment Steps

### Step 1: Navigate to web directory
```bash
cd web
```

### Step 2: Deploy to Vercel
```bash
vercel
```

Follow the prompts:
- **Set up and deploy?** → Yes
- **Which scope?** → Select your account
- **Link to existing project?** → No (first time) or Yes (if updating)
- **Project name?** → Press Enter for default or enter custom name
- **Directory?** → `./` (current directory)
- **Override settings?** → No

### Step 3: Set Environment Variables

After deployment, go to **Vercel Dashboard** → Your Project → **Settings** → **Environment Variables**

Add these variables:

| Variable | Value | Description |
|----------|-------|-------------|
| `JWT_SECRET` | `your-secure-random-string` | Generate with: `openssl rand -base64 32` |
| `REACT_APP_API_URL` | `/api` | API base URL (relative path) |

**Important:** Make sure to add these for **Production**, **Preview**, and **Development** environments.

### Step 4: Redeploy (if needed)

After setting environment variables, trigger a new deployment:
```bash
vercel --prod
```

Or redeploy from Vercel Dashboard.

## Testing Locally with Vercel

To test the Vercel setup locally:

```bash
cd web
vercel dev
```

This will:
- Start frontend on `http://localhost:3000`
- Start backend API on `http://localhost:3000/api`
- Simulate Vercel serverless environment

## Project Structure

```
web/
├── frontend/              # React frontend
│   ├── src/
│   ├── public/
│   └── package.json
├── backend/               # Express backend
│   ├── api/
│   │   └── index.js      # Serverless function entry point
│   ├── routes/
│   ├── config/
│   └── package.json
└── vercel.json           # Vercel configuration
```

## How It Works

1. **Frontend:** Built as static files and served from `frontend/build/`
2. **Backend:** Runs as serverless function at `/api/*` routes
3. **Database:** Uses SQLite in `/tmp/database.db` (⚠️ resets on cold starts)

## Important Notes

### ⚠️ Database Limitations

- SQLite data **resets on cold starts** (after inactivity)
- Data persists during function execution but not between deployments
- **For production:** Consider migrating to Vercel Postgres

### Environment Variables

- Set `JWT_SECRET` to a secure random string
- Set `REACT_APP_API_URL` to `/api` for production
- Don't commit `.env` files to git

## Troubleshooting

### Build Fails
- Check Vercel build logs
- Ensure `frontend/package.json` has `build` script
- Verify all dependencies are listed

### API Routes Return 404
- Check `vercel.json` routes configuration
- Verify `backend/api/index.js` exists
- Check function logs in Vercel Dashboard

### Database Not Working
- Remember: SQLite resets on cold starts
- Check function logs for database errors
- Consider using Vercel Postgres for persistence

## Next Steps

1. ✅ Deploy to Vercel
2. ✅ Set environment variables
3. 🔄 Test the deployment
4. 📊 Monitor logs and analytics
5. 🗄️ Consider migrating to Vercel Postgres for persistent data

## Support

- Vercel Docs: https://vercel.com/docs
- Check deployment logs in Vercel Dashboard
- See `README_VERCEL.md` for detailed information

