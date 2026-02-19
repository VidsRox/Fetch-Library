
## Architecture Overview


```
Application Layer (grab.c)
         ↓
    HTTP Layer (http.c, fetch.c)
         ↓
   Network Layer (network.c)
         ↓
    System Calls (Linux kernel)
```

---

# Architecture Documentation

## Design Principles

**Layered Architecture:** Each layer has a single responsibility and only depends on layers below it.

```
grab.c (Application)    ← User-facing API
fetch.c (Fetch Logic)   ← Orchestrates HTTP requests
http.c (Protocol)       ← Builds/parses HTTP messages
network.c (Transport)   ← TCP socket operations
utils.c (Utilities)     ← Helper functions
```

## Key Design Decisions

### 1. Modular File Structure
- **Why:** Separation of concerns, reusability, testability
- **Benefit:** Each component can be modified/tested independently

### 2. Direct Syscalls (Optional)
- **Why:** Understand low-level operations, reduce dependencies
- **Trade-off:** More code, but educational and smaller binary

### 3. Struct-Based API
```c
Request → build_http_request() → TCP bytes → parse_http_response() → Response
```
- **Why:** Type-safe, matches HTTP structure, easy to validate
- **Alternative rejected:** Raw strings (error-prone, inflexible)

### 4. Byte Order Conversion
- **Why:** Network byte order (big-endian) ≠ CPU byte order (little-endian)
- **Functions:** `htons()` for ports, `inet_addr()` for IP addresses

### 5. Heap-Allocated Responses
```c
Response* fetch_sync(Request *req);
```
- **Why:** Response data must outlive function scope
- **Requirement:** Caller must free with `free_response()`

### 6. Socket Workflow
```c
socket() → connect() → send() → recv() → close()
```
- **Why:** Standard TCP pattern, allows connection reuse, explicit resource management

### 7. String Building/Parsing
- **HTTP is text-based:** Sockets send/receive bytes, not structs
- **Conversion required:** Struct ↔ formatted text string




### **Visual Representation of math in 'inet_addr' function in utils.c:**
```
32-bit integer breakdown:

Byte 3      Byte 2      Byte 1      Byte 0
[192    ]   [168    ]   [1      ]   [1      ]
 C0          A8          01          01
 ↑           ↑           ↑           ↑
 shift=24    shift=16    shift=8     shift=0
```
---

## Why This Format?

### **IP Addresses Are Just 32-Bit Numbers**

The Internet Protocol defines addresses as **32-bit integers**, not strings!
```
Human-readable:  "192.168.1.1"
Computer format:  0xC0A80101  (3232235777 in decimal)

---


```


### **Memory Layout Visualization for 'Sockaddr' struct in network.h**
```
Offset   Field        Bytes   Example Value
------   -----        -----   -------------
0-1      family       2       0x0002 (AF_INET)
2-3      port         2       0x0050 (port 80 in network order)
4-7      addr         4       0x8EFAB664 (142.250.182.100)
8-15     padding      8       0x0000000000000000

Total: 16 bytes

```
### Design for Concurrency

Here's what you need to build:
```
┌─────────────────────────────────────┐
│     User Application                │
│  fetch_async(url1, callback1)       │
│  fetch_async(url2, callback2)       │
│  fetch_async(url3, callback3)       │
└──────────────┬──────────────────────┘
               │
┌──────────────▼──────────────────────┐
│     Thread Pool (4 worker threads)  │
│  ┌────────┐ ┌────────┐ ┌────────┐  │
│  │Worker 1│ │Worker 2│ │Worker 3│  │
│  └───┬────┘ └───┬────┘ └───┬────┘  │
└──────┼─────────┼─────────┼─────────┘
       │         │         │
┌──────▼─────────▼─────────▼─────────┐
│      Task Queue (with mutex)        │
│  [Task url1] [Task url2] [Task url3]│
└─────────────────────────────────────┘
       │         │         │
       ▼         ▼         ▼
   ┌───────┐ ┌───────┐ ┌───────┐
   │Socket1│ │Socket2│ │Socket3│
   └───────┘ └───────┘ └───────┘