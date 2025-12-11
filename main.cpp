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
    std::cout << "\033[2J\033[H";
    std::cout << "╔═════════════════════════════════════════════════════════════════════════════════════╗\n";
    std::cout << "║                                                                                     ║\n";
    std::cout << "║   ███╗   ███╗██╗██████╗ ███████╗    ██╗███╗   ██╗████████╗███████╗██████╗ ██████╗   ║\n";
    std::cout << "║   ████╗ ████║██║██╔══██╗██╔════╝    ██║████╗  ██║╚══██╔══╝██╔════╝██╔══██╗██╔══██╗  ║\n";
    std::cout << "║   ██╔████╔██║██║██████╔╝███████╗    ██║██╔██╗ ██║   ██║   █████╗  ██████╔╝██████╔╝  ║\n";
    std::cout << "║   ██║╚██╔╝██║██║██╔═══╝ ╚════██║    ██║██║╚██╗██║   ██║   ██╔══╝  ██╔══██╗██╔═══╝   ║\n";
    std::cout << "║   ██║ ╚═╝ ██║██║██║     ███████║    ██║██║ ╚████║   ██║   ███████╗██║  ██║██║       ║\n";
    std::cout << "║   ╚═╝     ╚═╝╚═╝╚═╝     ╚══════╝    ╚═╝╚═╝  ╚═══╝   ╚═╝   ╚══════╝╚═╝  ╚═╝╚═╝       ║\n";
    std::cout << "║                                                                                     ║\n";
    std::cout << "║                                    by Brysen Landis                                 ║\n";
    std::cout << "║                                                                                     ║\n";
    std::cout << "╚═════════════════════════════════════════════════════════════════════════════════════╝\n\n";
    std::cout << "  USAGE:\n";
    std::cout << "    ./a.out                 → Interactive mode\n";
    std::cout << "    ./a.out <file>          → Load and run program\n";
    std::cout << "    ./a.out <file> -step    → Step through execution\n\n";
    std::cout << "Press Enter to start interactive mode...";
    std::cin.get();
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
            std::cout << "\033[2J\033[H";
            std::cout << "╔════════════════════════════════════════════════════════════════════════════════════════════════════╗\n";
            std::cout << "║                                                                                                    ║\n";
            std::cout << "║   ███╗   ███╗██╗██████╗ ███████╗    ██╗███╗   ██╗████████╗███████╗██████╗ ██████╗                ║\n";
            std::cout << "║   ████╗ ████║██║██╔══██╗██╔════╝    ██║████╗  ██║╚══██╔══╝██╔════╝██╔══██╗██╔══██╗               ║\n";
            std::cout << "║   ██╔████╔██║██║██████╔╝███████╗    ██║██╔██╗ ██║   ██║   █████╗  ██████╔╝██████╔╝               ║\n";
            std::cout << "║   ██║╚██╔╝██║██║██╔═══╝ ╚════██║    ██║██║╚██╗██║   ██║   ██╔══╝  ██╔══██╗██╔═══╝                ║\n";
            std::cout << "║   ██║ ╚═╝ ██║██║██║     ███████║    ██║██║ ╚████║   ██║   ███████╗██║  ██║██║                    ║\n";
            std::cout << "║   ╚═╝     ╚═╝╚═╝╚═╝     ╚══════╝    ╚═╝╚═╝  ╚═══╝   ╚═╝   ╚══════╝╚═╝  ╚═╝╚═╝                    ║\n";
            std::cout << "║                                                                                                    ║\n";
            std::cout << "║                                       STEP MODE                                                    ║\n";
            std::cout << "║                                                                                                    ║\n";
            std::cout << "╚════════════════════════════════════════════════════════════════════════════════════════════════════╝\n\n";
            
            interpreter.displayState();
            std::cout << "\nPress Enter=next, r=regs, q=quit\n";
            std::string input;
            
            while (true)
            {
                std::cout << "\n> ";
                std::getline(std::cin, input);
                
                if (input == "q" || input == "quit") break;
                if (input == "r" || input == "regs") {
                    std::cout << "\033[2J\033[H";
                    std::cout << "╔════════════════════════════════════════════════════════════════════════════════════════════════════╗\n";
                    std::cout << "║                                       STEP MODE                                                    ║\n";
                    std::cout << "╚════════════════════════════════════════════════════════════════════════════════════════════════════╝\n\n";
                    interpreter.displayState();
                    std::cout << "\nPress Enter=next, r=regs, q=quit\n";
                } else {
                    interpreter.step();
                    std::cout << "\033[2J\033[H";
                    std::cout << "╔════════════════════════════════════════════════════════════════════════════════════════════════════╗\n";
                    std::cout << "║                                       STEP MODE                                                    ║\n";
                    std::cout << "╚════════════════════════════════════════════════════════════════════════════════════════════════════╝\n\n";
                    interpreter.displayState();
                    std::cout << "\nPress Enter=next, r=regs, q=quit\n";
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
