#include "syntax.hh"

void Statement :: accept(Generator& generator)
{
    switch (this -> type) {
        case accept:
        case reject:

            break;

        case conditional:
            generator.generate(this -> conditional);
            break;

        case operation:
            generator.generate(this -> operation);
            break;

        default: assert(!"unrecognised statement type");
    }
}

void Conditional :: accept(Generator& generator)
{
    generator.generate(this -> condition);
    generator.generate(this -> success);
    generator.generate(this -> failure);
}

void Operation :: accept(Generator& generator)
{
    //generator.generate
}