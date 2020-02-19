#include <sstream>

#include "generator.hh"
#include "syntax.hh"

std::list<state> generator::operator()(program& p)
{
    std::list<state> states = generate(p.statements);

    for (auto& [name, ss] : p.blocks) {
        auto states = generate(ss);
        blocks.insert({name, &*states.begin()});
        states.splice(states.end(), states);
    }

    for (const auto& [ref, name] : references)
        *ref = blocks[name];

    return states;
}

std::list<state> generator::generate(const statement_list& ss)
{
    std::list<state> states;

    for (const auto& s : ss) {
        std::visit(visitor {
            [&](const operation& o) {states.push_back(generate(o));},
            [&](const conditional& c) {states.push_back(generate(c));},
            [&](const reference& r) {},
            [&](const result& r) {states.push_back(result_state(r, s.source));},
        }, s.value);
    }

    return states;
}

state generator::generate(const operation& ss)
{
    return {};
}

state generator::generate(const conditional& ss)
{
    return {};
}

state generator::result_state(result r, yy::location l)
{
    std::stringstream source;
    source << (r == accept ? "accept" : "reject") << " " << l;

    std::map<symbol, symbol> w;
    std::map<symbol, direction> m;
    std::map<symbol, std::variant<state*, result>> t;

    for (const auto& s : symbols) {
        w.insert({s, s});
        m.insert({s, right});
        t.insert({s, r});
    }
    return {source.str(), w, m, t};
}