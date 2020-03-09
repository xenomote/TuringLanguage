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
    std::list<state> generate(state& start, const statement_list& ss);
};


#endif