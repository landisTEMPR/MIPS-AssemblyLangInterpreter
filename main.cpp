/*
File: main.cpp
Author: Brysen Landis
*/

#include <iostream>
#include <vector>
#include <string>
#include "interpreter.h"

void printHelp()
{
    std::cout << "\nMIPS Interpreter\n";
    std::cout << "Usage:\n";
    std::cout << "  ./mips                  Interactive mode\n";
    std::cout << "  ./mips <file>           Load and run\n";
    std::cout << "  ./mips <file> -step     Step through\n\n";
}

int main(int argc, char* argv[])
{
    MIPSInterpreter interpreter;
    
    if (argc == 1)
    {
        printHelp();
        interpreter.runInteractive();
    }
    else if (argc >= 2)
    {
        std::string filename = argv[1];
        
        if (filename == "-h" || filename == "--help")
        {
            printHelp();
            return 0;
        }
        
        interpreter.loadFile(filename);
        
        if (argc > 2 && std::string(argv[2]) == "-step")
        {
            std::cout << "\nStep mode (Enter=next, r=regs, q=quit)\n";
            std::string input;
            
            while (true)
            {
                std::cout << "\n> ";
                std::getline(std::cin, input);
                
                if (input == "q" || input == "quit") break;
                if (input == "r" || input == "regs") {
                    interpreter.displayState();
                } else {
                    interpreter.step();
                }
            }
        }
        else
        {
            interpreter.run();
            std::cout << "\n";
            interpreter.displayState();
        }
    }
    
    return 0;
}
