#include <sstream>

#include "generator.hh"
#include "syntax.hh"

std::list<state> generator::operator()(program& p)
{
    auto states = generate(p.statements);

    for (auto& [name, ss] : p.blocks) {
        auto block = generate(ss);
        blocks.insert({name, &*block.begin()});
        states.splice(states.end(), block);
    }

    for (const auto& [ref, name] : references)
        *ref = blocks[name];

    return states;
}

std::list<state> generator::generate(const statement_list& ss)
{
    for (const auto& s : ss) {
        std::visit(visitor {
            [&](const result& r){},
            [&](const reference& r){},
            [&](const operation& o){},
            [&](const conditional& c){},
        }, s.value);
    }

    return states;
}

state generator::generate(const operation& o)
{
    std::stringstream s;
    s << "operation";

    std::map<symbol, symbol> w;
    std::map<symbol, direction> m;
    std::map<symbol, std::variant<state*, result>> t; 

    for (const auto& s : symbols) {
        symbol out = !o.output.has_value() ? s : symbol {};
        
        // std::visit(visitor {
        //     [&](const symbol& sym){return sym;},
        //     [&](bool m){return symbol {m, s.character};},
        // }, o.output);

        // w.insert({s, out})
        // m.insert({s, o.travel});
        // t.insert({s});
    }

    return {s.str(), w, m, t};
}

state generator::generate(const conditional& ss)
{


    return {};
}