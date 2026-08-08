# Two-Pass Assembler (C / Linux)

A **two-pass assembler** implemented in C that compiles source files written in a custom Assembly-like language into a custom base 4 output files, including symbol table management, macro expansion, and syntax/semantic error detection.

The project is written for a Linux environment, follows the **ANSI C (C90)** standard, and is compiled with `gcc`.

## Table of Contents

- [About](#about)
- [Processing Pipeline](#processing-pipeline)
- [File Structure](#file-structure)
- [Output Files](#output-files)
- [Prerequisites](#prerequisites)
- [Build & Run](#build--run)
- [Examples](#examples)
- [Error Handling](#error-handling)

## About

The assembler takes source files with a `.as` extension, written in a predefined assembly language, and processes them through a pre-processing stage followed by two main passes:

1. **Pre-Assembler** — Parses and expands macro definitions (`mcro` / `mcroend`) in the source file, producing an expanded `.am` file.
2. **First Pass** — Reads the expanded file, builds the symbol table, and determines addresses for instructions, data, and definitions (Code Image / Data Image).
3. **Second Pass** — Resolves addresses for symbols that were unknown during the first pass (such as jump targets and references), handles `.entry` and `.extern` directives, and generates the final output files.

Throughout the process, errors (both syntactic and semantic) are detected and reported to the user with the relevant line number. Where possible, processing continues rather than stopping immediately, so that as many errors as possible can be reported in a single run.

## Processing Pipeline

```
Input file (.as)
      │
      ▼
 Pre-Assembler  ──► Macro expansion ──► .am file
      │
      ▼
   First Pass    ──► Build symbol table + Code/Data Image
      │
      ▼
   Second Pass   ──► Resolve addresses, .entry / .extern
      │
      ▼
 Output Handler  ──► .ob / .ent / .ext
```

## File Structure

| File | Description |
|---|---|
| `assembler.c` | Main entry point (`main`) — runs the full processing pipeline for each input file |
| `pre_assembler.c/.h` | Handles macro expansion and generation of the `.am` file |
| `first_pass.c/.h` | Implements the first pass — builds the symbol table and initial encoding |
| `second_pass.c/.h` | Implements the second pass — resolves addresses and `.entry`/`.extern` symbols |
| `label.c/.h` | Manages the symbol table (labels) — insertion, lookup, and validation |
| `output_handler.c/.h` | Generates the final output files (`.ob`, `.ent`, `.ext`) |
| `error_handler.c/.h` | Centralizes error handling across all processing stages |
| `string_utils.c/.h` | String-processing helper functions (parsing, trimming, etc.) |
| `defined_strings.c/.h` | Constant definitions — instruction names, registers, reserved words, etc. |
| `globals.c/.h` | Global definitions, shared data structures and macros used across the project |
| `command_line_structure.h` | Defines the structure of a parsed instruction/command line |
| `makefile` | Build file for compiling the project |
| `Valid examples/` | Sample input files that assemble successfully |
| `Errors examples/` | Sample input files demonstrating various detected errors |

## Output Files

For a valid input file named `example.as`, the assembler produces:

- **`example.ob`** — The object file: encoded instructions and data (decimal/hexadecimal), by address.
- **`example.ent`** — List of symbols defined as `.entry` and their addresses (only generated if `.entry` definitions exist).
- **`example.ext`** — List of external symbols (`.extern`) and every address where they are referenced (only generated if `.extern` definitions exist).

If any error is detected in the input file, no output files are generated for that file — only the error messages are printed.

## Prerequisites

- A Linux-based OS (or WSL / a compatible Unix environment)
- `gcc` compiler
- `make`

## Build & Run

Clone the repository and build using the `makefile`:

```bash
git clone https://github.com/shaharerez1/c-two-pass-assembler.git
cd c-two-pass-assembler
make
```

Run the assembler on an input file (without the `.as` extension):

```bash
./assembler example
```

Multiple files can be passed at once:

```bash
./assembler file1 file2 file3
```

Clean intermediate object files (`.o`) and the compiled binary:

```bash
make clean
```

## Examples

The `Valid examples/` and `Errors examples/` directories contain sample `.as` files:

- **Valid examples** — Files demonstrating correct usage of the language's syntax (instructions, macros, `.data`, `.string`, `.entry`, `.extern`, etc.).
- **Errors examples** — Files demonstrating a variety of common errors that the assembler is expected to detect and report.

## Error Handling

The `error_handler` module is responsible for collecting and reporting errors detected throughout all processing stages (pre-processing, first pass, and second pass). Every error is reported together with the source file line number where it was found, to make it easier to locate and fix the issue in the source code.

---

Written as a C programming project in a Linux environment.
