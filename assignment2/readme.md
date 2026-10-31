Assignment 2:

This program takes a hostname as a command line argument and resolves it using gethostbyname().
It prints:

- lookup time in milliseconds
- official hostname
- aliases
- IPv4 addresses

Files:

- getaddr.c
- Makefile
- readme.txt

Build:
make

Run:
../bin/getaddr <'hostname'>

Examples:
../bin/getaddr www.bestbuy.com
../bin/getaddr google.com

Example output:
../bin/getaddr www.bestbuy.com
execution time: 34.380 milliseconds
Official name: user-att-108-235-0-0.e5816.x.akamaiedge.net
Aliases: www.bestbuy.com, www.bestbuy.com.edgekey.net, e5816.x.akamaiedge.net
Addresses: 23.222.21.2
