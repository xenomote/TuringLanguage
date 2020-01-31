%option noyywrap

%{
    #include <iostream>

    #include "lexer.hh"
    #include "parser.yy.hh"

    #define TAB 4

    #define YY_USER_ACTION yylloc->columns (yyleng);
    #define YY_DECL int yylex(              \
        yy::parser::semantic_type* yylval,  \
        yy::parser::location_type* yylloc   \
    )

    typedef yy::parser::token token;
    typedef yy::parser::semantic_type value_type;

    int indentation = 0;
    int spaces = 0;
    int lines = 0;
    bool newline = false;

    int indent();
    int dedent();
%}

%x SCOPE COMMENT

%%

%{
    yylloc -> step();
%}

" "|\t // discard whitespace

\n {
    // mark the beginning of a scope
    spaces = 0;
    lines = 1;
    newline = true;

    yylloc -> lines();

    BEGIN (SCOPE);
}

"="                         return token::EQUALS;
":"                         return token::COLON;
","                         return token::COMMA;
"if"                        return token::IF;
"or"                        return token::OR;
"else"                      return token::ELSE;
"accept"                    return token::ACCEPT;
"reject"                    return token::REJECTION;
"write"                     return token::WRITE;
"go"                        return token::GO;
"do"                        return token::DO;
"mark"                      return token::MARK;
"unmark"                    return token::UNMARK;
"marked"                    return token::MARKED;
"unmarked"                  return token::UNMARKED;
"left"                      return token::LEFT;
"right"                     return token::RIGHT;
"while"                     return token::WHILE;
"until"                     return token::UNTIL;
"times"                     return token::TIMES;
"backwards"                 return token::BACKWARDS;

"."[1-9][[:digit:]]* {
    std::string number = yytext;

    yylval -> emplace<int>(std::stoi(number.substr(1)));
    return token::NUMBER;
}

[[:alpha:]]+[[:alnum:]]+ {
    std::string text = yytext;

    yylval -> emplace<std::string>(text);
    return token::IDENTIFIER;
}

[[:graph:]] {
    yylval -> emplace<char>(yytext[0]);
    return token::SYMBOL;
}

<*>"//"[^\n]*               // discard comments    
<*>"/*"                     BEGIN (COMMENT); // begin multiline comment

<COMMENT>[^*]               // skip comment body
<COMMENT>"*"                // skip single asterix
<COMMENT>"*/"               BEGIN (INITIAL);// end multiline comment

<SCOPE>" "                  spaces++; // increase the indent
<SCOPE>\t                   spaces += TAB; // add a tab to the indent
<SCOPE>\n {
    // reset the indent
    lines++;
    spaces = 0;

    yylloc -> lines();
} 
<SCOPE>. {
    unput(*yytext);
    yylloc -> columns(-1);
    yylloc -> step();


    if (spaces > TAB * indentation)         return indent();
    else if (spaces < TAB * indentation)    return dedent();

    else if (lines > 0) {
        lines--;
        return token::NEWLINE;
    }

    else if (spaces == TAB * indentation) BEGIN(INITIAL);

    else {
        throw yy::parser::syntax_error(*yylloc, "mismatched indentation, tabs must be in groups of " + std::to_string(TAB));
    }
}

<<EOF>> {   
    
    if (indentation > 0) return dedent();

    if (indentation == 0) {
        indentation--;
        return token::NEWLINE;
    }

    return EOF;
}

%%

int dedent()
{
    if (newline) {
        newline = false;
        return token::NEWLINE;
    }
    lines = 0;
    indentation--;
    return token::DEDENT;
}

int indent()
{
    lines = 0;
    indentation++;
    return token::INDENT;
}