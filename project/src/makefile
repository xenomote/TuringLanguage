all: lex.yy.c lex.yy.h parser.tab.c parser.tab.h syntax.c syntax.h
	gcc lex.yy.c lex.yy.h parser.tab.c parser.tab.h syntax.c syntax.h -lfl -g -o compiler.out

parser.tab.c parser.tab.h: parser.y
	bison -d parser.y -W -v -r all

lex.yy.c lex.yy.h: lexer.l parser.tab.h
	flex --header-file=lex.yy.h lexer.l

clean:
	rm lex.yy.c lex.yy.h parser.tab.c parser.tab.h parser.tab.cc parser.tab.hh stack.hh parser lexer