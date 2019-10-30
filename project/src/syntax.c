#include <stdlib.h>

#include "syntax.h"

list_t* push(void* value, list_t* list) {
    list_t* node = malloc(sizeof(list_t));
    
    node -> value = value;
    node -> next = list;

    return node;
}

program_t* program(list_t* groups, statement_t* statements, list_t* blocks) {
    program_t* program = malloc(sizeof(program_t));

    program -> groups = groups;
    program -> statements = statements;
    program -> blocks = blocks;

    return program;
}

group_t* group(char* name, char* symbols, char* marked_symbols) {
    group_t* group = malloc(sizeof(group_t));

    group -> name = name;
    group -> symbols = symbols;
    group -> marked_symbols = marked_symbols;

    return group;
}

block_t* block(char* name, statement_t* statements) {
    block_t* block = malloc(sizeof(block_t));

    block -> name = name;
    block -> statements = statements;

    return block;
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

statement_t* operation(write_t* write, travel_t* travel, statement_t* next, block_t* transition) {
    statement_t* statement = malloc(sizeof(statement_t));
    operation_t* operation = malloc(sizeof(operation_t)); 

    statement -> type = OPERATION;
    statement -> operation = operation;

    operation -> write = write;
    operation -> travel = travel;
    operation -> next = next;
    operation -> transition = transition;

    return statement;
}