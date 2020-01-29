%language "c++"

%define parse.assert
%define parse.error verbose

%locations
%defines

%define api.parser.class {Parser}

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

    std::string buffer = "";
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
    @$.begin.filename = @$.end.filename = new std::string("stdin");
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
    %empty                  {}
    | group newlines                {$$ = {$group};}
    | groups group newlines {$$ = $1; $$.insert($group);}
    ;

blocks:  
    %empty                  {}
    | blocks block newlines {$$ = $1; $$.insert($block);}
    ; 

group: 
    IDENTIFIER "=" condition  {$$ = {$IDENTIFIER, $condition};}
    ;

block: 
    IDENTIFIER ":" scope    {$$ = {$IDENTIFIER, $scope};}
    ;

scope: 
    INDENT newlines statements DEDENT newlines {$$ = $statements;}
    ;

statements:
    statement                       {$$ = {$statement};}
    | statements newlines statement {$$ = $1; $$.push_back($statement);}
    ;

statement:
    conditional             {}
    | operation             {}
    | ACCEPT                {$$ = accept;}
    | REJECTION             {$$ = reject;}
    | DO block_reference    {$$ = $block_reference;}
    ;

conditional:
    cases                       {$$ = {$cases, {}};}
    | cases ELSE scope {$$ = {$cases, $scope};}
    ;

cases:
    if_case                 {$$ = {$if_case};}
    | cases or_case   {$$ = $1; $$.insert($or_case);}
    ;

if_case:
    IF condition scope  {$$ = {$condition, $scope};}
    ;

or_case:
    OR condition scope  {$$ = {$condition, $scope};}
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
    NEWLINE 
    | newlines NEWLINE
    | newlines
    ;
%%

void yy::Parser::error(const location_type& l, const std::string& m)
{
    std::cerr << l << ": " << m << std::endl;
}