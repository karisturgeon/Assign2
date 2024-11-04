#include "parse_args.h"
#include <arpa/inet.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#define SERVER_IP "127.0.0.1"    // Change to the server's IP if needed
#define PORT 8080                // Must match the server port

int main(int argc, char *argv[])
{
    int                sock_fd;
    struct sockaddr_in server_addr;
    const char        *input_message = NULL;
    const char        *filter        = NULL;
    uint8_t            filter_size;
    uint8_t            message_size;

    // Parse command-line arguments
    if(parse_args(argc, argv, &input_message, &filter) != EXIT_SUCCESS)
    {
        exit(EXIT_FAILURE);
    }

    // Create a TCP socket
    sock_fd = socket(AF_INET, SOCK_STREAM, 0);
    if(sock_fd < 0)
    {
        perror("Socket creation failed");
        exit(EXIT_FAILURE);
    }

    // Set up the server address struct
    memset(&server_addr, 0, sizeof(server_addr));
    server_addr.sin_family = AF_INET;
    server_addr.sin_port   = htons(PORT);

    if(inet_pton(AF_INET, SERVER_IP, &server_addr.sin_addr) <= 0)
    {
        perror("Invalid address or address not supported");
        close(sock_fd);
        exit(EXIT_FAILURE);
    }

    // Connect to the server
    if(connect(sock_fd, (struct sockaddr *)&server_addr, sizeof(server_addr)) < 0)
    {
        perror("Connection to server failed");
        close(sock_fd);
        exit(EXIT_FAILURE);
    }

    // Send the filter size and filter type
    filter_size = (uint8_t)strlen(filter);
    if(write(sock_fd, &filter_size, sizeof(uint8_t)) != sizeof(uint8_t))
    {
        perror("Error writing filter size to socket");
        close(sock_fd);
        exit(EXIT_FAILURE);
    }
    if(write(sock_fd, filter, filter_size) != filter_size)
    {
        perror("Error writing filter to socket");
        close(sock_fd);
        exit(EXIT_FAILURE);
    }

    // Send the input message size and message
    message_size = (uint8_t)strlen(input_message);
    if(write(sock_fd, &message_size, sizeof(uint8_t)) != sizeof(uint8_t))
    {
        perror("Error writing message size to socket");
        close(sock_fd);
        exit(EXIT_FAILURE);
    }
    if(write(sock_fd, input_message, message_size) != message_size)
    {
        perror("Error writing message to socket");
        close(sock_fd);
        exit(EXIT_FAILURE);
    }

    // Read the transformed message size and content
    if(read(sock_fd, &message_size, sizeof(uint8_t)) > 0)
    {
        char response[UINT8_MAX + 1];
        if(read(sock_fd, response, message_size) != message_size)
        {
            perror("Error reading transformed message");
        }
        else
        {
            response[message_size] = '\0';    // Null-terminate the string
            printf("\nTransformed Message: %s\n", response);
        }
    }
    else
    {
        perror("Error reading message size from server");
    }

    // Clean up and close the socket
    close(sock_fd);
    return 0;
}
