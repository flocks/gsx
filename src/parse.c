#include "parse.h"

void parse(Pattern *pattern, char* input) {
  size_t size = 0;
  while (input[size] != '\0' && input[size] != '.') {
	++size;
  };

  char* component = (char*)malloc(size*sizeof(char));
  memcpy(component, input, size);
  component[size] = '\0';

  if(input[size] == '.') size++;

  pattern->component = component;

  char* strToken = strtok(&input[size], ",");
  while (strToken != NULL) {
	if (strToken[0] == '^') {
	  Props props = { .name = ++strToken };
	  pattern->exclude_props.props[pattern->exclude_props.count++] = props;
	}
	else {
	  Props props = { .name = strToken };
	  pattern->include_props.props[pattern->include_props.count++] = props;
	}
	strToken = strtok(NULL, ",");
  }
}

void print_props(const Props *pattern) {
  printf("Props name: %s\n", pattern->name);
}

void print_pattern(const Pattern* pattern) {
  printf("Component: %s\n", pattern->component);
  printf("Include: \n");
  for (size_t i = 0; i < pattern->include_props.count; i++) {
	print_props(&pattern->include_props.props[i]);
	printf("\n");

  }
  printf("Exclude: \n");
  for (size_t i = 0; i < pattern->exclude_props.count; i++) {
	print_props(&pattern->exclude_props.props[i]);
	printf("\n");

  }
}

void free_pattern(Pattern* pattern) {
  free(pattern->component);
}
