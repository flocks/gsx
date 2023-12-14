#include <linux/limits.h>
#include <tree_sitter/api.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <errno.h>



TSLanguage *tree_sitter_tsx();

char* readFile(char* file_name) {
  FILE *f = fopen(file_name, "r");

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

  fread(source_code, 1, size, f);
  fclose(f);

  return source_code;
}

int tree(char* source_code, TSParser* parser) {
  TSTree *tree = ts_parser_parse_string(
    parser,
    NULL,
    source_code,
    strlen(source_code)
  );

  TSNode root_node = ts_tree_root_node(tree);
  char *string = ts_node_string(root_node);
  printf("%s\n", string);

  // Free all of the heap-allocated memory.
  free(string);
  ts_tree_delete(tree);
  return 0;
}

int main(int argc, char** argv) {

  const char *directory = NULL;
  const char *pattern = NULL;
  char command[1024];

  if (argc < 2) {
	fprintf(stderr, "USAGE: tsjsx src Button.variant");
	exit(EXIT_FAILURE);
  }

  if (argc == 2) {
	pattern = argv[1];
  }

  if (argc >= 3) {
	directory = argv[1];
	pattern = argv[2];
  }

  if (directory == NULL) {
	directory = "";
  }

  if (strlen(directory) + strlen(pattern) > 1000) {
	fprintf(stderr, "OUPS");
	exit(EXIT_FAILURE);
  }
  sprintf(command, "grep -lr --include \\*.tsx %s %s", pattern, directory);

  // Create a parser.
  TSParser *parser = ts_parser_new();

  // Set the parser's language (JSON in this case).
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
    tree(source_code, parser);
    free(source_code);
  }
  ts_parser_delete(parser);

  pclose(cmd);
  return 0;
}

