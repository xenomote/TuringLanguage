#include "parser.yy.hh"

int main()
{
    yy::Parser parser;
    
    if (parser() == 0) std::cout << "success" << std::endl;
}

// #define ARG_STRING ""

// #include <iostream>
// #include <string>

// #include <getopt.h>

// #include "machine.hh"

// using std::string;
// using std::cin, std::cout, std::cerr, std::endl;

// bool debug = false;

// int main(int argc, char** argv)
// {
//     int opt;
//     std::string tape_file = "";

//     while (opt = getopt(argc, argv, ARG_STRING) > -1) {
//         switch (opt) {
//             case 't':
//                 tape_file = argv[optind];
//                 break;

//             case 'd':
//                 debug = true;
//                 break;

//             default:
//                 cerr << "unrecognised option" << endl << opt;
//                 exit(EXIT_FAILURE);
//         }
//     }

//     //auto machine = new Machine()
// }