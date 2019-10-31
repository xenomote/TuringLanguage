#ifndef SYNTAX_H
#define SYNTAX_H

#include <stdbool.h>

#define LEFT false;
#define RIGHT true;

typedef struct list {
    char* name;
    void* value;
    struct list* next;
} list_t;

typedef struct program {
    struct list* groups;
    struct statement* statements;
    struct list* blocks;
} program_t;

typedef enum statement_type {
    ACCEPT,
    REJECT,
    CONDITIONAL,
    OPERATION
} statement_type;

typedef struct statement {
    statement_type type;
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

typedef enum condition_type {
    MARKED,
    UNMARKED,
    UNMARKED_SYMBOL,
    MARKED_SYMBOL
} condition_type;

typedef struct condition {
    condition_type type;
    union {
        char* symbols;
        struct {
            struct condition* left;
            struct condition* right;
        };
    };
} condition_t;

typedef struct operation {
    struct write* write;
    struct travel* travel;
    struct statement* next;
} operation_t;

typedef enum write_type {
    WRITE,
    MARK,
    UNMARK
} write_type;

typedef struct write {
    write_type type;
    char* string;
    bool reversed;
    int repetition;
} write_t;

typedef struct travel {
    bool direction;
    int repetition;
    struct condition* until;
} travel_t;

list_t* push(char* name, void* value, list_t* list);
void* find(char* name, list_t* list);

condition_t* marked();
condition_t* unmarked();
condition_t* symbol(bool marked, char symbol);
condition_t* join(condition_t* left, condition_t* right);

statement_t* accept();
statement_t* reject();
statement_t* conditional(condition_t* condition, statement_t* success, statement_t* failure);
statement_t* operation(write_t* write, travel_t* travel, statement_t* next);

write_t* mark();
write_t* unmark();
write_t* writes(char* string, bool reversed, int repetition);

travel_t* travel(bool direction, int repetition, condition_t* until);

#endif