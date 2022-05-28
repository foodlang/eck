#include "syntax/lex.h"

/* Compiler entrypoint */
int main(void)
{
	lex_token tokenBuffer;
	lex_setup(stdin);
	while (lex_fetch(&tokenBuffer)) {
		if (tokenBuffer.kind == '0') {
			printf("%ld: %ld\n", tokenBuffer.pos, tokenBuffer.value.u64);
		} else if (tokenBuffer.kind == ('0' << 8 | '.')) {
			printf("%ld: %Lf\n", tokenBuffer.pos, tokenBuffer.value.long_double);
		} else {
			printf("%ld: %c\n", tokenBuffer.pos, (char)tokenBuffer.kind);
		}
	}
	return 0;
}