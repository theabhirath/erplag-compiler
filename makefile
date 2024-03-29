ast: src/ast/ast.c src/lexer/lexer.c src/lexer/reserved_words.c src/parser/parser.c src/parser/set.c src/parser/stack.c src/parser/linked_list.c
	gcc -o ast src/ast/ast.c src/lexer/lexer.c src/lexer/reserved_words.c src/parser/parser.c src/parser/set.c src/parser/stack.c src/parser/linked_list.c

symtab: src/ast/symbol_table.c src/ast/ast.c src/lexer/lexer.c src/lexer/reserved_words.c src/parser/parser.c src/parser/set.c src/parser/stack.c src/parser/linked_list.c
	gcc -o symtab src/ast/symbol_table.c src/ast/ast.c src/lexer/lexer.c src/lexer/reserved_words.c src/parser/parser.c src/parser/set.c src/parser/stack.c src/parser/linked_list.c

code.out: src/codegen/codegen.c src/ast/symbol_table.c src/ast/ast.c src/lexer/lexer.c src/lexer/reserved_words.c src/parser/parser.c src/parser/set.c src/parser/stack.c src/parser/linked_list.c
	gcc -w -o code.out src/codegen/codegen.c src/ast/symbol_table.c src/ast/ast.c src/lexer/lexer.c src/lexer/reserved_words.c src/parser/parser.c src/parser/set.c src/parser/stack.c src/parser/linked_list.c

driver: driver.c src/codegen/codegen.c src/ast/symbol_table.c src/ast/ast.c src/lexer/lexer.c src/lexer/reserved_words.c src/parser/parser.c src/parser/set.c src/parser/stack.c src/parser/linked_list.c
	gcc -w -o driver driver.c src/codegen/codegen.c src/ast/symbol_table.c src/ast/ast.c src/lexer/lexer.c src/lexer/reserved_words.c src/parser/parser.c src/parser/set.c src/parser/stack.c src/parser/linked_list.c


runasm: 
	nasm -f elf64 $(asm) -o code.o
	gcc -no-pie -o code code.o
	./code

clean:
	rm -f driver ast ast.txt code code.asm symtab symtab.txt a.out 3AC.txt parseTree.txt
