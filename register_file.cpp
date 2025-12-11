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
    std::cout << "\n╔════════════════════════════════════════════════════════════════════════════════════════════════════╗" << std::endl;
    std::cout << "║                                        REGISTER FILE                                               ║" << std::endl;
    std::cout << "╠═════════════╦═════════════╦═════════════╦═════════════╦═════════════╦═════════════╦═════════════╣" << std::endl;
    
    // Row 1: zero, at, v0, v1, a0, a1, a2
    std::cout << "║ $0 (zero)   ║ $1 (at)     ║ $2 (v0)     ║ $3 (v1)     ║ $4 (a0)     ║ $5 (a1)     ║ $6 (a2)     ║" << std::endl;
    std::cout << "║ " << std::setw(11) << std::right << reg[0] 
              << " ║ " << std::setw(11) << std::right << reg[1]
              << " ║ " << std::setw(11) << std::right << reg[2]
              << " ║ " << std::setw(11) << std::right << reg[3]
              << " ║ " << std::setw(11) << std::right << reg[4]
              << " ║ " << std::setw(11) << std::right << reg[5]
              << " ║ " << std::setw(11) << std::right << reg[6] << " ║" << std::endl;
    std::cout << "╠═════════════╬═════════════╬═════════════╬═════════════╬═════════════╬═════════════╬═════════════╣" << std::endl;
    
    // Row 2: a3, t0-t5
    std::cout << "║ $7 (a3)     ║ $8 (t0)     ║ $9 (t1)     ║ $10 (t2)    ║ $11 (t3)    ║ $12 (t4)    ║ $13 (t5)    ║" << std::endl;
    std::cout << "║ " << std::setw(11) << std::right << reg[7]
              << " ║ " << std::setw(11) << std::right << reg[8]
              << " ║ " << std::setw(11) << std::right << reg[9]
              << " ║ " << std::setw(11) << std::right << reg[10]
              << " ║ " << std::setw(11) << std::right << reg[11]
              << " ║ " << std::setw(11) << std::right << reg[12]
              << " ║ " << std::setw(11) << std::right << reg[13] << " ║" << std::endl;
    std::cout << "╠═════════════╬═════════════╬═════════════╬═════════════╬═════════════╬═════════════╬═════════════╣" << std::endl;
    
    // Row 3: t6, t7, s0-s4
    std::cout << "║ $14 (t6)    ║ $15 (t7)    ║ $16 (s0)    ║ $17 (s1)    ║ $18 (s2)    ║ $19 (s3)    ║ $20 (s4)    ║" << std::endl;
    std::cout << "║ " << std::setw(11) << std::right << reg[14]
              << " ║ " << std::setw(11) << std::right << reg[15]
              << " ║ " << std::setw(11) << std::right << reg[16]
              << " ║ " << std::setw(11) << std::right << reg[17]
              << " ║ " << std::setw(11) << std::right << reg[18]
              << " ║ " << std::setw(11) << std::right << reg[19]
              << " ║ " << std::setw(11) << std::right << reg[20] << " ║" << std::endl;
    std::cout << "╠═════════════╬═════════════╬═════════════╬═════════════╬═════════════╬═════════════╬═════════════╣" << std::endl;
    
    // Row 4: s5, s6, s7, t8, t9, k0, k1
    std::cout << "║ $21 (s5)    ║ $22 (s6)    ║ $23 (s7)    ║ $24 (t8)    ║ $25 (t9)    ║ $26 (k0)    ║ $27 (k1)    ║" << std::endl;
    std::cout << "║ " << std::setw(11) << std::right << reg[21]
              << " ║ " << std::setw(11) << std::right << reg[22]
              << " ║ " << std::setw(11) << std::right << reg[23]
              << " ║ " << std::setw(11) << std::right << reg[24]
              << " ║ " << std::setw(11) << std::right << reg[25]
              << " ║ " << std::setw(11) << std::right << reg[26]
              << " ║ " << std::setw(11) << std::right << reg[27] << " ║" << std::endl;
    std::cout << "╠═════════════╬═════════════╬═════════════╬═════════════╬═════════════╬═════════════╬═════════════╣" << std::endl;
    
    // Row 5: Special registers - gp, sp, fp, ra
    std::cout << "║ $28 (gp)    ║ $29 (sp)    ║ $30 (fp)    ║ $31 (ra)    ║             ║             ║             ║" << std::endl;
    std::cout << "║ " << std::setw(11) << std::right << reg[28]
              << " ║ " << std::setw(11) << std::right << reg[29]
              << " ║ " << std::setw(11) << std::right << reg[30]
              << " ║ " << std::setw(11) << std::right << reg[31]
              << " ║             ║             ║             ║" << std::endl;
    std::cout << "╚═════════════╩═════════════╩═════════════╩═════════════╩═════════════╩═════════════╩═════════════╝" << std::endl;
}
