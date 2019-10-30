%{
    #include "parser.tab.h"

    extern void yyerror(const char* s);

    int indent = 0;
    int spaces = 0;

    #define TAB 4
%}


%x SCOPE COMMENT NUMBER

%%

\n              BEGIN (SCOPE);      // checks for the beginning of a scope

<*>"//"[^\n]*                       // discard line comments    
<*>"/*"         BEGIN (COMMENT);    // starts a multiline comment

"."[1-9][[:digit:]]*        {fprintf(stdout, "number\n");return NUMBER;}
[[:alpha:]]+[[:alnum:]]+    {fprintf(stdout, "identifier\n");return IDENTIFIER;}
[[:graph:]]                 {fprintf(stdout, "symbol\n");return SYMBOL;}

<COMMENT>"*/"   BEGIN (INITIAL);    // end multiline comment
<COMMENT>[^*]                       // skip comment body
<COMMENT>"*"                        // skip single *

<SCOPE>" "  {spaces++;}         // increase the indent
<SCOPE>\n   {spaces = 0;}       // reset the indent
<SCOPE>\t   {spaces += TAB;}    // add a tab to the indent
<SCOPE>.    {
                unput(*yytext);
                if (spaces == TAB * indent) BEGIN(INITIAL);

                else if (spaces == TAB * (indent + 1))
                {
                    indent++;
                    return INDENT;
                }
                else if (spaces == TAB * (indent - 1))
                {
                    indent--;
                    return UNINDENT;
                }
                else printf("%s\n","error");// yyerror("mismatched indentation");
            }

%%