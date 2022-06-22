#include "common/def.h"

/* Compiler entrypoint */
int main(void)
{
	expression *expr;
	FILE *tmp = tmpfile();
	fprintf(tmp, "1 + 2 * 3 & 54 + (3 + 4 * (2 + 1))");
	fseek(tmp, 0, SEEK_SET);
	lex_setup(tmp);
	expr = parse_expression();
	printf("Resulting expression:\n\n");
	expression_print(expr, 0);
	printf("\nResulting code:\n\n");
	gen_expression(&x86_64, expr);
	x86_64.clear(NULL);
	delete_tree(expr);
	fclose(tmp);
	return 0;
}