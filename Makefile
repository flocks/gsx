CC = gcc
TREE_SITTER=$(CURDIR)/tree-sitter
TREE_SITTER_TSX=$(CURDIR)/tree-sitter-typescript/tsx
STATIC_LIB = $(TREE_SITTER)/libtree-sitter.a

default: gsx

$(STATIC_LIB):
	cd $(TREE_SITTER) && $(MAKE)

scanner.o: $(TREE_SITTER_TSX)/src/scanner.c
	$(CC) -c $< -o $@

parser.o: $(TREE_SITTER_TSX)/src/parser.c
	$(CC) -c $< -o $@

gsx: src/parse.h src/parse.c src/main.c scanner.o parser.o $(STATIC_LIB)
	$(CC) -Wall -Wextra -std=c11 -pedantic -o gsx $(CGLAGS) src/parse.c src/main.c scanner.o parser.o $(STATIC_LIB) 

clean:
	rm -f *.o && rm -f main

install:
	@echo "Installing GSX..."
	install -D -m 755 gsx $(DESTDIR)$(PREFIX)/bin/gsx
	@echo "GSX installed to $(DESTDIR)$(PREFIX)/bin"

uninstall:
	@echo "Uninstalling GSX..."
	rm $(DESTDIR)$(PREFIX)/bin/gsx
	@echo "GSX has been removed from $(DESTDIR)$(PREFIX)/bin"

.PHONY: install clean uninstall
