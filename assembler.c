/**
 * @author Shahar Erez, ID - 212751697
 * Assembler implementation for the Open University's "Systems Programming Lab" course.
 *
 * Supports two passes over source files with extension ".as". Input files are
 * provided via command line arguments (without the ".as" extension).
 *
 * Produces a ".am" file with macros expanded and comments/blank lines ignored.
 * If the source is correct, also produces:
 *   - ".ent": entry labels addresses
 *   - ".ext": external labels usage addresses
 *   - ".obj": full translation to machine language in a unique base 4 (code/data parts)
 *      *assumption* : defined number of machine lines per file
 *
 * On source errors, reports to stdout and does not create output files.
 *
 * --------------------- Module breakdown ---------------------
 * 1. command_line_structure.h  - Core structures representing each source line,
 *                                including line type, instruction/operation type,
 *                                addressing methods, operands, and encoding.
 *
 * 2. defined_strings.c / .h    - Predefined commands, addressing types, and error messages.
 *
 * 3. error_handler.c / .h      - Centralized error messages and reporting with file/line info.
 *
 * 4. first_pass.c / .h         - First pass: parses lines, detects labels, stores addresses,
 *                                and performs basic syntax validation.
                                  Initiates second_pass.c on success.
 *
 * 5. globals.c / .h            - Global variables shared across modules.
 *
 * 6. label.c / .h              - Manages assembler labels: parsing, validation, storage, and address updates.
 *
 * 7. output_handler.c / .h     - Generates output files (.obj, .ext, .ent) based on collected data.
 *
 * 8. pre_assembler.c / .h      - Preprocessing: expands macros, removes comments and blank lines,
 *                                creates intermediate .am files for first pass.
 *
 * 9. second_pass.c / .h        - Second pass: converts instructions to machine code,
 *                                resolves final addresses and label references.
 *
 * 10. string_utils.c / .h      - String helpers and syntax validators: trimming, splitting,
 *                                 copying, counting args, comma checks, register and matrix validation.
 */


#include <stdio.h>
#include "pre_assembler.h"
#include "first_pass.h"

int main(int argc, char* argv[])
{
    int file_counter = 1; /* Initialize file counter (possible to enter more than one file as command arg) */
    int error_flag; /* Error status flag will track errors through all passes */
    Macro* macros_list = NULL; /* Initialize macros linked list (will be freed) */

    for (; file_counter < argc; file_counter++) /* Loop through all files */
    {
        error_flag = FALSE; /* reset */
        macros_list = pre_assembler(argv[file_counter],&error_flag);
        if (!error_flag)
            first_pass(argv[file_counter], &error_flag, &macros_list);
        free_macros(macros_list); /* release allocated memory */
    }
    return 0;
}