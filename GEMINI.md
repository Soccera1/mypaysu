# GEMINI.md - MyPaySU Context

This file provides instructional context for Gemini when working on the **MyPaySU** project.

## Project Overview
MyPaySU is a Soviet-themed payment platform designed for "wealth redistribution" using a fictional currency called **Soviet Monero (SXMR)**. The project follows a classic client-server architecture.

- **Purpose:** A fun, themed financial platform demonstrating networked C/C++ applications.
- **Architecture:**
    - **Backend (Central Bureau):** A multi-threaded C++ server that manages an in-memory ledger.
    - **Frontend (People's Wallet):** A lightweight C client with a custom graphical user interface built directly on X11 (Xlib).
    - **Protocol:** Custom binary structures (`SovietMessage`) exchanged over TCP on port `1917`.

## Technologies & Dependencies
- **Backend:** C++11, POSIX Threads (`pthread`), BSD Sockets.
- **Frontend:** C99, Xlib, Xft (for anti-aliased fonts), Fontconfig.
- **Design:** Soviet Aesthetic (Red: `#CC0000`, Gold: `#FFD700`).

## Building and Running

### Build Commands
The project uses a standard `Makefile`.
- `make` - Builds both the server and the client.
- `make backend` - Builds only the `mypaysu-server`.
- `make frontend` - Builds only the `mypaysu-client`.
- `make clean` - Removes binary artifacts.

### Execution
1.  **Start the Server:**
    ```bash
    ./mypaysu-server
    ```
    The server listens on all interfaces at port `1917`.

2.  **Start the Client:**
    ```bash
    ./mypaysu-client
    ```
    The client attempts to connect to `127.0.0.1:1917` by default.

## Key Files
- `include/common.h`: Defines the communication protocol and shared constants.
- `src/backend/server.cpp`: Server logic, user registration, and ledger management.
- `src/frontend/main.c`: Xlib GUI implementation, event loop, and network client.
- `PROTOCOL.md`: Detailed breakdown of the binary communication protocol.

## Development Conventions

### Coding Standards
- **Backend (C++):** Use modern C++ features where appropriate. Ensure all ledger operations are protected by `std::mutex`.
- **Frontend (C):** Use pure C for the GUI. Avoid high-level toolkits (GTK/Qt). Use `Xft` for font rendering.
- **UI Layout:** Never use absolute pixel coordinates for layout. UI elements must be centered or relative to the window dimensions (handled in `update_layout()` in `main.c`).

### Thematic Constraints
- **Colors:** Use `#CC0000` (Background) and `#FFD700` (Foreground/Highlights) for the UI.
- **Tone:** All user-facing strings and logs should maintain the "Soviet" persona (e.g., using terms like "Comrade", "Proletariat", "Capitalist Spy", "Redistribute Wealth").

### Testing
- No automated test suite is currently implemented.
- Manual testing involves running the server and connecting one or more clients to verify registration, login, balance checks, and transfers.
