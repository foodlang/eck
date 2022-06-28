#include "common/def.h"

/* Compiler entrypoint */
int main(void)
{
	/*expression *expr;
	foodtype t;*/
	FILE *tmp = tmpfile();
	asm_target = stdout;
	fprintf(tmp, "{ 1 ? 2 : 3; }");
	fseek(tmp, 0, SEEK_SET);
	lex_setup(tmp);
	statement();
	dump_all();
	fclose(tmp);
	return 0;
}