
%language "c++"

//%define parse.assert
%define parse.error verbose

%locations
%defines

%define api.parser.class {Parser}

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

    std::map<std::string, Condition*> groups;
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
%type <Condition*> condition until symbol symbols
%type <Write*> write
%type <Travel*> travel
%type <Statement**> transition
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
            const auto& [reference, location] = value;
            if (reference == nullptr) {
                error(location, "unsatisfied reference to \"" + name + "\"");
            }
                
        }

        program = $statement;
    }
    ;

optional_groups:
    %empty
    | groups
    ;

groups: 
    group newlines          {}
    | groups group newlines {}
    ;


group: 
    IDENTIFIER "=" symbols {groups.insert({$IDENTIFIER, $symbols});}
    ;

optional_blocks:
    %empty
    | blocks
    ;

blocks:  
    block newlines          {}
    | blocks block newlines {}
    ; 

block: 
    IDENTIFIER ":" scope
    {
        auto reference = blocks.find($IDENTIFIER);

        if (reference == blocks.end()) {
            blocks.insert({$IDENTIFIER, {&$scope, @IDENTIFIER}});
        }

        else if ((reference -> second).first != nullptr) {
            error(@IDENTIFIER, "erroneous redefinition of " + $IDENTIFIER);
        }

        else reference -> second = {&$scope, @IDENTIFIER};
    }
    ;

symbols: 
    symbols "," symbol              {}
    | symbol                        {}
    ;

symbol: 
    SYMBOL                          {}
    | MARKED SYMBOL                 {}
    ;

scope: 
    INDENT newlines statement optional_newlines DEDENT {$$ = $statement;}
    ;

newlines:
    NEWLINE optional_newlines

optional_newlines:
    %empty                      {}
    | optional_newlines NEWLINE {}
    ;

statement:
    write travel transition            {$$ = new Statement();}
    | IF condition scope newlines else {$$ = new Statement();}
    | ACCEPT                           {$$ = new Statement();}
    | REJECTION                        {$$ = new Statement();}
    ;

else: 
    OR condition scope else         {$$ = new Statement();}
    | ELSE scope                    {$$ = $scope;}
    | statement                     {$$ = $statement;}
    ;

write: 
    %empty                                      {$$ = nullptr;}
    | MARK COMMA                                {$$ = new Write();}
    | UNMARK COMMA                              {$$ = new Write();}
    | WRITE string reversal repetition COMMA    {$$ = new Write();}
    ;

travel: 
    GO direction repetition until   {$$ = new Travel();}
    ;

transition: 
    newlines statement               {$$ = &$statement;}
    | COMMA DO IDENTIFIER
    {
        auto reference = blocks.find($IDENTIFIER);

        if (reference == blocks.end()) {
            blocks.insert({$IDENTIFIER, {nullptr, @IDENTIFIER}});
        }

        $$ = (*reference).second.first;
    }
    ;

direction: 
    LEFT                            {$$ = left;}
    | RIGHT                         {$$ = right;}
    ;

string: 
    string SYMBOL   {buffer += $SYMBOL;}
    | SYMBOL        {buffer = $SYMBOL;}
    ;

reversal: 
    %empty                          {$$ = left;}
    | BACKWARDS                     {$$ = right;}
    ;

repetition: 
    %empty                          {$$ = 1;}
    |  NUMBER TIMES                 {$$ = $TIMES;}
    ;

until: 
    %empty                          {$$ = nullptr;}
    | UNTIL condition               {$$ = $condition;}
    ;

condition:
    symbol                          {$$ = $symbol;}
    | MARKED                        {$$ = new Condition();}
    | UNMARKED                      {$$ = new Condition();}
    | condition OR symbol           {$$ = new Condition();}
    | condition OR IDENTIFIER 
    {
        auto group = groups.find($IDENTIFIER);

        if (group != groups.end())
            //$$ = join($1, group);
            ;

        else
            Parser::error(@IDENTIFIER, "couldnt find group");
    }
    | IDENTIFIER
    {
        auto group = groups.find($IDENTIFIER);

        if (group != groups.end())
            //$$ = join($1, group);
            ;

        else
            Parser::error(@IDENTIFIER, "couldnt find group");
    }
    ;    

%%

void yy::Parser::error(const location_type& l, const std::string& m)
{
  std::cerr << l << ": " << m << '\n';
}