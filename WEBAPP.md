# MyPaySU Web Application Guide

## Overview

MyPaySU has been transformed into a modern web application! The frontend is now a responsive HTML/CSS/JavaScript single-page application that communicates with a C++ HTTP backend server via REST API.

## Architecture

### Backend: C++ HTTP Server
- **File:** `src/backend/webserver.cpp`
- **Port:** 1917 (honoring the Great October Socialist Revolution)
- **Technology:** 
  - POSIX sockets for HTTP communication
  - Multi-threaded request handling
  - In-memory data store with mutex protection
  - REST API with JSON responses

### Frontend: Web Application
- **Files:** `public/index.html`, `public/style.css`, `public/app.js`
- **Technology:**
  - Modern HTML5
  - CSS3 with responsive design
  - Vanilla JavaScript (no frameworks required!)
  - Fetch API for REST calls

## Quick Start

### 1. Build the Server

```bash
make
```

### 2. Run the Server

```bash
./mypaysu-web
```

Expected output:
```
=== MyPaySU Web Central Bureau Online ===
Dedicated to the Great Leader, Vladimir Ilyich Lenin
Web server listening on http://localhost:1917
```

### 3. Access the Application

Open your web browser and navigate to:
```
http://localhost:1917
```

## Features

### User Interface
- **Soviet-themed design:** Red and gold color scheme
- **Responsive layout:** Works on desktop, tablet, and mobile
- **Smooth transitions:** Modern CSS animations
- **Real-time updates:** Instant balance updates after transfers

### Functionality
- User registration with automatic 100 SXMR subsidy
- Secure login (credentials stored in memory)
- Balance checking
- Fund transfers between users
- Session management (logout functionality)

## Development Notes

### File Serving
The server serves static files from the `public/` directory:
- HTML files with `text/html` content type
- CSS files with `text/css` content type
- JavaScript files with `application/javascript` content type

### API Communication
All API endpoints use POST requests with `application/x-www-form-urlencoded` content type and return JSON responses.

### Concurrency
Each HTTP request is handled in a separate detached thread, allowing multiple simultaneous connections.

### Security Notes
⚠️ **This is a demonstration application.** In production:
- Use HTTPS instead of HTTP
- Implement proper authentication (JWT, OAuth, etc.)
- Add input validation and sanitization
- Use a real database instead of in-memory storage
- Implement rate limiting
- Add CSRF protection
- Hash passwords instead of storing them in plain text

## Browser Compatibility

Tested and working on:
- Chrome/Chromium
- Firefox
- Safari
- Edge

Requires a modern browser with:
- Fetch API support
- ES6 JavaScript support
- CSS3 support

## Port Configuration

The server runs on port 1917 by default. To change the port, edit the `PORT` constant in `src/backend/webserver.cpp`:

```cpp
#define PORT 1917  // Change this value
```

Then rebuild with `make clean && make`.

## Troubleshooting

### "Connection Failed" Error
- Make sure the server is running: `./mypaysu-web`
- Check that port 1917 is not blocked by a firewall
- Verify the server is listening: `netstat -tlnp | grep 1917`

### "Invalid credentials" on Login
- Make sure you registered the user first
- Check that username and password are spelled correctly
- Remember: the system is case-sensitive

### Balance Not Updating
- Click the "REFRESH" button to manually update
- Check the browser console for JavaScript errors
- Verify the server is still running

## Native Desktop vs Web Application

MyPaySU offers two frontend alternatives:

| Feature | Native Desktop (X11) | Web Application |
|---------|---------------------|-----------------|
| Platform | Linux | Any OS with browser |
| Interface | X11/Xlib GUI | HTML/CSS/JS |
| Protocol | Binary TCP | HTTP REST API |
| Portability | Desktop only | Desktop + Mobile |
| Dependencies | X11, Xft, Fontconfig | None (browser-based) |
| Build Target | `make native` | `make` |

Both frontends provide the same core functionality - choose the one that best fits your needs!

---

*For the glory of the Revolution! ☭*
