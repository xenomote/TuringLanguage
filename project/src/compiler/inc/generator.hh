#ifndef GENERATOR_H
#define GENERATOR_H

#include <list>
#include <variant>

#include "machine.hh"
#include "syntax.hh"

using mapping = std::map<symbol, std::set<successor*>>;
using interface = std::map<symbol, successor>;

class generator {
public:
    generator(const program& p) : p(p) {};
    std::list<state> operator()();

private:
    mapping generate(const statement_list& ss, mapping& outputs);

    interface make_interface(const successor& target);
    mapping make_mapping(state& target);
    mapping make_mapping(interface& target);

    mapping merge(const std::set<mapping>& outputs);

    void connect(const mapping& outputs, const interface& input);

    void set_outputs(state& state, const std::optional<tape_write>& write);
    void set_travel(state& state, direction travel);
    void set_next(state& state, const successor& next);

    std::set<symbol> generate_grouping(const std::set<grouping>& groups);

    const program& p;
    
    std::list<state> states;
    std::map<reference, std::set<symbol>> groups;
    std::map<reference, interface> blocks;
    std::map<reference, mapping> backpatch;
};


#endif