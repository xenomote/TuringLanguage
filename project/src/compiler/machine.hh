#ifndef RUNTIME_H
#define RUNTIME_H

#include <list>
#include <array>

#define TAPE_SIZE 1024
#define MAX_INSTANCES 1024
#define MAX_STEPS 1000000

static const int blank = 0;

enum Direction {left, right};

template <int n>
struct State {
    std::string source;
    std::array<char, n> write;
    std::array<int, n> transition;
    std::array<Direction, n> direction;
};

template <int n, int m>
class Machine {
private:
    std::array<State<n>, m> states;
    typename std::array<State<n>, m>::iterator state;

    std::list<int> tape;
    typename std::list<int>::iterator head;

public:
    Machine(std::array<State<n>, m> states, std::list<int> tape)
    : tape(tape)
    , states(states)
    , head(begin(tape))
    , state(begin(states))
    {}

    bool halted()
    {
        return state -> transition[*head] == nullptr;
    }

    void step()
    {
        auto symbol = *head;
        
        *head = state -> write[symbol];
        
        if (head == prev(tape.end())) {
            tape.push_back(blank);
        }

        switch (state -> direction[symbol]) 
        {
            case left: if (head != tape.begin()) head--;
                break;
        
            case right: head++;
                break;
        } 

        state = state -> transition[symbol];
    }
};

#endif