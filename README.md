# Run Instructions
Complie the server using the following command:
```
 g++ src/server.cpp --std=c++17 -o server -lpthread
```
Boot up the server using

```
./server
```
HTTP server implemented in C++ without any third party library.

## Features

* Implemented only using GNU C/C++ Library
* Handle at multiple concurrent connections.
* Handle GET requests
* Asynchronous request handling
* Multithreaded/Thread Pooling