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

    Conditional* conditional;
    Operation* operation;
};

struct Conditional {
    Condition* condition;
    Statement* success;
    Statement* failure;
};

struct Condition {
    enum struct type {
        symbols,
        marked,
        unmarked
    } type;

    std::list<Symbol*> symbols;
};

struct Symbol {
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
        write,
        mark,
        unmark
    } type;

    std::string string;
    bool reversed;
    int repetition;
};

struct Travel {
    bool direction;
    int repetition;
    Condition* until;
};

#endif