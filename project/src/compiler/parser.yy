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

%type <until_loop> until_loop
%type <while_loop> while_loop
%type <int> repetition

%type <direction> travel direction
%type <std::optional<tape_write>> write
%type <std::vector<modifier>> modifiers
%type <modifier> modifier

%type <symbol> symbol
%type <std::set<symbol>> symbols
%type <std::string> reference
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
    %empty                          {$$ = {};}
    | groups group newlines {$$ = $1; $$.insert($group);}
    ;

blocks:  
    %empty                          {$$ = {};}
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
    conditional                     {$$ = {$conditional, @1};}
    | operation NEWLINE             {$$ = {$operation, @1};}
    | ACCEPT NEWLINE                {$$ = {accept, @1};}
    | REJECTION NEWLINE             {$$ = {reject, @1};}
    | DO reference NEWLINE    {$$ = {$reference, @1};}
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
    | symbols               {$$ = {{$symbols}, @1};}
    | condition OR grouping {$$ = $1; $$.value.insert($grouping);}
    | condition OR symbols  {$$ = $1; $$.value.insert($symbols);}
    ;

grouping:
    reference                   {$$ = $reference;}
    | MARKED                    {$$ = marked;}
    | UNMARKED                  {$$ = unmarked;}
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
    %empty                  {$$ = {};}
    | modifiers modifier    {$$ = $1; $$.push_back($modifier);}
    ;

modifier:
    while_loop      {$$ = $1;}
    | until_loop    {$$ = $1;}
    | repetition    {$$ = $1;}
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