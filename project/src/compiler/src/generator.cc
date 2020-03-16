#include <sstream>

#include "generator.hh"
#include "syntax.hh"

std::list<state> generator::operator()()
{
    for (const auto& [r, cond] : p.groups) {
        groups[r] = generate_grouping(cond.value);
    }

    auto& start = states.emplace_back();
    std::stringstream name;
    name << "start";
    start = {name.str(), {}};

    interface map = make_interface(start);
    generate(p.statements, map);

    for(auto& [ref, outputs] : backpatch)
        connect(outputs, blocks[ref]);
    
    return states;
}

interface generator::generate(const statement_list& ss, interface& outputs)
{
    for (const auto& s : ss) {
        std::visit(visitor {
            [&](const result& res) {
                connect(outputs, res);            
                outputs = {};
            },

            [&](const reference& ref){
                if (blocks.find(ref) == blocks.end()) {
                    const auto& statements = p.blocks.at(ref);

                    state dummy = {"", empty_mapping()};
                    blocks[ref] = dummy.transitions;
                    auto inputs = make_interface(dummy);

                    generate(statements, inputs);
                }

                add_all(backpatch[ref], outputs);
                outputs = {};
            },

            [&](const operation& o){
                if (o.modifiers.empty()) {
                    auto& current = states.emplace_back();
                    std::stringstream name;
                    name << "line " << s.source.begin.line;
                    current = {name.str(), {}};

                    set_outputs(outputs, o.output);
                    set_travel(outputs, o.travel);

                    connect(outputs, &current);

                    outputs = make_interface(current);
                }

                // THIS DOES NOT WORK
                else for (const auto& mod : o.modifiers) {
                    std::visit(visitor {
                        [&](loop l)
                        {
                            auto g = generate_grouping({l.predicate.value});

                            set_outputs(outputs, o.output);
                            set_travel(outputs, o.travel);

                            interface loop_outputs;
                            for (auto& [target, symbols] : outputs)
                                for (auto& sym : symbols)
                                    if (g.count(sym) ^ l.inverted)
                                        target -> transitions[sym].next = target;
                                    else
                                        loop_outputs[target].insert(sym);

                            outputs = loop_outputs;
                        },

                        [&](int i)
                        {

                        },
                    }, mod);
                }
            },

            [&](const conditional& c){
                interface condition_outputs;
                for (const auto& [cond, condition_statements] : c.conditions) {
                    std::set<symbol> condition_symbols = generate_grouping({cond.value});

                    interface entry_points;
                    for (auto& [target, symbols] : outputs)
                        for (const auto& sym : symbols)
                            if (condition_symbols.count(sym)) {
                                symbols.erase(sym);
                                entry_points[target].insert(sym);
                            }
                    
                    auto exit_points = generate(condition_statements, entry_points);
                    
                    add_all(condition_outputs, exit_points);
                }

                if (c.else_condition.has_value()) {
                    auto condition_statements = c.else_condition.value();
                    auto exit_points = generate(condition_statements, outputs);
                    outputs = exit_points;
                }

                add_all(condition_outputs, outputs);

                outputs = condition_outputs;
            },
        }, s.value);
    }

    return outputs;
}

void generator::connect(interface& outputs, const successor& target)
{
    for (auto& [source, symbols] : outputs)
        for (auto& sym : symbols)
            source -> transitions[sym].next = target;
}

void generator::connect(interface& outputs, const mapping& inputs)
{
    for (auto& [source, symbols] : outputs)
        for (auto& sym : symbols)
            source -> transitions[sym] = inputs.at(sym);
}

void generator::set_outputs(const interface& outputs, const std::optional<tape_write>& write)
{
    if (write.has_value()) std::visit(visitor {
        [&](symbol out) 
        {
            for (auto& [source, symbols] : outputs) 
                for (auto& sym : symbols)
                    source -> transitions[sym].output = out;
        },

        [&](bool mark) 
        {
            for (auto& [source, symbols] : outputs) 
                for (auto& sym : symbols)
                    source -> transitions[sym].output = {mark, sym.character};
        },
    }, write.value());

    else for (auto& [source, symbols] : outputs) 
        for (auto& sym : symbols)
            source -> transitions[sym].output = sym;
}

void generator::set_travel(const interface& outputs, direction travel)
{
    for (auto& [source, symbols] : outputs) 
        for (auto& sym : symbols)
            source -> transitions[sym].travel = travel;
}


interface generator::make_interface(state& target)
{
    interface outputs;
    outputs[&target] = p.symbols;
    return outputs;
}

void generator::add_all(interface& outputs, const interface& others)
{
    for (const auto& [source, symbols] : others)
        outputs[source].insert(symbols.begin(), symbols.end());
}

mapping generator::empty_mapping()
{
    mapping outputs;
    for (auto sym : p.symbols) outputs[sym];
    return outputs;
}

std::set<symbol> generator::generate_grouping(const std::set<grouping>& g)
{
    std::set<symbol> s;
    
    for (auto& group : g) {
        std::visit(visitor {
            [&](const symbol& sym)
            {
                s.insert(sym);
            },

            [&](const reference& ref)
            {
                auto& t = groups.at(ref);
                s.insert(t.begin(), t.end());
            },

            [&](marking m)
            {
                for (const auto& sym : p.symbols)
                    if (sym.marked == m)
                        s.insert(sym);
            },
        }, group);
    }

    return s;
}