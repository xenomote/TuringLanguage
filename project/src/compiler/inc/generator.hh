#ifndef GENERATOR_H
#define GENERATOR_H

#include <list>
#include <variant>

#include "machine.hh"
#include "syntax.hh"

using interface = std::map<state*, std::set<symbol>>;

class generator {
public:
    generator(const program& p) : p(p) {};
    std::list<state> operator()();

private:
    interface generate(const statement_list& ss, interface& inputs);

    interface make_interface(state& target);
    interface make_interface(mapping& target);

    mapping empty_mapping();

    void add_all(interface& outputs, const interface& others);

    void set_outputs(const interface& outputs, const std::optional<tape_write>& write);
    void set_travel(const interface& ouptuts, direction travel);
    void set_next(interface& outputs, const successor& target);

    void patch(interface& outputs, mapping& target);

    std::set<symbol> generate_grouping(const std::set<grouping>& groups);

    const program& p;
    
    std::map<reference, std::set<symbol>> groups;
    std::map<reference, mapping> blocks;
    std::map<reference, interface> backpatch;
    std::list<state> states;
    std::list<state> dummies;
};


#endif