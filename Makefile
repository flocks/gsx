CC = gcc
TREE_SITTER=$(CURDIR)/tree-sitter
TREE_SITTER_TSX=$(CURDIR)/tree-sitter-typescript/tsx
STATIC_LIB = $(TREE_SITTER)/libtree-sitter.a
INCLUDE=$(TREE_SITTER)/lib/include

default: gsx

$(STATIC_LIB):
	cd $(TREE_SITTER) && $(MAKE)

scanner.o: $(TREE_SITTER_TSX)/src/scanner.c
	$(CC) -c $< -o $@ -I$(INCLUDE)

parser.o: $(TREE_SITTER_TSX)/src/parser.c
	$(CC) -c $< -o $@ -I$(INCLUDE)

gsx: src/parse.h src/parse.c src/utils.c src/main.c scanner.o parser.o $(STATIC_LIB)
	$(CC) -Wall -Wextra -std=c11 -pedantic -o gsx $(CGLAGS) src/utils.c src/parse.c src/main.c scanner.o parser.o $(STATIC_LIB) -I$(TREE_SITTER)/lib/include

test/test: gsx src/utils.c test/test.c
	$(CC) -Wall -Wextra -o test/test src/utils.c test/test.c

run_test: test/test
	./test/test

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
