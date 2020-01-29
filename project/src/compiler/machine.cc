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