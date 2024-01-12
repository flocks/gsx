#ifndef GSX_HEADER_H
#define GSX_HEADER_H

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <stdbool.h>

#define MAX_PROPS 10

// wrap in a structure because I expect more field in the future
typedef struct {
  char* name;
} Props;

typedef struct {
  Props props[MAX_PROPS];
  size_t count;
} Props_Arr;


typedef struct {
  char* component;
  Props_Arr include_props;
  Props_Arr exclude_props;
} Pattern;

void parse(Pattern *pattern, char* input);
void print_pattern(const Pattern* p);
void print_props(const Props* p);
void free_pattern(Pattern* p);

#endif /* GSX_HEADER_H */

