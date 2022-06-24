#include "../common/def.h"

#include <assert.h>

void declaration(void)
{
	foodtype t;
	lex_token tok;
	
	/* 1. Parse type */
	assert(tparse(&t));

	/* 2. Parse identifier */
	if (!lex_fetch(&tok)) {
		derror(&tok, "expected an identifier (end of file)\n");
		return;
	}

	if (tok.kind != 'I') {
		derror(&tok, "expected an identifier\n");
		return;
	}

	if (!decl((const char *)tok.value.str, &t)) {
		derror(&tok, "duplicate declaration; shadowing is not allowed in Food (at least yet)\n");
		return;
	}

	/* 2. Parse identifier */
	if (!lex_fetch(&tok)) {
		derror(&tok, "expected a semicolon (;) or an equal symbol (=)\n");
		return;
	}

	if (tok.kind == '=') {
		/* TODO: Assignments at declaration */
		dfatal("assignments during declarations are not yet supported\n");
	} else if (tok.kind == ';') {
		return;
	} else {
		derror(&tok, "expected a semicolon (;) or an equal symbol (=)\n");
		return;
	}
}

void statement(void)
{
	lex_token tok;
	size_t base;

	base = lex_pos();
	if (!lex_fetch(&tok)) {
		derror(&tok, "expected a statement\n");
		return;
	}
	lex_move(base);

	switch (tok.kind) {

		default: {
			foodtype dummy;
			if (tparse(&dummy)) {
				lex_move(base);
				declaration();
			} else {
				expression *tree;
				lex_move(base);
				tree = parse_expression();
				(void)tree; /* TODO: Something with the expression... */
			}
		}
	}
}
