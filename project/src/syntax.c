#include <stdlib.h>

#include "syntax.h"

Program* program(Group* groups, Statement* statements, Block* blocks) {
    Program* program = malloc(sizeof(Program));

    program -> groups = groups;
    program -> statements = statements;
    program -> blocks = blocks;

    return program;
}

Group* group(char* name, char* symbols, char* marked_symbols) {
    Group* group = malloc(sizeof(Group));

    group -> name = name;
    group -> symbols = symbols;
    group -> marked_symbols = marked_symbols;

    return group;
}

Block* block(char* name, Statement* statements) {
    Block* block = malloc(sizeof(Block));

    block -> name = name;
    block -> statements = statements;

    return block;
}

Statement* conditional(Condition* condition, Statement* success, Statement* failure) {
    Statement* statement = malloc(sizeof(Statement));
    Conditional* conditional = malloc(sizeof(Conditional));

    statement -> type = CONDITIONAL;
    statement -> body -> conditional = conditional;

    conditional -> condition = condition;
    conditional -> success = success;
    conditional -> failure = failure;

    return statement;
}

Statement* operation(Write* write, Travel* travel, Statement* next, Block* transition) {
    Statement* statement = malloc(sizeof(Statement));
    Operation* operation = malloc(sizeof(Operation)); 

    statement -> type = OPERATION;
    statement -> body -> operation = operation;

    operation -> write = write;
    operation -> travel = travel;
    operation -> next = next;
    operation -> transition = transition;

    return statement;
}