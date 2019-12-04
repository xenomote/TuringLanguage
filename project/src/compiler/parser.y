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

%token INDENT UNINDENT NEWLINE
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

%token EQUALS       "="
%token COMMA        ","
%token COLON        ":"

%type <list> groups blocks
%type <group> group
%type <block> block
%type <statement> program statement scope else
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
    groups blocks statement         {
                                        list_t* node = blocks;
                                        
                                        while (node != NULL) {
                                            statement_t** reference = node -> value;
                                            
                                            if (reference == NULL || *reference == NULL)
                                                yyerror("undefined block");

                                            node = node -> next;
                                        }

                                        program = $3;
                                    }
    ;

groups: 
    GROUPS newlines                         {}
    | groups  group newlines       {}
    ;

blocks: 
    BLOCKS newlines                         {}
    | blocks  block newlines    {}
    ; 

group: 
    IDENTIFIER "=" symbols          {groups = push($1, $3, groups);}
    ;

block: 
    IDENTIFIER ":" scope            {
                                        statement_t** reference = find($1, blocks);

                                        if (reference == NULL) {
                                            reference = malloc(sizeof(statement_t*));
                                            blocks = push($1, reference, blocks);
                                        }

                                        *reference = $3;
                                    }
    ;

symbols: 
    symbols "," symbol              {$$ = join($1, $3);}
    | symbol                        {$$ = $1;}
    ;

symbol: 
    SYMBOL                          {$$ = symbol(false, $1);}
    | MARKED SYMBOL                 {$$ = symbol(true, $2);}
    ;

scope: 
    INDENT newlines statement optional_newlines UNINDENT {$$ = $3;}
    ;

optional_newlines:
    %empty
    | newlines
    ;

newlines:
    NEWLINE
    | newlines NEWLINE
    ;

statement:
    write travel transition         {$$ = operation($1, $2, $3);}
    | IF condition scope newlines else   {$$ = conditional($2, $3, $5);}
    | ACCEPT                        {$$ = accept();}
    | REJECTION                     {$$ = reject();}
    ;

else: 
    OR condition scope else         {$$ = conditional($2, $3, $4);}
    | ELSE scope                    {$$ = $2;}
    | statement                     {$$ = $1;}
    ;

write: 
    %empty                                      {$$ = NULL;}
    | MARK COMMA                                {$$ = mark();}
    | UNMARK COMMA                              {$$ = unmark();}
    | WRITE string reversal repetition COMMA    {$$ = writes(strdup(buffer), $3, $4);}
    ;

travel: 
    GO direction repetition until   {$$ = travel($2, $3, $4);}
    ;

transition: 
    newlines statement               {$$ = &$2;}
    | COMMA DO IDENTIFIER           {
                                        statement_t** reference = find($3, blocks);

                                        if (reference == NULL) {
                                            reference = malloc(sizeof(statement_t*));
                                            *reference = NULL;
                                            blocks = push($3, reference, blocks);
                                        }

                                        $$ = reference;
                                    }
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
    | BACKWARDS                     {$$ = LEFT_D;}
    ;

repetition: 
    %empty                          {$$ = 1;}
    |  NUMBER TIMES                 {$$ = $1;}
    ;

until: 
    %empty                          {$$ = NULL;}
    | UNTIL condition               {$$ = $2;}
    ;

condition:
    symbol                          {$$ = $1;}
    | MARKED                        {$$ = marked();}
    | UNMARKED                      {$$ = unmarked();}
    | condition OR symbol           {$$ = join($1, $3);}
    | condition OR IDENTIFIER       {
                                        condition_t* group = find($3, groups);

                                        if (group != NULL)
                                            $$ = join($1, group);

                                        else
                                            yyerror("couldnt find group");
                                    }
    | IDENTIFIER                    {
                                        condition_t* group = find($1, groups);

                                        if (group != NULL)
                                            $$ = group;

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