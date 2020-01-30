#ifndef RUNTIME_H
#define RUNTIME_H

#include <list>
#include <vector>

#define TAPE_SIZE 1024
#define MAX_INSTANCES 1024
#define MAX_STEPS 1000000

static const int blank = 0;

enum direction {left, right};
enum result {accept = true, reject = false};

struct symbol
{
    bool marked;
    char symbol;

    friend bool operator<(const struct symbol& a, const struct symbol& b);
};

struct State 
{
    std::string source;
    std::vector<char> write;
    std::vector<int> transition;
    std::vector<direction> travel;
};

class Machine 
{
private:
    std::list<int> tape;
    std::vector<State> states;

    std::list<int>::iterator head;
    std::vector<State>::iterator state;

public:
    Machine(std::vector<State> states, std::list<int> tape)
    : tape(tape)
    , states(states)
    , head(begin(tape))
    , state(begin(states))
    {}

    bool halted()
    {
        return state -> transition[*head] == 0;
    }

    void step()
    {
        auto symbol = *head;
        
        *head = state -> write[symbol];
        
        if (head == prev(tape.end())) {
            tape.push_back(blank);
        }

        if (state -> travel[symbol] == left) {
            if (head != tape.begin()) head--;
        } 
        
        else head++;

        state = next(states.begin(), state -> transition[symbol]);
    }
};

#endif