%{
    #include "parser.tab.h"

    extern void yyerror(const char* s);

    int indent = 0;
    int spaces = 0;

    void update_location() {
        yylloc.first_line = yylloc.last_line;
        yylloc.first_column = yylloc.last_column;

        for(int i = 0; yytext[i] != '\0'; i++) {
            if(yytext[i] == '\n') {
                yylloc.last_line++;
                yylloc.last_column = 0;
            }
            else {
                yylloc.last_column++;
            }
        }
    }

    #define TAB 4
    #define YY_USER_ACTION update_location();
%}


%x SCOPE COMMENT NUMBER

%%

\n              {BEGIN (SCOPE); return NEWLINE;}  // checks for the beginning of a scope

<*>"//"[^\n]*                       // discard line comments    
<*>"/*"         BEGIN (COMMENT);    // starts a multiline comment

"="                         return EQUALS;
":"                         return COLON;
","                         return COMMA;
"if"                        return IF;
"or"                        return OR;
"else"                      return ELSE;
"accept"                    return ACCEPT;
"reject"                    return REJECTION;
"write"                     return WRITE;
"go"                        return GO;
"do"                        return DO;
"mark"                      return MARK;
"unmark"                    return UNMARK;
"marked"                    return MARKED;
"unmarked"                  return UNMARKED;
"left"                      return LEFT;
"right"                     return RIGHT;
"until"                     return UNTIL;
"times"                     return TIMES;
"backwards"                 return BACKWARDS;

"."[1-9][[:digit:]]*        return NUMBER;
[[:alpha:]]+[[:alnum:]]+    return IDENTIFIER;
[[:graph:]]                 return SYMBOL;

<COMMENT>"*/"   BEGIN (INITIAL);            // end multiline comment
<COMMENT>[^*]                               // skip comment body
<COMMENT>"*"                                // skip single *

<SCOPE>" "  {spaces++;}                     // increase the indent
<SCOPE>\n   {spaces = 0; return NEWLINE;}   // reset the indent
<SCOPE>\t   {spaces += TAB;}                // add a tab to the indent
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
                else yyerror("mismatched indentation");
            }

%%