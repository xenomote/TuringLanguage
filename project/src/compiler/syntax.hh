#ifndef SYNTAX_H
#define SYNTAX_H

#include <optional>
#include <variant>
#include <map>
#include <set>

#include "machine.hh"

struct program;
struct conditional;
struct operation;

using group_reference = std::string;
using block_reference = std::string;
using statement = std::variant<operation, conditional, block_reference, result>;
using grouping = std::variant<std::set<symbol>, group_reference, bool>;
using condition = std::set<grouping>;

struct program
{
    std::list<statement> statements;
    std::map<std::string, condition> groups;
    std::map<std::string, std::list<statement>> blocks;
};

struct conditional
{
    std::map<condition, std::list<statement>> conditions;
    std::optional<std::list<statement>> else_condition;
};

struct while_loop   {condition predicate;};
struct until_loop   {condition predicate;};

using modifier = std::variant<while_loop, until_loop, int>;
using tape_write = std::variant<symbol, bool>;

struct operation
{
    direction travel;
    std::optional<tape_write> output;
    std::list<modifier> modifiers;
};

#endif