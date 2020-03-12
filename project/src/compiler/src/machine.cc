#include <array>
#include <list>

#include "machine.hh"

bool operator<(const symbol& a, const symbol& b)
{
    return std::tie(a.character, a.marked) < std::tie(b.character, b.marked);
}

bool operator==(const symbol& a, const symbol& b)
{
    return std::tie(a.character, a.marked) == std::tie(b.character, b.marked);
}

bool machine::halted()
{
    auto [out, travel, next] = 
        s -> mapping
        .at(*head);
    return std::holds_alternative<result>(next);
}

void machine::step()
{
    auto [out, travel, next] = s -> mapping.at(*head);

    std::visit(visitor {
        [&](state* next){
            if (head == prev(tape.end())) {
                tape.push_back(blank);
            }

            if (travel == left) {
                if (head != tape.begin()) --head;
            } 
            
            else ++head;

            s = next;
        },

        [&](result) {},
    }, next);
    
}