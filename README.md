# Server-Client Socket Programming Project

## Overview

This project demonstrates a personal implementation of a server-client application using socket programming. The goal was to build a networked system where a client queries a server for information based on names provided. The project showcases my understanding of stream socket communication, error handling, and concurrent client management.

## Project Details

The application involves two main components:

1. **Client**
   - Connects to a server using specified address and port.
   - Sends names to the server and processes responses.
   - Handles various scenarios such as connection failures and unexpected server issues.

2. **Server**
   - Listens for client connections and processes queries.
   - Responds with relevant information based on a customer file.
   - Manages multiple client connections concurrently using network socket and synchronous IO multiplexing.

## Key Features

### Client (`client.c`)

- **Connection Handling:** Establishes a connection to the server with provided address and port.
- **Interaction:** Reads names from standard input and sends them to the server.
- **Response Handling:** Receives and displays server responses or error messages.
- **Error Management:** Includes handling for connection failures, unexpected disconnections, and protocol errors.

### Server (`server.c`)

- **Connection Management:** Binds to a port and listens for incoming client connections.
- **Request Processing:** Handles client queries based on a predefined customer file.
- **Concurrency:** Supports multiple simultaneous clients using `select()` synchronous I/O multiplexing.
- **Error Handling:** Checking signals, manages client malformed, disconnections, server errors.

## Technical Approach

- **Socket Programming:** Implemented using TCP/IP sockets for reliable communication.
- **Concurrency Techniques:** Used IO multiplexing to handle multiple client requests.
- **Error Handling:** Comprehensive error checking and handling signals to manage network issues and unexpected client behavior.

## Testing and Usage

- **Local Testing:** Used tools like `nc` (netcat) for manual testing and debugging.
- **SSH Tunneling:** Utilized SSH for testing client-server interactions across different networks.

## Conclusion

This project is a practical demonstration of my skills in network programming, particularly in building robust client-server applications. It highlights my ability to handle real-world scenarios and manage various aspects of socket communication effectively.

For a detailed look at the implementation, refer to the source files: `client.c` and `server.c`.
