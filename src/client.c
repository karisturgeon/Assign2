

#include "client.h"
#include <fcntl.h>
#include <parse_args.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#define FIFO_FILE1 "./fifo1"
#define FIFO_FILE2 "./fifo2"

int main(int argc, char *argv[])
{
    return client(argc, argv);
}

int client(int argc, char *argv[])
{
    int         fd1;
    int         fd2;
    uint8_t     size;
    const char *input_message = NULL;
    const char *filter        = NULL;
    uint8_t     filter_size;

    if(parse_args(argc, argv, &input_message, &filter) != EXIT_SUCCESS)
    {
        // parse_args already prints error messages
        exit(EXIT_FAILURE);
    }
    if(filter == NULL)
    {
        fprintf(stderr, "Error: Filter is NULL.\n");
        exit(EXIT_FAILURE);
    }

    filter_size = (uint8_t)strlen(filter);

    // Open the FIFOs for writing
    fd1 = open(FIFO_FILE1, O_WRONLY | O_CLOEXEC);
    fd2 = open(FIFO_FILE2, O_RDONLY | O_CLOEXEC);

    if(fd1 == -1)
    {
        perror("Client: Error opening FIFO1");
        goto cleanup;
    }
    if(fd2 == -1)
    {
        perror("Client: Error opening FIFO2");
        goto cleanup;
    }

    if(write(fd1, &filter_size, sizeof(uint8_t)) != sizeof(uint8_t))
    {
        perror("Error writing filter size to FIFO");
        goto cleanup;
    }
    if(write(fd1, filter, filter_size) != filter_size)
    {
        perror("Error writing filter to FIFO");
        goto cleanup;
    }
    // Calculate the message size
    size = (uint8_t)strlen(input_message);    // Size of the message (excluding null terminator)
    if(size == 0)
    {
        printf("Cannot have empty input_message\n");
        goto cleanup;
    }
    // Write the size of the message first
    if(write(fd1, &size, sizeof(uint8_t)) != sizeof(uint8_t))
    {
        perror("Error writing size to FIFO");
        goto cleanup;
    }

    // Write the actual message to the FIFO
    if(write(fd1, input_message, size) != size)    // Write the exact message size without the null terminator
    {
        perror("Error writing message to FIFO");
        goto cleanup;
    }
    if(read(fd2, &size, sizeof(uint8_t)) > 0)
    {
        char word[UINT8_MAX + 1];
        if(read(fd2, word, size) != size)
        {
            perror("Error reading transformed message");
        }
        else
        {
            if(strcmp(word, "Server is shutting down...\n") == 0)
            {
                printf("%sServer has shut down. Exiting...\n", word);
                goto cleanup;
            }
            word[size] = '\0';
            printf("\n%s\n", word);
        }
    }
cleanup:
    if(fd1 != -1)
    {
        close(fd1);    // Close FIFO1 if it's open
    }
    if(fd2 != -1)
    {
        close(fd2);    // Close FIFO2 if it's open
    }
    return 0;
}
