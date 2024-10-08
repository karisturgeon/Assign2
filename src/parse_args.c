
#include "../include/parse_args.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

int parse_args(int argc, char *argv[], const char **input_message, const char **filter)
{
    int opt;

    while((opt = getopt(argc, argv, ":i:f:h")) != -1)
    {
        switch(opt)
        {
            case 'i':
                *input_message = optarg;
                break;
            case 'f':
                *filter = optarg;
                //                printf("Filter: %s\n", filter);
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

    if(*input_message == NULL || *filter == NULL)
    {
        fprintf(stderr, "Error: Missing required arguments.\n");
        return EXIT_FAILURE;
    }
    handle_arguments(argv[0], *input_message, *filter);

    return EXIT_SUCCESS;
}

void handle_arguments(const char *binary_name, const char *input_message, const char *filter)
{
    // Check if any required argument is missing
    if(input_message == NULL)
    {
        usage(binary_name, EXIT_FAILURE, "Missing required argument: -i input message.");
    }
    if(filter == NULL)
    {
        usage(binary_name, EXIT_FAILURE, "Missing required argument: -f filter.");
    }
}

_Noreturn void usage(const char *program_name, int exit_code, const char *message)
{
    if(message)
    {
        fprintf(stderr, "%s\n", message);
    }

    // Print the usage message
    fprintf(stderr, "Usage: %s -i inputMessage -f filter\n", program_name);
    fputs("Options:\n", stderr);
    fputs("  -h            Display this help message\n", stderr);
    fputs("  -i <String>   Input message (required)\n", stderr);
    fputs("  -f <filter>   Filter type (required: 'upper', 'lower', 'null')\n", stderr);
    exit(exit_code);
}
