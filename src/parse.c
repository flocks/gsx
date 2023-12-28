#include "parse.h"

void parse(Pattern *p, char* input) {
  size_t size = 0;
  while (input[size] != '\0' && input[size] != '.') {
	++size;
  };

  char* component = (char*)malloc(size*sizeof(char));
  memcpy(component, input, size);
  component[size] = '\0';

  if(input[size] == '.') size++;

  p->component = component;

  char* strToken = strtok(&input[size], ",");
  while (strToken != NULL) {
	if (strToken[0] == '^') {
	  Props props = { .name = ++strToken };
	  p->exclude_props.props[p->exclude_props.count++] = props;
	}
	else {
	  Props props = { .name = strToken };
	  p->include_props.props[p->include_props.count++] = props;
	}
	strToken = strtok(NULL, ",");
  }
}

void print_props(const Props *p) {
  printf("Props name: %s\n", p->name);
}

void print_pattern(const Pattern* p) {
  printf("Component: %s\n", p->component);
  printf("Include: \n");
  for (size_t i = 0; i < p->include_props.count; i++) {
	print_props(&p->include_props.props[i]);
	printf("\n");

  }
  printf("Exclude: \n");
  for (size_t i = 0; i < p->exclude_props.count; i++) {
	print_props(&p->exclude_props.props[i]);
	printf("\n");

  }
}

void free_pattern(Pattern* p) {
  free(p->component);
}
