#include <linux/limits.h>
#include <tree_sitter/api.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <errno.h>

#include "parse.h"

TSLanguage *tree_sitter_tsx();

char* readFile(char* file_name) {
  FILE *f = fopen(file_name, "rb");

  if (f == NULL) {
    fprintf(stderr, "Couldn't open %s", file_name);
    fprintf(stderr, "%d", errno);

    exit(EXIT_FAILURE);
  }
  if(fseek(f, 0, SEEK_END) != 0) {
    fprintf(stderr, "Error reading %s", file_name);
    exit(EXIT_FAILURE);
  }
  long size = ftell(f);
  char* source_code = (char *)malloc(size+1);
  rewind(f);

  fread(source_code, 1, size, f);
  fclose(f);

  return source_code;
}

void traverse_node(TSNode node, const char* file_name, char* source_code, Pattern* p) {
  const char *name = ts_node_type(node);
  if (strcmp(name, "jsx_self_closing_element") == 0 ||
	  strcmp(name, "jsx_opening_element") == 0) {

	TSNode child = ts_node_child(node, 1);
	if (!ts_node_is_null(child)) {
	  size_t start_offset = ts_node_start_byte(child);
	  size_t end_offset = ts_node_end_byte(child);
	  size_t length = end_offset - start_offset;
	  char *sub = (char *)malloc(length + 1);
	  memcpy(sub, source_code + start_offset, length);
	  sub[length] = '\0';

	  if (strcmp(sub, p->component) == 0) {
		printf("%s\n", sub);
	  }
	  free(sub);
	}
  }

  for (uint32_t i = 0; i < ts_node_child_count(node); ++i) {
	traverse_node(ts_node_child(node, i), file_name, source_code, p);
  }
}

int tree(char* source_code, TSParser* parser, const char* file_name, Pattern *p) {
  TSTree *tree = ts_parser_parse_string(
    parser,
    NULL,
    source_code,
    strlen(source_code)
  );

  TSNode root_node = ts_tree_root_node(tree);
  traverse_node(root_node, file_name, source_code, p);

  ts_tree_delete(tree);
  return 0;
}

int main(int argc, char** argv) {

  const char *directory = NULL;
  Pattern p = {0};
  char command[1024];

  if (argc < 2) {
	fprintf(stderr, "USAGE: tsjsx src Button.variant\n");
	exit(EXIT_FAILURE);
  }

  if (argc == 2) {
	parse(&p, argv[1]);
  }

  if (argc >= 3) {
	directory = argv[1];
	parse(&p, argv[2]);
  }

  if (directory == NULL) {
	directory = "";
  }

  if (strlen(directory) + strlen(p.component) > 1000) {
	fprintf(stderr, "Pattern is probably too big");
	exit(EXIT_FAILURE);
  }
  sprintf(command, "grep -lr --include \\*.tsx %s %s", p.component, directory);

  TSParser *parser = ts_parser_new();
  ts_parser_set_language(parser, tree_sitter_tsx());

  FILE *cmd;
  char result[1024];

  cmd = popen(command, "r");
  if (cmd == NULL) {
    fprintf(stderr, "Error while running grep");
    exit(EXIT_FAILURE);
  }

  while (fgets(result, sizeof(result), cmd)) {
	result[strlen(result)-1] = '\0';
    char* source_code = readFile(result);
    tree(source_code, parser, result, &p);
    free(source_code);
  }

  ts_parser_delete(parser);
  pclose(cmd);
  free_pattern(&p);

  return 0;
}
