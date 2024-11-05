#include "process_request.h"
#include <arpa/inet.h>
#include <errno.h>
#include <fcntl.h>
#include <netinet/in.h>
#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>

#define PORT 8080    // Server port
#define BACKLOG 5    // Number of allowed pending connections#define BUFFER_SIZE 1024

int main(void)
{
    int                     server_fd;
    int                     client_fd;
    struct sockaddr_in      server_addr;
    struct sockaddr_storage client_addr;
    struct sigaction        sa;
    socklen_t               client_addr_len = sizeof(client_addr);
#if defined(__clang__)
    #pragma clang diagnostic push
    #pragma clang diagnostic ignored "-Wdisabled-macro-expansion"
#endif
    memset(&sa, 0, sizeof(sa));
    sa.sa_handler = signal_handler;
#if defined(__clang__)
    #pragma clang diagnostic pop
#endif

    sigemptyset(&sa.sa_mask);
    signal(SIGINT, signal_handler);

    // Create the server socket

    server_fd = socket(AF_INET, SOCK_STREAM, 0);    // NOLINT(android-cloexec-socket)

    if(server_fd < 0)
    {
        errno = 1;
        perror("Socket creation failed");
        exit(EXIT_FAILURE);
    }

    memset(&server_addr, 0, sizeof(server_addr));
    server_addr.sin_family      = AF_INET;
    server_addr.sin_addr.s_addr = INADDR_ANY;    // Bind to any available interface
    server_addr.sin_port        = htons(PORT);

    // Bind the socket
    if(bind(server_fd, (struct sockaddr *)&server_addr, sizeof(server_addr)) < 0)
    {
        errno = 1;
        perror("Bind failed");
        close(server_fd);
        exit(EXIT_FAILURE);
    }

    // Listen for incoming connections
    if(listen(server_fd, BACKLOG) < 0)
    {
        errno = 1;
        perror("Listen failed");
        close(server_fd);
        exit(EXIT_FAILURE);
    }

    printf("Server is listening on port %d...\n", PORT);

    while(1)
    {
        pid_t pid;
        int   client_fd_copy;
        // Accept a client connection
        client_fd = accept(server_fd, (struct sockaddr *)&client_addr, &client_addr_len);
        if(client_fd < 0)
        {
            perror("Accept failed");
            continue;
        }

        // Create a new process to handle the client

        pid = fork();
        if(pid < 0)
        {
            perror("Fork failed");
            close(client_fd);
        }
        else if(pid == 0)
        {
            // Child process
            close(server_fd);
            client_fd_copy = client_fd;
            process_request(&client_fd_copy);
            close(client_fd);
            exit(EXIT_SUCCESS);
        }
        else
        {
            close(client_fd);
        }
    }
}

void signal_handler(int signal_number)
{
    if(signal_number == SIGINT)
    {
        _exit(EXIT_SUCCESS);
    }
}
