#ifndef RUNTIME_H
#define RUNTIME_H

#include <stdbool.h>

#define TAPE_SIZE 1024
#define MAX_INSTANCES 1024
#define MAX_STEPS 1000000

typedef struct action
{
    bool direction;
    char symbol;
    int state;
} action_t;

typedef struct machine
{
    action_t** transitions
} machine_t;

action_t* action(machine_t* machine, int state, char symbol);

typedef struct tape
{
    int tape_length;
    char tape [TAPE_SIZE];
} tape_t;

tape_t* load_tape(const char* path);
char* symbol_at(tape_t* tape, int position);

typedef struct configuration
{
    int position;
    int state;

    struct configuration* previous;
} configuration_t;

configuration_t* starting_configuration();
void simulate(configuration_t* configuration, tape_t* tape, int max_steps);

#endif