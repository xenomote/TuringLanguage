#include <getopt.h>

#include <iostream>
#include <fstream>

#include "parser.yy.hh"
#include "machine.hh"

//extern std::istream yyin;
extern FILE* yyin;

static const char* ARG_STRING = "m:t:d";

bool debug = false;

int main(int argc, char** argv)
{
    using namespace std;

    int opt;

    ifstream machine_file, tape_file;
    machine_file.exceptions(ifstream::failbit | ifstream::badbit);

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
                tape_file.open(optarg, ios::in);
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
    yyin = m;
    
    program output;

    yy::parser parser(filename, output);

    if (debug) {
        parser.set_debug_stream(cout);
        parser.set_debug_level(true);
    }

    if (parser() == 0) {
        cout << "success" << endl;

        ensure_exit(output);
    }

    else {
        cout << "failure" << endl;
    }

    cout << "done" << endl;
}