#include <array>
#include <list>
#include <iostream>

#include "machine.hh"

bool operator<(const symbol& a, const symbol& b)
{
    return std::tie(a.character, a.marked) < std::tie(b.character, b.marked);
}

bool operator==(const symbol& a, const symbol& b)
{
    return std::tie(a.character, a.marked) == std::tie(b.character, b.marked);
}

std::ostream& operator<<(std::ostream& out, const symbol& s)
{
    out << s.character;
    if (s.marked) out << '*';
    return out;
}

std::ostream& operator<<(std::ostream& out, const machine& m)
{
    for (auto it = m.tape.begin(); it != m.tape.end(); ++it) {
        if (it == m.head) {
            out << '[' << *it << ']';
        }

        else out << ' ' << *it << ' ';
    }

    out << ": " << m.s -> source;

    std::visit(visitor {
        [&](state* s)   {out << " -> " << s -> source;},
        [&](result r)   {out << (r == reject ? " [REJECT]" : " [ACCEPT]");},
    }, m.s -> mapping.at(*m.head).next);

    return out;
}

bool machine::halted() const
{
    auto [out, travel, next] = s -> mapping.at(*head);
    return std::holds_alternative<result>(next);
}

void machine::step()
{
    auto [out, travel, next] = s -> mapping.at(*head);

    std::visit(visitor {
        [&](state* next)
        {
            if (head == prev(tape.end())) {
                tape.push_back(blank);
            }

            *head = out;

            if (travel == left) {
                if (head != tape.begin()) --head;
            } 
            
            else ++head;

            s = next;
        },

        [&](result){},
    }, next);
    
}