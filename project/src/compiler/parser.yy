%language "c++"

%define parse.assert
%define parse.error verbose
%define parse.trace

%locations
%defines

%define api.parser.class {Parser}

%parse-param {std::string& filename}
%parse-param {program& result}

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
}

%define api.value.type variant
%token END  0  "end of file"
%token INDENT DEDENT NEWLINE
%token IF OR ELSE
%token ACCEPT REJECTION
%token WRITE GO DO
%token MARK UNMARK MARKED UNMARKED
%token LEFT RIGHT
%token WHILE UNTIL NUMBER BACKWARDS

%token <std::string> IDENTIFIER
%token <char> SYMBOL
%token <int> TIMES

%token EQUALS       "="
%token COMMA        ","
%token COLON        ":"

%type <program> program

%type <std::map<std::string, condition>> groups
%type <std::map<std::string, std::list<statement>>> blocks

%type <std::pair<std::string, condition>> group
%type <std::pair<std::string, std::list<statement>>> block

%type <statement> statement
%type <std::list<statement>> statements scope

%type <operation> operation
%type <conditional> conditional

%type <std::map<condition, std::list<statement>>> cases
%type <std::pair<condition, std::list<statement>>> if_case or_case
%type <std::optional<std::list<statement>>> else_case

%type <condition> condition
%type <grouping> grouping

%type <until_loop> until_loop
%type <while_loop> while_loop
%type <int> repetition

%type <direction> travel direction
%type <std::optional<tape_write>> write
%type <std::list<modifier>> modifiers
%type <modifier> modifier

%type <symbol> symbol
%type <std::set<symbol>> symbols
%type <std::string> block_reference group_reference
%type <int> NUMBER

%initial-action {
    @$.begin.filename = @$.end.filename = &filename;
}

%start program

%%

program:
    groups statements blocks 
    {
        $$ = {$statements, $groups, $blocks};
        result = $$;
    }
    ;

groups: 
    %empty                          {}
    | groups group NEWLINE {$$ = $1; $$.insert($group);}
    ;

blocks:  
    %empty                          {}
    | blocks block NEWLINE {$$ = $1; $$.insert($block);}
    ; 

group: 
    IDENTIFIER "=" condition    {$$ = {$IDENTIFIER, $condition};}
    ;

block: 
    IDENTIFIER ":" scope    {$$ = {$IDENTIFIER, $scope};}
    ;

scope: 
    NEWLINE INDENT statements DEDENT NEWLINE    {$$ = $statements;}
    ;

statements:
    statement              {$$ = {$statement};}
    | statements NEWLINE statement {$$ = $1; $$.push_back($statement);}
    ;

statement:
    conditional             {}
    | operation             {}
    | ACCEPT                {$$ = accept;}
    | REJECTION             {$$ = reject;}
    | DO block_reference    {$$ = $block_reference;}
    ;

conditional:
    if_case  {}
    if_case else_case {}
    ;

if_case:
    IF condition scope  {$$ = {$condition, $scope};}
    ;

// cases:
//     %empty                              {}
//     | cases NEWLINE or_case    {$$ = $1; $$.insert($or_case);}
//     ;

// or_case:
//     OR condition scope  {$$ = {$condition, $scope};}
//     ;

else_case:
    %empty                                  {}
    | ELSE NEWLINE scope   {$$ = $scope;}
    ;

condition:
    grouping                {$$ = {$grouping};}
    | symbols               {$$ = {$symbols};}
    | condition OR grouping {$$ = $1; $$.insert($grouping);}
    | condition OR symbols  {$$ = $1; $$.insert($symbols);}
    ;

grouping:
    group_reference             {}
    | MARKED                    {$$ = true;}
    | UNMARKED                  {$$ = false;}
    ;

symbols: 
    symbol                  {$$ = {$symbol};}
    | symbols "," symbol    {$$ = $1; $$.insert($symbol);}
    ;

symbol: 
    SYMBOL          {$$ = {false, $SYMBOL};}
    | MARKED SYMBOL {$$ = {true, $SYMBOL};}
    ;

operation:
    write travel modifiers  {$$ = {$travel, $write, $modifiers};}
    ;

write: 
    %empty                  {$$ = {};}
    | MARK COMMA            {$$ = true;}
    | UNMARK COMMA          {$$ = false;}
    | WRITE symbol COMMA    {$$ = $symbol;}
    ;

travel: 
    GO direction    {$$ = $direction;}
    ;

modifiers:
    %empty                  {}
    | modifiers modifier    {$$ = $1; $$.push_back($modifier);}
    ;

modifier:
    while_loop      {}
    | until_loop    {}
    | repetition    {}
    ;

while_loop:
    WHILE condition {$$ = {$condition};}
    ;

until_loop:
    UNTIL condition {$$ = {$condition};}
    ;

repetition: 
    NUMBER TIMES    {$$ = $NUMBER;}
    ;

direction: 
    LEFT    {$$ = left;}
    | RIGHT {$$ = right;}
    ;

group_reference:
    IDENTIFIER
    ;

block_reference:
    IDENTIFIER
    ;

newlines:
    %empty
    | newlines NEWLINE
    ;
%%

void yy::Parser::error(const location_type& l, const std::string& m)
{
    std::cerr << l << ": " << m << std::endl;
}