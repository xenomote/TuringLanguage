#include <stdbool.h>

#define CONDITIONAL false;
#define OPERATION true;

typedef struct program {
    struct group* groups;
    struct statement* statements;
    struct block* blocks;
} Program;

typedef struct group {
    char* name;
    char* symbols;
    char* marked_symbols;
} Group;

typedef struct block {
    char* name;
    struct statement* statements;
} Block;

typedef struct statement {
    bool type;
    union body* body;
} Statement;

union body {
    struct conditional* conditional;
    struct operation* operation;
};

typedef struct conditional {
    struct condition* condition;
    struct statement* success;
    struct statement* failure;
} Conditional;

typedef struct condition {
    bool marked;
    bool unmarked;
    char* symbols;
    char* marked_symbols;
} Condition;

typedef struct operation {
    struct write* write;
    struct travel* travel;
    struct statement* next;
    struct block* transition;
} Operation;

typedef struct write {
    char action;
    char* string;
    bool reversed;
    int repetition;
} Write;

typedef struct travel {
    bool direction;
    int repetition;
    struct condition* until;
} Travel;