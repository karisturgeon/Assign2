#include "process_request.h"
#include "lower_filter.h"
#include "null_filter.h"
#include "upper_filter.h"
#include <fcntl.h>
#include <pthread.h>
#include <signal.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <unistd.h>

_Noreturn void *process_request(void *arg)
{
    client_data_t *client_data = (client_data_t *)arg;
    int            fd1         = client_data->fd1;
    int            fd2         = client_data->fd2;

    uint8_t size;
    uint8_t filter_size;

    // Read filter size
    if(read(fd1, &filter_size, sizeof(uint8_t)) > 0)
    {
        char filter_name[UINT8_MAX + 1];
        void (*filter_func)(char *) = NULL;

        // Read filter name
        if(read(fd1, filter_name, filter_size) != filter_size)
        {
            perror("Error reading filter name");
        }
        else
        {
            filter_name[filter_size] = '\0';

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
            }

            if(filter_func != NULL)
            {
                // Read message size
                if(read(fd1, &size, sizeof(uint8_t)) > 0)
                {
                    char word[UINT8_MAX + 1];

                    // Read the actual message
                    if(read(fd1, word, size) != size)
                    {
                        perror("Error reading message");
                    }
                    else
                    {
                        word[size] = '\0';

                        // Apply the filter function to each character
                        for(int i = 0; i < size; i++)
                        {
                            filter_func(&word[i]);
                        }

                        // Send back the transformed message
                        if(write(fd2, &size, sizeof(uint8_t)) != sizeof(uint8_t) || write(fd2, word, size) != size)
                        {
                            perror("Error writing transformed message");
                        }
                    }
                }
            }
            else
            {
                fprintf(stderr, "Error: No valid filter function was selected.\n");
            }
        }
    }

    close(fd1);    // Close FIFO1 for this client
    close(fd2);    // Close FIFO2 for this client

    free(client_data);     // Free the allocated memory for client data
    pthread_exit(NULL);    // End the thread
}
