#ifndef RUNTIME_H
#define RUNTIME_H

#include <stdbool.h>

#define TAPE_SIZE 1024

typedef struct tape
{
    int tape_length;
    char tape [TAPE_SIZE];
} tape_t;

typedef struct history
{
    int history_length;
    bool history [TAPE_SIZE];
} history_t;


typedef struct configuration
{
    int position;
    int state;

    struct tape* tape;
    struct history* history;
} configuration_t;

tape_t* load_tape(const char* path);
history_t* empty_history();
configuration_t* starting_configuration(tape_t* tape, history_t* history);

#endif