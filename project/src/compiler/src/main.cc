#include <getopt.h>

#include <iostream>
#include <fstream>

#include <memory>

#include "parser.hh"
#include "semantics.hh"
#include "machine.hh"

//extern std::istream yyin;
extern FILE* yyin;

static const char* ARG_STRING = "m:t:d";

bool debug = false;

using namespace std;

int main(int argc, char** argv)
{
    int opt;

    // ifstream machine_file, tape_file;
    // machine_file.exceptions(ifstream::failbit | ifstream::badbit);

    string filename;
    FILE* m;

    while ((opt = getopt(argc, argv, ARG_STRING)) > -1) {
        switch (opt) {
            case 'm':
                //machine_file.open(optarg, ios::in);
                filename = optarg;
                m = fopen(optarg, "r");
                break;

            case 't':
                //tape_file.open(optarg, ios::in);
                break;

            case 'd':
                debug = true;
                break;

            default:
                cerr << "unrecognised option" << endl << opt;
                exit(EXIT_FAILURE);
        }
    }

    //yyin.rdbuf(machine_file.rdbuf());

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

                cout << "success" << endl;
            }

            catch (semantic_error& e) {
                cout << "compilation failed: " << e.what() << endl;
            }
        }

        else cout << "failed to parse" << endl;
        
        fclose(m);
    }

    else cout << "could not open " << filename << endl;

    cout << "done" << endl;
}