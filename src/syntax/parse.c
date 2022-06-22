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

/* A constructor for a binary expression. */
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

/* A constructor for a ternary expression.
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
		fprintf(stderr, "no input is present\n");
		abort();
	}

	if (s_currentToken.kind == '0') {
		literalType.qualifiers = 0;
		if (s_currentToken.value.i64 - s_currentToken.value.i32)
			 literalType.kind = TYPE_LONG;
		else literalType.kind = TYPE_INT;
		literalType.extra = NULL;
		yield = s_literal_expression(EXPRESSION_INTEGER_LITERAL, &s_currentToken, &literalType);
		yield->isLValue = FALSE;
		return yield;
	} else if (s_currentToken.kind == OP2('0', '.')) {
		literalType.qualifiers = 0;
		if (s_currentToken.value.double_float - s_currentToken.value.single_float)
			 literalType.kind = TYPE_DOUBLE;
		else literalType.kind = TYPE_FLOAT;
		literalType.extra = NULL;
		yield = s_literal_expression(EXPRESSION_FLOATING_LITERAL, &s_currentToken, &literalType);
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
	fprintf(stderr, "invalid expression\n");
	abort();
}

/* Parses all postfix unary operators and also the member access operators. */
static expression *parse_postfix(void)
{
	expression *yield;
	size_t position;

	yield = parse_literal();
	position = lex_pos();
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
			derror(&s_currentToken, "postfix operators require lvalue operands\n");
			break;
		}
		if (s_currentToken.kind == OP2('+', '+')) {
			yield = s_unary_expression(EXPRESSION_POSTFIX_INCREMENT, &s_currentToken, &yield->type, yield);
		} else if (s_currentToken.kind == OP2('-', '-')) {
			yield = s_unary_expression(EXPRESSION_POSTFIX_DECREMENT, &s_currentToken, &yield->type, yield);
		}
		position = lex_pos();
		lex_fetch(&s_currentToken);
	}
	lex_move(position);
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

		uint32_t kind = kind_unary(op.kind);
		assert(kind);

		/* TODO: Add support for sizeof() and alignof() */
		yield = parse_postfix();
		if (op.kind == '&' && !yield->isLValue) {
			derror(&op, "address-of operator (&v) requires a lvalue operand\n");
		}

		yield = s_unary_expression(kind, &op, &yield->type, yield);
	} else {
		lex_move(base);
		yield = parse_postfix();
	}
	return yield;
}

static expression *multiplicative(void)
{
	expression *left;
	expression *right;
	foodtype type;
	size_t base;
	lex_token operator;
	left = parse_prefix();
	base = lex_pos();
	if (!lex_fetch(&operator))
		return left;
	while (operator.kind == '*' || operator.kind == '/' || operator.kind == '%') {
		right = parse_prefix();
		type_expression(&type, NULL, &left->type, &right->type);
		left = s_binary_expression(kind_binary(operator.kind),
			&operator, & type, left, right);
		base = lex_pos();
		if (!lex_fetch(&operator))
			break;
	}
	/*if (type_globalize(&type) == TYPE_GLBL_NONE_IMPLICIT_CAST) {
		derror(&operator, "type must be arithmetic-capable\n");
	}*/
	lex_move(base);
	return left;
}

static expression *additive(void)
{
	expression *left;
	expression *right;
	foodtype type;
	size_t base;
	lex_token operator;
	left = multiplicative();
	base = lex_pos();
	if (!lex_fetch(&operator))
		return left;
	while (operator.kind == '+' || operator.kind == '-') {
		right = multiplicative();
		type_expression(&type, NULL, &left->type, &right->type);
		left = s_binary_expression(kind_binary(operator.kind),
			&operator, & type, left, right);
		base = lex_pos();
		if (!lex_fetch(&operator))
			break;
	}
	/*if (type_globalize(&type) == TYPE_GLBL_NONE_IMPLICIT_CAST) {
		derror(&operator, "type must be arithmetic-capable\n");
	}*/
	lex_move(base);
	return left;
}

static expression *shifts(void)
{
	expression *left;
	expression *right;
	foodtype type;
	size_t base;
	lex_token operator;
	left = additive();
	base = lex_pos();
	if (!lex_fetch(&operator))
		return left;
	while (operator.kind == OP2('<', '<') || operator.kind == OP2('>', '>')) {
		right = additive();
		type_expression(&type, NULL, &left->type, &right->type);
		left = s_binary_expression(kind_binary(operator.kind),
			&operator, & type, left, right);
		base = lex_pos();
		if (!lex_fetch(&operator))
			break;
	}
	/*if (type_globalize(&type) != TYPE_GLBL_INTEGER)
		derror(&operator, "bit shifts require integer operands\n");*/
	lex_move(base);
	return left;
}

static expression *compare(void)
{
	expression *left;
	expression *right;
	foodtype type;
	size_t base;
	lex_token operator;
	left = shifts();
	base = lex_pos();
	if (!lex_fetch(&operator))
		return left;
	while (operator.kind == '+' || operator.kind == '-') {
		right = shifts();
		type_expression(&type, NULL, &left->type, &right->type);
		left = s_binary_expression(kind_binary(operator.kind),
			&operator, & type, left, right);
		base = lex_pos();
		if (!lex_fetch(&operator))
			break;
	}
	/*if (type_globalize(&type) == TYPE_GLBL_NONE_IMPLICIT_CAST)
		derror(&operator, "type must be arithmetic-capable\n");*/
	lex_move(base);
	return left;
}

static expression *equality(void)
{
	expression *left;
	expression *right;
	foodtype type;
	size_t base;
	lex_token operator;
	left = compare();
	base = lex_pos();
	if (!lex_fetch(&operator))
		return left;
	while (operator.kind == OP2('=', '=') || operator.kind == OP2('!', '=')) {
		right = compare();
		type_expression(&type, NULL, &left->type, &right->type);
		left = s_binary_expression(kind_binary(operator.kind),
			&operator, & type, left, right);
		base = lex_pos();
		if (!lex_fetch(&operator))
			break;
	}
	/* no restrictions */
	lex_move(base);
	return left;
}

static expression *bitwise_and(void)
{
	expression *left;
	expression *right;
	foodtype type;
	size_t base;
	lex_token operator;
	left = equality();
	base = lex_pos();
	if (!lex_fetch(&operator))
		return left;
	while (operator.kind == '&') {
		right = equality();
		type_expression(&type, NULL, &left->type, &right->type);
		left = s_binary_expression(kind_binary(operator.kind),
			&operator, & type, left, right);
		base = lex_pos();
		if (!lex_fetch(&operator))
			break;
	}
	/*if (type_globalize(&type) != TYPE_GLBL_INTEGER)
		derror(&operator, "type must be integer\n");*/
	lex_move(base);
	return left;
}

static expression *bitwise_xor(void)
{
	expression *left;
	expression *right;
	foodtype type;
	size_t base;
	lex_token operator;
	left = bitwise_and();
	base = lex_pos();
	if (!lex_fetch(&operator))
		return left;
	while (operator.kind == '^') {
		right = bitwise_and();
		type_expression(&type, NULL, &left->type, &right->type);
		left = s_binary_expression(kind_binary(operator.kind),
			&operator, & type, left, right);
		base = lex_pos();
		if (!lex_fetch(&operator))
			break;
	}
	/*if (type_globalize(&type) != TYPE_GLBL_INTEGER)
		derror(&operator, "type must be integer\n");*/
	lex_move(base);
	return left;
}

static expression *bitwise_or(void)
{
	expression *left;
	expression *right;
	foodtype type;
	size_t base;
	lex_token operator;
	left = bitwise_xor();
	base = lex_pos();
	if (!lex_fetch(&operator))
		return left;
	while (operator.kind == '|') {
		right = bitwise_xor();
		type_expression(&type, NULL, &left->type, &right->type);
		left = s_binary_expression(kind_binary(operator.kind),
			&operator, & type, left, right);
		base = lex_pos();
		if (!lex_fetch(&operator))
			break;
	}
	/*if (type_globalize(&type) != TYPE_GLBL_INTEGER)
		derror(&operator, "type must be integer\n");*/
	lex_move(base);
	return left;
}

static expression *logical_and(void)
{
	expression *left;
	expression *right;
	foodtype type;
	size_t base;
	lex_token operator;
	left = bitwise_or();
	base = lex_pos();
	if (!lex_fetch(&operator))
		return left;
	while (operator.kind == OP2('&', '&')) {
		right = bitwise_or();
		type_expression(&type, NULL, &left->type, &right->type);
		left = s_binary_expression(kind_binary(operator.kind),
			&operator, & type, left, right);
		base = lex_pos();
		if (!lex_fetch(&operator))
			break;
	}
	/*if (type_globalize(&type) != TYPE_GLBL_INTEGER)
		derror(&operator, "type must be integer or boolean\n");*/
	lex_move(base);
	return left;
}

static expression *logical_or(void)
{
	expression *left;
	expression *right;
	foodtype type;
	size_t base;
	lex_token operator;
	left = logical_and();
	base = lex_pos();
	if (!lex_fetch(&operator))
		return left;
	while (operator.kind == OP2('|', '|')) {
		right = logical_and();
		type_expression(&type, NULL, &left->type, &right->type);
		left = s_binary_expression(kind_binary(operator.kind),
			&operator, & type, left, right);
		base = lex_pos();
		if (!lex_fetch(&operator))
			break;
	}
	/*if (type_globalize(&type) != TYPE_GLBL_INTEGER)
		derror(&operator, "type must be integer or boolean\n");*/
	lex_move(base);
	return left;
}

expression *parse_expression(void)
{
	return logical_or();
}
