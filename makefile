driver: src/driver.c src/lexer/lexer.c src/lexer/reserved_words.c src/parser/parser.c src/parser/set.c src/parser/stack.c src/parser/linked_list.c
	gcc -o driver src/driver.c src/lexer/lexer.c src/lexer/reserved_words.c src/parser/parser.c src/parser/set.c src/parser/stack.c src/parser/linked_list.c

ast: src/ast/ast.c src/lexer/lexer.c src/lexer/reserved_words.c src/parser/parser.c src/parser/set.c src/parser/stack.c src/parser/linked_list.c
	gcc -o ast src/ast/ast.c src/lexer/lexer.c src/lexer/reserved_words.c src/parser/parser.c src/parser/set.c src/parser/stack.c src/parser/linked_list.c

symtab: src/ast/symbol_table.c src/ast/ast.c src/lexer/lexer.c src/lexer/reserved_words.c src/parser/parser.c src/parser/set.c src/parser/stack.c src/parser/linked_list.c
	gcc -o symtab src/ast/symbol_table.c src/ast/ast.c src/lexer/lexer.c src/lexer/reserved_words.c src/parser/parser.c src/parser/set.c src/parser/stack.c src/parser/linked_list.c

code: src/codegen/codegen.c src/ast/symbol_table.c src/ast/ast.c src/lexer/lexer.c src/lexer/reserved_words.c src/parser/parser.c src/parser/set.c src/parser/stack.c src/parser/linked_list.c
	gcc -o code src/codegen/codegen.c src/ast/symbol_table.c src/ast/ast.c src/lexer/lexer.c src/lexer/reserved_words.c src/parser/parser.c src/parser/set.c src/parser/stack.c src/parser/linked_list.c

runasm: code.asm
	nasm -f elf64 code.asm
	gcc -o code code.o
	./code

clean:
	rm -f driver ast ast.txt code code.asm symtab symtab.txt a.out 3AC.txt parseTree.txt