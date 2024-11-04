
#ifndef PARSE_ARGS_H
#define PARSE_ARGS_H
int            parse_args(int argc, char *argv[], const char **input_message, const char **filter, const char **server_ip, int *port);
_Noreturn void usage(const char *program_name, int exit_code, const char *message);
#endif    // PARSE_ARGS_H
