CC = gcc
CFLAGS = -I ~/tree-sitter/lib/include -Werror -Wextra -pedantic
LIBS = ~/tree-sitter/libtree-sitter.a
TSX_PARSER = ~/tree-sitter-typescript/tsx/src/parser.c ~/tree-sitter-typescript/tsx/src/scanner.c


tsx: tsx.c
	$(CC) $(CGLAGS) tsx.c $(TSX_PARSER) $(LIBS) -o tsx

main: main.c
	$(CC) $(CGLAGS) main.c $(TSX_PARSER) $(LIBS) -o main
