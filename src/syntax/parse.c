/*
	Parser for ECK

	A parser makes meaning out of the Lexer's tokens.
*/

#include "../common/def.h"
#include <stdlib.h>
#include <string.h>
#include <assert.h>

/* A constructor for a literal expression. */
static expression *s_literal_expression(uint32_t kind, lex_token *token, foodtype *type)
{
	expression *yield = malloc(sizeof(expression));
	memset(yield, 0, sizeof(expression));
	yield->kind = kind;
	yield->token = *token;
	yield->type = *type;
	return yield;
}

/* A constructor for an unary expression.*/
static expression *s_unary_expression(uint32_t kind, lex_token *token, foodtype *type, expression *child)
{
	expression *yield = malloc(sizeof(expression));
	memset(yield, 0, sizeof(expression));
	yield->kind = kind;
	yield->token = *token;
	yield->type = *type;
	yield->left = child;
	return yield;
}

/* A constructor for a binary expression.
static expression *s_binary_expression(uint32_t kind, lex_token *token, foodtype *type, expression *left, expression *right)
{
	expression *yield = malloc(sizeof(expression));
	memset(yield, 0, sizeof(expression));
	yield->kind = kind;
	yield->token = *token;
	yield->type = *type;
	yield->left = left;
	yield->right = right;
	return yield;
}

/ * A constructor for a ternary expression.
static expression *s_ternary_expression(uint32_t kind, lex_token *token, foodtype *type, expression *extra, expression *left, expression *right)
{
	expression *yield = malloc(sizeof(expression));
	memset(yield, 0, sizeof(expression));
	yield->kind = kind;
	yield->token = *token;
	yield->type = *type;
	yield->extra = extra;
	yield->left = left;
	yield->right = right;
	return yield;
}*/

/* The currently read lexical token. */
static lex_token s_currentToken;

/* Parses literal expressions. */
static expression *parse_literal(void)
{
	foodtype literalType;
	expression *yield = NULL;
	if (!lex_fetch(&s_currentToken)) {
		abort();
	}

	if (s_currentToken.kind == '0') {
		literalType.qualifiers = 0;
		if (s_currentToken.value.i64 - s_currentToken.value.i32)
			 literalType.kind = TYPE_LONG;
		else literalType.kind = TYPE_INT;
		literalType.extra = NULL;
		yield = s_literal_expression(EXPRESSION_INTEGER_LITERAL, MEMORIZE(lex_token, &s_currentToken), &literalType);
		yield->isLValue = FALSE;
		return yield;
	} else if (s_currentToken.kind == OP2('0', '.')) {
		literalType.qualifiers = 0;
		if (s_currentToken.value.double_float - s_currentToken.value.single_float)
			 literalType.kind = TYPE_DOUBLE;
		else literalType.kind = TYPE_FLOAT;
		literalType.extra = NULL;
		yield = s_literal_expression(EXPRESSION_INTEGER_LITERAL, &s_currentToken, &literalType);
		yield->isLValue = FALSE;
		return yield;
	} else if (s_currentToken.kind == KEYWORD_TRUE) {
		literalType.qualifiers = 0;
		literalType.kind = TYPE_BOOL;
		literalType.extra = NULL;
		yield = s_literal_expression(EXPRESSION_BOOLEAN_LITERAL, &s_currentToken, &literalType);
		yield->isLValue = FALSE;
		return yield;
	} else if (s_currentToken.kind == KEYWORD_FALSE) {
		literalType.qualifiers = 0;
		literalType.kind = TYPE_BOOL;
		literalType.extra = NULL;
		yield = s_literal_expression(EXPRESSION_BOOLEAN_LITERAL, &s_currentToken, &literalType);
		yield->isLValue = FALSE;
		return yield;
	}
	printf("invalid expression\n");
	abort();
}

/* Parses all postfix unary operators and also the member access operators. */
static expression *parse_postfix(void)
{
	expression *yield = parse_literal();

	if (!lex_fetch(&s_currentToken)) {
		return yield;
	}

	while (
	   s_currentToken.kind == OP2('+', '+')
	|| s_currentToken.kind == OP2('-', '-')
	/*|| s_currentToken.kind == '(' <--- comment because it is not yet supported
	|| s_currentToken.kind == '['
	|| s_currentToken.kind == '.'*/) {
		if (!yield->isLValue) {
			fprintf(stderr, "postfix operators only take in LValues\n");
			abort();
		}
		if (s_currentToken.kind == OP2('+', '+')) {
			yield = s_unary_expression(EXPRESSION_POSTFIX_INCREMENT, &s_currentToken, &yield->type, yield);
		} else if (s_currentToken.kind == OP2('-', '-')) {
			yield = s_unary_expression(EXPRESSION_POSTFIX_DECREMENT, &s_currentToken, &yield->type, yield);
		}
		lex_fetch(&s_currentToken);
	}
	return yield;
}

static expression *parse_prefix(void)
{
	expression *yield;
	const size_t base = lex_pos();
	lex_token op;
	lex_fetch(&op);
	if (op.kind == OP2('+', '+')
	 || op.kind == OP2('-', '-')
	 || op.kind == '+'
	 || op.kind == '-'
	 || op.kind == '!'
	 || op.kind == '~'
	 || op.kind == '*'
	 || op.kind == '&'
	 || op.kind == KEYWORD_SIZEOF
	 || op.kind == KEYWORD_ALIGNOF) {

		uint32_t kind = 0;
		switch (op.kind)
		{
			case OP2('+', '+'): kind = EXPRESSION_PREFIX_INCREMENT; break;
			case OP2('-', '-'): kind = EXPRESSION_PREFIX_DECREMENT; break;
			case '+': kind = EXPRESSION_POSTFIX_UNARY_PLUS; break;
			case '-': kind = EXPRESSION_POSTFIX_UNARY_MINUS; break;
			case '!': kind = EXPRESSION_POSTFIX_LOGICAL_NOT; break;
			case '~': kind = EXPRESSION_POSTFIX_BITWISE_NOT; break;
			case '*': kind = EXPRESSION_DEREFERENCE; break;
			case '&': kind = EXPRESSION_ADDRESS_OF; break;
		}
		assert(kind);

		/* TODO: Add support for sizeof() and alignof() */

		yield = parse_postfix();
		yield = s_unary_expression(kind, MEMORIZE(lex_token, &op), &yield->type, yield);
	} else {
		lex_move(base);
		yield = parse_postfix();
	}
	return yield;
}

expression *parse_expression(void)
{
	return parse_prefix();
}
