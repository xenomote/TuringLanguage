#include <assert.h>

#include "syntax.hh"

StringGenerator::StringGenerator()
: buffer()
, indent(0)
{}

std::string StringGenerator::output()
{
    return buffer.str();
}

void StringGenerator::generate(Statement& statement)
{
    switch (statement.type) {
        case Statement::type::accept:
            buffer << "accept" << std::endl;
            break;

        case Statement::type::reject:
            buffer << "reject" << std::endl;
            break;

        case Statement::type::conditional:
            generate(*statement.conditional);
            break;

        case Statement::type::operation:
            generate(*statement.operation);
            break;

        default: assert(!"unrecognised statement type");
    }
}

void StringGenerator::generate(Conditional& conditional)
{
    buffer << "if ";
    generate(*conditional.condition);

    buffer << "success:" << std::endl; 
    generate(*conditional.success);
    buffer << "failure:" << std::endl;
    generate(*conditional.failure);
}

void StringGenerator::generate(Operation& operation)
{
    buffer << "operation" << std::endl;

    if (operation.write != nullptr) generate(*operation.write);
    else buffer << "no write" << std::endl;

    if (operation.travel != nullptr) generate(*operation.travel);
    else buffer << "no travel" << std::endl;



    if (operation.next == nullptr) 
        buffer << "no transition" << std::endl;

    else if (*operation.next == nullptr) 
        buffer << "unsatisfied transition" << std::endl;
    
    else {
        auto jump = **operation.next;
        *operation.next = nullptr;
        generate(jump);
    }
    
}

void StringGenerator::generate(Write& write)
{
    switch (write.type)
    {
        case Write::type::mark:
            buffer << "mark";
            break;

        case Write::type::unmark:
            buffer << "unmark";
            break;

        case Write::type::write:
            buffer << "write " << write.string;
    }
    
    if (write.repetition > 1) buffer << write.repetition << " times";

    buffer << std::endl;
}

void StringGenerator::generate(Travel& travel)
{
    buffer << "travel " << (travel.direction == left ? "left" : "right");
    
    if (travel.until) {
        buffer << " until ";
        generate(*travel.until);
    }

    else buffer << std::endl;
}

void StringGenerator::generate(Condition& condition)
{
    buffer << "condition" << std::endl;

    switch (condition.type)
    {
        case Condition::type::marked:
            buffer << " marked" << std::endl;
            break;

        case Condition::type::unmarked:
            buffer << " unmarked" << std::endl;
            break;

        case Condition::type::symbols:
            for (const auto& symbol : condition.symbols) {
                buffer << '\t';
                generate(*symbol);
            }
            break;

        default: assert(!"unrecognised condition type");
    }
}

void StringGenerator::generate(Symbol& symbol)
{
    buffer << "symbol " 
        << (symbol.type == Symbol::type::marked ? "marked " : "")
        << symbol.symbol
        << std::endl;
}