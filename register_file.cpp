#include "register_file.h"
#include <iostream>
#include <iomanip>

RegisterFile::RegisterFile() 
{
    for (int i = 0; i < 32; i++) 
    {
        reg[i] = 0;
    }
    
    // Initialize regMap with register names
    regMap["zero"] = 0; regMap["at"] = 1;
    regMap["v0"] = 2; regMap["v1"] = 3;
    regMap["a0"] = 4; regMap["a1"] = 5;
    regMap["a2"] = 6; regMap["a3"] = 7;
    regMap["t0"] = 8; regMap["t1"] = 9;
    regMap["t2"] = 10; regMap["t3"] = 11;
    regMap["t4"] = 12; regMap["t5"] = 13;
    regMap["t6"] = 14; regMap["t7"] = 15;
    regMap["s0"] = 16; regMap["s1"] = 17;
    regMap["s2"] = 18; regMap["s3"] = 19;
    regMap["s4"] = 20; regMap["s5"] = 21;
    regMap["s6"] = 22; regMap["s7"] = 23;
    regMap["t8"] = 24; regMap["t9"] = 25;
    regMap["k0"] = 26; regMap["k1"] = 27;
    regMap["gp"] = 28; regMap["sp"] = 29;
    regMap["fp"] = 30; regMap["ra"] = 31;
}

unsigned int RegisterFile::getReg(const std::string& regName) 
{
    std::string cleanName = regName;
    if (cleanName[0] == '$') 
    {
        cleanName = cleanName.substr(1);
    }
    
    if (regMap.find(cleanName) != regMap.end()) 
    {
        return reg[regMap[cleanName]];
    }
    
    // Try numeric register
    if (std::isdigit(cleanName[0]))
    {
        int regNum = std::stoi(cleanName);
        if (regNum >= 0 && regNum < 32)
        {
            return reg[regNum];
        }
    }
    
    std::cerr << "Error: Invalid register name: " << regName << std::endl;
    return 0;
}

void RegisterFile::setReg(const std::string& regName, unsigned int value) 
{
    std::string cleanName = regName;
    if (cleanName[0] == '$') 
    {
        cleanName = cleanName.substr(1);
    }
    
    auto it = regMap.find(cleanName);
    if (it != regMap.end()) 
    {
        if (it->second != 0) // Don't allow writing to $zero
        {
            reg[it->second] = value;
        }
        return;
    }
    
    // Try numeric register
    if (std::isdigit(cleanName[0]))
    {
        int regNum = std::stoi(cleanName);
        if (regNum > 0 && regNum < 32) // Don't allow writing to $0
        {
            reg[regNum] = value;
            return;
        }
    }
    
    std::cerr << "Error: Cannot set register: " << regName << std::endl;
}

unsigned int RegisterFile::getRegByNum(int regNum)
{
    if (regNum >= 0 && regNum < 32)
    {
        return reg[regNum];
    }
    std::cerr << "Error: Invalid register number: " << regNum << std::endl;
    return 0;
}

void RegisterFile::setRegByNum(int regNum, unsigned int value)
{
    if (regNum > 0 && regNum < 32) // Don't allow writing to $0
    {
        reg[regNum] = value;
    }
    else if (regNum == 0)
    {
        // Silently ignore writes to $zero
        return;
    }
    else
    {
        std::cerr << "Error: Invalid register number: " << regNum << std::endl;
    }
}

int RegisterFile::getRegNumber(const std::string& regName)
{
    std::string cleanName = regName;
    if (cleanName[0] == '$') 
    {
        cleanName = cleanName.substr(1);
    }
    
    // Check if it's a named register
    if (regMap.find(cleanName) != regMap.end())
    {
        return regMap[cleanName];
    }
    
    // Check if it's a numeric register
    if (std::isdigit(cleanName[0]))
    {
        return std::stoi(cleanName);
    }
    
    std::cerr << "Error: Invalid register name: " << regName << std::endl;
    return 0;
}

void RegisterFile::displayRegisters() 
{
    std::cout << "\n";
    std::cout << "┌──────────────────────────────────────────────────────────────────────────────────────────────────┐\n";
    std::cout << "│ zero at   v0   v1   a0   a1   a2   a3   t0   t1   t2   t3   t4   t5   t6   t7                    │\n";
    std::cout << "│ ";
    for (int i = 0; i < 16; i++) {
        std::cout << std::setw(4) << reg[i] << " ";
    }
    std::cout << "                 │\n";
    std::cout << "├──────────────────────────────────────────────────────────────────────────────────────────────────┤\n";
    std::cout << "│ s0   s1   s2   s3   s4   s5   s6   s7   t8   t9   k0   k1   gp   sp         fp   ra              │\n";
    std::cout << "│ ";
    for (int i = 16; i < 32; i++) {
        std::cout << std::setw(4) << reg[i] << " ";
    }
    std::cout << "           │\n";
    std::cout << "└──────────────────────────────────────────────────────────────────────────────────────────────────┘\n";
}
