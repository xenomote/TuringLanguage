SRC_DIR:=src
INC_DIR:=inc
GEN_DIR:=gen
OBJ_DIR:=obj

GEN_FILES:=parser.cc parser.hh lexer.cc location.hh

SRC:=$(wildcard $(SRC_DIR)/*.cc)
GEN:=$(GEN_FILES:%=$(GEN_DIR)/%)
OBJ:=$(SRC:$(SRC_DIR)/%.cc=$(OBJ_DIR)/%.o) $(OBJ_DIR)/parser.o $(OBJ_DIR)/lexer.o
OUT:=compiler.out

CXX:=g++
CXXFLAGS:=-I$(SRC_DIR) -I$(INC_DIR) -I$(GEN_DIR) 
LDFLAGS:=-lstdc++
CFLAGS:=-std=c++17 -Wall -Wextra -g

YACC:=bison
YFLAGS:=

LEX:=flex
LFLAGS:=

.PHONY: all clean

all: $(OUT)

#DEPENDENCIES

$(OBJ_DIR)/main.o $(OBJ_DIR)/semantics.o: $(GEN_DIR)/parser.cc

$(OUT): $(OBJ)
	$(CXX) $(LDFLAGS) $^ -o $@

#PATTERNS

$(OBJ_DIR)/%.o: $(SRC_DIR)/%.cc | $(OBJ_DIR)
	$(CXX) $(CXXFLAGS) $(CFLAGS) -c $< -o $@

$(OBJ_DIR)/%.o: $(GEN_DIR)/%.cc | $(OBJ_DIR)
	$(CXX) $(CXXFLAGS) $(CFLAGS) -c $< -o $@

#GENERATED SOURCES

$(GEN_DIR)/lexer.cc: lexer.ll | $(GEN_DIR)
	$(LEX) $(LFLAGS) -o $@ $< 

$(GEN_DIR)/parser.cc $(GEN_DIR)/parser.hh: parser.yy | $(GEN_DIR)
	$(YACC) $(YFLAGS) --defines=$(GEN_DIR)/parser.hh -o $(GEN_DIR)/parser.cc $<

#DIRECTORIES

$(OBJ_DIR):
	@mkdir $(OBJ_DIR)

$(GEN_DIR):
	@mkdir $(GEN_DIR)

#CLEANUP

clean:
	$(RM) -d $(OUT) $(OBJ) $(GEN) $(OBJ_DIR) $(GEN_DIR)
