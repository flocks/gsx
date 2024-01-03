#define _GNU_SOURCE // make popen available
#include <ctype.h>
#include <linux/limits.h>
#include <tree_sitter/api.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <errno.h>

#include "parse.h"

#define INIT_RESULT_CAPACITY 32
#define MAX_COMMAND_LINE_LENGTH 1024
#define MAX_FILE_NAME 1024

TSLanguage *tree_sitter_tsx();

typedef struct {
  TSNode* items;
  size_t size;
  size_t capacity;
} Result;


void append_node(Result* r, TSNode item) {
  if (r->size >= r->capacity) {
	size_t new_capacity = r->capacity * 2;
	if (new_capacity == 0) {
	  new_capacity = INIT_RESULT_CAPACITY;
	}
	r->items = (TSNode*)realloc(r->items, new_capacity * sizeof(r->items[0]));
	r->capacity = new_capacity;
  }

  r->items[r->size++] = item;
}

void free_result(Result *r) {
  free(r->items);
  r->items = NULL;
  r->size = r->capacity = 0;
}

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

char* get_node_content(TSNode node, char* source_code) {
  uint32_t start_offset = ts_node_start_byte(node);
  uint32_t end_offset = ts_node_end_byte(node);
  uint32_t length = end_offset - start_offset;

  char *name = (char *)malloc(length + 1);
  if (name == NULL) {
	fprintf(stderr, "Error allocating for name");
	exit(EXIT_FAILURE);
  }
  memcpy(name, source_code + start_offset, length);
  name[length] = '\0';

  return name;
}

bool filter_node(TSNode node, char* source_code, Pattern* p) {
  if (p->include_props.count == 0 && p->exclude_props.count == 0) {
	return true;
  }

  TSNode sibling = ts_node_next_sibling(node);
  size_t count = 0;
  while(!ts_node_is_null(sibling)) {
	if (strcmp(ts_node_type(sibling), "jsx_attribute") == 0)  {
	  TSNode prop = ts_node_child(sibling, 0);
	  char *name = get_node_content(prop, source_code);

	  for (size_t i = 0; i < p->exclude_props.count; i++) {
		if (strcmp(name, p->exclude_props.props[i].name) == 0)
		  return false;
	  }
	  for (size_t i = 0; i < p->include_props.count; i++) {
		if (strcmp(name, p->include_props.props[i].name) == 0)
		  count++;
	  }

	  free(name);
	}
	sibling = ts_node_next_sibling(sibling);
  }

  if (p->include_props.count > 0) {
	return p->include_props.count == count;
  }
  return true;

}

void traverse_node(TSNode node, const char* file_name, char* source_code, Pattern* p, Result *r) {
  const char *type = ts_node_type(node);
  if (strcmp(type, "jsx_self_closing_element") == 0 ||
	  strcmp(type, "jsx_opening_element") == 0) {

	// Component name is the second child, because first child is `<`
	TSNode child = ts_node_child(node, 1);
	if (!ts_node_is_null(child)) {
	  char *name = get_node_content(child, source_code);
	  if (strcmp(name, p->component) == 0) append_node(r, child);
	  free(name);
	}
  }

  for (uint32_t i = 0; i < ts_node_child_count(node); ++i) {
	traverse_node(ts_node_child(node, i), file_name, source_code, p, r);
  }
}

TSTree* build_tree(char* source_code, const char* file_name, TSParser* parser, Pattern *p, Result* r) {
  TSTree *tree = ts_parser_parse_string(parser, NULL, source_code, strlen(source_code));

  TSNode root_node = ts_tree_root_node(tree);
  traverse_node(root_node, file_name, source_code, p, r);

  return tree;
}

void print_result(TSNode node, char* source_code, char* file_name) {
  TSPoint point = ts_node_start_point(node);
  TSNode parent = ts_node_parent(node);
  if(!ts_node_is_null(parent)) {
	char* parent_name = get_node_content(parent, source_code);
	char *ptr = strtok(parent_name, "\n");
	int i = 0;
	while(ptr != NULL) {
	  if (i == 0) {
		printf("%s:%d:%s\n", file_name, i + point.row + 1, ptr);
	  } else {
		printf("%s-%d-%s\n", file_name, i + point.row + 1, ptr);
	  }
	  ptr = strtok(NULL, "\n");
	  i++;
	}
	printf("--\n");
	free(parent_name);
  }
}

int main(int argc, char** argv) {

  const char *directory = NULL;
  Pattern p = {0};
  char command[MAX_COMMAND_LINE_LENGTH];

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
  sprintf(command, "rg -l %s %s", p.component, directory);

  TSParser *parser = ts_parser_new();
  ts_parser_set_language(parser, tree_sitter_tsx());

  FILE *cmd;
  char file_path[MAX_FILE_NAME];

  cmd = popen(command, "r");
  if (cmd == NULL) {
    fprintf(stderr, "Error while running grep");
    exit(EXIT_FAILURE);
  }

  while (fgets(file_path, sizeof(file_path), cmd)) {
	file_path[strlen(file_path)-1] = '\0';
	char* source_code = readFile(file_path);
	Result result_ast = { .items = NULL, .size = 0, .capacity = 0 };

	TSTree* tree = build_tree(source_code, file_path, parser, &p, &result_ast);

	for (size_t i = 0; i < result_ast.size; i++) {
	  if(filter_node(result_ast.items[i], source_code, &p)) {
		print_result(result_ast.items[i], source_code, file_path);
	  }
	}

	free_result(&result_ast);
	free(source_code);
	ts_tree_delete(tree);
  }

  free_pattern(&p);
  ts_parser_delete(parser);
  pclose(cmd);

  return 0;
}
