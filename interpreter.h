/*
File: interpreter.h
Author: Brysen Landis
*/

#ifndef INTERPRETER_H
#define INTERPRETER_H

#include <iostream>
#include <vector>
#include <string>
#include <map>
#include <sstream>
#include <fstream>
#include <algorithm>
#include <iomanip>
#include "register_file.h"
#include "memory.h"

class MIPSInterpreter
{
public:
    MIPSInterpreter();
    
    // Main execution modes
    void runInteractive();
    void runManualMode();
    void loadFile(const std::string& filename);
    void run();  // Run all instructions
    void step(); // Execute one instruction
    void displayState();
    void reset();
    
private:
    RegisterFile regFile;
    Memory mem;
    
    unsigned int PC;
    unsigned int HI, LO;
    
    std::vector<std::string> textSegment;
    std::map<std::string, unsigned int> labels;
    std::map<unsigned int, std::string> addressToInstruction;
    
    // Memory addresses
    static const unsigned int TEXT_BASE = 0x00400000;
    static const unsigned int DATA_BASE = 0x10010000;
    static const unsigned int STACK_BASE = 0x7ffffffc;
    
    unsigned int currentDataAddr;
    bool inDataSection;
    bool halted;
    
    // Parsing functions
    std::vector<std::string> tokenize(const std::string& line);
    std::string cleanLine(const std::string& line);
    void parseFile(const std::string& filename);
    void processDataDirective(const std::vector<std::string>& tokens);
    
    // Instruction execution
    void executeInstruction(const std::string& instr);
    void executeRType(const std::vector<std::string>& tokens);
    void executeIType(const std::vector<std::string>& tokens);
    void executeJType(const std::vector<std::string>& tokens);
    void executePseudoInstruction(const std::vector<std::string>& tokens);
    void executeSyscall();
    
    // Helper functions
    int parseImmediate(const std::string& str);
    int getRegisterNumber(const std::string& regName);
    bool isLabel(const std::string& token);
    unsigned int getLabelAddress(const std::string& label);
    
    void clearScreen();
    void printBanner(const std::string& mode);
};

#endif
