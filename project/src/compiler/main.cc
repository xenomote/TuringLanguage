#include <getopt.h>

#include <iostream>
#include <fstream>

#include "parser.yy.hh"
#include "machine.hh"

//extern std::istream yyin;
extern FILE* yyin;

static const char* ARG_STRING = "m:t:d";

bool debug = true;

int main(int argc, char** argv)
{
    using namespace std;

    int opt;
    ifstream machine_file, tape_file;

    machine_file.exceptions(ifstream::failbit | ifstream::badbit);

    FILE* m;

    while ((opt = getopt(argc, argv, ARG_STRING)) > -1) {
        switch (opt) {
            case 'm':
                //machine_file.open(optarg, ios::in);
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
    
    Statement* program = nullptr;
    yy::Parser parser(program);

    StringGenerator generator;

    if (parser() == 0 && program != nullptr) {
        generator.generate(*program);
        cout << generator.output();
    }

    else {
        cout << "failure";
    }

    cout << "done";
}