
#ifndef PROCESS_REQUEST_H
#define PROCESS_REQUEST_H
void *process_request(void *arg);
void  signal_handler(int signal_number);

typedef struct
{
    int fd1;
    int fd2;
} client_data_t;

#endif    // PROCESS_REQUEST_H
