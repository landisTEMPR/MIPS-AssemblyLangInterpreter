/*****************************************
File : main.cpp
Author : Brysen Landis

Description : MIPS Interpreter for CISS360 
******************************************/

#include <iostream>
#include <vector>
#include <string>
#include "interpreter.h"

void printHelp()
{
    std::cout << "\n==================================" << std::endl;
    std::cout << "MIPS Interpreter - Brysen Landis" << std::endl;
    std::cout << "==================================" << std::endl;
    std::cout << "\nUsage:" << std::endl;
    std::cout << "  ./mips                    - Interactive mode" << std::endl;
    std::cout << "  ./mips <filename>         - Load and run file" << std::endl;
    std::cout << "  ./mips <filename> -step   - Load file and step through" << std::endl;
    std::cout << "\nInteractive Commands:" << std::endl;
    std::cout << "  load <file>    - Load MIPS assembly file" << std::endl;
    std::cout << "  run            - Execute all instructions" << std::endl;
    std::cout << "  step           - Execute one instruction" << std::endl;
    std::cout << "  regs           - Display register contents" << std::endl;
    std::cout << "  reset          - Reset interpreter state" << std::endl;
    std::cout << "  help           - Show this help" << std::endl;
    std::cout << "  quit/exit      - Exit interpreter" << std::endl;
    std::cout << "\nYou can also enter MIPS instructions directly!\n" << std::endl;
}

int main(int argc, char* argv[])
{
    MIPSInterpreter interpreter;
    
    if (argc == 1)
    {
        // Interactive mode
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
            // Step mode
            std::cout << "\nStepping mode. Press Enter to execute next instruction, 'q' to quit." << std::endl;
            std::string input;
            
            while (true)
            {
                std::cout << "\nPress Enter for next instruction (or 'r' for regs, 'q' to quit): ";
                std::getline(std::cin, input);
                
                if (input == "q" || input == "quit")
                {
                    break;
                }
                else if (input == "r" || input == "regs")
                {
                    interpreter.displayState();
                }
                else
                {
                    interpreter.step();
                }
            }
        }
        else
        {
            // Run mode
            interpreter.run();
            std::cout << "\n=== Final State ===" << std::endl;
            interpreter.displayState();
        }
    }
    
    return 0;
}
