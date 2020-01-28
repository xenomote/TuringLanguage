%language "c++"

%define parse.assert
%define parse.error verbose

%locations
%defines

%define api.parser.class {Parser}

%parse-param {Statement*& program}

%code top
{
    #include <string>
    #include <sstream>
    #include <list>
    #include <map>
}

%code requires
{
    #include "syntax.hh"
}

%code
{
    extern int yylex(
        yy::Parser::semantic_type* yylval,
        yy::Parser::location_type* yylloc
    );

    std::string buffer = "";

    Statement* program;

    std::map<std::string, std::list<Symbol*>*> groups;
    std::map<std::string, std::pair<Statement**, yy::Parser::location_type>> blocks;
}

%define api.value.type variant
%token END  0  "end of file"
%token INDENT DEDENT NEWLINE
%token IF OR ELSE
%token ACCEPT REJECTION
%token WRITE GO DO
%token MARK UNMARK MARKED UNMARKED
%token LEFT RIGHT
%token UNTIL NUMBER BACKWARDS

%token <std::string> IDENTIFIER
%token <char> SYMBOL
%token <int> TIMES

%token EQUALS       "="
%token COMMA        ","
%token COLON        ":"

%type <Statement*> program statement scope else
%type <Condition*> condition until group_ref
%type <Conditional*> conditional
%type <Operation*> operation

%type <std::list<Symbol*>*> symbols
%type <Symbol*> symbol

%type <Write*> write
%type <Travel*> travel
%type <Statement**> transition block_ref
%type <int> repetition
%type <Direction> direction reversal
%type <std::string> string

%initial-action {
    @$.begin.filename = @$.end.filename = new std::string("stdin");
}

%start program

%%

program:
    optional_newlines
    optional_groups
    statement
    optional_newlines 
    optional_blocks
    {
        for (const auto& [name, value] : blocks) {
            const auto& [pointer, location] = value;
            if (*pointer == nullptr) {
                throw syntax_error(location, "unsatisfied reference to \"" + name + "\"");
            }
                
        }

        $program = $statement;
        program = $program;
    }
    ;

optional_groups:
    %empty
    | groups
    | error
    ;

optional_blocks:
    %empty
    | blocks
    | error
    ;

groups: 
    group newlines
    | groups group newlines
    | groups error newlines
    ;

blocks:  
    block newlines
    | blocks block newlines
    | blocks error newlines
    ; 

group: 
    IDENTIFIER "=" symbols
    {
        auto reference = groups.find($IDENTIFIER);
        
        // if it does not already exist
        if (reference == groups.end()) {
            groups.insert({$IDENTIFIER, $symbols});
        }

        // if it already exists
        else {
            throw syntax_error(@IDENTIFIER, "erroneous redefinition of " + $IDENTIFIER);
        }
    }
    ;

block: 
    IDENTIFIER ":" scope
    {
        auto reference = blocks.find($IDENTIFIER);

        // if it has not been declared
        if (reference == blocks.end()) {
            auto pointer = new Statement* ($scope);
            blocks.insert({$IDENTIFIER, {pointer, @IDENTIFIER}});
        }

        // if it has already been defined
        else if (*((reference -> second).first) != nullptr) {
            throw syntax_error(@IDENTIFIER, "erroneous redefinition of " + $IDENTIFIER);
        }

        // if it has been declared but not defined
        else {
            auto& pointer = reference -> second.first;
            auto& location = reference -> second.second;

            *pointer = $scope;
            location = @IDENTIFIER;
        }
    }
    ;

symbols: 
    symbol
    {
        $$ = new std::list<Symbol*>();
        $$ -> push_back($symbol);
    }
    | symbols[list] "," symbol
    {
        $list -> push_back($symbol);
        $$ = $list;
    }
    ;

symbol: 
    SYMBOL          {$$ = new Symbol {Symbol::type::unmarked, $SYMBOL};}
    | MARKED SYMBOL {$$ = new Symbol {Symbol::type::marked, $SYMBOL};}
    ;

scope: 
    INDENT newlines statement optional_newlines DEDENT          {$$ = $statement;}
    | INDENT error DEDENT   {}
    ;

newlines:
    NEWLINE optional_newlines
    ;

optional_newlines:
    %empty
    | optional_newlines NEWLINE
    ;

statement:
    operation           {$$ = new Statement {Statement::type::operation, nullptr, $operation};}
    | IF conditional    {$$ = new Statement {Statement::type::conditional, $conditional, nullptr};}
    | ACCEPT            {$$ = new Statement {Statement::type::accept, nullptr, nullptr};}
    | REJECTION         {$$ = new Statement {Statement::type::reject, nullptr, nullptr};}
    ;

operation:
    write travel transition {$$ = new Operation {$write, $travel, $transition};}
    ;

conditional:
    condition scope newlines else    {$$ = new Conditional {$condition, $scope, $else};}
    ;

else: 
    OR conditional  {$$ = new Statement {Statement::type::conditional, $conditional, nullptr};}
    | ELSE scope    {$$ = $scope;}
    | statement     {$$ = $statement;}
    ;

write: 
    %empty                                      {$$ = nullptr;}
    | MARK COMMA                                {$$ = new Write {Write::type::mark, "", false, 0};}
    | UNMARK COMMA                              {$$ = new Write {Write::type::unmark, "", false, 0};}
    | WRITE string reversal repetition COMMA    {$$ = new Write {Write::type::write, $string, $reversal, $repetition};}
    ;

travel: 
    GO direction repetition until   {$$ = new Travel {$direction, $repetition, $until};}
    ;

transition: 
    newlines statement {
        Statement** pointer = new Statement* ($statement);
        $$ = pointer;
    }
    | COMMA DO block_ref    {$$ = $block_ref;}
    | newlines error        {}
    ;

direction: 
    LEFT    {$$ = left;}
    | RIGHT {$$ = right;}
    ;

string: 
    string SYMBOL   {buffer += $SYMBOL;}
    | SYMBOL        {buffer = $SYMBOL;}
    ;

reversal: 
    %empty      {$$ = left;}
    | BACKWARDS {$$ = right;}
    ;

repetition: 
    %empty          {$$ = 1;}
    |  NUMBER TIMES {$$ = $TIMES;}
    ;

until: 
    %empty              {$$ = nullptr;}
    | UNTIL condition   {$$ = $condition;}
    ;

condition:
    group_ref                   {$$ = $group_ref;}
    | symbol                    {$$ = new Condition {Condition::type::symbols, {$symbol}};}
    | MARKED                    {$$ = new Condition {Condition::type::marked, {}};}
    | UNMARKED                  {$$ = new Condition {Condition::type::unmarked, {}};}
    | condition[c] OR symbol
    {
        $$ = $c;
        auto& a = $c -> symbols;

        a.push_back($symbol); 
    }
    | condition[c] OR group_ref
    {
        $$ = $c;
        auto& a = $c -> symbols;
        auto& b = $group_ref -> symbols;

        a.insert(a.end(), b.begin(), b.end());
    }
    ;

block_ref:
    IDENTIFIER
    {
        auto reference = blocks.find($IDENTIFIER);

        if (reference == blocks.end()) {
            Statement** pointer = new Statement* (nullptr);
            const auto& [iterator, x] = blocks.insert({$IDENTIFIER, {pointer, @IDENTIFIER}});
            reference = iterator;
        }

        $$ = reference -> second.first;
    }
    ;

group_ref:
    IDENTIFIER
    {
        auto reference = groups.find($IDENTIFIER);

        if (reference != groups.end()) {
            auto group = reference -> second;

            $$ = new Condition {Condition::type::symbols, *group};
        }

        else throw syntax_error(@IDENTIFIER, "couldnt find group");
    }
    ;
%%

void yy::Parser::error(const location_type& l, const std::string& m)
{
    std::cerr << l << ": " << m << std::endl;
}