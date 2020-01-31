#ifndef SYNTAX_H
#define SYNTAX_H

#include <exception>
#include <optional>
#include <variant>
#include <map>
#include <set>

#include "machine.hh"
#include "location.hh"

struct program;
struct conditional;
struct operation;

using group_reference = std::string;
using block_reference = std::string;

using statement = std::variant<operation, conditional, block_reference, result>;
using statement_wrap = std::pair<statement, yy::location>;
using statement_list = std::list<statement_wrap>;

using grouping = std::variant<std::set<symbol>, group_reference, bool>;
using condition = std::set<grouping>;

struct program
{
    statement_list statements;
    std::map<std::string, condition> groups;
    std::map<std::string, statement_list> blocks;
};

struct conditional
{
    std::list<std::pair<condition, statement_list>> conditions;
    std::optional<statement_list> else_condition;
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

// error type

const std::string msg(const std::string message, yy::location location);

class semantic_error : public std::runtime_error 
{
public:
    semantic_error(const std::string message, yy::location location)
    : runtime_error(msg(message, location))
    , message(message)
    , location(location)
    {};


private:
    std::string message;
    yy::location location;
};

// allow anonymous visitor declaration

template<class... Ts> struct visitor : Ts... { using Ts::operator()...; };
template<class... Ts> visitor(Ts...) -> visitor<Ts...>;

// check block exit points

void ensure_exit(const program& p);
void ensure_exit(const statement_list& ss);

void ensure_return(const statement_list& ss);
void ensure_return(const statement_wrap& s);
void ensure_return(const conditional& c);

bool is_exit_point(const statement_list& ss);
bool is_exit_point(const statement_wrap& s);
bool is_exit_point(const conditional& c);

#endif