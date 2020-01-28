#ifndef SYNTAX_H
#define SYNTAX_H

#include <optional>
#include <variant>
#include <map>
#include <set>

#include "machine.hh"

struct program;
struct condition;
struct conditional;
struct operation;

using reference = std::string;
using statement = std::variant<operation, conditional, reference, result>;

struct program
{
    std::list<statement> statements;
    std::map<std::string, condition> groups;
    std::map<std::string, std::list<statement>> blocks;
};

struct condition
{
    std::set<symbol> symbols;
};

struct conditional
{
    std::map<condition, std::list<statement>> conditions;
};

struct while_loop   {condition condition;};
struct until_loop   {condition condition;};
struct repetition   {int repetitions;};

using modifier = std::variant<while_loop, until_loop, repetition>;

struct operation
{
    direction direction;
    std::optional<symbol> write;
    std::list<modifier> modifiers;
};

#endif