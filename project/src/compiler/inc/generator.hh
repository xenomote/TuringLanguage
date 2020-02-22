#ifndef GENERATOR_H
#define GENERATOR_H

#include <list>
#include <variant>

#include "machine.hh"
#include "syntax.hh"

class generator {
public:
    std::list<state> operator()(program& p);

private:
    std::list<state> generate(const statement_list& ss);

    state generate(const operation& ss);
    state generate(const conditional& ss);

    std::list<state> states;
    std::list<symbol> symbols;

    std::list<state*> previous;
    
    std::map<reference, state*> blocks;
    std::list<std::pair<state**, reference>> references;
};


#endif