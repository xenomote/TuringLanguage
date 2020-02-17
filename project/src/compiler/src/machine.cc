#include <array>
#include <list>

#include "machine.hh"

bool operator<(const symbol& a, const symbol& b)
{
    return (a.symbol == b.symbol) 
        ? !a.marked && b.marked
        : a.symbol < b.symbol
        ;  
}

bool machine::halted()
{
    return true;//std::holds_alternative<result>(s.transition[*head]);
}

void machine::step()
{
    auto sym = *head;

    std::visit(visitor {
        [&](const auto next) {step(sym, next);},
        [](const result) {},
    }, s -> transition[sym]);
}

void machine::step(symbol& sym, state* next)
{ 
    *head = s -> write[sym];
    
    if (head == prev(tape.end())) {
        tape.push_back(blank);
    }

    if (s -> travel[sym] == left) {
        if (head != tape.begin()) head--;
    } 
    
    else head++;

    s = next;
}