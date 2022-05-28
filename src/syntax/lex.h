#ifndef ECK_SYNTAX_LEX_H
#define ECK_SYNTAX_LEX_H

/*
	Lexer for eck
	Does the lexing for every token Food needs

	Functions:
	void lex_setup(FILE *stream): Setups the lexer
	int lex_fetch(Token *const tokenBuffer): Gets the next token. Returns 0 if EOF
*/

#include <stdio.h>
#include "../common/def.h"

void lex_setup(FILE *stream);
int lex_fetch(lex_token *tokenBuffer);

#endif