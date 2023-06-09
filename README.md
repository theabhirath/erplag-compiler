# erplag-compiler

This is a compiler for the toy language ERPLAG. It is capable of converting code written in ERPLAG to x86 NASM assembly and execute the same.

## To run the compiler

1. Run ````make driver```` to prepare the executable
2. Run ````./driver <Input File> <Assembly Code File>````. A menu will appear offering different levels of execution
3. To run the assembly file, run ````make runasm <Assembly Code File>````

## Limitations

The compiler does not support code generation for dynamically sized arrays

## Credits

In alphabetical order,
- [Abhirath Anand](https://github.com/theabhirath)
- [Rohan Srinivasan](https://github.com/Rohan-Witty)
- [Samriddha Sinha](https://github.com/sam-india-007)
- [Srijan Shashwat](https://github.com/SrijanShashwat)
