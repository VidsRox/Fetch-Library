# Fetch Library in C - Built from Scratch

A minimal HTTP client library written in C without using standard networking libraries like libcurl or even libc networking functions. This project uses direct Linux system calls to interact with the kernel, and supports concurrent HTTP requests via a custom thread pool built from scratch.

## About This Project

This is my attempt at building a fetch library from the ground up. I started by coding along with [Dr. Jonas Birch's HTTP fetch library video](https://youtu.be/A8kiEAxo080?si=OuRKisgYsxSAKlWY), but decided to extend it into a complete end-to-end implementation to deeply understand:

- C programming fundamentals
- Pointer manipulation and memory management
- Networking concepts (TCP/IP, sockets, HTTP)
- Low-level systems programming
- Direct system call usage
- Concurrency, threading, and synchronization primitives

Before starting this project, I knew next to nothing about these concepts. Claude AI was instrumental in helping me understand how everything works under the hood - from byte-order conversion to TCP handshakes to race conditions.

While I now have a much better understanding of these concepts than I ever did back in college, I recognize that I'll need to practice with more projects to develop a truly concrete grasp of low-level programming and networking.

## What It Does

The library currently supports:
- ✅ TCP socket creation via direct syscalls
- ✅ HTTP/1.1 request construction
- ✅ HTTP response parsing
- ✅ Manual byte-order conversion (htons/htonl)
- ✅ Zero external dependencies (no libcurl, no standard network libs)
- ✅ Concurrent HTTP requests via thread pool
- ✅ Async API with callbacks and user-defined context
- ✅ Thread-safe task queue with condition variables

## Architecture

```
Application Layer (grab.c)
         ↓
    Async API (fetch_async / fetch_sync)
         ↓
   Thread Pool (thread_pool.c)
         ↓
   Task Queue (task_queue.c)
         ↓
    HTTP Layer (http.c, fetch.c)
         ↓
   Network Layer (network.c)
         ↓
    System Calls (Linux kernel)
```

### Components

| File | Purpose |
|------|---------|
| `grab.c` | Application entry point and demo |
| `fetch.c` | Orchestrates request/response cycle, async API |
| `http.c` | HTTP protocol formatting and parsing |
| `network.c` | Socket operations via syscalls |
| `utils.c` | Helper functions (string ops, byte conversion) |
| `task_queue.c` | Thread-safe circular buffer for pending tasks |
| `thread_pool.c` | Worker threads that execute tasks concurrently |

## Building and Running

```bash
cd src
make
./grab
```

Example output:
```
========================================
Sequential Fetch Performance Test
========================================
Making 10 requests to Google...
Fetching request 1... 2799 bytes in 522.59 ms
...
Total time: 6950.68 ms (6.95 seconds)

========================================
Async Fetch Performance Test
========================================
Making 10 requests to Google...
Async response: 2870 bytes
Async response: 4125 bytes
...
Total async time: 1826.10 ms (1.83 seconds)
```

**3.8x speedup** from concurrent execution with 4 worker threads.

## Concurrency Design

### Task Queue (`task_queue.c`)
A thread-safe circular buffer that holds pending HTTP requests. Uses a mutex to protect the buffer from concurrent modification and a condition variable to put worker threads to sleep when there's no work, rather than spinning.

```
enqueue (main thread) → [T1][T2][T3][  ][  ] → dequeue (worker thread)
                          ↑ tail              ↑ head
```

### Thread Pool (`thread_pool.c`)
A fixed set of worker threads that share one task queue. Each worker loops forever: dequeue a task, execute it, repeat. Workers sleep when the queue is empty and wake up when a task arrives. Shuts down gracefully - workers finish their current task before exiting.

### Async API (`fetch.c`)
`fetch_async` packages a request and callback into a task and submits it to the thread pool, then returns immediately. A worker thread later calls `fetch_sync` and fires the callback with the response.

```c
// Synchronous (blocks until done):
Response* resp = fetch_sync(&req);

// Asynchronous (returns immediately, callback fires later):
fetch_async(&req, my_callback, context);
```

## What I Learned

### Networking Concepts
- **TCP Three-Way Handshake**: How connections are established (SYN, SYN-ACK, ACK)
- **Socket Programming**: Creating, connecting, sending, receiving, closing
- **HTTP Protocol**: Request/response structure, headers, methods, status codes
- **Byte Order**: Network (big-endian) vs host (little-endian) byte order
- **IP Addressing**: Converting dotted-decimal notation to binary format

### Systems Programming
- **System Calls**: Direct kernel interaction via `syscall()`
- **File Descriptors**: How sockets are just file descriptors
- **Memory Management**: Stack vs heap, malloc/free, ownership patterns
- **Pointer Arithmetic**: Walking through buffers, zero-copy techniques
- **String Manipulation**: Building requests, parsing responses without libc

### Concurrency
- **Race Conditions**: Why shared state needs protection
- **Mutex**: Ensuring only one thread modifies shared data at a time
- **Condition Variables**: Sleeping efficiently instead of busy-waiting
- **Thread Pool Pattern**: Reusing threads instead of creating per-request
- **Producer-Consumer**: Main thread produces tasks, workers consume them
- **Ownership**: Who allocates and who frees memory across thread boundaries

### Key Implementation Details

**Thread-safe enqueue:**
```c
void task_queue_enqueue(TaskQueue* q, Task* task) {
    pthread_mutex_lock(&q->lock);
    while (q->size == q->capacity) {
        pthread_cond_wait(&q->not_full, &q->lock);
    }
    q->tasks[q->tail] = task;
    q->tail = (q->tail + 1) % q->capacity;
    q->size++;
    pthread_cond_signal(&q->not_empty);
    pthread_mutex_unlock(&q->lock);
}
```

**Async fetch with callback and context:**
```c
void fetch_async(Request* req, fetch_callback callback, void* context);

// Usage:
void my_callback(Response* resp, void* context) {
    Request* req = (Request*)context;
    printf("Got %d bytes\n", (int)resp->size);
    free_response(resp);
    free(req);
}

fetch_async(request, my_callback, request);
```


---

**Note**: This is a learning project, not production-ready code. It's designed to understand networking and concurrency from first principles, not to replace robust libraries like libcurl.