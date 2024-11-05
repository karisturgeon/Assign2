
#include "../include/parse_args.h"
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

int parse_args(int argc, char *argv[], const char **input_message, const char **filter, const char **server_ip, int *port)
{
    int   opt;
    char *endptr = NULL;
    while((opt = getopt(argc, argv, ":i:f:a:p:h")) != -1)
    {
        switch(opt)
        {
            case 'i':
                *input_message = optarg;
                break;
            case 'f':
                *filter = optarg;
                break;
            case 'a':
                *server_ip = optarg;
                break;
            case 'p':
                *port = (int)strtol(optarg, &endptr, 0);
                break;

            case 'h':
                usage(argv[0], EXIT_SUCCESS, NULL);
            case ':':    // Handle missing argument case (for options -i, -o, -f)
                fprintf(stderr, "Option -%c requires an argument.\n", optopt);
                return EXIT_FAILURE;
            case '?':
                if(optopt == 'i' || optopt == 'f')
                {
                    fprintf(stderr, "Option -%c requires an argument.\n", optopt);
                }
                else
                {
                    fprintf(stderr, "Unknown option `-%c'.\n", optopt);
                }
                return EXIT_FAILURE;
            default:
                return EXIT_FAILURE;
        }
    }

    if(*input_message == NULL)
    {
        fprintf(stderr, "Error: Missing required arguments (INPUT MESSAGE).\n\n");
        usage(argv[0], EXIT_FAILURE, NULL);
    }
    if((uint8_t)strlen(*input_message) == 0)
    {
        fprintf(stderr, "Error: Input message cannot be empty.\n\n");
        return EXIT_FAILURE;
    }
    if(*filter == NULL)
    {
        fprintf(stderr, "Error: Missing required arguments (FILTER).\n\n");
        usage(argv[0], EXIT_FAILURE, NULL);
    }
    if(*server_ip == NULL)
    {
        fprintf(stderr, "Error: Missing required arguments (SERVER IP ADDRESS).\n\n");
        usage(argv[0], EXIT_FAILURE, NULL);
    }
    if(port == 0)
    {
        fprintf(stderr, "Error: Missing required arguments (PORT).\n\n");
        usage(argv[0], EXIT_FAILURE, NULL);
    }

    if(strcmp(*filter, "upper") != 0 && strcmp(*filter, "lower") != 0 && strcmp(*filter, "null") != 0)
    {
        fprintf(stderr, "Error: Invalid filter value '%s'. Accepted 'upper', 'lower', or 'null'\n\n", *filter);
        usage(argv[0], EXIT_FAILURE, NULL);
        return EXIT_FAILURE;
    }

    return EXIT_SUCCESS;
}

_Noreturn void usage(const char *program_name, int exit_code, const char *message)
{
    if(message)
    {
        fprintf(stderr, "%s\n", message);
    }

    // Print the usage message
    fprintf(stderr, "Usage: %s -i inputMessage -f filter -a serverIp -p port\n", program_name);
    fputs("Options:\n", stderr);
    fputs("  -h            Display this help message\n", stderr);
    fputs("  -i <String>   Input message (required: \"Hello World\")\n", stderr);
    fputs("  -f <filter>   Filter type (required: 'upper', 'lower', 'null')\n", stderr);
    fputs("  -a <String>   Server IP Address \n", stderr);
    fputs("  -p <Int>      Server Port Number \n\n", stderr);
    exit(exit_code);
}
