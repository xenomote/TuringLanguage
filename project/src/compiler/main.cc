#include "parser.yy.hh"

int main()
{
    yy::Parser parser;
    
    if (parser() == 0) std::cout << "success" << std::endl;
}