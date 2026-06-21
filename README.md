# Computer from First Principles — Nand2Tetris (Part I & II)

> Developed as part of the Computer Architecture course at IIT Madras (Nand2Tetris) in 2025; individual implementation.

## Project Highlights
- Complete Hack computer in HDL: Built every component from NAND gates to a fully functional 16-bit CPU and memory system.
- End-to-end C++ toolchain: Custom implementation of an Assembler, VM Translator, and Jack Compiler.
- Test-Driven Development: All components verified against official Nand2Tetris hardware and software specifications.
- Modular Design: Clean, object-oriented C++ implementation for the entire software stack.

## Overview
This repository contains my complete implementation of a 16-bit computer system built from the ground up, starting with basic NAND logic gates and culminating in a full software toolchain capable of running high-level Jack programs.

The project follows the Nand2Tetris curriculum and implements both:
- Part I: Hardware platform (Hack computer) — HDL designs for all CPU, memory, and I/O components
- Part II: Software stack (C++) — Assembler, VM Translator, and Jack Compiler

---

## Part I — Hardware Platform (HDL)

Built the complete Hack computer architecture from first principles using Hardware Description Language (HDL).

### Implemented Components
- Logic Gates: NAND, NOT, AND, OR, XOR, MUX, DMUX, 16-bit logic operations, multiplexers, demultiplexers
- Arithmetic Units: Half Adder, Full Adder, 16-bit Add, Incrementer, ALU
- Sequential Logic: Bit, Register, Program Counter (PC)
- Memory Hierarchy: RAM8 → RAM64 → RAM512 → RAM4K → RAM16K
- CPU: Instruction decoding, control logic, and execution unit
- Memory Subsystem: Address decoding and memory-mapped I/O
- Top-Level Computer: Complete integration of CPU, memory, and peripherals

### File Structure
```
hardware/
├── logic_gates/      # Gates built from NAND
├── arithmetic/       # Adder, ALU
├── memory/           # RAM hierarchy
├── cpu/              # CPU design
├── Computer.hdl      # Top-level integration
└── Memory.hdl        # Memory-mapped I/O integration
```

---

## Part II — Software Toolchain (C++)

Implemented a complete compiler and runtime stack to execute programs on the Hack computer.

### 1. [Assembler](./software/assembler)
Converts Hack assembly to binary machine code.
- Two-pass assembly algorithm
- Symbol table with label and variable resolution
- Generates 16-bit .hack binary files

### 2. [VM Translator](./software/vm_translator)
Translates Nand2Tetris Virtual Machine code to Hack assembly.
- Stack-based VM architecture
- Support for arithmetic, memory segments, branching, and function calls
- Handles multi-file translation and recursion

### 3. [Jack Compiler](./software/jack_compiler)
Full compiler from high-level Jack language to VM code.
- Tokenizer: Lexical analysis
- Parser: Recursive descent syntax analysis
- Symbol Table: Scope management for variables/classes
- Code Generator: VM instruction emission

---

## Repository Structure
```
nand2tetris/
├── hardware/           # HDL implementations
│   ├── logic_gates/
│   ├── arithmetic/
│   ├── memory/
│   └── cpu/
├── software/           # C++ software toolchain
│   ├── [assembler](./software/assembler)
│   ├── [vm_translator](./software/vm_translator)
│   ├── [jack_compiler](./software/jack_compiler)
│   └── [OS](./software/OS)
└── README.md
```

## Quick Start

### Prerequisites
- C++17 or later
- g++ compiler
- Nand2Tetris Hardware Simulator (optional, for HDL testing)

### How to Run Everything
Follow these steps to compile and run a sample Jack program (e.g., Square):

```bash
# 1. Clone the repository
git clone https://github.com/spai006/nand2tetris.git
cd nand2tetris

# 2. Build the tools
g++ -std=c++17 -O2 software/jack_compiler/src/JackCompiler.cpp -o jack_compiler
g++ -std=c++17 -O2 software/vm_translator/src/translator.cpp -o vm_translator
g++ -std=c++17 -O2 software/assembler/src/assembler.cpp -o assembler

# 3. Compile a Jack project (e.g., Square)
./jack_compiler software/jack_compiler/tests/part2/Square/
./vm_translator software/jack_compiler/tests/part2/Square/
./assembler software/jack_compiler/tests/part2/Square/Square.asm

# Output: Square.hack (Ready to be loaded into the CPU)
```

## Course Context
Developed as part of the Computer Architecture course at IIT Madras (Nand2Tetris) in 2025. This project explores the layers of abstraction in computer systems from NAND gates to high-level programming.

Course reference: [nand2tetris.org](https://www.nand2tetris.org/)
