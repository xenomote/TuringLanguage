#include <algorithm>
#include <sstream>
#include <string>

#include "syntax.hh"

const std::string msg(const std::string message, yy::location location)
{
    std::stringstream s;
    s << message << " " << location;
    return s.str();
}

// check block exit points

void ensure_exit(const program& p)
{
    ensure_exit(p.statements);

    for (const auto& b : p.blocks)
        ensure_exit(b.second);
}

void ensure_exit(const statement_list& ss)
{
    for (auto s = ss.begin(); s != ss.end(); ++s) {
        if (std::next(s) == ss.end()) {
            if (!is_exit_point(*s))
                throw semantic_error("missing end point", s -> second);
        }

        else ensure_return(*s);
    }
}



void ensure_return(const statement_list& ss)
{
    for (const auto& s : ss) ensure_return(s);
}

void ensure_return(const statement_wrap& s)
{
    std::visit(visitor {
        [s](const auto&)            {throw semantic_error("early exit causes unreachable statements", s.second);},
        [](const conditional& c)    {ensure_return(c);},
        [](const operation&)        {},
    }, s.first);
}

void ensure_return(const conditional& c)
{
    for (const auto& [cond, ss] : c.conditions)
        if (!is_exit_point(ss))
            return;

    if (c.else_condition.has_value())
        ensure_return(c.else_condition.value());
}



bool is_exit_point(const statement_list& ss)
{
    for (auto s = ss.begin(); s != ss.end(); ++s) {
        if (std::next(s) == ss.end())
            return is_exit_point(*s);
            
        else ensure_return(*s);
    }

    return false;
}

bool is_exit_point(const statement_wrap& s)
{
    return std::visit(visitor {
        [](const auto&) -> bool             {return true;},
        [](const operation&) -> bool        {return false;},
        [](const conditional& c) -> bool    {return is_exit_point(c);},
    }, s.first);
}

bool is_exit_point(const conditional& c)
{
    for (const auto& [cond, ss] : c.conditions)
        if (!is_exit_point(ss))
            return false;

    if (c.else_condition.has_value() && !is_exit_point(c.else_condition.value()))
        return false;

    return true;
}

// check if condition overlap

// check group references

// check block references