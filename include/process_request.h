
#ifndef PROCESS_REQUEST_H
#define PROCESS_REQUEST_H
_Noreturn void *process_request(void *arg);
void            signal_handler(int signal_number);

typedef struct
{
    int fd;
    int fd2;
} client_data_t;

#endif    // PROCESS_REQUEST_H
