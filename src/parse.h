#ifndef GSX_HEADER_H
#define GSX_HEADER_H

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <stdbool.h>

#define MAX_PROPS 10

typedef struct {
  char* name;
  bool is_present;
} Props;

typedef struct {
  char* component;
  Props props[MAX_PROPS];
  size_t nb_props;
} Pattern;

void parse(Pattern *p, char* input);
void print_pattern(const Pattern* p);
void print_props(const Props* p);
void free_pattern(Pattern* p);

#endif /* GSX_HEADER_H */

