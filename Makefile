CC = gcc
TREE_SITTER=$(CURDIR)/tree-sitter
TREE_SITTER_TSX=$(CURDIR)/tree-sitter-typescript/tsx
STATIC_LIB = $(TREE_SITTER)/libtree-sitter.a

default: main

$(STATIC_LIB):
	cd $(TREE_SITTER) && $(MAKE)

scanner.o: $(TREE_SITTER_TSX)/src/scanner.c
	$(CC) -c $< -o $@

parser.o: $(TREE_SITTER_TSX)/src/parser.c
	$(CC) -c $< -o $@

main: src/parse.c src/main.c scanner.o parser.o $(STATIC_LIB)
	$(CC) -Wall -g -Wextra -o main $(CGLAGS) src/parse.c src/main.c scanner.o parser.o $(STATIC_LIB) 

.PHONY:clean
clean:
	rm -f *.o && rm -f main

