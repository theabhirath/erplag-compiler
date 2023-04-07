driver: src/driver.c src/lexer/lexer.c src/lexer/reserved_words.c src/parser/parser.c src/parser/set.c src/parser/stack.c src/parser/linked_list.c
	gcc -o driver src/driver.c src/lexer/lexer.c src/lexer/reserved_words.c src/parser/parser.c src/parser/set.c src/parser/stack.c src/parser/linked_list.c

ast: src/ast/ast.c src/lexer/lexer.c src/lexer/reserved_words.c src/parser/parser.c src/parser/set.c src/parser/stack.c src/parser/linked_list.c
	gcc -o ast src/ast/ast.c src/lexer/lexer.c src/lexer/reserved_words.c src/parser/parser.c src/parser/set.c src/parser/stack.c src/parser/linked_list.c

symtab: src/ast/symbol_table.c src/ast/ast.c src/lexer/lexer.c src/lexer/reserved_words.c src/parser/parser.c src/parser/set.c src/parser/stack.c src/parser/linked_list.c
	gcc -o symtab src/ast/symbol_table.c src/ast/ast.c src/lexer/lexer.c src/lexer/reserved_words.c src/parser/parser.c src/parser/set.c src/parser/stack.c src/parser/linked_list.c

clean:
	rm -f driver ast ast.txt