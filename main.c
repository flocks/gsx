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

char* find_node_name(TSNode node, char* source_code) {
  size_t start_offset = ts_node_start_byte(node);
  size_t end_offset = ts_node_end_byte(node);
  size_t length = end_offset - start_offset;

  char *name = (char *)malloc(length + 1);
  memcpy(name, source_code + start_offset, length);
  name[length] = '\0';

  return name;
}

int check_prop(char* name, Pattern* p) {
  for (size_t i = 0; i < p->nb_props; i++) {
	if (strncmp(p->props[i].name, name, strlen(p->props[i].name)) == 0) {
	  /* if (p->props[i].is_present == false) return 0; */
	  return 1;
	}
  }
  return 0;
}

void traverse_node(TSNode node, const char* file_name, char* source_code, Pattern* p) {
  const char *name = ts_node_type(node);
  if (strcmp(name, "jsx_self_closing_element") == 0 ||
	  strcmp(name, "jsx_opening_element") == 0) {

	// Component name is the second child, because first child is `<`
	TSNode child = ts_node_child(node, 1);
	if (!ts_node_is_null(child)) {
	  char *name = find_node_name(child, source_code);
	  // component match! we need to check its props
	  if (strcmp(name, p->component) == 0) {
		TSPoint point = ts_node_start_point(node);
		TSNode sibling = ts_node_next_sibling(child);
		bool valid = true;
		size_t count = 0;
		for (size_t i = 0; i < p->nb_props; i++) {
		  if (p->props[i].is_present) count++;
		}
		size_t count_match = 0;

		while(!ts_node_is_null(sibling)) {
		  if (strcmp(ts_node_type(sibling), "jsx_attribute") == 0) {
			if (p->nb_props == 0) {
				printf("%s:%d:%d %s\n", file_name, point.row + 1, point.column + 1, name);
				break;
			}
			char *propName = find_node_name(sibling, source_code);

			for (size_t i = 0; i < p->nb_props; i++) {
			  bool props_match = strncmp(p->props[i].name,propName,  strlen(p->props[i].name)) == 0;
			  if (p->props[i].is_present == false && props_match) {
				valid = false;
				break;
			  } else if (p->props[i].is_present && props_match) {
				count_match++;
			  }
			}

			free(propName);
		  }
		  sibling = ts_node_next_sibling(sibling);
		}
		if (count > 0 && count_match < count) {
		  valid = false;
		} 
		if (valid) {
		  printf("%s:%d:%d %s\n", file_name, point.row + 1, point.column + 1, name);
		}
	  }
	  free(name);
	}
  }

  // TODO how to skip node that we already seen with the siblings discovery
  for (uint32_t i = 0; i < ts_node_child_count(node); ++i) {
	traverse_node(ts_node_child(node, i), file_name, source_code, p);
  }
}

int tree(char* source_code, const char* file_name, TSParser* parser, Pattern *p) {
  TSTree *tree = ts_parser_parse_string(
    parser,
    NULL,
    source_code,
    strlen(source_code)
  );

  TSNode root_node = ts_tree_root_node(tree);
  /* print_pattern(p); */
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
  sprintf(command, "rg -l %s %s", p.component, directory);

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
    tree(source_code, result, parser, &p);
    free(source_code);
  }

  ts_parser_delete(parser);
  pclose(cmd);
  free_pattern(&p);

  return 0;
}
