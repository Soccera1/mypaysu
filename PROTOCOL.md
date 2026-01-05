# MyPaySU Communication Protocol

The MyPaySU platform uses a custom binary protocol over TCP on port **1917**. All communication consists of fixed-size structures exchanged between the client and server.

## Message Structure

All messages use the `SovietMessage` C-structure defined in `include/common.h`. The structure is sent directly over the wire.

| Field | Type | Size (Bytes) | Description |
| :--- | :--- | :--- | :--- |
| `type` | `int` | 4 | The operation code (Request or Response type). |
| `username` | `char[32]` | 32 | Username of the requester or account holder. |
| `password` | `char[32]` | 32 | Password for authentication or registration. |
| `target_user` | `char[32]` | 32 | Recipient username for transfers. |
| `amount` | `double` | 8 | Amount to transfer or the balance in a response. |
| `message` | `char[256]` | 256 | Status message or error description. |

**Total Size:** ~364 bytes (depending on padding/alignment).

## Request Types

These codes are sent from the Client to the Server.

| Type | Code | Description | Required Fields |
| :--- | :--- | :--- | :--- |
| `REQ_REGISTER` | `0` | Register a new user. | `username`, `password` |
| `REQ_LOGIN` | `1` | Authenticate a user. | `username`, `password` |
| `REQ_BALANCE` | `2` | Request account balance. | `username` |
| `REQ_TRANSFER` | `3` | Transfer funds to another user. | `username`, `target_user`, `amount` |

## Response Types

These codes are returned from the Server to the Client.

| Type | Code | Description |
| :--- | :--- | :--- |
| `RES_OK` | `0` | Operation successful. `message` contains success text. |
| `RES_ERROR` | `1` | Operation failed. `message` contains error details. |

## Example Workflow

### Registration
1.  **Client** sends `REQ_REGISTER` with `username="ivan"`, `password="secret"`.
2.  **Server** checks if "ivan" exists.
3.  **Server** responds with `RES_OK` and `message="Welcome..."` or `RES_ERROR`.

### Transfer
1.  **Client** sends `REQ_TRANSFER` with `username="ivan"`, `target_user="boris"`, `amount=50.0`.
2.  **Server** verifies "ivan" has sufficient funds and "boris" exists.
3.  **Server** deducts 50.0 from "ivan", adds 50.0 to "boris".
4.  **Server** responds with `RES_OK`.
