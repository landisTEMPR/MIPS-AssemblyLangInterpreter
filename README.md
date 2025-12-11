# MIPS Interpreter
**Author:** Brysen Landis

A full-featured MIPS assembly interpreter with a dynamic full-screen interface.

## Files

### Source Files
- `main.cpp` - Entry point and mode handlers
- `interpreter.cpp` / `interpreter.h` - Core interpreter logic
- `register_file.cpp` / `register_file.h` - Register management
- `memory.cpp` / `memory.h` - Memory system

### Test Programs
- `test_loop.asm` - Counting loop
- `test_arithmetic.asm` - Interactive addition
- `test_factorial.asm` - Recursive factorial
- `test_array.asm` - Array operations

### Build
- `Makefile` - Build system
- `a.out` - Executable

## Quick Start

```bash
make                       # Compile
./a.out                    # Interactive mode
./a.out program.asm        # Run program
./a.out program.asm -step  # Step through
```

## Features

✅ Full MIPS-I instruction set
✅ Dynamic full-screen interface  
✅ Real-time register updates
✅ Three execution modes
✅ Complete syscall support
✅ File loading and execution
✅ Step-by-step debugging

## Modes

**Interactive:** Load files, run programs, access manual mode
**Manual:** Type instructions, see instant register updates
**Step:** Debug programs instruction-by-instruction

Type `manual` in interactive mode to enter manual instruction mode.
