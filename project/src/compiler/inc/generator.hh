#ifndef GENERATOR_H
#define GENERATOR_H

#include <list>
#include <variant>

#include "machine.hh"
#include "syntax.hh"

class generator {
public:
    std::list<state> operator()(const program& p);

private:
    void generate(const program& p, const statement_list& ss, state& start);

    std::list<state> states;
    std::map<reference, successor> blocks;
    std::map<reference, std::list<successor*>> backpatch;
};


#endif