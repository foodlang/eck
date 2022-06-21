#include "common/def.h"

/* Compiler entrypoint */
int main(void)
{
	expression *expr;
	lex_setup(stdin);
	expr = parse_expression();
	printf("%d\n", expr->kind);
	/*if (expr->token.kind == '0') {
		printf("%ld: %ld\n", expr->token.pos, expr->token.value.u64);
	} else if (expr->token.kind == ('0' << 8 | '.')) {
		printf("%ld: %Lf\n", expr->token.pos, expr->token.value.long_double);
	} else if (expr->token.kind == 'I') {
		printf("%ld: %s\n", expr->token.pos, expr->token.value.str);
	} else if (expr->token.kind >= KEYWORD_ATOMIC) {
		printf("%ld: %ld\n", expr->token.pos, expr->token.kind);
	} else {
		printf("%ld: %c\n", expr->token.pos, (char)expr->token.kind);
	}*/

	delete_tree(expr);
	return 0;
}