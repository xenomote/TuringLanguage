#include <sstream>

#include "generator.hh"
#include "syntax.hh"

std::list<state> generator::operator()(program& p)
{
    state start = {};
    std::list<state> states = {start};

    std::list<std::variant<state*, result>*> predecessors = {};
    std::map<reference, std::variant<state*, result>*> blocks = {};

    for (const auto& s : p.symbols) {
        predecessors.push_back(&start.transition[s]);
    }

    for (const auto& s : p.statements) {
        std::visit(visitor {
            [&](const result& r) {
                for (auto& s : predecessors) *s = r;
            },

            [&](const reference& r){
                const auto& b = blocks.find(r);

                if (b == blocks.end()) {
                    const auto& b = blocks.emplace(r, reject);

                    const auto& ss = p.blocks.find(r);
                    
                    generate(s, ss -> second);
                }

                for (auto& s : predecessors) *s = b -> second;
            },

            [&](const operation&){

            },

            [&](const conditional&){

            },

            [&](const auto&){},
        }, s.value);
    }

    return states;
}

std::list<state> generate(state& start, const std::list<state>& states)
{

}