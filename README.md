# MyPaySU - The People's Payment Platform

**MyPaySU** is a revolutionary payment platform themed around the glory of the Soviet Union and "Soviet Monero" (SXMR). It features a robust C++ backend server and a graphical C/Xlib frontend, designed to facilitate the redistribution of wealth among the proletariat.

![Soviet Theme](https://via.placeholder.com/800x600/CC0000/FFD700?text=MyPaySU+Interface)

## Features

*   **Centralized Ledger:** A secure, in-memory ledger managed by the Central Bureau (Server).
*   **State Subsidies:** Every new comrade receives an initial grant of 100.00 SXMR.
*   **Wealth Redistribution:** Seamlessly transfer funds to other comrades.
*   **Soviet Aesthetic:** A dedicated graphical interface built with X11 and Xft, featuring the colors of the revolution (Red & Gold) and utilizing "Liberation Sans" typography.
*   **Dynamic Interface:** The People's Window adapts to any size, ensuring the interface is always centered and accessible.

## Prerequisites

To build the project, you need a Linux environment with GCC/G++ and X11 development libraries.

### Debian/Ubuntu
```bash
sudo apt-get update
sudo apt-get install build-essential libx11-dev libxft-dev libfontconfig1-dev
```

## Build Instructions

The project uses a standard `Makefile` for compilation.

1.  **Build Everything:**
    ```bash
    make
    ```

2.  **Build Server Only:**
    ```bash
    make backend
    ```

3.  **Build Client Only:**
    ```bash
    make frontend
    ```

4.  **Clean Build Artifacts:**
    ```bash
    make clean
    ```

## Usage

### 1. Start the Central Bureau (Server)
The server must be running to process transactions. It listens on port **1917**.

```bash
./mypaysu-server
```
*You should see:* `=== MyPaySU Central Bureau Online ===`

### 2. Launch the People's Wallet (Client)
Open a new terminal window and start the client application.

```bash
./mypaysu-client
```

### 3. User Guide
*   **Register:** Enter a new username and password, then click "REGISTER". You will be granted 100 SXMR.
*   **Login:** Enter your credentials and click "LOGIN".
*   **Dashboard:**
    *   View your current balance.
    *   Enter a recipient's username and an amount to transfer.
    *   Click "REDISTRIBUTE WEALTH" to send funds.
    *   Click "REFRESH" to update your balance.

## Project Structure

*   `src/backend/` - C++ Server implementation (`server.cpp`).
*   `src/frontend/` - C Client implementation with Xlib GUI (`main.c`).
*   `include/` - Shared header files (`common.h`).
*   `Makefile` - Build configuration.

## License

This software is shared property of the people, licensed under the **GNU Affero General Public License v3 (AGPLv3)**. 

In the spirit of collective ownership and transparency, the AGPLv3 ensures that all who interact with the People's Payment Platform—even over a network—have the right to access, study, and improve its source code. Any redistributions or modifications must also be shared under the same revolutionary terms.

See the [LICENSE](LICENSE) file for the full text of the agreement.
