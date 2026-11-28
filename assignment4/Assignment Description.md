
Page
1
of 5
CSC 5250 – Concurrent Programming
Programming Assignment: Concurrent HTTPS Requests
Using OpenSSL
Total Points: 50
Programming Language: C
Platform: Ubuntu Linux
Libraries: OpenSSL, POSIX Threads (pthreads)
Objective
In this assignment, you will modify an existing HTTPS client to send multiple HTTPS requests
concurrently using POSIX threads.
You may use the HTTPS client implementation provided in the textbook as your starting point.
By completing this assignment, you will gain experience in:
• Creating and managing multiple threads.
• Performing concurrent network operations.
• Measuring and comparing sequential and concurrent execution.
• Managing thread-specific resources.
• Using semaphores to control concurrency (graduate students).
Part 1: Modify the HTTPS Client (10 Points)
Modify the textbook's HTTPS client to enable server certificate verification, using Ubuntu’s
trusted CA certificates.
Your program must:
1. Connect to the specified server on port 443.
2. Establish a TLS connection using OpenSSL.
3. Send an HTTP GET request for /.
4. Receive the server's response and display the HTTP status code.
5. Properly close the TLS connection and socket.
Your program should report connection or TLS errors without crashing.
Part 2: Concurrent HTTPS Requests (20 Points)
Modify your program to accept multiple hostnames as command-line arguments.
Example:
./https_client www.example.com www.google.com www.wikipedia.org
Your program must:
1. Create one POSIX thread for each hostname.
2. Each thread must independently establish its own TCP and TLS connection.
3. Each thread must send an HTTPS GET request and receive the response.
4. The main thread must wait for all worker threads to finish using pthread_join().
5. Display the hostname, HTTP status code, and success or failure of each request.
6. Ensure that a failed request does not prevent other threads from completing.
7. Properly release all thread, socket, and OpenSSL connection resources.
Example output:
www.example.com: HTTP 200
www.google.com: HTTP 200
www.wikipedia.org: HTTP 200
All requests completed.
Actual HTTP status codes and the order of the output may vary.
Note: Each thread must use its own SSL connection object. Do not share an active SSL
connection between threads.
Part 3: Performance Comparison (10 Points)
Measure the total execution time required to complete all HTTPS requests.
Implement two execution modes:
• Sequential mode: Process one hostname at a time.
• Concurrent mode: Process all hostnames using separate threads.
Use clock_gettime() with CLOCK_MONOTONIC to measure elapsed time.
Perform the following experiment:
1. Select at least five HTTPS hostnames.
2. Run the program in sequential mode.
3. Run the program in concurrent mode using the same hostnames.
4. Repeat each experiment three times.
5. Record the total execution time in milliseconds.
Trial - Sequential Time (ms) - Concurrent Time (ms)
1
2
3
Average
Answer the following questions:
1. Which execution mode was faster in your experiment?
2. Why can concurrent HTTPS requests finish faster than sequential requests?
3. Does multithreading always guarantee better performance? Explain.
Part 4: Analysis and Submission (10 Points)
Answer the following questions:
1. Why should each thread have its own SSL * connection object?
2. What happens if one HTTPS request fails while the other threads are still running?
3. How do network latency and TLS handshakes affect the execution time?
4. What are two potential disadvantages of creating a large number of threads?
Submission Requirements
Submit the following:
• Source code: Complete C source code for your implementation.
• README: Instructions for compiling and running your program, including how to
select sequential or concurrent mode.
• Screenshots: Screenshots demonstrating sequential and concurrent execution.
• Report: A short PDF containing your performance results, observations, and answers to
the analysis questions.
Your program must compile using:
gcc -Wall -Wextra -pthread https_client.c -o https_client -lssl -lcrypto
Additional Requirement for Graduate Students: Limiting
Concurrent HTTPS Requests
Graduate students must complete this section in addition to Parts 1–4.
In Part 2, your program creates one thread for each HTTPS request. However, allowing too many
requests to run simultaneously may consume excessive system resources.
Modify your concurrent HTTPS client to limit the number of active HTTPS requests using a
POSIX semaphore.
Requirements
1. Accept an additional command-line argument specifying the maximum number of
concurrent HTTPS requests.
Example:
./https_client 3 www.example.com www.google.com www.wikipedia.org
www.gnu.org www.kernel.org
Here, 3 indicates that at most three HTTPS requests may be active simultaneously.
2. Run your program with concurrency limits of 1, 2, and 3 using the same set of at least
five HTTPS requests.
3. Record the total execution time for each configuration.
Trial Limit = 1 (ms) Limit = 2 (ms) Limit = 3 (ms)
1
2
3
Average
Graduate Analysis Questions
1. How does the semaphore limit the number of concurrent HTTPS requests?
2. What happens when a thread calls sem_wait() and no permits are available?
3. Does increasing the concurrency limit always reduce execution time? Explain your
observations.
Include your implementation, performance results, and analysis in the same submission.
Grading Rubric
Undergraduate Students
Component Points
Part 1: Modified HTTPS client 10
Part 2: Concurrent HTTPS requests 20
Part 3: Performance comparison 10
Part 4: Analysis and submission 10
Total 50
Graduate Students
Component Points
Part 1: Modified HTTPS client 7
Part 2: Concurrent HTTPS requests 14
Part 3: Performance comparison 7
Part 4: Analysis and submission 7
Graduate extension: Semaphore implementation 7
Graduate extension: Concurrency limit verification 4
Graduate extension: Performance analysis 4
Total 50
General Guidelines
• You may use the HTTPS client code provided in the textbook as your starting point.
• All concurrency-related modifications must be your own implementation.
• Use proper error handling and release allocated resources.
• Certificate verification must remain enabled. Do not disable it to make failed connections
succeed.
• Performance results may vary depending on network conditions and server response
times. Your analysis should explain the results you observe rather than assume
concurrent execution will always be faster.
• Your program must run on Linux and compile without errors using the specified
command.
