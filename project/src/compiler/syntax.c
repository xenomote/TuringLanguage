#include <stdlib.h>

#include "syntax.h"

list_t* push(char* name, void* value, list_t* list) {
    list_t* node = malloc(sizeof(list_t));
    
    node -> name = name;
    node -> value = value;
    node -> next = list;

    return node;
}

void* find(char* name, list_t* list) {
    while (list != NULL) {
        if (list -> name == name) return list -> value;
        list = list -> next;
    }

    return NULL;
}

condition_t* marked() {
    condition_t* marked = malloc(sizeof(condition_t));

    marked -> type = MARKED_T;

    return marked;
}

condition_t* unmarked() {
    condition_t* unmarked = malloc(sizeof(condition_t));

    unmarked -> type = UNMARKED_T;

    return unmarked;
}

condition_t* symbol(bool marked, char character) {
    condition_t* symbol = malloc(sizeof(condition_t));

    symbol -> type = marked ? MARKED_SYMBOL_T : UNMARKED_SYMBOL_T;
    symbol -> symbol = character;

    return symbol;
}

condition_t* join(condition_t* left, condition_t* right){
    condition_t* join = malloc(sizeof(condition_t));

    join -> type = JOIN_T;
    join -> left = left;
    join -> right = right;

    return join;
}

statement_t* accept() {
    statement_t* accept = malloc(sizeof(statement_t));

    accept -> type = ACCEPT_T;

    return accept;
}

statement_t* reject() {
    statement_t* reject = malloc(sizeof(statement_t));

    reject -> type = REJECT_T;

    return reject;
}

statement_t* conditional(condition_t* condition) {
    statement_t* statement = malloc(sizeof(statement_t));
    conditional_t* conditional = malloc(sizeof(conditional_t));

    statement -> type = CONDITIONAL_T;
    statement -> conditional = conditional;

    conditional -> condition = condition;

    return statement;
}

statement_t* operation(write_t* write, travel_t* travel, statement_t** next) {
    statement_t* statement = malloc(sizeof(statement_t));
    operation_t* operation = malloc(sizeof(operation_t)); 

    statement -> type = OPERATION_T;
    statement -> operation = operation;

    operation -> write = write;
    operation -> travel = travel;
    operation -> next = next;

    return statement;
}

write_t* mark() {
    write_t* mark = malloc(sizeof(write_t));

    mark -> type = MARK_T;

    return mark;
}

write_t* unmark() {
    write_t* unmark = malloc(sizeof(write_t));

    unmark -> type = UNMARK_T;

    return unmark;
}

write_t* writes(char* string, bool reversed, int repetition) {
    write_t* write = malloc(sizeof(write_t));

    write -> type = WRITE_T;
    write -> string = string;
    write -> reversed = reversed;
    write -> repetition = repetition;

    return write;
}

travel_t* travel(bool direction, int repetition, condition_t* until) {
    travel_t* travel = malloc(sizeof(travel_t));

    travel -> direction = direction;
    travel -> repetition = repetition;
    travel -> until = until;

    return travel;
}