#define _GNU_SOURCE // make popen available
#include <stdio.h>
#include <stdlib.h>
#include <errno.h>

#define CHUNK_SIZE 64*100

char* read_file(char* file_name) {
  FILE *f = fopen(file_name, "rb");

  if (f == NULL) {
    fprintf(stderr, "Couldn't open %s", file_name);
    fprintf(stderr, "%d", errno);

    exit(EXIT_FAILURE);
  }
  if (fseek(f, 0, SEEK_END) != 0) {
    fprintf(stderr, "Error reading %s", file_name);
    exit(EXIT_FAILURE);
  }
  long size = ftell(f);
  char* content = (char *)malloc((size+1) * sizeof(char));
  rewind(f);

  fread(content, 1, size, f);
  content[size] = '\0';

  fclose(f);
  return content;
}

char* read_command(char* command) {
  char *result =  (char *)malloc(CHUNK_SIZE * sizeof(char));
  size_t bytes_length = 0;
  size_t length = 0;

  FILE *output = popen(command, "r");
  if (output == NULL) {
	fprintf(stderr, "Error running command: %s\n", command);
	exit(EXIT_FAILURE);
  }
  while((bytes_length = fread(result, 1, CHUNK_SIZE, output)) > 0) {
	length += bytes_length;
	result = (char *)realloc(result, (length + 1) * sizeof(char));
  }
  result[length] = '\0';

  pclose(output);
  return result;

}
