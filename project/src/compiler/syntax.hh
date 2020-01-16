#ifndef SYNTAX_H
#define SYNTAX_H

#include "machine.hh"

class Syntax;
class Generator;

struct Statement;
struct Conditional;
struct Operation;

struct Write;
struct Travel;
struct Condition;
struct Symbol;

class Syntax {
public:
    virtual ~Syntax() {};
    
    virtual void accept(Generator& generator) = 0;
};

class Generator {
public:
    virtual ~Generator() {};

    virtual void generate(Statement& statement) = 0;
    virtual void generate(Conditional& conditional) = 0;
    virtual void generate(Operation& operation) = 0;
};

////////////////////////////////////////////////////////////////////////////////

class Statement : public Syntax {
    enum struct type {
        accept,
        reject,
        conditional,
        operation
    } type;

    Conditional* conditional;
    Operation* operation;
};

class Conditional : public Syntax {
    Condition* condition;
    Statement* success;
    Statement* failure;
};

class Operation : public Syntax {
    Write* write;
    Travel* travel;
    Statement** next;
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

////////////////////////////////////////////////////////////////////////////////

class StateGenerator : public Generator
{

};

class StringGenerator : public Generator
{

};

#endif