#include "common/def.h"
#include <assert.h>
#include <string.h>

bool_t compile_object(const char *source, const char *output)
{
	size_t pos, source_length;
	FILE *sfile, *sout;
	lex_token token;
	memset(&token, 0, sizeof(lex_token));

	sfile = fopen(source, "r");
	sout = fopen(output, "w");

	assert(sfile);
	assert(sout);

	pos = 0; /* should be at start of file. */
	fseek(sfile, 0, SEEK_END);
	source_length = ftell(sfile);
	fseek(sfile, 0, SEEK_SET);

	asm_target = sout;
	lex_setup(sfile);

	while (pos < source_length) {
		statement(); /* TODO: Change to declaration when functions */
		pos = lex_pos();
	}

	if (!is_clean()) {
		return FALSE;
	}
	return TRUE;
}
