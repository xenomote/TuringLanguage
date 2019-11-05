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
    condition_t* condition;
    write_t* write;
    travel_t* travel;
}

%locations
%define parse.error verbose

%token INDENT UNINDENT NEWLINE IF OR ELSE ACCEPT REJECTION WRITE GO DO MARK UNMARK MARKED UNMARKED LEFT RIGHT
%token <identifier> IDENTIFIER
%token <symbol>     SYMBOL
%token <number>     NUMBER
%token EQUALS       "="
%token COMMA        ","
%token COLON        ":"

%type <list> groups blocks
%type <group> group
%type <block> block
%type <statement> program statement scope transition else
%type <condition> condition until symbol symbols
%type <write> write
%type <travel> travel
%type <number> repetition
%type <direction> direction reversal
%type <string> string

%start program

%%

program: 
    groups statement blocks         {program = $2;}
    ;

groups: 
    %empty                          {}
    | groups group                  {}
    ;

group: 
    IDENTIFIER "=" symbols          {groups = push($1, $3, groups);}
    ;

blocks: 
    %empty                          {}
    | blocks block                  {}
    ; 


block: 
    IDENTIFIER ":" scope            {blocks = push($1, $3, blocks);}
    ;

symbols: 
    symbols "," symbol              {$$ = join($1, $3);}
    | symbol                        {$$ = $1;}
    ;

symbol: 
    SYMBOL                          {$$ = symbol(false, $1);}
    | MARKED SYMBOL               {$$ = symbol(true, $2);}
    ;

scope: 
    INDENT statement UNINDENT       {$$ = $2;}
    ;

statement:
    write travel transition         {$$ = operation($1, $2, $3);}
    | IF condition scope else       {$$ = conditional($2, $3, $4);}
    | ACCEPT                        {$$ = accept();}
    | REJECTION                     {$$ = reject();}
    ;

else: 
    OR condition scope else         {$$ = conditional($2, $3, $4);}
    | ELSE scope                    {$$ = $2;}
    | statement                     {$$ = $1;}
    ;

write: 
    %empty                                  {$$ = NULL;}
    | MARK                                  {$$ = mark();}
    | UNMARK                                {$$ = unmark();}
    | WRITE string reversal repetition      {$$ = writes(strdup(buffer), $3, $4);}
    ;

travel: 
    GO direction repetition until   {$$ = travel($2, $3, $4);}
    ;

transition: 
    DO IDENTIFIER                   {
                                        statement_t* block = find($2, blocks);
                                        
                                        if (block != NULL)
                                            $$ = block;
                                        else
                                            yyerror("couldnt find block");
                                    }
    | NEWLINE statement             {$$ = $2;}
    ;

direction: 
    LEFT                            {$$ = LEFT_D;}
    | RIGHT                         {$$ = RIGHT_D;}
    ;

string: 
    string SYMBOL                   {
                                        buffer[n++] = $2;
                                        buffer[n] = '\0';
                                    }
    | SYMBOL                        {
                                        n = 0;
                                        buffer[n++] = $1;
                                        buffer[n] = '\0';                                        
                                    }
    ;

reversal: 
    %empty                          {$$ = RIGHT_D;}
    | "backwards"                   {$$ = LEFT_D;}
    ;

repetition: 
    %empty                          {$$ = 1;}
    |  NUMBER "times"               {$$ = $1;}
    ;

until: 
    %empty                          {$$ = NULL;}
    | "until" condition             {$$ = $2;}
    ;

condition:
    symbol                          {$$ = $1;}
    | IDENTIFIER                    {
                                        condition_t* group = find($1, groups);

                                        if (group != NULL)
                                            $$ = group;

                                        else
                                            yyerror("couldnt find group");
                                    }
    | "marked"                      {$$ = marked();}
    | "unmarked"                    {$$ = unmarked();}
    | condition "or" symbol         {$$ = join($1, $3);}
    | condition "or" IDENTIFIER     {
                                        condition_t* group = find($3, groups);

                                        if (group != NULL)
                                            $$ = join($1, group);

                                        else
                                            yyerror("couldnt find group");
                                    }
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