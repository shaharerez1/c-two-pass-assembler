The examples in this folder show 3 different examples of perfectly valid source code.

Each of the examples is accompanied by a .as file, which is the source file, and the other files with the same name (and different extension), which are the output produced by the assembler as a result of running on that .as file

- In the first example (guide_example.as), this is the exact code that appears at the end of Maman14's instructions stage, there referenced as "ps.as".

- In the second example (macros_ex.as), there is source code that contains the use of macros, combined with code and data segments, without the use of entry/extern directives, to illustrate that this is a situation in which .ext/.ent files are not created.

- In the third example (mixed_example.as), there is relatively extensive source code, which combines the use of macros with all types of different directives, with the code and data segments mixed, and it can be seen that a separation is still created in the .obj file between the segments.
