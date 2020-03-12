#ifndef GENERATOR_H
#define GENERATOR_H

#include <list>
#include <variant>

#include "machine.hh"
#include "syntax.hh"

using mapping = std::map<symbol, std::set<transition*>>;
using interface = std::map<symbol, transition>;

class generator {
public:
    generator(const program& p) : p(p) {};
    std::list<state> operator()();

private:
    mapping generate(const statement_list& ss, mapping& outputs);

    mapping make_mapping(interface& inputs);

    void connect(const mapping& outputs, const interface& input);
    void connect(const mapping& outputs, direction travel, const successor& next);

    mapping merge(const std::set<mapping>& outputs);

    std::set<symbol> generate_grouping(const grouping& g);

    const program& p;
    
    std::list<state> states;
    std::map<reference, std::set<symbol>> groups;
    std::map<reference, interface> blocks;
    std::map<reference, mapping> backpatch;
};


#endif