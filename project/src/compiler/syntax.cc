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
                throw semantic_error("missing end point", s -> source);;
        }

        else ensure_return(*s);
    }
}

void ensure_return(const statement_list& ss)
{
    for (const auto& s : ss) ensure_return(s);
}

void ensure_return(const statement& s)
{
    std::visit(visitor {
        [s](const auto&)            {throw semantic_error("early exit causes unreachable statements", s.source);},
        [](const conditional& c)    {ensure_return(c);},
        [](const operation&)        {},
    }, s.value);
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

bool is_exit_point(const statement& s)
{
    return std::visit(visitor {
        [](const auto&) -> bool             {return true;},
        [](const operation&) -> bool        {return false;},
        [](const conditional& c) -> bool    {return is_exit_point(c);},
    }, s.value);
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

void ensure_distinct_conditions(const program& p)
{
    for (auto& s : p.statements)
        ensure_distinct_conditions(s);

    for (auto& [name, ss] : p.blocks)
        for (auto& s : ss)
            ensure_distinct_conditions(s);
}

void ensure_distinct_conditions(const statement& s)
{
    std::visit(visitor {
        [](const auto&) {},
        [](const conditional& c) {ensure_distinct_conditions(c);},
    }, s.value);
}

void ensure_distinct_conditions(const conditional& c)
{
    std::set<grouping> taken = {};

    for (auto& [cond, s] : c.conditions) {  //!!! does not work !!!
        if (intersect(taken, cond.value))
            throw semantic_error("condition overlaps preceeding cases", cond.source);

        taken.insert(cond.value.begin(), cond.value.end());
    }
}

// check references

void ensure_valid_references(const program& p)
{
    for (const auto& s : p.statements) {
        ensure_valid_references(p, s);
    }

    for (const auto& block : p.blocks) {
        std::cout << "hmm";
        //for (const statement& s : block.second) {
            //ensure_valid_references(p, s);
        //}
    }
}

void ensure_valid_references(const program& p, const statement& s)
{
    std::visit(visitor {
        [](const auto&){},
        [p, s](const reference& r) {
            if (p.blocks.find(r) == p.blocks.end())
                throw new semantic_error("unsatisfied reference to " + r, s.source);
        },
        [p](const conditional& c) {ensure_valid_references(p, c);},
    }, s.value);
}

void ensure_valid_references(const program& p, const conditional& c)
{
    for (const auto& [cond, ss] : c.conditions)
        for (const auto& g : cond.value)
            std::visit(visitor {
                [](const auto&) {},
                [p, cond](const reference& r) {
                    if (p.blocks.find(r) == p.blocks.end())
                        throw new semantic_error("unsatisfied reference to " + r, cond.source);  
                }
            }, g);
}