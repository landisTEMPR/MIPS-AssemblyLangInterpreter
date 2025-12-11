#ifndef MEMORY_H
#define MEMORY_H

#include <map>
#include <cstdint>
#include <iostream>

class Memory
{
public:
    Memory() {}
    
    // Byte operations
    unsigned char fetch(unsigned int addr);
    void store(unsigned int addr, unsigned char value);
    
    // Halfword operations (16-bit)
    unsigned short fetchHalfword(unsigned int addr);
    void storeHalfword(unsigned int addr, unsigned short value);
    
    // Word operations (32-bit)
    unsigned int fetchWord(unsigned int addr);
    void storeWord(unsigned int addr, unsigned int value);
    
    void displayMemoryRange(unsigned int start, unsigned int end);
    
private:
    std::map<unsigned int, unsigned char> mem;
};

#endif
