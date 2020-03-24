#include <sstream>

#include "generator.hh"
#include "syntax.hh"

std::list<state> generator::operator()()
{
    for (const auto& [r, cond] : p.groups) {
        groups[r] = generate_grouping(cond.value);
    }

    auto& start = states.emplace_back(state {"start", {}});
    auto inputs = make_interface(&start);

    generate(p.statements, inputs);

    for (auto& [ref, inputs] : references) {
        inputs.patch(blocks.at(ref));
    }

    return states;
}

interface generator::generate(const statement_list& ss, interface& inputs)
{
    interface output = inputs;
    for (auto& s : ss) output = generate(s, output);
    return output;
}

interface generator::generate(const statement& s, interface& inputs)
{
    return std::visit(visitor {
        [&](result x)       {return generate(x, inputs);},
        [&](reference x)    {return generate(x, inputs);},
        [&](operation x)    {return generate(x, inputs);},
        [&](conditional x)  {return generate(x, inputs);},
    }, s.value);
}

interface generator::generate(const result& res, interface& inputs)
{
    inputs.set({}, right, res);

    return {};
}

interface generator::generate(const reference& ref, interface& inputs)
{
    if (blocks.find(ref) == blocks.end()) {
        blocks[ref] = {ref, {}};
        auto outputs = make_interface(&blocks[ref]);
        generate(p.blocks.at(ref), outputs);
    }

    references[ref].absorb(inputs);

    return {};
}

interface generator::generate(const operation& op, interface& inputs)
{
    interface outputs;

    if (!op.modifiers.empty()) {
        auto loop_inputs = inputs;

        for (auto& mod : op.modifiers) {
            outputs = {};

            std::visit(visitor {
                [&](int i){},
                [&](loop l)
                {
                    auto group = generate_grouping(l.predicate.value);
                    std::set<symbol> remaining(p.symbols);
                    for (auto& sym : group) remaining.erase(sym);

                    if (l.inverted) std::swap(group, remaining);

                    auto name = "line " + std::to_string(op.line) + " loop";
                    auto& loop_state = states.emplace_back(state {name, {}});
                    auto loop_outputs = make_interface(&loop_state);

                    loop_inputs.absorb(loop_outputs.select(group));
                    outputs.absorb(loop_outputs.select(remaining));
                    outputs.absorb(loop_inputs.select(remaining));

                    loop_inputs.set(op.output, op.travel, &loop_state);
                },
            }, mod);

            loop_inputs = outputs;
        }
    }

    else {
        auto name = "line " + std::to_string(op.line);
        auto& op_state = states.emplace_back(state {name, {}});
        inputs.set(op.output, op.travel, &op_state);
        outputs = make_interface(&op_state);
    }

    return outputs;
}

interface generator::generate(const conditional& branch, interface& inputs)
{
    interface outputs;
    std::set<symbol> remaining(p.symbols);

    for (auto& [c, ss] : branch.conditions) {
        auto group = generate_grouping(c.value);
        auto subset = inputs.select(group);

        outputs.absorb(generate(ss, subset));

        for (auto& sym : group) remaining.erase(sym);
    }

    if (branch.else_condition.has_value()) {
        auto& ss = branch.else_condition.value();
        auto subset = inputs.select(remaining);

        outputs.absorb(generate(ss, subset));
    }

    else outputs.absorb(inputs.select(remaining));

    return outputs;
}

interface generator::make_interface(state* source)
{
    interface outputs;
    for (auto sym : p.symbols) outputs[sym].insert(source);
    return outputs;
}

std::set<symbol> generator::generate_grouping(const std::set<grouping>& g)
{
    std::set<symbol> s;
    
    for (auto& group : g) {
        std::visit(visitor {
            [&](const reference& ref)
            {
                auto& t = groups.at(ref);
                for (auto& sym : t) s.insert(sym);
            },

            [&](marking m)
            {
                for (const auto& sym : p.symbols) if (sym.marked == m) s.insert(sym);
            },

            [&](const symbol& sym)
            {
                s.insert(sym);
            },
        }, group);
    }

    return s;
}

void interface::absorb(const interface& inputs) {
    std::map<state*, std::set<symbol>> duplicates;

    for (auto& [sym, sources] : inputs) {
        for (auto& source : sources) {
            auto [x, inserted] = (*this)[sym].insert(source);

            if (!inserted) duplicates[source].insert(sym);
        }
    }

    if (!duplicates.empty()) throw duplicate_error(duplicates);
}

void interface::set(std::optional<tape_write> write, direction travel, successor next)
{
    for (auto& [sym, sources] : *this) {
        for (auto& source : sources) {
            auto& trans = source->transitions[sym];

            if (write.has_value()) {
                std::visit(visitor {
                    [&](symbol out){trans.output = out;},
                    [&](bool m){trans.output = symbol {m, sym.character};},
                }, write.value());
            }

            else trans.output = sym;

            trans.travel = travel;
            trans.next = next;
        }
    }
}

void interface::patch(state& target)
{
    for (auto& [sym, sources] : *this) {
        for (auto& source : sources) {
            source->transitions[sym] = target.transitions[sym];
        }
    }
}

interface interface::select(std::set<symbol> symbols)
{
    interface outputs;
    for (auto& sym : symbols) outputs[sym] = (*this)[sym];
    return outputs;
}
