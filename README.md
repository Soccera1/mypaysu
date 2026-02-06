# MyPaySU - The People's Payment Platform

**Dedicated to the memory of Vladimir Ilyich Lenin.**

**MyPaySU** is a revolutionary payment platform themed around the glory of the Soviet Union and "Soviet Monero" (SXMR). Now available as a modern web application!

## Features

*   **Centralized Ledger:** A secure, in-memory ledger managed by the Central Bureau (Server).
*   **State Subsidies:** Every new comrade receives an initial grant of 100.00 SXMR.
*   **Wealth Redistribution:** Seamlessly transfer funds to other comrades.
*   **Soviet Aesthetic:** A beautiful web interface featuring the colors of the revolution (Red & Gold).
*   **Modern Web Technology:** Responsive HTML/CSS/JavaScript frontend with C++ HTTP backend.
*   **Cross-Platform:** Works on any device with a web browser!

## Web Application

The default build target is now the web application, which provides a modern browser-based interface.

### Prerequisites

To build the project, you need a Linux environment with GCC/G++:

#### Debian/Ubuntu
```bash
sudo apt-get update
sudo apt-get install build-essential
```

### Build Instructions

```bash
make
```

This will compile the web server (`mypaysu-web`).

### Usage

#### 1. Start the Web Server

```bash
./mypaysu-web
```

You should see:
```
=== MyPaySU Web Central Bureau Online ===
Dedicated to the Great Leader, Vladimir Ilyich Lenin
Web server listening on http://localhost:1917
```

#### 2. Open Your Browser

Navigate to: **http://localhost:1917**

#### 3. User Guide

*   **Register:** Enter a new username and password, then click "REGISTER". You will be granted 100 SXMR.
*   **Login:** Enter your credentials and click "LOGIN".
*   **Dashboard:**
    *   View your current balance.
    *   Enter a recipient's username and an amount to transfer.
    *   Click "REDISTRIBUTE WEALTH" to send funds.
    *   Click "REFRESH" to update your balance.
    *   Click "LOGOUT" to return to the login screen.

## Legacy Desktop Application

The original X11-based desktop client and server are still available for those who prefer the traditional experience.

### Build Legacy Applications

```bash
make legacy
```

This builds both:
- `mypaysu-server` - TCP server (port 1917)
- `mypaysu-client` - X11/Xlib GUI client

### Legacy Prerequisites

```bash
sudo apt-get install build-essential libx11-dev libxft-dev libfontconfig1-dev
```

### Legacy Usage

1. Start the server: `./mypaysu-server`
2. In another terminal, start the client: `./mypaysu-client`

## Project Structure

*   `src/backend/webserver.cpp` - Modern HTTP web server with REST API
*   `src/backend/server.cpp` - Legacy TCP server
*   `src/frontend/main.c` - Legacy X11/Xlib GUI client
*   `public/` - Web frontend files:
    *   `index.html` - Main HTML page
    *   `style.css` - Stylesheet (Soviet themed!)
    *   `app.js` - Client-side JavaScript
*   `include/` - Shared header files (`common.h`)
*   `Makefile` - Build configuration

## API Endpoints

The web server exposes the following REST API endpoints:

- `POST /api/register` - Register a new user
  - Parameters: `username`, `password`
- `POST /api/login` - Authenticate a user
  - Parameters: `username`, `password`
- `POST /api/balance` - Get user balance
  - Parameters: `username`
- `POST /api/transfer` - Transfer funds between users
  - Parameters: `username`, `target_user`, `amount`

All endpoints return JSON responses with the format:
```json
{
  "success": true/false,
  "message": "Status message",
  "balance": 123.45  // For balance and transfer responses
}
```

## Clean Build

```bash
make clean
```

## License

This software is shared property of the people, licensed under the **GNU Affero General Public License v3 (AGPLv3)**. 

In the spirit of collective ownership and transparency, the AGPLv3 ensures that all who interact with the People's Payment Platform—even over a network—have the right to access, study, and improve its source code. Any redistributions or modifications must also be shared under the same revolutionary terms.

See the [LICENSE](LICENSE) file for the full text of the agreement.

---

*"The goal of socialism is communism." - V.I. Lenin*
