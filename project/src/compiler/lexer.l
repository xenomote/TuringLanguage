%{
    #include "parser.tab.h"

    extern void yyerror(const char* s);

    int indent = 0;
    int spaces = 0;
    int lines = 0;

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

    #define ECHO printf("(%d, %d) %s\n", yylloc.first_line, yylloc.first_column, yytext);

    #define TAB 4
    #define YY_USER_ACTION update_location();
%}


%x SCOPE COMMENT NUMBER

%%

" "|\t                      // discard whitespace
\n { 
    BEGIN (SCOPE);
    spaces = 0;
    lines = 1;
}

<*>"//"[^\n]*               // discard comments    
<*>"/*"                     BEGIN (COMMENT); // begin multiline comment

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

<COMMENT>[^*]               // skip comment body
<COMMENT>"*"                // skip single asterix
<COMMENT>"*/"               BEGIN (INITIAL); // end multiline comment

<SCOPE>" "                  spaces++; // increase the indent
<SCOPE>\t                   spaces += TAB; // add a tab to the indent
<SCOPE>\n                   {spaces = 0; lines++;} // reset the indent
<SCOPE>. {
    unput(*yytext);
    yylloc.last_column--;

    if (spaces == TAB * indent && lines == 0) BEGIN(INITIAL);

    else if (spaces % TAB == 0 && spaces/TAB != indent) {
        if (spaces > TAB * indent) {
            indent++;
            return INDENT;
        }

        else {
            indent--;
            return DEDENT;
        }
    }

    else if (lines > 0) {
        lines--;
        return NEWLINE;
    }

    else yyerror("mismatched indentation");
}

<<EOF>> {   
    if (indent > 0) {
        indent--;
        return DEDENT;
    }

    if (indent == 0) {
        indent--;
        return NEWLINE;
    }

    return EOF;
}
%%