#ifndef SYNTAX_H
#define SYNTAX_H

#include "runtime/machine.hh"

struct Statement;
struct Conditional;
struct Condition;
struct Symbol;
struct Operation;
struct Write;
struct Travel;

struct Statement {
    enum struct type {
        accept,
        reject,
        conditional,
        operation
    } type;

    union {
        Conditional* conditional;
        Operation* operation;
    };
};

struct Conditional {
    Condition* condition;
    Statement* success;
    Statement* failure;
};

struct Condition {};
struct Marked : Condition {};
struct Unmarked : Condition {};
struct Symbol : Condition {
    enum struct type {
        marked,
        unmarked
    } type;

    char symbol;
};

struct Operation {
    Write* write;
    Travel* travel;
    Statement** next;
};

struct Write {
    enum struct type {
        replace,
        mark,
        unmark
    } type;

    char* string;
    bool reversed;
    int repetition;
};

struct Travel {
    bool direction;
    int repetition;
    Condition* until;
};

#endif