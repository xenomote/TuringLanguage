#ifndef GENERATOR_H
#define GENERATOR_H

#include <list>
#include <variant>
#include <stdexcept>

#include "machine.hh"
#include "syntax.hh"

class duplicate_error : public std::runtime_error
{
public:
    duplicate_error(std::map<state*, std::set<symbol>> duplicates)
    : runtime_error("duplicate mappings detected")
    , duplicates(duplicates)
    {}

    std::map<state*, std::set<symbol>> duplicates;
};

// map of input symbols from set of source states
class interface : public std::map<symbol, std::set<state*>>
{
public:
    void absorb(const interface& inputs);
    void set(std::optional<tape_write> write, direction travel, successor next);
    void patch(state& target);
    interface select(std::set<symbol> symbols);
};

class generator {
public:
    generator(const program& p) : p(p) {};
    std::list<state> operator()();

private:
    interface generate(const statement_list& ss, interface& inputs);
    interface generate(const statement& s, interface& inputs);

    interface generate(const result& s, interface& inputs);
    interface generate(const reference& s, interface& inputs);
    interface generate(const operation& s, interface& inputs);
    interface generate(const conditional& s, interface& inputs);

    interface generate(const operation& s, interface& inputs, std::list<modifier>::const_reverse_iterator mod);

    interface make_interface(state* source);

    std::set<symbol> generate_grouping(const std::set<grouping>& groups);

    const program& p;
    
    std::list<state> states;
    std::map<reference, state> blocks;
    std::map<reference, interface> references;
    std::map<reference, std::set<symbol>> groups;
};


#endif