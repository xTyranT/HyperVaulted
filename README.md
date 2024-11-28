# HyperVaulted

HyperVaulted is a minimalistic web server project designed for educational purposes as part of the 42 Network curriculum. It introduces core concepts of network programming HTTP protocol handling and concurrency. This project involves building a functional web server from scratch in C capable of handling HTTP requests and serving static files efficiently.

  
  
## 🧐 Features

Here're some of the project's best features:

- HTTP/1.1 Support: Handles basic GET POST and DELETE requests.
- Static File Serving: Delivers HTML and other static files from a root directory.
- Error Handling: Generates proper HTTP status codes for common scenarios (404 500 etc.).
- Concurrency: Supports handling multiple simultaneous connections using select or threading (implementation varies).
- Custom Configurations: Allows setting the port root directory and default files through a configuration file.

## 🛠️ Installation Steps

First, Clone the Repository:

```
git clone https://github.com/xTyranT/HyperVaulted.git
```

Change Directory

```
cd HyperVaulted
```

Build the Project

```
make
```
## 🖥️ Usage

Once you’ve set up and built the **HyperVaulted** project, run the command below:

---

### 1. Start the Server
Run the server by specifying the configuration file:
```bash
./HyperVaulted ./conf/HyperVaulted.conf
```

### 2. Open a browser and navigate to:
Run the server by specifying the configuration file:
```bash
./HyperVaulted ./conf/HyperVaulted.conf
```

### 2. Access the Server
You can access the server using your browser or tools like `curl`:

- **In a browser**: Open the address `http://localhost:8080` to see the server’s response.

## 🧠 Concepts Behind HyperVaulted

The **HyperVaulted** project is an HTTP web server built from scratch, inspired by Nginx and Apache, and adheres to the principles of modern web architecture. Here are the core concepts it covers:

### 1. **HTTP Protocol**
- Implements the **HTTP/1.1 protocol** to handle client requests and server responses.
- Supports essential HTTP methods such as `GET`, `POST`, and `DELETE`.

### 2. **Request/Response Handling**
- Parses incoming HTTP requests to extract headers, methods, and body.
- Generates and sends appropriate HTTP responses, including status codes and headers.

### 3. **Routing**
- Routes requests based on the configuration file, mapping URIs to specific resources or CGI scripts.

### 4. **Static and Dynamic Content**
- Serves **static files** (HTML, images, videos) from a specified directory.
- Executes **CGI scripts** (e.g., PHP, Python) for dynamic content generation.

### 5. **Error Management**
- Custom error pages for common HTTP errors (e.g., `404 Not Found`, `500 Internal Server Error`).

### 6. **Connection Management**
- Handles multiple simultaneous client connections using **multiplexing** or non-blocking I/O techniques.

### 7. **Configuration**
- Flexible configuration through a custom `.conf` file that defines:
  - Listening ports
  - Server root directories
  - Error pages
  - CGI handlers

### 8. **Modular Design**
- Designed with modularity and scalability in mind, allowing easy addition of new features or enhancements.

---

These concepts ensure that **HyperVaulted** functions as a lightweight and efficient server while providing essential features for modern web applications.

## 👥 Contributors

This project is developed and maintained by:

- [TyranT](https://github.com/xTyranT)
- [Yahamdan](https://github.com/yahamdan)

---
Thank you for checking out **Webserv**! Happy coding! ✨


