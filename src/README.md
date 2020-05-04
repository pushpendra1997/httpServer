# HTTP Server with C++ Socket Programming

## Compile
    g++ server.cpp -o server.o -lpthread

## Run
    ./server.o

## Features Supported:
- Supports 10000+ Clients
- Delivers 130,000 response/second
- Keeps connection alive till keep-alive timeout
- Delivers HTTP response according to RFC standard
- Use multiple threads to serve the clients
- Cache the file to reduce File I/O
- Delivers 404 for invalid path, 500 for Internal Server Error
- Allows linking a path to a file

To supply 130,000 response/second, Limit of open files on Linux must be increased & keepAlive timeout in config.hpp must be increased.
Tested with WRK2 (https://github.com/giltene/wrk2).

## Files/Folders:
- server.cpp : main C++ file with server class.
- fileHandler.hpp : Handles File operations, and returns appropriate content corresponding to a path.
- utils.hpp : Utility Data Structures and Function.
- config.hpp : Config file for the server.
- templates/ : Contains all HTML pages
