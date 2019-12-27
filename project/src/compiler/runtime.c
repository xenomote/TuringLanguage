#define ARG_STRING ""

#include <stdlib.h>
#include <stdio.h>
#include <getopt.h>

#include "runtime.h"

bool debug = false;

int main(int argc, char** argv)
{
    int opt;
    char* tape_file = "";

    while (opt = getopt(argc, argv, ARG_STRING) > -1) {
        switch (opt) {
            case 't':
                tape_file = argv[optind];
                break;

            case 'd':
                debug = true;
                break;

            default:
                fprintf(stderr, "unrecognised option %c", opt);
                exit(EXIT_FAILURE);
        }
    }

    tape_t* tape;
    history_t* history;
    configuration_t* configuration;

    if (strncmp(tape_file, "", 1) != 0) {
        tape = load_tape(tape_file);
    }

    history = empty_history();

    configuration = starting_configuration(tape, history);

    simulate(configuration);
}

void simulate(configuration_t* configuration)
{
    
}
