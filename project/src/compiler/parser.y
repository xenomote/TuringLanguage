%{
    #include <stdio.h>
    #include <stdbool.h>
    #include <string.h>
    #include "lex.yy.h"
    #include "syntax.h"

    char n = 0;
    char buffer[256] = "";

    statement_t* program = NULL;

    list_t* groups = NULL;
    list_t* blocks = NULL;
    
    void yyerror(const char*);
    
    extern void yy_push_state(int);
    extern void yy_pop_state();

    extern int yylex();
    extern int yyparse();

    extern FILE* yyin;
%}

%code requires {
    #include <stddef.h>
    #include "syntax.h"
}

%union {
    char symbol;
    int number;
    bool direction;
    char* string;
    char* identifier;

    list_t* list;
    statement_t* statement;
    statement_t** reference;
    condition_t* condition;
    write_t* write;
    travel_t* travel;
}

%locations
%define parse.error verbose

%token INDENT DEDENT NEWLINE
%token IF OR ELSE
%token ACCEPT REJECTION
%token WRITE GO DO
%token MARK UNMARK MARKED UNMARKED
%token LEFT RIGHT
%token UNTIL TIMES BACKWARDS
%token GROUPS BLOCKS

%token <identifier> IDENTIFIER
%token <symbol>     SYMBOL
%token <number>     NUMBER
%token <string>     STRING

%token EQUALS       "="
%token COMMA        ","
%token COLON        ":"

%type <list> groups blocks
%type <group> group
%type <block> block
%type <statement> program statements else_statements scope else
%type <condition> condition until symbol symbols
%type <write> write
%type <travel> travel
%type <reference> transition
%type <number> repetition
%type <direction> direction reversal
%type <string> string

%start program

%%

program:
    groups statements blocks
    ;

groups:
    group newlines1 groups
    | %empty
    ;


group:
    IDENTIFIER EQUALS symbols
    ;

symbols:
    symbols COMMA SYMBOL
    | SYMBOL
    ;

blocks:
    blocks newlines1 block
    | %empty
    ;

block:
    IDENTIFIER COLON indent statements
    ;

statements:
    IF condition indent if_statements
    | write go do
    | ACCEPT ors
    | REJECTION ors
    ;


write:
    WRITE SYMBOL COMMA
    | WRITE STRING reversal repetition COMMA
    | MARK COMMA           
    | UNMARK COMMA
    | %empty
    ;

go:
    GO direction until
    ;

do:
    COMMA DO IDENTIFIER
    | newlines1 statements
    ;


if_statements:
    write go if_do
    | IF condition indent if_statements
    | ACCEPT do_ors
    | REJECTION do_ors
    ;

if_do:
    COMMA DO IDENTIFIER dedent statements
    | COMMA DO IDENTIFIER do_ors
    | if_statements
    | ors
    ;

ors:
    dedent OR condition indent or_statements
    | else
    ;

or_statements:
    IF condition indent if_statements
    | write go or_do
    | ACCEPT dedent
    | REJECTION dedent
    ;

or_do:
    COMMA DO IDENTIFIER dedent statements
    | or_statements
    ;

else:
    dedent ELSE indent else_statements
    | dedent statements
    ;

else_statements:
    IF condition indent if_statements
    | write go else_do
    | ACCEPT dedent
    | REJECTION dedent
    ;

else_do:
    COMMA DO IDENTIFIER dedent statements
    | else_statements
    ;

do_ors:
    dedent OR condition indent do_or_statements
    | dedent newlines statements
    | do_else
    ;

do_or_statements:
    IF condition indent if_statements
    | write go do_or_do
    | ACCEPT do_else
    | REJECTION do_else
    ;

do_or_do:
    COMMA DO IDENTIFIER dedent
    | COMMA DO IDENTIFIER do_else
    | do_or_statements
    | dedent statements
    ;

do_else:
    dedent newlines ELSE indent do_else_statements
    ;

do_else_statements:
    IF condition indent if_statements
    | write go do_else_do
    | ACCEPT DEDENT
    | REJECTION DEDENT 
    ;

do_else_do:
    COMMA DO IDENTIFIER DEDENT DEDENT
    | do_else_statements
    | DEDENT statements
    ;

condition:
    SYMBOL
    | MARKED
    | UNMARKED
    | condition OR SYMBOL
    ;

direction:
    LEFT
    | RIGHT
    ;

reversal:
    BACKWARDS
    | %empty
    ;

repetition:
    NUMBER
    | %empty
    ;

until:
    UNTIL condition
    | %empty
    ;

indent:
    INDENT newlines
    ;

dedent:
    DEDENT NEWLINE
    ;

newlines1:
    NEWLINE newlines
    ;

newlines:
    %empty
    | newlines NEWLINE
    ;
%%

void yyerror(const char* s) {
    fprintf(stderr, "%s line %d, col %d\n", s, yylloc.first_line, yylloc.first_column);
    exit(EXIT_FAILURE);
}

int main(int argc, char** argv) {
    if (argc > 1) {
        FILE* file = fopen(argv[1], "r");
        if (file == NULL) yyerror("could not open file");
        yyin = file;
    }

    yyparse();
}