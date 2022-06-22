#include "common/def.h"

/* Compiler entrypoint */
int main(void)
{
	expression *expr;
	FILE *tmp = tmpfile();
	fprintf(tmp, "1 + 2 * 3");
	fseek(tmp, 0, SEEK_SET);
	lex_setup(tmp);
	expr = parse_expression();
	expression_print(expr, 0);
	delete_tree(expr);
	fclose(tmp);
	return 0;
}