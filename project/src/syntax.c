#include <stdlib.h>

#include "syntax.h"


list_t* push(char* name, void* value, list_t* list) {
    list_t* node = malloc(sizeof(list_t));
    
    node -> name = name;
    node -> value = value;
    node -> next = list;

    return node;
}

void* find(char* name, list_t* list) {
    while (list != NULL) {
        if (list -> name == name) return list -> value;
        list = list -> next;
    }

    return NULL;
}

program_t* program(list_t* groups, statement_t* statements, list_t* blocks) {
    program_t* program = malloc(sizeof(program_t));

    program -> groups = groups;
    program -> statements = statements;
    program -> blocks = blocks;

    return program;
}

statement_t* accept() {
    statement_t* accept = malloc(sizeof(statement_t));

    accept -> type = ACCEPT;

    return accept;
}

statement_t* reject() {
    statement_t* reject = malloc(sizeof(statement_t));

    reject -> type = REJECT;

    return reject;
}

statement_t* conditional(condition_t* condition, statement_t* success, statement_t* failure) {
    statement_t* statement = malloc(sizeof(statement_t));
    conditional_t* conditional = malloc(sizeof(conditional_t));

    statement -> type = CONDITIONAL;
    statement -> conditional = conditional;

    conditional -> condition = condition;
    conditional -> success = success;
    conditional -> failure = failure;

    return statement;
}

statement_t* operation(write_t* write, travel_t* travel, statement_t* next) {
    statement_t* statement = malloc(sizeof(statement_t));
    operation_t* operation = malloc(sizeof(operation_t)); 

    statement -> type = OPERATION;
    statement -> operation = operation;

    operation -> write = write;
    operation -> travel = travel;
    operation -> next = next;

    return statement;
}



write_t* mark() {
    write_t* mark = malloc(sizeof(write_t));

    mark -> type = MARK;

    return mark;
}

write_t* unmark() {
    write_t* unmark = malloc(sizeof(write_t));

    unmark -> type = UNMARK;

    return unmark;
}

write_t* writes(char* string, bool reversed, int repetition) {
    write_t* write = malloc(sizeof(write_t));

    write -> type = WRITE;
    write -> string = string;
    write -> reversed = reversed;
    write -> repetition = repetition;

    return write;
}

travel_t* travel(bool direction, int repetition, condition_t* until) {
    travel_t* travel = malloc(sizeof(travel_t));

    travel -> direction = direction;
    travel -> repetition = repetition;
    travel -> until = until;

    return travel;
}