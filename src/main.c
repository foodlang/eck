#include "common/def.h"
#include <malloc.h>
#include <string.h>

/* Compiler entrypoint */
int main(int argc, char *argv[])
{
	int i, yield = 0;
	char *source;
	char *output;
	bool_t status;
	size_t len;
	if (argc < 0) {
		dfatal("No file specified.");
	}

	for (i = 1; i < argc; i++) {
		source = argv[i];
		len = strlen(source);
		output = malloc(len + 2);
		strcpy(output, source);
		output[len] = '.';
		output[len + 1] = 's';

		status = compile_object(source, output);

		free(output);

		if (!status) {
			yield = 1;
		}
	}

	return yield;
}