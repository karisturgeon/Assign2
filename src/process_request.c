#include "process_request.h"
#include "lower_filter.h"
#include "null_filter.h"
#include "upper_filter.h"
#include <errno.h>
#include <fcntl.h>
#include <pthread.h>
#include <signal.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <unistd.h>

void *process_request(void *arg)
{
    int client_fd;

    uint8_t size;
    uint8_t filter_size;
    char    filter_name[UINT8_MAX + 1];
    char    word[UINT8_MAX + 1];
    void (*filter_func)(char *) = NULL;
    client_fd                   = *((int *)arg);
    // free(arg);

    // Read filter size
    if(read(client_fd, &filter_size, sizeof(uint8_t)) <= 0)
    {
        errno = 1;
        perror("Error reading filter size");
        goto cleanup;
    }

    //        printf("reading from client.... \n");
    // Read filter name
    if(read(client_fd, filter_name, filter_size) != filter_size)
    {
        printf("Error reading filter name");
        goto cleanup;
    }
    filter_name[filter_size] = '\0';
    //            printf("filter name: %s\n", filter_name);
    // Select the appropriate filter function
    if(strcmp(filter_name, "upper") == 0)
    {
        filter_func = upper_filter;
    }
    else if(strcmp(filter_name, "lower") == 0)
    {
        filter_func = lower_filter;
    }
    else if(strcmp(filter_name, "null") == 0)
    {
        filter_func = null_filter_wrapper;
    }
    else
    {
        fprintf(stderr, "Unknown filter: %s\n", filter_name);
        goto cleanup;
    }

    if(filter_func != NULL)
    {
        // Read message size
        if(read(client_fd, &size, sizeof(uint8_t)) <= 0)
        {
            errno = 1;
            perror("Error reading message size");
            goto cleanup;
        }

        // Read the actual message
        if(read(client_fd, word, size) != size)
        {
            errno = 1;
            perror("Error reading message");
            goto cleanup;
        }

        word[size] = '\0';
        printf("Word: %s, Filter: %s\n", word, filter_name);
        // Apply the filter function to each character
        for(int i = 0; i < size; i++)
        {
            filter_func(&word[i]);
        }
        printf("Transformed Word: %s\n", word);
        // Send back the transformed message
        if(write(client_fd, &size, sizeof(uint8_t)) != sizeof(uint8_t) || write(client_fd, word, size) != size)
        {
            perror("Error writing transformed message");
            goto cleanup;
        }
    }

    else
    {
        fprintf(stderr, "Error: No valid filter function was selected.\n");
        goto cleanup;
    }

cleanup:
    close(client_fd);    // Free the allocated memory for client data
    return NULL;
}
