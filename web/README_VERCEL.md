# Deploying to Vercel

This guide explains how to deploy both frontend and backend to Vercel as a single application.

## Prerequisites

1. Vercel account (sign up at https://vercel.com)
2. Vercel CLI installed: `npm install -g vercel`

## Project Structure

```
web/
├── frontend/          # React frontend
├── backend/           # Express backend
│   └── api/          # Serverless function entry point
└── vercel.json       # Vercel configuration
```

## Deployment Steps

### Option 1: Deploy via Vercel CLI

1. **Navigate to the web directory:**
```bash
cd web
```

2. **Login to Vercel:**
```bash
vercel login
```

3. **Deploy:**
```bash
vercel
```

Follow the prompts:
- Set up and deploy? **Yes**
- Which scope? (Select your account)
- Link to existing project? **No**
- Project name? (Enter a name or press Enter for default)
- Directory? **./** (current directory)
- Override settings? **No**

4. **Set Environment Variables:**

After deployment, go to Vercel Dashboard → Your Project → Settings → Environment Variables and add:

- `JWT_SECRET`: A secure random string (e.g., generate with `openssl rand -base64 32`)
- `REACT_APP_API_URL`: `/api` (for production)

### Option 2: Deploy via GitHub

1. **Push your code to GitHub** (if not already)

2. **Go to Vercel Dashboard:**
   - Click "New Project"
   - Import your GitHub repository
   - Select the `web` folder as the root directory
   - Framework Preset: **Other** (or leave as auto-detected)

3. **Configure Build Settings:**
   - Root Directory: `web`
   - Build Command: (auto-detected)
   - Output Directory: `frontend/build`

4. **Add Environment Variables:**
   - `JWT_SECRET`: Your secret key
   - `REACT_APP_API_URL`: `/api`

5. **Deploy**

## Important Notes

### Database Limitations

⚠️ **SQLite in Serverless Environment:**
- The database uses `/tmp/database.db` in Vercel serverless functions
- Data persists during function execution but **resets on cold starts**
- This means data will be lost when:
  - Function goes cold (after inactivity)
  - New deployment happens
  - Function container restarts

**For Production Use:**
Consider migrating to:
- **Vercel Postgres** (recommended, free tier available)
- **PlanetScale** (MySQL)
- **Supabase** (PostgreSQL)

### Testing Locally

To test the Vercel setup locally:

```bash
cd web
vercel dev
```

This will:
- Start the frontend on http://localhost:3000
- Start the backend API on http://localhost:3000/api
- Simulate the Vercel serverless environment

### Production Build

The frontend will automatically build when deploying. The build process:
1. Runs `npm install` in `frontend/`
2. Runs `npm run build` to create the production build
3. Serves static files from `frontend/build/`
4. Routes `/api/*` requests to the serverless function

## Troubleshooting

### Build Fails
- Check that all dependencies are in `package.json`
- Ensure `frontend/package.json` has a `build` script
- Check Vercel build logs for specific errors

### API Routes Not Working
- Verify `vercel.json` routes are correct
- Check that `backend/api/index.js` exists
- Ensure environment variables are set

### Database Issues
- Remember: SQLite data resets on cold starts
- Consider using Vercel Postgres for persistent data
- Check function logs in Vercel Dashboard

## Environment Variables

Set these in Vercel Dashboard → Settings → Environment Variables:

| Variable | Value | Description |
|----------|-------|-------------|
| `JWT_SECRET` | Random string | Secret key for JWT tokens |
| `REACT_APP_API_URL` | `/api` | API base URL (relative) |
| `DB_PATH` | `/tmp/database.db` | Database path (optional, auto-set) |

## Next Steps

1. **Migrate to Vercel Postgres** for persistent data storage
2. **Set up custom domain** in Vercel Dashboard
3. **Configure CI/CD** - automatic deployments on git push
4. **Monitor** - Use Vercel Analytics and Logs

## Support

- Vercel Docs: https://vercel.com/docs
- Vercel Community: https://github.com/vercel/vercel/discussions

