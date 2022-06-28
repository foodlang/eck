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

		case KEYWORD_IF: {
			expression *condition;
			int condition_reg;
			uint64_t condition_label, then_label, lead_label, else_label = 0;
			lex_fetch(&tok);
			if (!lex_fetch(&tok)) {
				derror(&tok, "expected open bracket (\n");
				return;
			}
			if (tok.kind != '(') {
				derror(&tok, "expected open bracket (\n");
				return;
			}
			condition = parse_expression();
			if (!lex_fetch(&tok)) {
				derror(&tok, "expected closing bracket )\n");
				return;
			}
			if (tok.kind != ')') {
				derror(&tok, "expected closing bracket )\n");
				return;
			}
			condition_label = label();
			then_label = label();
			lead_label = label();
			code("jmp .L%ld", condition_label);
			code("\r.L%ld:", then_label);
			statement();
			code("jmp .L%ld", lead_label);
			if (lex_peek(&tok) && tok.kind == KEYWORD_ELSE) {
				lex_fetch(&tok);
				else_label = label();
				code("\r.L%ld:", else_label);
				statement();
				code("jmp .L%ld", lead_label);
			}
			code("\r.L%ld:", condition_label);
			condition_reg = g_expression(condition);
			rfree(condition_reg);
			code("test %s, %s", rget(condition_reg, 1), rget(condition_reg, 1));
			code("jne .L%ld", then_label);
			/* The else label must be non-null at this point, as we generate at least three labels before. */
			if (else_label) code("jmp .L%ld", else_label);
			code("\r.L%ld:", lead_label);
			return;
		}

		case KEYWORD_WHILE: {
			expression *condition;
			int condition_reg;
			uint64_t condition_label, lead_label;
			lex_fetch(&tok);
			if (!lex_fetch(&tok)) {
				derror(&tok, "expected open bracket (\n");
				return;
			}
			if (tok.kind != '(') {
				derror(&tok, "expected open bracket (\n");
				return;
			}
			condition = parse_expression();
			if (!lex_fetch(&tok)) {
				derror(&tok, "expected closing bracket )\n");
				return;
			}
			if (tok.kind != ')') {
				derror(&tok, "expected closing bracket )\n");
				return;
			}
			condition_label = label();
			lead_label = label();
			code("\r.L%ld:", condition_label);
			condition_reg = g_expression(condition);
			rfree(condition_reg); /* TODO: should this be done? */
			code("test %s, %s", rget(condition_reg, 1), rget(condition_reg, 1));
			code("je .L%ld", lead_label);
			statement();
			code("jmp .L%ld", condition_label);
			code("\r.L%ld:", lead_label);
			return;
		}

		default: {
			expression *tree;
			tree = parse_expression();
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
