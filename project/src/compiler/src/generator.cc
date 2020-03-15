#include <sstream>

#include "generator.hh"
#include "syntax.hh"

std::list<state> generator::operator()()
{
    for (const auto& [r, cond] : p.groups) {
        groups[r] = generate_grouping(cond.value);
    }

    mapping start;
    generate(p.statements, start);

    for(const auto& [ref, outputs] : backpatch)
        connect(outputs, blocks[ref]);
    
    return states;
}

mapping generator::generate(const statement_list& ss, mapping& outputs)
{
    for (const auto& s : ss) {
        std::visit(visitor {
            [&](const result& res) {
                auto inputs = make_interface(res);
                connect(outputs, inputs);            
                outputs.clear();
            },

            [&](const reference& r){
                auto& patches = backpatch[r];

                if (patches.empty()) {
                    patches = outputs;

                    const auto& [_, statements] = *p.blocks.find(r);
                    auto& inputs = blocks[r];
                    auto map = make_mapping(inputs);
                    
                    generate(statements, map);
                }

                else patches = merge({patches, outputs});
                
                outputs.clear();
            },

            [&](const operation& o){
                if (o.modifiers.empty()) {
                    auto& current = states.emplace_back();
                    std::stringstream name;
                    name << "line " << s.source.begin.line;
                    current = {name.str(), {}};

                    set_outputs(current, o.output);
                    set_travel(current, o.travel);

                    connect(outputs, make_interface(&current));

                    outputs = make_mapping(current);
                }

                // THIS DOES NOT WORK
                else for (const auto& mod : o.modifiers) {
                    std::visit(visitor {
                        [&](loop l)
                        {
                            auto& current = states.emplace_back();
                            std::stringstream name;
                            name << "line " << s.source.begin.line << " loop";
                            current = {name.str(), {}};
                            
                            set_outputs(current, o.output);
                            set_travel(current, o.travel);

                            auto g = generate_grouping(l.predicate.value);

                            // connect the predecessor to the loop state
                            mapping loop_inputs;
                            for (auto& sym : p.symbols) {
                                if (g.count(sym) ^ l.inverted) {
                                    auto& output = outputs[sym];
                                    loop_inputs.insert({sym, output});
                                    outputs.erase(sym);
                                }
                            }

                            connect(loop_inputs, make_interface(&current));
                            auto loop_outputs = make_mapping(current);

                            // connect the loop state to itself
                            mapping loop_cycle;
                            for (auto& sym : p.symbols) {
                                if (g.count(sym) ^ l.inverted) {
                                    auto& output = loop_outputs[sym];
                                    loop_cycle.insert({sym, output});
                                    loop_outputs.erase(sym);
                                }
                            }

                            // TRAVEL OUT OF LOOP STATE MUST BE BACKPATCHED BY SUBSEQUENT STATE

                            connect(loop_cycle, make_interface(&current));

                            outputs = merge({outputs, loop_outputs});
                        },

                        [&](int i)
                        {

                        },
                    }, mod);
                }
            },

            [&](const conditional& c){
                mapping condition_outputs;
                for (const auto& [cond, condition_statements] : c.conditions) {
                    std::set<symbol> condition_symbols = generate_grouping({cond.value});

                    mapping entry_points;
                    for (const auto& sym : condition_symbols) {
                        auto& output = outputs[sym];
                        entry_points.insert({sym, output});
                        outputs.erase(sym);
                    }
                    
                    auto exit_points = generate(condition_statements, entry_points);
                    condition_outputs = merge({condition_outputs, exit_points});
                }

                if (c.else_condition.has_value()) {
                    auto condition_statements = c.else_condition.value();
                    auto exit_points = generate(condition_statements, outputs);
                    outputs = {};
                    condition_outputs = merge({condition_outputs, exit_points});
                }

                else {
                    condition_outputs = merge({condition_outputs, outputs});
                }

                outputs = merge({outputs, condition_outputs});
            },
        }, s.value);
    }

    return outputs;
}

void generator::connect(const mapping& outputs, const interface& inputs)
{
    for (const auto& [sym, output_set] : outputs)
        for (const auto& output : output_set)
            *output  = inputs.at(sym);
}

void generator::set_outputs(state& state, const std::optional<tape_write>& write)
{
    if (write.has_value()) std::visit(visitor {
        [&](symbol out) {for (auto& in : p.symbols) state.mapping[in].output = out;},
        [&](bool mark)  {for (auto& in : p.symbols) state.mapping[in].output = {mark, in.character};},
    }, write.value());

    else for (auto& in : p.symbols) state.mapping[in].output = in;
}

void generator::set_travel(state& state, direction travel)
{
    for (auto& in : p.symbols) state.mapping[in].travel = travel;
}


interface generator::make_interface(const successor& target)
{
    interface inputs;
    for (auto sym : p.symbols) inputs[sym] = target;
    return inputs;
}

mapping generator::make_mapping(state& target)
{
    mapping outputs;
    for (auto& [sym, output] : target.mapping) outputs[sym] = {&output.next};
    return outputs;
}

mapping generator::make_mapping(interface& target)
{
    mapping outputs;
    for (auto& sym : p.symbols) outputs[sym] = {&target[sym]};
    return outputs; 
}

mapping generator::merge(const std::set<mapping>& outputs)
{
    mapping merged = {};

    for (const auto& output : outputs)
        for (const auto& [sym, o] : output)
            merged[sym].insert(o.begin(), o.end());

    return merged;
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