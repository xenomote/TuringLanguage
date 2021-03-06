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
enum marking {marked = true, unmarked = false};

struct symbol
{
    bool marked;
    char character;

    friend bool operator<(const struct symbol& a, const struct symbol& b);
    friend bool operator==(const struct symbol& a, const struct symbol& b);
    friend std::ostream& operator<<(std::ostream& out, const struct symbol& s);
};

static const symbol blank = symbol {false, '_'};

struct state;

using successor = std::variant<state*, result>;

struct transition
{
    symbol output;
    direction travel;
    successor next;
};

using mapping = std::map<symbol, transition>;

struct state 
{
    std::string source;
    mapping transitions;
    
};

class machine 
{
public:
    machine(const std::list<state>& ss, const std::list<symbol>& t)
    {
        tape = t;
        states = ss;
        head = tape.begin();
        s = &*ss.begin();
    }

    void step();
    bool halted() const;

    friend std::ostream& operator<<(std::ostream& out, const machine& m);

private:
    std::list<symbol> tape;
    std::list<state> states;

    std::list<symbol>::iterator head;
    const state * s;
};

// allow anonymous visitor declaration

template<class... Ts> struct visitor : Ts... { using Ts::operator()...; };
template<class... Ts> visitor(Ts...) -> visitor<Ts...>;

#endif