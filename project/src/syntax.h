#ifndef SYNTAX_H
#define SYNTAX_H

#include <stdbool.h>
#define CONDITIONAL false;
#define OPERATION true;

typedef struct list {
    void* value;
    struct list* next;
} list_t;

typedef struct program {
    struct list* groups;
    struct statement* statements;
    struct list* blocks;
} program_t;


typedef struct group {
    char* name;
    char* symbols;
    char* marked_symbols;
} group_t;

typedef struct block {
    char* name;
    struct statement* statements;
} block_t;

typedef struct statement {
    bool type;
    union {
        struct conditional* conditional;
        struct operation* operation;
    };
} statement_t;

typedef struct conditional {
    struct condition* condition;
    struct statement* success;
    struct statement* failure;
} conditional_t;

typedef struct condition {
    bool marked;
    bool unmarked;
    char* symbols;
    char* marked_symbols;
} condition_t;

typedef struct operation {
    struct write* write;
    struct travel* travel;
    struct statement* next;
    struct block* transition;
} operation_t;

typedef struct write {
    char action;
    char* string;
    bool reversed;
    int repetition;
} write_t;

typedef struct travel {
    bool direction;
    int repetition;
    struct condition* until;
} travel_t;

list_t* push(void* value, list_t* list);
program_t* program(list_t* groups, statement_t* statements, list_t* blocks);
group_t* group(char* name, char* symbols, char* marked_symbols);
block_t* block(char* name, statement_t* statements);
statement_t* conditional(condition_t* condition, statement_t* success, statement_t* failure);
statement_t* operation(write_t* write, travel_t* travel, statement_t* next, block_t* transition);

#endif