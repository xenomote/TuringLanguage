#include <sstream>

#include "generator.hh"
#include "syntax.hh"

std::list<state> generator::operator()(const program& p)
{

    generate(p, p.statements, states.emplace_back(state {}));

    for(const auto& [ref, links] : backpatch)
        for (const auto& link : links)
            *link = blocks[ref];
    
    return states;
}

void generator::generate(const program& p, const statement_list& ss, state& current)
{
    std::list<successor*> predecessors;

    for (const auto& s : p.symbols) predecessors.push_back(&current.transition[s]);

    for (const auto& s : p.statements) {
        std::visit(visitor {
            [&](const result& r) {
                for (auto& s : predecessors) *s = r;
            },

            [&](const reference& r){
                auto& refs = backpatch[r];

                if (refs.empty()) {
                    const auto& ss = p.blocks.find(r);
                    generate(p, ss -> second, states.emplace_back(state {}));
                }

                refs.splice(refs.end(), predecessors);
            },

            [&](const operation& o){
                //current = states.emplace_back(state {});

                for (auto& s : predecessors) *s = &current;
                predecessors.clear();

                for (const auto& s : p.symbols) {
                    current.travel[s] = o.travel;
                    // current.write[s] = 
                    predecessors.push_back(&current.transition[s]);
                }
            },

            [&](const conditional&){

            },
        }, s.value);
    }
}