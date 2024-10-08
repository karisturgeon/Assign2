
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

    if(strcmp(*filter, "upper") != 0 && strcmp(*filter, "lower") != 0 && strcmp(*filter, "null") != 0)
    {
        fprintf(stderr, "Error: Invalid filter value '%s'. Excepted 'upper', 'lower', or 'null'\n", *filter);
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
    fprintf(stderr, "Usage: %s -i inputMessage -f filter\n", program_name);
    fputs("Options:\n", stderr);
    fputs("  -h            Display this help message\n", stderr);
    fputs("  -i <String>   Input message (required: \"Hello World\")\n", stderr);
    fputs("  -f <filter>   Filter type (required: 'upper', 'lower', 'null')\n", stderr);
    exit(exit_code);
}
