#include "common/def.h"

/* Compiler entrypoint */
int main(void)
{
	/*expression *expr;*/
	foodtype t;
	FILE *tmp = tmpfile();
	fprintf(tmp, "int &const*");
	fseek(tmp, 0, SEEK_SET);
	lex_setup(tmp);
	if (tparse(&t)) {
		tprint(&t, 0);
	} else {
		dfatal("failed to parse type\n");
	}
	/*expr = parse_expression();
	printf("Resulting expression:\n\n");
	expression_print(expr, 0);
	printf("\nResulting code:\n\n");
	gen_expression(&x86_64, expr);
	x86_64.clear(NULL);
	delete_tree(expr);*/
	fclose(tmp);
	return 0;
}