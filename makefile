
driver: src/driver.c src/lexer/lexer.c src/lexer/reserved_words.c src/parser/parser.c src/parser/set.c src/parser/stack.c src/parser/linked_list.c
	gcc -o driver src/driver.c src/lexer/lexer.c src/lexer/reserved_words.c src/parser/parser.c src/parser/set.c src/parser/stack.c src/parser/linked_list.c