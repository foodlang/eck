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
		derror(&tok, "duplicate declaration; shadowing is not allowed in Food 1.0\n");
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
			size_t to_allocate = 0;
			lex_fetch(&tok);
			scope_enter();
			parse_locals();
			to_allocate = required_size_for_scope();
			if (to_allocate) {
				code("push rbp");
				code("mov rbp, rsp");
				code("sub rsp, %d", to_allocate);
			}
			while (lex_peek(&tok) && tok.kind != '}') {
				statement();
			}
			lex_fetch(&tok);
			scope_leave();
			if (to_allocate) {
				code("pop rbp");
			}
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
			goto_label("jmp", condition_label);
			here_label(then_label);
			statement();
			goto_label("jmp", lead_label);
			if (lex_peek(&tok) && tok.kind == KEYWORD_ELSE) {
				lex_fetch(&tok);
				else_label = label();
				here_label(else_label);
				statement();
				goto_label("jmp", lead_label);
			}
			here_label(condition_label);
			condition_reg = g_expression(condition);
			rfree(condition_reg);
			code("test %s, %s", rget(condition_reg, 1), rget(condition_reg, 1));
			goto_label("jne", then_label);
			/* The else label must be non-null at this point, as we generate at least three labels before. */
			if (else_label) goto_label("jmp", else_label);
			here_label(lead_label);
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
			here_label(condition_label);
			condition_reg = g_expression(condition);
			rfree(condition_reg); /* TODO: should this be done? */
			code("test %s, %s", rget(condition_reg, 1), rget(condition_reg, 1));
			goto_label("je", lead_label);
			statement();
			goto_label("jmp", condition_label);
			here_label(lead_label);
			return;
		}

		case KEYWORD_DO: {
			uint64_t do_label;
			expression *condition;
			int condition_reg;
			lex_fetch(&tok);
			do_label = label();
			here_label(do_label);
			statement();
			if (!lex_fetch(&tok)) {
				derror(&tok, "expected while keyword\n");
				return;
			}
			if (tok.kind != KEYWORD_WHILE) {
				derror(&tok, "expected closing bracket )\n");
				return;
			}
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
			if (!lex_fetch(&tok)) {
				derror(&tok, "expected semicolon ;\n");
				return;
			}
			if (tok.kind != ';') {
				derror(&tok, "expected semicolon ;\n");
				return;
			}
			condition_reg = g_expression(condition);
			code("test %s, %s", rget(condition_reg, 1), rget(condition_reg, 1));
			goto_label("jne", do_label);
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
