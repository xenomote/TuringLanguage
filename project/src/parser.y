%{
    #include <stdio.h>

    void yyerror(const char*);

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
symbols: symbols ',' SYMBOL | SYMBOL;

block: IDENTIFIER ':' scope;

scope: INDENT statement UNINDENT;
statement:
    "if" condition scope or else
    | write travel transition
    ;

or: %empty | "or" condition scope;
else: "else" scope | NEWLINE statement;

write: 
    %empty 
    | "write" string reversal repetition
    ;

travel: 
    %empty
    | "go" direction repetition until
    ;

transition: 
    "accept"
    | "reject"
    | "do" IDENTIFIER
    | NEWLINE statement
    ;

direction: "left" | "right";
string: string SYMBOL | SYMBOL;
reversal: %empty | "backwards";
repetition: %empty | NUMBER "times";
until: %empty | "until" condition;

condition:
    SYMBOL
    | IDENTIFIER
    | condition "or" SYMBOL
    | condition "or" IDENTIFIER
    ;    

%%

void yyerror(const char* s)
{

}