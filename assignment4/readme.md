Assignment 4: Concurrent HTTPS Requests Using OpenSSL

This program modifies the textbook HTTPS client to verify server certificates using Ubuntu's trusted CA certificates and send HTTPS GET requests to multiple hostnames either sequentially or concurrently using pthreads.

Files:

- https_client.c
- Makefile
- readme.md
- analysis.txt

Build:

Using the assignment gcc command:
gcc -Wall -Wextra -pthread https_client.c -o https_client -lssl -lcrypto

Or using Make (outputs to ../bin/https_client):
make

Run:

Concurrent mode (default):
./https_client www.example.com www.google.com www.wikipedia.org
./https_client -c www.example.com www.google.com www.wikipedia.org

Sequential mode:
./https_client -s www.example.com www.google.com www.wikipedia.org

Using Makefile targets:
make run-seq
make run-conc
make run ARGS="-s www.example.com www.google.com"

Example output:
../bin/https_client www.example.com www.google.com www.wikipedia.org
www.example.com: HTTP 200 (Success)
www.google.com: HTTP 200 (Success)
www.wikipedia.org: HTTP 200 (Success)
All requests completed.
Mode: Concurrent | Total execution time: 139.02 ms
