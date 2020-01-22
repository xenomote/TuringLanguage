#ifndef SYNTAX_H
#define SYNTAX_H

#include <sstream>

#include "machine.hh"

class Generator;

struct Statement;
struct Conditional;
struct Operation;
struct Write;
struct Travel;
struct Condition;
struct Symbol;

class Generator {
public:
    virtual ~Generator() {};

    virtual void generate(Statement& statement) = 0;
    virtual void generate(Conditional& conditional) = 0;
    virtual void generate(Operation& operation) = 0;
    virtual void generate(Write& write) = 0;
    virtual void generate(Travel& travel) = 0;
    virtual void generate(Condition& condition) = 0;
    virtual void generate(Symbol& symbol) = 0;
};

////////////////////////////////////////////////////////////////////////////////

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

struct Operation {
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
    Direction direction;
    int repetition;
    Condition* until;
};

////////////////////////////////////////////////////////////////////////////////

class StateGenerator : public Generator
{

};

class StringGenerator : public Generator {
public:
    void generate(Statement& statement);
    void generate(Conditional& conditional);
    void generate(Operation& operation);
    void generate(Write& write);
    void generate(Travel& travel);
    void generate(Condition& condition);
    void generate(Symbol& symbol);

    std::string output();

    StringGenerator();

private:
    std::stringstream buffer;
    int indent;
};

#endif