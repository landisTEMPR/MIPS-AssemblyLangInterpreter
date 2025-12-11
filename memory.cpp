#include "memory.h"
#include <iomanip>

unsigned char Memory::fetch(unsigned int addr)
{
    if (mem.find(addr) != mem.end())
    {
        return mem[addr];
    }
    return 0; // Uninitialized memory returns 0
}

void Memory::store(unsigned int addr, unsigned char value)
{
    mem[addr] = value;
}

unsigned short Memory::fetchHalfword(unsigned int addr)
{
    // MIPS is big-endian, but we'll use little-endian for simplicity
    unsigned short result = 0;
    result = fetch(addr) | (fetch(addr + 1) << 8);
    return result;
}

void Memory::storeHalfword(unsigned int addr, unsigned short value)
{
    store(addr, static_cast<unsigned char>(value & 0xFF));
    store(addr + 1, static_cast<unsigned char>((value >> 8) & 0xFF));
}

unsigned int Memory::fetchWord(unsigned int addr)
{
    unsigned int result = 0;
    result = fetch(addr) | 
            (fetch(addr + 1) << 8) | 
            (fetch(addr + 2) << 16) | 
            (fetch(addr + 3) << 24);
    return result;
}

void Memory::storeWord(unsigned int addr, unsigned int value)
{
    store(addr, static_cast<unsigned char>(value & 0xFF));
    store(addr + 1, static_cast<unsigned char>((value >> 8) & 0xFF));
    store(addr + 2, static_cast<unsigned char>((value >> 16) & 0xFF));
    store(addr + 3, static_cast<unsigned char>((value >> 24) & 0xFF));
}

void Memory::displayMemoryRange(unsigned int start, unsigned int end)
{
    std::cout << "\n=== Memory [0x" << std::hex << start << " - 0x" << end << "] ===" << std::endl;
    
    for (unsigned int addr = start; addr <= end; addr += 4)
    {
        unsigned int word = fetchWord(addr);
        std::cout << "0x" << std::hex << std::setw(8) << std::setfill('0') << addr 
                  << ": 0x" << std::setw(8) << std::setfill('0') << word 
                  << " (" << std::dec << static_cast<int>(word) << ")" << std::endl;
    }
}
