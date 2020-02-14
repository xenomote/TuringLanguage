#ifndef SEMANTICS_H
#define SEMANTICS_H

#include "syntax.hh"

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

    semantic_error(const std::string message)
    : runtime_error(message)
    {};


private:
    std::string message;
    yy::location location;
};

// check block exit points

void ensure_exit(const program& p);
void ensure_exit(const statement_list& ss);

void ensure_return(const statement_list& ss);
void ensure_return(const statement& s);
void ensure_return(const conditional& c);

bool is_exit_point(const statement_list& ss);
bool is_exit_point(const statement& s);
bool is_exit_point(const conditional& c);

// check if condition overlap

void ensure_distinct_conditions(const program& p);
void ensure_distinct_conditions(const program& p, const statement& s);
void ensure_distinct_conditions(const program& p, const conditional& c);

// check references

void ensure_valid_references(const program& p);
void ensure_valid_references(const program& p, const statement& s);
void ensure_valid_references(const program& p, const operation& o);
void ensure_valid_references(const program& p, const conditional& c);
void ensure_valid_references(const program& p, const condition& c);

template<class T>
bool intersect(const std::set<T>& a, const std::set<T>& b)
{
    for (auto& x : a)
        if (b.count(x) > 0)
            return true;

    return false;
}

#endif