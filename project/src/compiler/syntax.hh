#ifndef SYNTAX_H
#define SYNTAX_H

#include <exception>
#include <optional>
#include <variant>
#include <map>
#include <set>
#include <vector>

#include "machine.hh"
#include "location.hh"

template<class T>
struct syntax
{
    T value;
    yy::location source;

    friend bool operator<(const struct syntax<T>& a, const struct syntax<T>& b)
    {
        return a.value < b.value;
    }
};


struct program;
struct conditional;
struct operation;

using reference = std::string;

using statement = syntax<std::variant<operation, conditional, reference, result>>;
using statement_list = std::vector<statement>;

using grouping = std::variant<symbol, reference, marking>;
using condition = syntax<std::set<grouping>>;

struct program
{
    statement_list statements;
    std::map<std::string, condition> groups;
    std::map<std::string, statement_list> blocks;
};

struct conditional
{
    std::vector<std::pair<condition, statement_list>> conditions;
    std::optional<statement_list> else_condition;
};

struct while_loop {condition predicate;};
struct until_loop {condition predicate;};

using modifier = std::variant<while_loop, until_loop, int>;
using tape_write = std::variant<symbol, bool>;

struct operation
{
    direction travel;
    std::optional<tape_write> output;
    std::vector<modifier> modifiers;
};

// allow anonymous visitor declaration

template<class... Ts> struct visitor : Ts... { using Ts::operator()...; };
template<class... Ts> visitor(Ts...) -> visitor<Ts...>;


#endif