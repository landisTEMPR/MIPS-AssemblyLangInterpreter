/*
File: interpreter.cpp
Author: Brysen Landis
*/

#include "interpreter.h"
#include <cctype>

MIPSInterpreter::MIPSInterpreter() 
    : PC(TEXT_BASE), HI(0), LO(0), currentDataAddr(DATA_BASE), 
      inDataSection(false), halted(false)
{
    regFile.setReg("$sp", STACK_BASE);
}

void MIPSInterpreter::clearScreen()
{
    std::cout << "\033[2J\033[H";
}

void MIPSInterpreter::printBanner(const std::string& mode)
{
    std::cout << "╔═════════════════════════════════════════════════════════════════════════════════════╗\n";
    std::cout << "║                                                                                     ║\n";
    std::cout << "║   ███╗   ███╗██╗██████╗ ███████╗    ██╗███╗   ██╗████████╗███████╗██████╗ ██████╗   ║\n";
    std::cout << "║   ████╗ ████║██║██╔══██╗██╔════╝    ██║████╗  ██║╚══██╔══╝██╔════╝██╔══██╗██╔══██╗  ║\n";
    std::cout << "║   ██╔████╔██║██║██████╔╝███████╗    ██║██╔██╗ ██║   ██║   █████╗  ██████╔╝██████╔╝  ║\n";
    std::cout << "║   ██║╚██╔╝██║██║██╔═══╝ ╚════██║    ██║██║╚██╗██║   ██║   ██╔══╝  ██╔══██╗██╔═══╝   ║\n";
    std::cout << "║   ██║ ╚═╝ ██║██║██║     ███████║    ██║██║ ╚████║   ██║   ███████╗██║  ██║██║       ║\n";
    std::cout << "║   ╚═╝     ╚═╝╚═╝╚═╝     ╚══════╝    ╚═╝╚═╝  ╚═══╝   ╚═╝   ╚══════╝╚═╝  ╚═╝╚═╝       ║\n";
    std::cout << "║                                                                                     ║\n";
    std::cout << "║                                      " << std::setw(16) << std::left << mode << "                               ║\n";
    std::cout << "║                                                                                     ║\n";
    std::cout << "╚═════════════════════════════════════════════════════════════════════════════════════╝\n\n";
}


std::string MIPSInterpreter::cleanLine(const std::string& line)
{
    // Remove comments
    size_t commentPos = line.find('#');
    std::string cleaned = (commentPos != std::string::npos) ? line.substr(0, commentPos) : line;
    
    // Trim whitespace
    size_t start = cleaned.find_first_not_of(" \t\r\n");
    size_t end = cleaned.find_last_not_of(" \t\r\n");
    
    if (start == std::string::npos) return "";
    return cleaned.substr(start, end - start + 1);
}

std::vector<std::string> MIPSInterpreter::tokenize(const std::string& line)
{
    std::vector<std::string> tokens;
    std::stringstream ss(line);
    std::string token;
    
    while (ss >> token)
    {
        // Remove commas
        token.erase(std::remove(token.begin(), token.end(), ','), token.end());
        
        // Handle parentheses for memory operations: offset($reg)
        size_t parenPos = token.find('(');
        if (parenPos != std::string::npos)
        {
            std::string offset = token.substr(0, parenPos);
            std::string reg = token.substr(parenPos + 1, token.find(')') - parenPos - 1);
            if (!offset.empty()) tokens.push_back(offset);
            tokens.push_back(reg);
        }
        else
        {
            tokens.push_back(token);
        }
    }
    return tokens;
}

void MIPSInterpreter::parseFile(const std::string& filename)
{
    std::ifstream file(filename);
    if (!file.is_open())
    {
        std::cerr << "Error: Cannot open file " << filename << std::endl;
        return;
    }
    
    std::string line;
    unsigned int instructionCount = 0;
    
    while (std::getline(file, line))
    {
        std::string cleaned = cleanLine(line);
        if (cleaned.empty()) continue;
        
        // Check for section directives
        if (cleaned == ".text")
        {
            inDataSection = false;
            continue;
        }
        else if (cleaned == ".data")
        {
            inDataSection = true;
            continue;
        }
        else if (cleaned.substr(0, 6) == ".globl" || cleaned.substr(0, 7) == ".global")
        {
            // Ignore global directive
            continue;
        }
        
        // Check for labels
        size_t colonPos = cleaned.find(':');
        if (colonPos != std::string::npos)
        {
            std::string label = cleaned.substr(0, colonPos);
            if (inDataSection)
            {
                labels[label] = currentDataAddr;
            }
            else
            {
                labels[label] = TEXT_BASE + (instructionCount * 4);
            }
            
            // Process rest of line after label
            if (colonPos + 1 < cleaned.length())
            {
                cleaned = cleanLine(cleaned.substr(colonPos + 1));
                if (cleaned.empty()) continue;
            }
            else
            {
                continue;
            }
        }
        
        if (inDataSection)
        {
            std::vector<std::string> tokens = tokenize(cleaned);
            processDataDirective(tokens);
        }
        else
        {
            textSegment.push_back(cleaned);
            addressToInstruction[TEXT_BASE + (instructionCount * 4)] = cleaned;
            instructionCount++;
        }
    }
    
    file.close();
    PC = TEXT_BASE;
    
    std::cout << "Loaded " << instructionCount << " instructions from " << filename << std::endl;
    std::cout << "Found " << labels.size() << " labels" << std::endl;
}

void MIPSInterpreter::processDataDirective(const std::vector<std::string>& tokens)
{
    if (tokens.empty()) return;
    
    if (tokens[0] == ".word")
    {
        for (size_t i = 1; i < tokens.size(); i++)
        {
            int value = parseImmediate(tokens[i]);
            mem.storeWord(currentDataAddr, value);
            currentDataAddr += 4;
        }
    }
    else if (tokens[0] == ".byte")
    {
        for (size_t i = 1; i < tokens.size(); i++)
        {
            int value = parseImmediate(tokens[i]);
            mem.store(currentDataAddr, static_cast<unsigned char>(value));
            currentDataAddr += 1;
        }
    }
    else if (tokens[0] == ".asciiz" || tokens[0] == ".ascii")
    {
        // Find the string between quotes
        std::string fullLine;
        for (size_t i = 1; i < tokens.size(); i++)
        {
            fullLine += tokens[i];
            if (i < tokens.size() - 1) fullLine += " ";
        }
        
        size_t start = fullLine.find('"');
        size_t end = fullLine.rfind('"');
        if (start != std::string::npos && end != std::string::npos && start < end)
        {
            std::string str = fullLine.substr(start + 1, end - start - 1);
            
            // Process escape sequences
            for (size_t i = 0; i < str.length(); i++)
            {
                if (str[i] == '\\' && i + 1 < str.length())
                {
                    char nextChar = str[i + 1];
                    if (nextChar == 'n')
                    {
                        mem.store(currentDataAddr++, '\n');
                        i++;
                    }
                    else if (nextChar == 't')
                    {
                        mem.store(currentDataAddr++, '\t');
                        i++;
                    }
                    else if (nextChar == '0')
                    {
                        mem.store(currentDataAddr++, '\0');
                        i++;
                    }
                }
                else
                {
                    mem.store(currentDataAddr++, str[i]);
                }
            }
            
            if (tokens[0] == ".asciiz")
            {
                mem.store(currentDataAddr++, '\0');
            }
        }
    }
    else if (tokens[0] == ".space")
    {
        if (tokens.size() > 1)
        {
            int space = parseImmediate(tokens[1]);
            currentDataAddr += space;
        }
    }
}

int MIPSInterpreter::parseImmediate(const std::string& str)
{
    if (str.empty()) return 0;
    
    // Check for label reference
    if (labels.find(str) != labels.end())
    {
        return static_cast<int>(labels[str]);
    }
    
    // Handle hex
    if (str.substr(0, 2) == "0x" || str.substr(0, 2) == "0X")
    {
        return static_cast<int>(std::stoul(str, nullptr, 16));
    }
    
    // Handle decimal (including negative)
    return std::stoi(str);
}

int MIPSInterpreter::getRegisterNumber(const std::string& regName)
{
    std::string clean = regName;
    if (clean[0] == '$') clean = clean.substr(1);
    
    // Check if it's a number
    if (std::isdigit(clean[0]))
    {
        return std::stoi(clean);
    }
    
    // Use register file map
    return regFile.getRegNumber(clean);
}

bool MIPSInterpreter::isLabel(const std::string& token)
{
    return labels.find(token) != labels.end();
}

unsigned int MIPSInterpreter::getLabelAddress(const std::string& label)
{
    if (labels.find(label) != labels.end())
    {
        return labels[label];
    }
    std::cerr << "Error: Label " << label << " not found" << std::endl;
    return PC;
}

void MIPSInterpreter::executeInstruction(const std::string& instr)
{
    if (halted) return;
    
    std::vector<std::string> tokens = tokenize(instr);
    if (tokens.empty()) return;
    
    std::string opcode = tokens[0];
    
    // R-Type instructions
    if (opcode == "add" || opcode == "addu" || opcode == "sub" || opcode == "subu" ||
        opcode == "and" || opcode == "or" || opcode == "xor" || opcode == "nor" ||
        opcode == "slt" || opcode == "sltu" || opcode == "sll" || opcode == "srl" ||
        opcode == "sra" || opcode == "sllv" || opcode == "srlv" || opcode == "srav" ||
        opcode == "mult" || opcode == "multu" || opcode == "div" || opcode == "divu" ||
        opcode == "mfhi" || opcode == "mflo" || opcode == "mthi" || opcode == "mtlo" ||
        opcode == "jr" || opcode == "jalr")
    {
        executeRType(tokens);
    }
    // I-Type instructions
    else if (opcode == "addi" || opcode == "addiu" || opcode == "andi" || opcode == "ori" ||
             opcode == "xori" || opcode == "slti" || opcode == "sltiu" ||
             opcode == "lw" || opcode == "lh" || opcode == "lhu" || opcode == "lb" ||
             opcode == "lbu" || opcode == "sw" || opcode == "sh" || opcode == "sb" ||
             opcode == "beq" || opcode == "bne" || opcode == "blt" || opcode == "ble" ||
             opcode == "bgt" || opcode == "bge" || opcode == "bltz" || opcode == "blez" ||
             opcode == "bgtz" || opcode == "bgez" || opcode == "lui")
    {
        executeIType(tokens);
    }
    // J-Type instructions
    else if (opcode == "j" || opcode == "jal")
    {
        executeJType(tokens);
    }
    // Special instructions
    else if (opcode == "syscall")
    {
        executeSyscall();
        PC += 4;
    }
    else if (opcode == "nop")
    {
        PC += 4;
    }
    // Pseudo-instructions
    else if (opcode == "li" || opcode == "la" || opcode == "move" || 
             opcode == "clear" || opcode == "not")
    {
        executePseudoInstruction(tokens);
    }
    else
    {
        std::cerr << "Warning: Unsupported instruction: " << opcode << std::endl;
        PC += 4;
    }
}

void MIPSInterpreter::executeRType(const std::vector<std::string>& tokens)
{
    std::string opcode = tokens[0];
    
    if (opcode == "add" || opcode == "addu")
    {
        int rd = getRegisterNumber(tokens[1]);
        int rs = getRegisterNumber(tokens[2]);
        int rt = getRegisterNumber(tokens[3]);
        regFile.setRegByNum(rd, regFile.getRegByNum(rs) + regFile.getRegByNum(rt));
        PC += 4;
    }
    else if (opcode == "sub" || opcode == "subu")
    {
        int rd = getRegisterNumber(tokens[1]);
        int rs = getRegisterNumber(tokens[2]);
        int rt = getRegisterNumber(tokens[3]);
        regFile.setRegByNum(rd, regFile.getRegByNum(rs) - regFile.getRegByNum(rt));
        PC += 4;
    }
    else if (opcode == "and")
    {
        int rd = getRegisterNumber(tokens[1]);
        int rs = getRegisterNumber(tokens[2]);
        int rt = getRegisterNumber(tokens[3]);
        regFile.setRegByNum(rd, regFile.getRegByNum(rs) & regFile.getRegByNum(rt));
        PC += 4;
    }
    else if (opcode == "or")
    {
        int rd = getRegisterNumber(tokens[1]);
        int rs = getRegisterNumber(tokens[2]);
        int rt = getRegisterNumber(tokens[3]);
        regFile.setRegByNum(rd, regFile.getRegByNum(rs) | regFile.getRegByNum(rt));
        PC += 4;
    }
    else if (opcode == "xor")
    {
        int rd = getRegisterNumber(tokens[1]);
        int rs = getRegisterNumber(tokens[2]);
        int rt = getRegisterNumber(tokens[3]);
        regFile.setRegByNum(rd, regFile.getRegByNum(rs) ^ regFile.getRegByNum(rt));
        PC += 4;
    }
    else if (opcode == "nor")
    {
        int rd = getRegisterNumber(tokens[1]);
        int rs = getRegisterNumber(tokens[2]);
        int rt = getRegisterNumber(tokens[3]);
        regFile.setRegByNum(rd, ~(regFile.getRegByNum(rs) | regFile.getRegByNum(rt)));
        PC += 4;
    }
    else if (opcode == "slt")
    {
        int rd = getRegisterNumber(tokens[1]);
        int rs = getRegisterNumber(tokens[2]);
        int rt = getRegisterNumber(tokens[3]);
        int val1 = static_cast<int>(regFile.getRegByNum(rs));
        int val2 = static_cast<int>(regFile.getRegByNum(rt));
        regFile.setRegByNum(rd, (val1 < val2) ? 1 : 0);
        PC += 4;
    }
    else if (opcode == "sltu")
    {
        int rd = getRegisterNumber(tokens[1]);
        int rs = getRegisterNumber(tokens[2]);
        int rt = getRegisterNumber(tokens[3]);
        regFile.setRegByNum(rd, (regFile.getRegByNum(rs) < regFile.getRegByNum(rt)) ? 1 : 0);
        PC += 4;
    }
    else if (opcode == "sll")
    {
        int rd = getRegisterNumber(tokens[1]);
        int rt = getRegisterNumber(tokens[2]);
        int shamt = parseImmediate(tokens[3]);
        regFile.setRegByNum(rd, regFile.getRegByNum(rt) << shamt);
        PC += 4;
    }
    else if (opcode == "srl")
    {
        int rd = getRegisterNumber(tokens[1]);
        int rt = getRegisterNumber(tokens[2]);
        int shamt = parseImmediate(tokens[3]);
        regFile.setRegByNum(rd, regFile.getRegByNum(rt) >> shamt);
        PC += 4;
    }
    else if (opcode == "sra")
    {
        int rd = getRegisterNumber(tokens[1]);
        int rt = getRegisterNumber(tokens[2]);
        int shamt = parseImmediate(tokens[3]);
        int val = static_cast<int>(regFile.getRegByNum(rt));
        regFile.setRegByNum(rd, static_cast<unsigned int>(val >> shamt));
        PC += 4;
    }
    else if (opcode == "mult")
    {
        int rs = getRegisterNumber(tokens[1]);
        int rt = getRegisterNumber(tokens[2]);
        long long result = static_cast<long long>(static_cast<int>(regFile.getRegByNum(rs))) * 
                          static_cast<long long>(static_cast<int>(regFile.getRegByNum(rt)));
        LO = static_cast<unsigned int>(result & 0xFFFFFFFF);
        HI = static_cast<unsigned int>((result >> 32) & 0xFFFFFFFF);
        PC += 4;
    }
    else if (opcode == "multu")
    {
        int rs = getRegisterNumber(tokens[1]);
        int rt = getRegisterNumber(tokens[2]);
        unsigned long long result = static_cast<unsigned long long>(regFile.getRegByNum(rs)) * 
                                   static_cast<unsigned long long>(regFile.getRegByNum(rt));
        LO = static_cast<unsigned int>(result & 0xFFFFFFFF);
        HI = static_cast<unsigned int>((result >> 32) & 0xFFFFFFFF);
        PC += 4;
    }
    else if (opcode == "div")
    {
        int rs = getRegisterNumber(tokens[1]);
        int rt = getRegisterNumber(tokens[2]);
        int dividend = static_cast<int>(regFile.getRegByNum(rs));
        int divisor = static_cast<int>(regFile.getRegByNum(rt));
        if (divisor != 0)
        {
            LO = static_cast<unsigned int>(dividend / divisor);
            HI = static_cast<unsigned int>(dividend % divisor);
        }
        PC += 4;
    }
    else if (opcode == "divu")
    {
        int rs = getRegisterNumber(tokens[1]);
        int rt = getRegisterNumber(tokens[2]);
        unsigned int dividend = regFile.getRegByNum(rs);
        unsigned int divisor = regFile.getRegByNum(rt);
        if (divisor != 0)
        {
            LO = dividend / divisor;
            HI = dividend % divisor;
        }
        PC += 4;
    }
    else if (opcode == "mfhi")
    {
        int rd = getRegisterNumber(tokens[1]);
        regFile.setRegByNum(rd, HI);
        PC += 4;
    }
    else if (opcode == "mflo")
    {
        int rd = getRegisterNumber(tokens[1]);
        regFile.setRegByNum(rd, LO);
        PC += 4;
    }
    else if (opcode == "mthi")
    {
        int rs = getRegisterNumber(tokens[1]);
        HI = regFile.getRegByNum(rs);
        PC += 4;
    }
    else if (opcode == "mtlo")
    {
        int rs = getRegisterNumber(tokens[1]);
        LO = regFile.getRegByNum(rs);
        PC += 4;
    }
    else if (opcode == "jr")
    {
        int rs = getRegisterNumber(tokens[1]);
        PC = regFile.getRegByNum(rs);
    }
    else if (opcode == "jalr")
    {
        int rd = getRegisterNumber(tokens[1]);
        int rs = getRegisterNumber(tokens[2]);
        regFile.setRegByNum(rd, PC + 4);
        PC = regFile.getRegByNum(rs);
    }
}

void MIPSInterpreter::executeIType(const std::vector<std::string>& tokens)
{
    std::string opcode = tokens[0];
    
    if (opcode == "addi" || opcode == "addiu")
    {
        int rt = getRegisterNumber(tokens[1]);
        int rs = getRegisterNumber(tokens[2]);
        int imm = parseImmediate(tokens[3]);
        regFile.setRegByNum(rt, regFile.getRegByNum(rs) + imm);
        PC += 4;
    }
    else if (opcode == "andi")
    {
        int rt = getRegisterNumber(tokens[1]);
        int rs = getRegisterNumber(tokens[2]);
        unsigned int imm = static_cast<unsigned int>(parseImmediate(tokens[3])) & 0xFFFF;
        regFile.setRegByNum(rt, regFile.getRegByNum(rs) & imm);
        PC += 4;
    }
    else if (opcode == "ori")
    {
        int rt = getRegisterNumber(tokens[1]);
        int rs = getRegisterNumber(tokens[2]);
        unsigned int imm = static_cast<unsigned int>(parseImmediate(tokens[3])) & 0xFFFF;
        regFile.setRegByNum(rt, regFile.getRegByNum(rs) | imm);
        PC += 4;
    }
    else if (opcode == "xori")
    {
        int rt = getRegisterNumber(tokens[1]);
        int rs = getRegisterNumber(tokens[2]);
        unsigned int imm = static_cast<unsigned int>(parseImmediate(tokens[3])) & 0xFFFF;
        regFile.setRegByNum(rt, regFile.getRegByNum(rs) ^ imm);
        PC += 4;
    }
    else if (opcode == "slti")
    {
        int rt = getRegisterNumber(tokens[1]);
        int rs = getRegisterNumber(tokens[2]);
        int imm = parseImmediate(tokens[3]);
        int val = static_cast<int>(regFile.getRegByNum(rs));
        regFile.setRegByNum(rt, (val < imm) ? 1 : 0);
        PC += 4;
    }
    else if (opcode == "sltiu")
    {
        int rt = getRegisterNumber(tokens[1]);
        int rs = getRegisterNumber(tokens[2]);
        unsigned int imm = static_cast<unsigned int>(parseImmediate(tokens[3]));
        regFile.setRegByNum(rt, (regFile.getRegByNum(rs) < imm) ? 1 : 0);
        PC += 4;
    }
    else if (opcode == "lui")
    {
        int rt = getRegisterNumber(tokens[1]);
        unsigned int imm = static_cast<unsigned int>(parseImmediate(tokens[2])) & 0xFFFF;
        regFile.setRegByNum(rt, imm << 16);
        PC += 4;
    }
    else if (opcode == "lw")
    {
        int rt = getRegisterNumber(tokens[1]);
        int offset = parseImmediate(tokens[2]);
        int base = getRegisterNumber(tokens[3]);
        unsigned int addr = regFile.getRegByNum(base) + offset;
        regFile.setRegByNum(rt, mem.fetchWord(addr));
        PC += 4;
    }
    else if (opcode == "lh")
    {
        int rt = getRegisterNumber(tokens[1]);
        int offset = parseImmediate(tokens[2]);
        int base = getRegisterNumber(tokens[3]);
        unsigned int addr = regFile.getRegByNum(base) + offset;
        short value = static_cast<short>(mem.fetchHalfword(addr));
        regFile.setRegByNum(rt, static_cast<unsigned int>(static_cast<int>(value)));
        PC += 4;
    }
    else if (opcode == "lhu")
    {
        int rt = getRegisterNumber(tokens[1]);
        int offset = parseImmediate(tokens[2]);
        int base = getRegisterNumber(tokens[3]);
        unsigned int addr = regFile.getRegByNum(base) + offset;
        regFile.setRegByNum(rt, mem.fetchHalfword(addr));
        PC += 4;
    }
    else if (opcode == "lb")
    {
        int rt = getRegisterNumber(tokens[1]);
        int offset = parseImmediate(tokens[2]);
        int base = getRegisterNumber(tokens[3]);
        unsigned int addr = regFile.getRegByNum(base) + offset;
        char value = static_cast<char>(mem.fetch(addr));
        regFile.setRegByNum(rt, static_cast<unsigned int>(static_cast<int>(value)));
        PC += 4;
    }
    else if (opcode == "lbu")
    {
        int rt = getRegisterNumber(tokens[1]);
        int offset = parseImmediate(tokens[2]);
        int base = getRegisterNumber(tokens[3]);
        unsigned int addr = regFile.getRegByNum(base) + offset;
        regFile.setRegByNum(rt, mem.fetch(addr));
        PC += 4;
    }
    else if (opcode == "sw")
    {
        int rt = getRegisterNumber(tokens[1]);
        int offset = parseImmediate(tokens[2]);
        int base = getRegisterNumber(tokens[3]);
        unsigned int addr = regFile.getRegByNum(base) + offset;
        mem.storeWord(addr, regFile.getRegByNum(rt));
        PC += 4;
    }
    else if (opcode == "sh")
    {
        int rt = getRegisterNumber(tokens[1]);
        int offset = parseImmediate(tokens[2]);
        int base = getRegisterNumber(tokens[3]);
        unsigned int addr = regFile.getRegByNum(base) + offset;
        mem.storeHalfword(addr, static_cast<unsigned short>(regFile.getRegByNum(rt)));
        PC += 4;
    }
    else if (opcode == "sb")
    {
        int rt = getRegisterNumber(tokens[1]);
        int offset = parseImmediate(tokens[2]);
        int base = getRegisterNumber(tokens[3]);
        unsigned int addr = regFile.getRegByNum(base) + offset;
        mem.store(addr, static_cast<unsigned char>(regFile.getRegByNum(rt)));
        PC += 4;
    }
    else if (opcode == "beq")
    {
        int rs = getRegisterNumber(tokens[1]);
        int rt = getRegisterNumber(tokens[2]);
        if (regFile.getRegByNum(rs) == regFile.getRegByNum(rt))
        {
            if (isLabel(tokens[3]))
            {
                PC = getLabelAddress(tokens[3]);
            }
            else
            {
                int offset = parseImmediate(tokens[3]);
                PC = PC + 4 + (offset << 2);
            }
        }
        else
        {
            PC += 4;
        }
    }
    else if (opcode == "bne")
    {
        int rs = getRegisterNumber(tokens[1]);
        int rt = getRegisterNumber(tokens[2]);
        if (regFile.getRegByNum(rs) != regFile.getRegByNum(rt))
        {
            if (isLabel(tokens[3]))
            {
                PC = getLabelAddress(tokens[3]);
            }
            else
            {
                int offset = parseImmediate(tokens[3]);
                PC = PC + 4 + (offset << 2);
            }
        }
        else
        {
            PC += 4;
        }
    }
    else if (opcode == "blt" || opcode == "bltz")
    {
        int rs = getRegisterNumber(tokens[1]);
        int rt = 0;
        std::string label = tokens[2];
        
        if (opcode == "blt")
        {
            rt = getRegisterNumber(tokens[2]);
            label = tokens[3];
        }
        
        int val1 = static_cast<int>(regFile.getRegByNum(rs));
        int val2 = static_cast<int>(regFile.getRegByNum(rt));
        
        if (val1 < val2)
        {
            PC = isLabel(label) ? getLabelAddress(label) : PC + 4 + (parseImmediate(label) << 2);
        }
        else
        {
            PC += 4;
        }
    }
    else if (opcode == "blez")
    {
        int rs = getRegisterNumber(tokens[1]);
        int val = static_cast<int>(regFile.getRegByNum(rs));
        
        if (val <= 0)
        {
            if (isLabel(tokens[2]))
            {
                PC = getLabelAddress(tokens[2]);
            }
            else
            {
                int offset = parseImmediate(tokens[2]);
                PC = PC + 4 + (offset << 2);
            }
        }
        else
        {
            PC += 4;
        }
    }
    else if (opcode == "bgtz")
    {
        int rs = getRegisterNumber(tokens[1]);
        int val = static_cast<int>(regFile.getRegByNum(rs));
        
        if (val > 0)
        {
            if (isLabel(tokens[2]))
            {
                PC = getLabelAddress(tokens[2]);
            }
            else
            {
                int offset = parseImmediate(tokens[2]);
                PC = PC + 4 + (offset << 2);
            }
        }
        else
        {
            PC += 4;
        }
    }
    else if (opcode == "bgez")
    {
        int rs = getRegisterNumber(tokens[1]);
        int val = static_cast<int>(regFile.getRegByNum(rs));
        
        if (val >= 0)
        {
            if (isLabel(tokens[2]))
            {
                PC = getLabelAddress(tokens[2]);
            }
            else
            {
                int offset = parseImmediate(tokens[2]);
                PC = PC + 4 + (offset << 2);
            }
        }
        else
        {
            PC += 4;
        }
    }
}

void MIPSInterpreter::executeJType(const std::vector<std::string>& tokens)
{
    std::string opcode = tokens[0];
    
    if (opcode == "j")
    {
        if (isLabel(tokens[1]))
        {
            PC = getLabelAddress(tokens[1]);
        }
        else
        {
            unsigned int addr = parseImmediate(tokens[1]);
            PC = (PC & 0xF0000000) | ((addr & 0x03FFFFFF) << 2);
        }
    }
    else if (opcode == "jal")
    {
        regFile.setReg("$ra", PC + 4);
        if (isLabel(tokens[1]))
        {
            PC = getLabelAddress(tokens[1]);
        }
        else
        {
            unsigned int addr = parseImmediate(tokens[1]);
            PC = (PC & 0xF0000000) | ((addr & 0x03FFFFFF) << 2);
        }
    }
}

void MIPSInterpreter::executePseudoInstruction(const std::vector<std::string>& tokens)
{
    std::string opcode = tokens[0];
    
    if (opcode == "li")
    {
        int rt = getRegisterNumber(tokens[1]);
        int imm = parseImmediate(tokens[2]);
        
        // Check if immediate fits in 16 bits
        if (imm >= -32768 && imm <= 32767)
        {
            regFile.setRegByNum(rt, static_cast<unsigned int>(imm));
        }
        else
        {
            // Use lui and ori for larger values
            unsigned int upper = (imm >> 16) & 0xFFFF;
            unsigned int lower = imm & 0xFFFF;
            regFile.setRegByNum(rt, (upper << 16) | lower);
        }
        PC += 4;
    }
    else if (opcode == "la")
    {
        int rt = getRegisterNumber(tokens[1]);
        unsigned int addr;
        
        if (isLabel(tokens[2]))
        {
            addr = getLabelAddress(tokens[2]);
        }
        else
        {
            addr = parseImmediate(tokens[2]);
        }
        
        regFile.setRegByNum(rt, addr);
        PC += 4;
    }
    else if (opcode == "move")
    {
        int rd = getRegisterNumber(tokens[1]);
        int rs = getRegisterNumber(tokens[2]);
        regFile.setRegByNum(rd, regFile.getRegByNum(rs));
        PC += 4;
    }
    else if (opcode == "clear")
    {
        int rd = getRegisterNumber(tokens[1]);
        regFile.setRegByNum(rd, 0);
        PC += 4;
    }
    else if (opcode == "not")
    {
        int rd = getRegisterNumber(tokens[1]);
        int rs = getRegisterNumber(tokens[2]);
        regFile.setRegByNum(rd, ~regFile.getRegByNum(rs));
        PC += 4;
    }
}

void MIPSInterpreter::executeSyscall()
{
    unsigned int v0 = regFile.getReg("$v0");
    
    switch (v0)
    {
        case 1: // print integer
        {
            int value = static_cast<int>(regFile.getReg("$a0"));
            std::cout << value;
            break;
        }
        case 4: // print string
        {
            unsigned int addr = regFile.getReg("$a0");
            while (true)
            {
                unsigned char ch = mem.fetch(addr);
                if (ch == 0) break;
                std::cout << ch;
                addr++;
            }
            break;
        }
        case 5: // read integer
        {
            int value;
            std::cin >> value;
            regFile.setReg("$v0", static_cast<unsigned int>(value));
            break;
        }
        case 8: // read string
        {
            unsigned int addr = regFile.getReg("$a0");
            int maxLen = static_cast<int>(regFile.getReg("$a1"));
            std::string input;
            std::getline(std::cin, input);
            
            for (int i = 0; i < maxLen - 1 && i < static_cast<int>(input.length()); i++)
            {
                mem.store(addr + i, input[i]);
            }
            mem.store(addr + std::min(maxLen - 1, static_cast<int>(input.length())), '\0');
            break;
        }
        case 9: // sbrk (allocate heap memory)
        {
            unsigned int bytes = regFile.getReg("$a0");
            static unsigned int heapPtr = DATA_BASE + 0x10000;
            regFile.setReg("$v0", heapPtr);
            heapPtr += bytes;
            break;
        }
        case 10:
        {
            halted = true;
            break;
        }
        case 11: // print character
        {
            char ch = static_cast<char>(regFile.getReg("$a0"));
            std::cout << ch;
            break;
        }
        case 12: // read character
        {
            char ch;
            std::cin >> ch;
            regFile.setReg("$v0", static_cast<unsigned int>(ch));
            break;
        }
        default:
        {
            std::cerr << "Warning: Unsupported syscall: " << v0 << std::endl;
            break;
        }
    }
}

void MIPSInterpreter::loadFile(const std::string& filename)
{
    reset();
    parseFile(filename);
}

void MIPSInterpreter::run()
{
    while (!halted && PC >= TEXT_BASE && 
           addressToInstruction.find(PC) != addressToInstruction.end())
    {
        executeInstruction(addressToInstruction[PC]);
    }
    
    if (!halted)
    {
        std::cout << "Program complete.\n";
    }
}

void MIPSInterpreter::step()
{
    if (halted)
    {
        std::cout << "Program halted.\n";
        return;
    }
    
    if (addressToInstruction.find(PC) != addressToInstruction.end())
    {
        std::cout << "[0x" << std::hex << std::setw(8) << std::setfill('0') << PC << "] " 
                  << std::dec << addressToInstruction[PC] << "\n";
        executeInstruction(addressToInstruction[PC]);
    }
    else
    {
        std::cout << "No instruction at PC: 0x" << std::hex << PC << "\n";
        halted = true;
    }
}

void MIPSInterpreter::displayState()
{
    regFile.displayRegisters();
    std::cout << "PC: 0x" << std::hex << std::setw(8) << std::setfill('0') << PC 
              << " | HI: " << std::dec << std::setw(4) << HI 
              << " | LO: " << std::setw(4) << LO << "\n";
}

void MIPSInterpreter::reset()
{
    PC = TEXT_BASE;
    HI = 0;
    LO = 0;
    currentDataAddr = DATA_BASE;
    halted = false;
    textSegment.clear();
    labels.clear();
    addressToInstruction.clear();
    regFile = RegisterFile();
    mem = Memory();
    regFile.setReg("$sp", STACK_BASE);
}

void MIPSInterpreter::runInteractive()
{
    std::string input;
    clearScreen();
    printBanner("INTERACTIVE MODE");
    std::cout << "Commands: load <file>, run, step, regs, manual, reset, quit\n\n";
    
    while (true)
    {
        std::cout << "> ";
        if (!std::getline(std::cin, input)) break;
        
        std::string cleaned = cleanLine(input);
        if (cleaned.empty()) continue;
        
        std::vector<std::string> tokens = tokenize(cleaned);
        
        if (tokens[0] == "quit" || tokens[0] == "exit" || tokens[0] == "q")
        {
            break;
        }
        else if (tokens[0] == "load" && tokens.size() > 1)
        {
            loadFile(tokens[1]);
        }
        else if (tokens[0] == "run")
        {
            run();
        }
        else if (tokens[0] == "step")
        {
            step();
        }
        else if (tokens[0] == "regs")
        {
            clearScreen();
            printBanner("INTERACTIVE MODE");
            displayState();
            std::cout << "\nCommands: load <file>, run, step, regs, manual, reset, quit\n\n";
        }
        else if (tokens[0] == "reset")
        {
            reset();
            clearScreen();
            printBanner("INTERACTIVE MODE");
            std::cout << "Reset.\n\nCommands: load <file>, run, step, regs, manual, reset, quit\n\n";
        }
        else if (tokens[0] == "manual" || tokens[0] == "m")
        {
            runManualMode();
            clearScreen();
            printBanner("INTERACTIVE MODE");
            std::cout << "Commands: load <file>, run, step, regs, manual, reset, quit\n\n";
        }
        else
        {
            std::cout << "Unknown command. Type 'manual' for instruction mode.\n";
        }
    }
}

void MIPSInterpreter::runManualMode()
{
    std::string input;
    clearScreen();
    printBanner("MANUAL MODE");
    displayState();
    std::cout << "\nType MIPS instructions or 'back' to exit\n\n";
    
    while (true)
    {
        std::cout << "PC:0x" << std::hex << std::setw(8) << std::setfill('0') << PC << "> " << std::dec;
        if (!std::getline(std::cin, input)) break;
        
        std::string cleaned = cleanLine(input);
        if (cleaned.empty()) continue;
        
        if (cleaned == "back" || cleaned == "exit" || cleaned == "quit")
        {
            break;
        }
        else if (cleaned == "regs")
        {
            clearScreen();
            printBanner("MANUAL MODE");
            displayState();
            std::cout << "\nType MIPS instructions or 'back' to exit\n\n";
        }
        else
        {
            executeInstruction(cleaned);
            clearScreen();
            printBanner("MANUAL MODE");
            displayState();
            std::cout << "\nType MIPS instructions or 'back' to exit\n\n";
        }
    }
}
