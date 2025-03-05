# Concurrent Web Server

## References
1. **[Ostep Project](https://github.com/remzi-arpacidusseau/ostep-projects/tree/master/concurrency-webserver)**
2. **[Thread pool](https://nachtimwald.com/2019/04/12/thread-pool-in-c/)**
3. **[Beej's Guide to Network Programming in C](https://beej.us/guide/bgnet/html/#intro)**

## Walkthrough
This server is based on the Ostep web server core project. It serves as an application of Operating Systems concepts, particularly in multi-threading and scheduling algorithms. The project builds upon concepts discussed in the Ostep book's concurrency chapters, which are highly recommended for anyone new to multi-threading.

After studying network programming in C and implementing a thread pool based on the references above, I transformed the basic web server into a concurrent one. Additionally, I incorporated scheduling algorithms using two custom-built data structures:
- **Min-heap** for Shortest File First (SFF) scheduling
- **Linked list** for First In, First Out (FIFO) scheduling

## Features
- **Multi-threaded architecture:** Built using POSIX threads to efficiently handle multiple simultaneous HTTP requests.
- **Thread Pool Implementation:** Designed to enhance concurrency and resource utilization.
- **Producer-Consumer Synchronization:** Ensures smooth request handling, preventing deadlocks.
- **Custom Data Structures:** Implemented a min-heap and linked list to support scheduling policies.
- **Scheduling Algorithms:** Supports FIFO and SFF request scheduling.
- **Security Enforcement:** Restricts file access within a controlled directory hierarchy.
- **Performance Testing:** Evaluated using a custom batch script and Apache Benchmark (`ab`) under different traffic conditions.

The `spin.cgi` script populates the requested file size with dummy data (letter 'A') to reach the specified size.

### Observations
- **FIFO Scheduling:** Initially tested by only modifying the number of worker threads. The response times significantly improved from **1.5 seconds** for both file sizes to **0.24 seconds**.
- **SFF Scheduling:** When switching to SFF, the **512KB requests completed noticeably faster**, demonstrating the effectiveness of the scheduling algorithm.
- **Thread Count Impact:** Increasing the number of threads extensively resulted in performance degradation due to thread overhead.

[difference between SFF and FIFO at load test](https://github.com/user-attachments/assets/87eaa721-bdc0-40ea-a707-1bebef764852)

## Code Structure
- **Thread Pool Implementation:** `tpool.c`, `tpool.h`
- **Scheduling Algorithms:** 
  - `schedulers.c`
  - `linkedlist.c`, `linkedlist.h`
  - `heap.c`, `heap.h`

## Setup and Usage
### Prerequisites
- GCC Compiler
- POSIX-compliant environment (Linux/MacOS recommended)

### Compilation
```sh
make
```

### Running the Server
The following command was used to run the server:
```sh
./wserver -d . -p 10000 -t 5 -s FIFO
```
- `<d>`: the root directory in which users can access files.
- `<port>`: Port number for the server to listen on.
- `<threads>`: the number of threads in the threading pool.
- `<scheduling_policy>`: Choose between `FIFO` or `SFF`.

### Testing
To simulate load testing, you can use Apache Benchmark which would only show you the effect of the mutlithreading:
```sh
ab -n 1000 -c 50 http://localhost:<port>/index.html
```

## Testing and Performance Evaluation to test both multi-threading and scheduling featuers
A bash script was used to simulate load testing by making 1000 simultaneous requests with background workers. The requests were divided into two types:
- `512KB` requests using `/spin.cgi?size=512`
- `1024KB` requests using `/spin.cgi?size=1024`

## Contact
For any inquiries or contributions, feel free to reach out or submit a pull request!

