%{
    #include <stdio.h>
    #include <stdbool.h>
    #include "lex.yy.h"

    struct {
        char symbols[255];
        char marked_symbols[255];
    } symbols;

    struct {
        bool marked;
        char symbol;
    } symbol;

    void yyerror(const char*);

    extern void yy_push_state(int);
    extern void yy_pop_state();

    extern int yylex();
    extern int yyparse();
    
    extern FILE* yyin;
%}

%code requires {
    #include "syntax.h"
}

%union {
    char* identifier;
    char symbol;
    int number;

    program_t* program;
    statement_t* statement;
    group_t* group;
    block_t* block;
    list_t* list;
}

%token INDENT UNINDENT NEWLINE
%token <identifier> IDENTIFIER
%token <symbol>     SYMBOL
%token <number>     NUMBER

%type <list> groups blocks
%type <program> program
%type <statement> statement
%type <group> group
%type <block> block

%start program

%%

program: 
    groups statement blocks         {$$ = program($1, $2, $3);}
    ;

groups: 
    %empty                          {$$ = NULL;}
    | groups group                  {$$ = push($2, $1);}
    ;

blocks: 
    %empty                          {$$ = NULL;}
    | blocks block                  {$$ = push($2, $1);}
    ; 

group: 
    IDENTIFIER '=' symbols          {$$ = group(strdup($1), symbols.symbols, symbols.marked_symbols);}
    ;

symbols: 
    symbols ',' symbol 
    | symbol
    ;

symbol: 
    SYMBOL                          {symbol.marked = false; symbol.symbol = $SYMBOL;}
    | "marked" SYMBOL               {symbol.marked = true; symbol.symbol = $SYMBOL;}
    ;

block: 
    IDENTIFIER ':' scope
    ;

scope: 
    INDENT statement UNINDENT
    ;

statement:
    write travel transition 
    | "if" condition scope or else
    | "accept"
    | "reject"
    ;

or: 
    %empty 
    | "or" condition scope
    ;

else: 
    "else" scope 
    | NEWLINE statement
    ;

write: 
    %empty 
    | "mark"
    | "unmark"
    | "write" string reversal repetition
    ;

travel: 
    "go" direction repetition until
    ;

transition: 
    "do" IDENTIFIER
    | NEWLINE statement
    ;

direction: 
    "left" 
    | "right"
    ;

string: 
    string symbol 
    | symbol
    ;

reversal: 
    %empty 
    | "backwards"
    ;

repetition: 
    %empty 
    |  NUMBER "times"
    ;

until: 
    %empty 
    | "until" condition
    ;

condition:
    symbol
    | IDENTIFIER
    | "marked"
    | "unmarked"
    | condition "or" SYMBOL
    | condition "or" IDENTIFIER
    ;    

%%

void yyerror(const char* s) {
    fprintf(stderr, "%s\n", s);
}