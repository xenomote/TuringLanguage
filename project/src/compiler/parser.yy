%require "3.4.1"

%language "c++"

%define parse.assert
%define parse.error verbose
%define parse.trace

%locations
%defines

%define api.parser.class {parser}

%parse-param {std::string& filename} {program& result}

%code top
{
    #include <string>
    #include <sstream>
    #include <vector>
    #include <map>
    #include <memory>
}

%code requires
{
    #include <memory>
    #include "syntax.hh"
}

%code
{
    extern int yylex(
        yy::parser::semantic_type* yylval,
        yy::parser::location_type* yylloc
    );

    std::set<symbol> symbols;

    void register_symbol(char c) {
        symbols.insert({true, c});
        symbols.insert({false, c});
    }
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
%type <std::map<std::string, statement_list>> blocks

%type <std::pair<std::string, condition>> group
%type <std::pair<std::string, statement_list>> block

%type <statement> statement
%type <statement_list> statements scope

%type <operation> operation
%type <conditional> conditional

%type <std::vector<std::pair<condition, statement_list>>> cases
%type <std::pair<condition, statement_list>> if_case or_case
%type <std::optional<statement_list>> else_case

%type <condition> condition
%type <grouping> grouping

%type <direction> travel direction
%type <std::optional<tape_write>> write
%type <std::list<modifier>> modifiers
%type <modifier> modifier

%type <loop> loop
%type <int> repetition

%type <symbol> symbol
%type <std::string> reference
%type <int> NUMBER

%initial-action {
    @$.begin.filename = @$.end.filename = &filename;
    register_symbol('_');
}

%start program

%%

program:
    groups statements blocks 
    {        
        $$ = {$statements, $groups, $blocks, symbols};
        result = $$;
    }
    ;

groups: 
    %empty                  {$$ = {};}
    | groups group newlines {$$ = $1; $$.insert($group);}
    ;

blocks:  
    %empty                  {$$ = {};}
    | blocks block newlines {$$ = $1; $$.insert($block);}
    ; 

group: 
    IDENTIFIER "=" condition NEWLINE    {$$ = {$IDENTIFIER, $condition};}
    ;

block: 
    IDENTIFIER ":" scope    {$$ = {$IDENTIFIER, $scope};}
    ;

scope: 
    INDENT newlines statements DEDENT    {$$ = $statements;}
    ;

statements:
    statement newlines              {$$ = {$statement};}
    | statements statement newlines {$$ = $1; $$.push_back($statement);}
    ;

statement:
    conditional             {$$ = {$conditional, @1};}
    | operation NEWLINE     {$$ = {$operation, @1};}
    | ACCEPT NEWLINE        {$$ = {accept, @1};}
    | REJECTION NEWLINE     {$$ = {reject, @1};}
    | DO reference NEWLINE  {$$ = {$reference, @1};}
    ;

conditional:
    cases else_case {$$ = {$cases, $else_case};}
    ;

cases:
    if_case         {$$ = {$if_case};}
    | cases or_case {$$ = $1; $$.push_back($or_case);}
    ;

if_case:
    IF condition scope  {$$ = {$condition, $scope};}
    ;

or_case:
    OR condition scope  {$$ = {$condition, $scope};}
    ;

else_case:
    %empty          {$$ = {};}
    | ELSE scope    {$$ = $scope;}
    ;

condition:
    grouping                {$$ = {{$grouping}, @1};}
    | condition "," grouping {$$ = $1; $$.value.insert($grouping);}
    ;

grouping:
    reference   {$$ = $reference;}
    | symbol    {$$ = $symbol;}
    | MARKED    {$$ = {marked};}
    | UNMARKED  {$$ = {unmarked};}
    ;

symbol: 
    SYMBOL          {$$ = {false, $SYMBOL}; register_symbol($SYMBOL);}
    | MARKED SYMBOL {$$ = {true, $SYMBOL}; register_symbol($SYMBOL);}
    ;

operation:
    write travel modifiers  {$$ = {$travel, $write, $modifiers, @1.begin.line};}
    ;

write: 
    %empty                  {$$ = {};}
    | MARK COMMA          {$$ = true;}
    | UNMARK COMMA        {$$ = false;}
    | WRITE symbol COMMA   {$$ = $symbol;}
    ;

travel: 
    GO direction    {$$ = $direction;}
    ;

modifiers:
    %empty                  {$$ = {};}
    | modifiers modifier    {$$ = $1; $$.push_back($modifier);}
    ;

modifier:
    loop {$$ = $loop;}
    | NUMBER {$$ = $NUMBER;}
    ;

loop:
    UNTIL condition {$$ = {$condition, true};}
    | WHILE condition {$$ = {$condition, false};}
    ;

repetition: 
    TIMES NUMBER {$$ = 1;}
    ;

direction: 
    LEFT    {$$ = left;}
    | RIGHT {$$ = right;}
    ;

reference:
    IDENTIFIER {$$ = $IDENTIFIER;}
    ;

newlines:
    %empty
    | newlines NEWLINE
    ;
%%

void yy::parser::error(const location_type& l, const std::string& m)
{
    std::cerr << l << ": " << m << std::endl;
}