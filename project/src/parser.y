%{
    #include <stdio.h>
    #include "lex.yy.h"

    void yyerror(const char*);

    extern void yy_push_state(int);
    extern void yy_pop_state();

    extern int yylex();
    extern int yyparse();
    
    extern FILE* yyin;
%}

%union {
    char* identifier;
    char symbol;
    int number;
}

%token INDENT UNINDENT NEWLINE
%token <identifier> IDENTIFIER
%token <symbol>     SYMBOL
%token <number>     NUMBER

%start program

%%

program: groups statement blocks;
groups: %empty | groups group;
blocks: %empty | blocks block;

group: IDENTIFIER '=' symbols;
symbols: symbols ',' symbol | symbol;
symbol: SYMBOL | "marked" SYMBOL

block: IDENTIFIER ':' scope;

scope: INDENT statement UNINDENT;
statement:
    write travel transition
    | "if" condition scope or else
    | "accept"
    | "reject"
    ;

or: %empty | "or" condition scope;
else: "else" scope | NEWLINE statement;

write: 
    %empty 
    | "mark"
    | "unmark"
    | "write" string reversal repetition
    ;

travel: "go" direction repetition until;

transition: 
    "do" IDENTIFIER
    | NEWLINE statement
    ;

direction: "left" | "right";
string: string symbol | symbol;
reversal: %empty | "backwards";
repetition: %empty |  NUMBER "times";
until: %empty | "until" condition;

condition:
    symbol
    | IDENTIFIER
    | "marked"
    | "unmarked"
    | condition "or" SYMBOL
    | condition "or" IDENTIFIER
    ;    

%%

void yyerror(const char* s)
{
    fprintf(stderr, "%s\n", s);
}