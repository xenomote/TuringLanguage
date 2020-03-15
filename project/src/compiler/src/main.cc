#include <getopt.h>

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

int main(int argc, char** argv)
{
    int opt;

    string filename;
    FILE* m;

    while ((opt = getopt(argc, argv, ARG_STRING)) > -1) {
        switch (opt) {
            case 'm':
                filename = optarg;
                m = fopen(optarg, "r");
                break;

            case 't':
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

    yy::parser parser(filename, output);
    if (debug) {
        parser.set_debug_stream(cout);
        parser.set_debug_level(true);
    }

    if (m != NULL) {
        yyin = m;
        
        if (parser() == 0) {
            try {
                ensure_exit(output);
                ensure_distinct_conditions(output);
                ensure_valid_references(output);

                generator generate(output);

                list<state> states = generate();
                list<symbol> tape = {{false, '0'},{false, '0'},{false, '0'},{false, '0'}};
                
                machine m(states, tape);
                int c = 0;

                while (!m.halted()) {
                    cout << "step " << c << ": " << m << endl;
                    m.step();
                    c++;
                } 

                cout << "step " << c << ": " << m << endl;
            }

            catch (semantic_error& e) {
                cerr << "compilation failed: " << e.what() << endl;
            }
        }

        else cerr << "failed to parse" << endl;
        
        fclose(m);
    }

    else cerr << "could not open " << filename << endl;
}