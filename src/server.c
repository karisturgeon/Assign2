#include "server.h"
#include "process_request.h"
#include <fcntl.h>
#include <pthread.h>
#include <signal.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdnoreturn.h>
#include <string.h>
#include <sys/stat.h>
#include <unistd.h>

#define FIFO_FILE "./fifo"
#define FIFO_FILE2 "./fifo2"

int main(void)
{
    printf("Starting server...\n");
    return server();
}

noreturn int server(void)
{
    pthread_t        thread;
    const int        signals_to_handle[] = {SIGINT};
    struct sigaction sa;
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

    for(size_t i = 0; i < sizeof(signals_to_handle) / sizeof(signals_to_handle[0]); i++)
    {
        if(sigaction(signals_to_handle[i], &sa, NULL) < 0)
        {
            printf("Failed to set signal handler for signal %d\n", signals_to_handle[i]);
        }
    }

    mkfifo(FIFO_FILE, S_IRUSR | S_IWUSR | S_IRGRP | S_IROTH);
    mkfifo(FIFO_FILE2, S_IRUSR | S_IWUSR | S_IRGRP | S_IROTH);

    while(1)
    {
        int            fd;
        int            fd2;
        client_data_t *client_data;
        fd  = open(FIFO_FILE, O_RDONLY | O_CLOEXEC);
        fd2 = open(FIFO_FILE2, O_WRONLY | O_CLOEXEC);

        if(fd == -1)
        {
            perror("Server: Error opening FIFO1");
            exit(EXIT_FAILURE);
        }
        if(fd2 == -1)
        {
            perror("Server: Error opening FIFO2");
            exit(EXIT_FAILURE);
        }

        client_data = (client_data_t *)malloc(sizeof(client_data_t));
        if(client_data == NULL)
        {
            perror("Error allocating memory for client data");
            close(fd);
            close(fd2);
            continue;
        }
        client_data->fd  = fd;
        client_data->fd2 = fd2;

        if(pthread_create(&thread, NULL, process_request, (void *)client_data) != 0)
        {
            perror("Error creating thread");
            free(client_data);
            close(fd);
            close(fd2);
            continue;
        }
        pthread_detach(thread);
    }
    exit(EXIT_FAILURE);
}

void signal_handler(int signal_number)
{
    if(signal_number == SIGINT)
    {
        printf("Server: Terminating...\n");

        unlink(FIFO_FILE);
        unlink(FIFO_FILE2);
        exit(EXIT_SUCCESS);
    }
}
