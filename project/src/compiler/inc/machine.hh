#ifndef RUNTIME_H
#define RUNTIME_H

#include <list>
#include <vector>
#include <memory>
#include <map>
#include <variant>

#define TAPE_SIZE 1024
#define MAX_INSTANCES 1024
#define MAX_STEPS 1000000


enum direction {left, right};
enum result {accept, reject};
enum marking {marked, unmarked};

struct symbol
{
    bool marked;
    char symbol;

    friend bool operator<(const struct symbol& a, const struct symbol& b);
};

static const symbol blank = {false, 0};

struct state 
{
    std::string source;
    std::map<symbol, symbol> write;
    std::map<symbol, direction> travel;
    std::map<symbol, std::variant<state*, result>> transition;
};

class machine 
{
public:
    machine(std::list<state> states, std::list<symbol> tape)
    : tape(tape)
    , states(states)
    , head(begin(tape))
    , s(&*begin(states))
    {}

    void step();
    void step(symbol& sym, state* next);
    bool halted();

private:
    std::list<symbol> tape;
    std::list<state> states;

    std::list<symbol>::iterator head;
    state* s;
};

// allow anonymous visitor declaration

template<class... Ts> struct visitor : Ts... { using Ts::operator()...; };
template<class... Ts> visitor(Ts...) -> visitor<Ts...>;

#endif