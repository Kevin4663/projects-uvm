#include "chap10.h"
#include <pthread.h>

// Global SSL context and print mutex
SSL_CTX *ctx;
pthread_mutex_t lock;

struct thread_args {
    char *hostname;
};

SOCKET connect_to_host(char *hostname, char *port) {
    struct addrinfo hints;
    memset(&hints, 0, sizeof(hints));
    hints.ai_socktype = SOCK_STREAM;
    struct addrinfo *peer_address;

    if (getaddrinfo(hostname, port, &hints, &peer_address)) {
        return -1;
    }

    SOCKET server;
    server = socket(peer_address->ai_family,
            peer_address->ai_socktype, peer_address->ai_protocol);
    if (!ISVALIDSOCKET(server)) {
        freeaddrinfo(peer_address);
        return -1;
    }

    if (connect(server,
                peer_address->ai_addr, peer_address->ai_addrlen)) {
        CLOSESOCKET(server);
        freeaddrinfo(peer_address);
        return -1;
    }
    freeaddrinfo(peer_address);

    return server;
}

void send_request(SSL *s, char *hostname, char *port, char *path) {
    char buffer[2048];

    sprintf(buffer, "GET /%s HTTP/1.1\r\n", path);
    sprintf(buffer + strlen(buffer), "Host: %s:%s\r\n", hostname, port);
    sprintf(buffer + strlen(buffer), "Connection: close\r\n");
    sprintf(buffer + strlen(buffer), "User-Agent: https_client\r\n");
    sprintf(buffer + strlen(buffer), "\r\n");

    SSL_write(s, buffer, strlen(buffer));
}

void *do_https_request(void *arg) {
    struct thread_args *args = (struct thread_args *)arg;
    char *hostname = args->hostname;
    char *port = "443";

    // 1. Connect to server on port 443
    SOCKET server = connect_to_host(hostname, port);
    if (!ISVALIDSOCKET(server)) {
        pthread_mutex_lock(&lock);
        printf("%s: Connection failed (Failed)\n", hostname);
        pthread_mutex_unlock(&lock);
        return NULL;
    }

    // 2. Establish TLS connection using OpenSSL
    // Each thread gets its own SSL * object
    SSL *ssl = SSL_new(ctx);
    if (!ssl) {
        pthread_mutex_lock(&lock);
        printf("%s: SSL_new() failed (Failed)\n", hostname);
        pthread_mutex_unlock(&lock);
        CLOSESOCKET(server);
        return NULL;
    }

    // Set SNI and hostname verification
    if (!SSL_set_tlsext_host_name(ssl, hostname)) {
        pthread_mutex_lock(&lock);
        fprintf(stderr, "%s: SSL_set_tlsext_host_name() failed.\n", hostname);
        pthread_mutex_unlock(&lock);
    }
    SSL_set1_host(ssl, hostname);

    SSL_set_fd(ssl, server);
    if (SSL_connect(ssl) == -1) {
        long verify_res = SSL_get_verify_result(ssl);
        pthread_mutex_lock(&lock);
        if (verify_res != X509_V_OK) {
            printf("%s: Certificate verification error: %s (Failed)\n",
                    hostname, X509_verify_cert_error_string(verify_res));
        } else {
            printf("%s: SSL_connect() failed (Failed)\n", hostname);
        }
        pthread_mutex_unlock(&lock);
        SSL_free(ssl);
        CLOSESOCKET(server);
        return NULL;
    }

    // Verify certificate
    X509 *cert = SSL_get_peer_certificate(ssl);
    if (!cert) {
        pthread_mutex_lock(&lock);
        printf("%s: No certificate presented by server (Failed)\n", hostname);
        pthread_mutex_unlock(&lock);
        SSL_shutdown(ssl);
        SSL_free(ssl);
        CLOSESOCKET(server);
        return NULL;
    }
    X509_free(cert);

    long vp = SSL_get_verify_result(ssl);
    if (vp != X509_V_OK) {
        pthread_mutex_lock(&lock);
        printf("%s: Certificate verification failed: %s (Failed)\n",
                hostname, X509_verify_cert_error_string(vp));
        pthread_mutex_unlock(&lock);
        SSL_shutdown(ssl);
        SSL_free(ssl);
        CLOSESOCKET(server);
        return NULL;
    }

    // 3. Send HTTP GET request for /
    send_request(ssl, hostname, port, "");

    // 4. Receive response and parse HTTP status code
    char response[4096];
    int bytes_received = SSL_read(ssl, response, sizeof(response) - 1);
    if (bytes_received <= 0) {
        pthread_mutex_lock(&lock);
        printf("%s: Failed to read response (Failed)\n", hostname);
        pthread_mutex_unlock(&lock);
    } else {
        response[bytes_received] = '\0';

        // Look for HTTP status code in first line (e.g. "HTTP/1.1 200 OK")
        int status_code = 0;
        if (sscanf(response, "HTTP/%*s %d", &status_code) == 1) {
            pthread_mutex_lock(&lock);
            printf("%s: HTTP %d (Success)\n", hostname, status_code);
            pthread_mutex_unlock(&lock);
        } else {
            pthread_mutex_lock(&lock);
            printf("%s: Could not parse HTTP status (Failed)\n", hostname);
            pthread_mutex_unlock(&lock);
        }
    }

    // 5. Properly close TLS connection and socket
    SSL_shutdown(ssl);
    CLOSESOCKET(server);
    SSL_free(ssl);

    return NULL;
}

int main(int argc, char *argv[]) {
    if (argc < 2) {
        fprintf(stderr, "Usage: %s [-s|-c] hostname1 [hostname2 ...]\n", argv[0]);
        return 1;
    }

    // Default to concurrent mode (1 = concurrent, 0 = sequential)
    int concurrent_mode = 1;
    int start_idx = 1;

    if (strcmp(argv[1], "-s") == 0) {
        concurrent_mode = 0;
        start_idx = 2;
    } else if (strcmp(argv[1], "-c") == 0) {
        concurrent_mode = 1;
        start_idx = 2;
    }

    if (start_idx >= argc) {
        fprintf(stderr, "Error: Please provide at least one hostname.\n");
        fprintf(stderr, "Usage: %s [-s|-c] hostname1 [hostname2 ...]\n", argv[0]);
        return 1;
    }

    // Initialize OpenSSL
    SSL_library_init();
    OpenSSL_add_all_algorithms();
    SSL_load_error_strings();

    ctx = SSL_CTX_new(TLS_client_method());
    if (!ctx) {
        fprintf(stderr, "SSL_CTX_new() failed.\n");
        return 1;
    }

    // Enable server certificate verification using Ubuntu's trusted CA certificates
    SSL_CTX_set_verify(ctx, SSL_VERIFY_PEER, NULL);
    if (!SSL_CTX_set_default_verify_paths(ctx)) {
        fprintf(stderr, "Failed to load trusted CA certificates.\n");
        ERR_print_errors_fp(stderr);
        SSL_CTX_free(ctx);
        return 1;
    }

    pthread_mutex_init(&lock, NULL);

    int num_hosts = argc - start_idx;
    pthread_t threads[num_hosts];
    struct thread_args args[num_hosts];

    // Start timer using CLOCK_MONOTONIC
    struct timespec start_time, end_time;
    clock_gettime(CLOCK_MONOTONIC, &start_time);

    if (concurrent_mode) {
        // Concurrent mode: create one thread per hostname
        for (int i = 0; i < num_hosts; i++) {
            args[i].hostname = argv[start_idx + i];
            if (pthread_create(&threads[i], NULL, do_https_request, &args[i]) != 0) {
                fprintf(stderr, "Error creating thread for %s\n", args[i].hostname);
            }
        }

        // Wait for all worker threads to finish
        for (int i = 0; i < num_hosts; i++) {
            pthread_join(threads[i], NULL);
        }
    } else {
        // Sequential mode: run one hostname at a time
        for (int i = 0; i < num_hosts; i++) {
            args[i].hostname = argv[start_idx + i];
            do_https_request(&args[i]);
        }
    }

    clock_gettime(CLOCK_MONOTONIC, &end_time);

    // Calculate elapsed time in milliseconds
    double elapsed_ms = (end_time.tv_sec - start_time.tv_sec) * 1000.0 +
                        (end_time.tv_nsec - start_time.tv_nsec) / 1000000.0;

    printf("All requests completed.\n");
    if (concurrent_mode) {
        printf("Mode: Concurrent | Total execution time: %.2f ms\n", elapsed_ms);
    } else {
        printf("Mode: Sequential | Total execution time: %.2f ms\n", elapsed_ms);
    }

    // Clean up resources
    pthread_mutex_destroy(&lock);
    SSL_CTX_free(ctx);

    return 0;
}
