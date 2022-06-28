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

static void parse_locals(void)
{
	foodtype discard_foodtype;
	size_t base;

	base = lex_pos();
	while (tparse(&discard_foodtype)) {
		lex_move(base);
		declaration();
		base = lex_pos();
	}
	lex_move(base);
}

void statement(void)
{
	lex_token tok;
	if (!lex_peek(&tok)) {
		derror(&tok, "expected a statement\n");
		return;
	}

	switch (tok.kind) {

		case ';':
			lex_fetch(&tok);
			return;
		
		case '{': {
			lex_fetch(&tok);
			scope_enter();
			parse_locals();
			while (lex_peek(&tok) && tok.kind != '}') {
				statement();
			}
			lex_fetch(&tok);
			scope_leave();
			return;
		}

		default: {
			expression *tree;
			tree = parse_expression();
			expression_print(tree, 0);
			if (!lex_fetch(&tok)) {
				derror(&tok, "expected a semicolon\n");
				return;
			}
			if (tok.kind != ';') {
				derror(&tok, "expected a semicolon\n");
				return;
			}
			rfree(g_expression(tree));
			delete_tree(tree);
		}
	}
}
