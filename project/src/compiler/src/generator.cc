#include <sstream>
#include <stack>
#include <iterator>
#include <stdexcept>

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

    // check all possible transitions have been mapped
    for (auto& state : states) {
        for (auto& sym : p.symbols) {
            if (!state.transitions.count(sym))
                throw std::runtime_error("unfulfilled transition in " + state.source);
        }
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
    return generate(op, inputs, op.modifiers.rbegin());
}

interface generator::generate(const operation& op, interface& inputs, std::list<modifier>::const_reverse_iterator mod, std::string suffix)
{
    interface outputs;

    if (mod == op.modifiers.rend()) {
        auto& op_state = states.emplace_back(state {"line " + std::to_string(op.line) + suffix, {}});
        inputs.set(op.output, op.travel, &op_state);
        outputs = make_interface(&op_state);
    }

    else {
        auto sub_mod = std::next(mod);

        std::visit(visitor {
            [&](const int& repetitions)
            {
                outputs = inputs;

                for (int i = 0; i < repetitions; i++) {
                    outputs = generate(op, outputs, sub_mod, " repetition " + std::to_string(i) + suffix);
                }
            },
            [&](const loop& l)
            {
                auto group = generate_grouping(l.predicate.value);
                std::set<symbol> remaining(p.symbols);
                for (auto& sym : group) remaining.erase(sym);

                if (l.inverted) std::swap(group, remaining);

                auto loop_inputs = inputs.select(group);
                auto loop_outputs = generate(op, loop_inputs, sub_mod, std::string("") + (l.inverted ? " until" : " while") + " loop" + suffix);

                loop_inputs.absorb(loop_outputs.select(group));
                outputs.absorb(loop_outputs.select(remaining));
                outputs.absorb(inputs.select(remaining));
            },
        }, *mod);
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
    interface i;
    for (auto sym : p.symbols) i.inputs[sym].insert(source);
    return i;
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

    for (auto& [sym, sources] : inputs.inputs) {
        for (auto& source : sources) {

            auto& connections = this->inputs[sym];

            // connect newcomer to existing interface
            if (transitions.count(sym))
                source->transitions[sym] = transitions[sym];

            auto [x, inserted] = connections.insert(source);

            if (!inserted) duplicates[source].insert(sym);
        }
    }

    if (!duplicates.empty()) throw duplicate_error(duplicates);
}

void interface::set(std::optional<tape_write> write, direction travel, successor next)
{
    for (auto& [sym, sources] : inputs) {
        auto& trans = transitions[sym];

        if (write.has_value()) {
            std::visit(visitor {
                [&](symbol out){trans.output = out;},
                [&](bool m){trans.output = symbol {m, sym.character};},
            }, write.value());
        }

        else trans.output = sym;

        trans.travel = travel;
        trans.next = next;

        for (auto& source : sources) {
            source->transitions[sym] = trans;
        }
    }
}

void interface::patch(state& target)
{
    for (auto& [sym, sources] : inputs) {
        for (auto& source : sources) {
            source->transitions[sym] = target.transitions[sym];
        }
    }
}

interface interface::select(std::set<symbol> symbols)
{
    interface outputs;
    for (auto& sym : symbols) outputs.inputs[sym] = inputs[sym];
    return outputs;
}
