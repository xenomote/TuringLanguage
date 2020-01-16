CXX=g++
CXXFLAGS=-std=c++17 -Wall -Wextra -g -fmax-errors=5
LINK.o=$(LINK.cc)

YACC=bison
YFLAGS=

LEX=flex
LFLAGS=

OBJECTS=main.o compiler.o lexer.o parser.o

#DEPENDENCIES

compiler: $(OBJECTS)

main.o: parser.yy.hh

compiler.o: compiler.hh parser.hh lexer.hh

parser.o: parser.hh lexer.hh

lexer.o: lexer.hh parser.yy.hh

#CLEANUP

clean:
	rm compiler stack.hh position.hh location.hh *.yy.* *.ll.* *.o *.gch

#PATTERNS

%.yy:
%.ll:

%.ll.cc: %.ll
	$(LEX) $(LFLAGS) -o $*.ll.cc $< 

%.yy.cc %.yy.hh: %.yy
	$(YACC) $(YFLAGS) --defines=$*.yy.hh -o $*.yy.cc $<

%.o: %.yy.cc
	$(CXX) $(CXXFLAGS) -c -o $@ $<

%.o: %.ll.cc
	$(CXX) $(CXXFLAGS) -c -o $@ $<

%.o: %.cc
	$(CXX) $(CXXFLAGS) -c -o $@ $<