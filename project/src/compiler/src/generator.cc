#include <sstream>

#include "generator.hh"
#include "syntax.hh"

std::list<state> generator::operator()()
{
    for (const auto& [r, cond] : p.groups) {
        auto& s = groups[r];
        auto g = generate_grouping(r);
        s.insert(g.begin(), g.end());
    }

    auto& start = states.emplace_back();
    start = {"start", {}};
    auto outputs = make_mapping(start.mapping);
    generate(p.statements, outputs);

    for(const auto& [ref, outputs] : backpatch)
        connect(outputs, blocks[ref]);
    
    return states;
}

mapping generator::generate(const statement_list& ss, mapping& outputs)
{
    for (const auto& s : ss) {
        std::visit(visitor {
            [&](const result& res) {
                connect(outputs, right, res);            
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
                // IMPLEMENT MODIFIERS

                auto& current = states.emplace_back();
                std::stringstream name;
                name << "line " << s.source.begin.line;
                current = {name.str(), {}};
                connect(outputs, o.travel, &current);

                if (o.output.has_value())
                    set_outputs(outputs, o.output.value());

                outputs = make_mapping(current.mapping);
            },

            [&](const conditional& c){
                mapping condition_outputs;

                for (const auto& [cond, condition_statements] : c.conditions) {
                    std::set<symbol> condition_symbols;

                    for (const auto& g : cond.value) {
                        auto s = generate_grouping(g);
                        condition_symbols.insert(s.begin(), s.end());
                    }

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
                    condition_outputs = merge({condition_outputs, exit_points});
                }

                else {
                    condition_outputs = merge({condition_outputs, outputs});
                }

                outputs = condition_outputs;
            },
        }, s.value);
    }

    return outputs;
}

void generator::connect(const mapping& outputs, const interface& inputs)
{
    for (const auto& [sym, outputs] : outputs)
        for (const auto& output : outputs)
            *output = inputs.at(sym);
}

void generator::connect(const mapping& outputs, direction travel, const successor& next)
{
    for (const auto& [write, outputs] : outputs)
        for (const auto& output : outputs)
            *output = {write, travel, next};
}

void generator::set_outputs(const mapping& outputs, const tape_write& write)
{
    std::visit(visitor {
        [&](symbol sym)
        {
            for (auto& [_, outs] : outputs)
                for (auto& out : outs)
                    out -> output = sym;
        },

        [&](bool mark)
        {
            for (auto& [sym, outs] : outputs)
                for (auto& out : outs)
                    out -> output = {mark, sym.character};
        },
    }, write);
}

mapping generator::make_mapping(interface& inputs)
{
    mapping outputs;

    for (auto sym : p.symbols) {
        const auto& [it, success] = inputs.insert({sym, {}});
        std::set<transition*> trans = {&it -> second};
        outputs.insert({sym, trans});
    }

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

std::set<symbol> generator::generate_grouping(const grouping& g)
{
    std::set<symbol> s;
    
    std::visit(visitor {
        [&](const symbol& sym)
        {
            s.insert(sym);
        },

        [&](const reference& ref)
        {
            auto& t = groups[ref];
            s.insert(t.begin(), t.end());
        },

        [&](marking m)
        {
            for (const auto& sym : p.symbols)
                if (sym.marked == m)
                    s.insert(sym);
        },
    }, g);

    return s;
}