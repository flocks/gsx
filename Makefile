CC = gcc
CFLAGS =-I ~/tree-sitter/lib/include 
LIBS = ~/tree-sitter/libtree-sitter.a
TSX_PARSER = ~/tree-sitter-typescript/tsx/src/parser.c ~/tree-sitter-typescript/tsx/src/scanner.c


main: parse.c main.c
	$(CC) -Wall -g -Wextra -o main $(CGLAGS) parse.c main.c $(TSX_PARSER) $(LIBS) 

