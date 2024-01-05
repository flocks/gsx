#include <stdio.h>
#include <stdbool.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>

#include "../src/utils.h"

typedef const char* Filepath;
typedef const char* Command;
typedef const char* Name;

typedef struct {
  Name name;
  Command command;
  Filepath file;
} Test;


bool run_test(Test* test) {
  bool success = true;
  char *result = read_command(test->command);
  char *expected = read_file(test->file);

  if (strlen(result) > 0 && memcmp(result, expected, strlen(result)) == 0) {
	printf("%s: OK\n", test->name);
  } else {
	printf("%s: KO\n", test->name);
	printf("Expected: \n");
	printf("%s\n", expected);
	printf("Got: \n");
	printf("%s\n", result);
	success = false;
  }
  printf("----------------------------------------\n");
  free(expected);
  free(result);
  return success;
}

Test tests[] = {
  (Test) {
	.name = "Button",
	.command = "./gsx test/samples/ Button",
	.file = "./test/simple.txt",
  },
  (Test) {
	.name = "Button.^variant",
	.command = "./gsx test/samples/ Button.^variant",
	.file = "./test/exclude_one_prop.txt",
  },
  (Test) {
	.name = "Button.variant",
	.command = "./gsx test/samples/ Button.variant",
	.file = "./test/one_prop.txt",
  },
  (Test) {
	.name = "Button.variant,^size",
	.command = "./gsx test/samples/ Button.variant,^size",
	.file = "./test/include_one_exclude_one.txt",
  },
};

int main(void) {
  size_t nb_test = sizeof(tests) / sizeof(tests[0]);
  printf("Running all tests\n\n");
  int success = 0;
  for (size_t i = 0; i < nb_test; ++i) {
	if (!run_test(&tests[i])) {
	  success = 1;
	}
  }
  return success;
}

