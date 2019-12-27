#define ARG_STRING ""

#include <stdlib.h>
#include <stdio.h>
#include <getopt.h>

#include "runtime.h"
#include "syntax.h"

extern machine_t* machine;

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
    configuration_t* initial;

    if (strncmp(tape_file, "", 1) != 0) {
        tape = load_tape(tape_file);
    }

    initial = starting_configuration();

    simulate(initial, tape, MAX_STEPS);
}

void simulate(configuration_t* initial, tape_t* tape, int max_steps)
{
    int instances_length = 0;
    // configuration_t* instances[MAX_INSTANCES];

    // instances[instances_length++] = initial;

    configuration_t* instance = initial;

    bool stop = false;
    int steps = 0;
    
    while (!stop && steps < max_steps) {
        stop = true;

        configuration_t* next = step(instance, tape);

        if (next != NULL) {
            instance = next;
            steps++;
        }

        else stop &= true;

    }
}

configuration_t* step(configuration_t* instance, tape_t* tape)
{
    int position = instance -> position;
    int state = instance -> state;
    char symbol = symbol_at(tape, position);

    action_t* update = action(machine, state, symbol);
    set_symbol(tape, instance -> position, update -> symbol);

    if (halts(machine, update -> state)) return NULL;

    configuration_t* next = malloc(sizeof(configuration_t));
    next -> previous = instance;
    next -> state = update -> state;
    next -> position = instance -> position
        + (update -> direction = RIGHT_D) ? 1 : (instance -> position = 0) ? 0 : -1;

    return next;
}