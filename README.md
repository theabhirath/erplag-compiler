# erplag-compiler

This is a compiler for the toy language ERPLAG, implemented as part of our BITS course CS F363 Compiler Construction. It is capable of converting code written in ERPLAG to x86 NASM assembly and execute the same. The compiler has been written in C.

## Organisation of the project

The structure of the project as visualised by `tree` is given below:

```
.
├── LICENSE
├── README.md
├── driver.c
├── makefile
├── src
│   ├── ast_symbol_table
│   ├── codegen
│   ├── lexer
│   ├── parser
│   └── stage1_driver.c
└── tests
    ├── codegen
    ├── stage1
    └── stage2

10 directories, 6 files
```

Most of the code is organised in the `src` directory. Our compiler implements a lexer, a top-down LL(1) parser, an abstract syntax tree (AST), a symbol table that couples with type checking and semantic analysis, and code generation for most use cases. These files are organised in their respective subdirectories.  The file `stage1_driver.c` in the `src/` folder was used for showcasing limited functionality of the codebase (the lexer and parser) for the stage 1 submission of our project. The main driver is in the project root. To run the main driver, see the instructions below.

## To run the compiler

1. Run `make driver` to prepare the executable.
2. Run `./driver <Input File> <Assembly Code File>`. A menu will appear offering different levels of execution. The user can choose one as per their requirement. The `<Assembly Code File>` will be generated if that option is selected.
3. To run the assembly file, run `make runasm asm=<Assembly Code File>`.

## Known limitations

The compiler does not support code generation for dynamically sized arrays.

## Credits

In alphabetical order,
- [Abhirath Anand](https://github.com/theabhirath)
- [Rohan Srinivasan](https://github.com/Rohan-Witty)
- [Samriddha Sinha](https://github.com/sam-india-007)
- [Srijan Shashwat](https://github.com/SrijanShashwat)

