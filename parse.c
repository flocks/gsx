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

  char * strToken = strtok(&input[size], "," );
  while (strToken != NULL && p->nb_props < MAX_PROPS) {
	bool is_present = true;
	if (strToken[0] == '!') {
	  is_present = false;
	  strToken++;
	}
	Props props = {
	  .name = strToken,
	  .is_present = is_present
	};
	p->props[p->nb_props++] = props;
	strToken = strtok(NULL, ",");
  }
}

void print_props(const Props *p) {
  printf("Props name: %s\n", p->name);
  printf("Props presence: %d\n", p->is_present);
}

void print_pattern(const Pattern* p) {
  printf("Component: %s\n", p->component);
  for (size_t i = 0; i < p->nb_props; i++) {
	print_props(&p->props[i]);
	printf("\n");

  }
}

void free_pattern(Pattern* p) {
  free(p->component);
}
