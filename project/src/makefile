all: lex.yy.c parser.tab.c parser.tab.h
	gcc lex.yy.c parser.tab.c -lfl -o compiler

parser.tab.c parser.tab.h: parser.y
	bison -d parser.y

lex.yy.c: lexer.l parser.tab.h
	flex lexer.l

clean:
	rm lex.yy.c parser.tab.c parser.tab.h parser.tab.cc parser.tab.hh stack.hh