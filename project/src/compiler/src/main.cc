#include <getopt.h>
#include <ctype.h>

#include <exception>
#include <iostream>
#include <fstream>
#include <memory>

#include "parser.hh"
#include "semantics.hh"
#include "machine.hh"
#include "generator.hh"

//extern std::istream yyin;
extern FILE* yyin;

static const char* ARG_STRING = "m:t:d";

bool debug = false;

using namespace std;

list<symbol> read_tape(FILE* f)
{
    list<symbol> t;

    while (true) {
        int c;
        
        if ((c = fgetc(f)) == EOF) break;
        if (isspace(c)) continue;
        char sym = c;

        bool mark = false;
        if ((c = fgetc(f)) != EOF && !isspace(c)) {
            if (c == '*') mark = true;
            else throw runtime_error("bad tape format");
        }

        t.emplace_back(symbol {mark, sym});
    }

    return t;
}

int main(int argc, char** argv)
{
    int opt;

    string machine_filename, tape_filename;
    FILE* machine_file, * tape_file;

    while ((opt = getopt(argc, argv, ARG_STRING)) > -1) {
        switch (opt) {
            case 'm':
                machine_filename = optarg;
                machine_file = fopen(optarg, "r");
                break;

            case 't':
                tape_filename = optarg;
                tape_file = fopen(optarg, "r");
                break;

            case 'd':
                debug = true;
                break;

            default:
                cerr << "unrecognised option" << endl << opt;
                exit(EXIT_FAILURE);
        }
    }

    program output;
    list<symbol> tape;

    yy::parser parser(machine_filename, output);
    

    if (machine_file != NULL) yyin = machine_file;
    else {
        cerr << "could not open machine file \"" << machine_filename << "\"" << endl;
        return EXIT_FAILURE;
    }

    if (tape_file != NULL) tape = read_tape(tape_file);
    else {
        cerr << "could not open tape file \"" << tape_filename << "\"" << endl;
        return EXIT_FAILURE;
    }

    if (parser() == 0) {
        try {
            ensure_exit(output);
            ensure_distinct_conditions(output);
            ensure_valid_references(output);

            generator generate(output);

            list<state> states = generate();
            
            machine m(states, tape);
            int steps = 0;

            while (!m.halted()) {
                cout << "step " << steps << ": " << m << endl;
                m.step();
                steps++;
            } 

            cout << "step " << steps << ": " << m << endl;
        }

        catch (semantic_error& e) {
            cerr << "compilation failed: " << e.what() << endl;
        }
    }

    else cerr << "failed to parse" << endl;
        
    fclose(machine_file);
    fclose(tape_file);
}