#include "../common/def.h"

#include <assert.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>

typedef enum type_glbl_kind
{
	TYPE_GLBL_NEVER_COMPATIBLE,
	TYPE_GLBL_INTEGER,
	TYPE_GLBL_FLOATING,
	TYPE_GLBL_POINTERS
} type_glbl_kind;

/* Returns the size of a compatible type, in bytes. */
static uint8_t s_type_compatible_size(uint8_t type)
{
	switch (type)
	{
		case TYPE_BOOL:
		case TYPE_BYTE:
		case TYPE_SBYTE:
			return 1;

		case TYPE_SHORT:
		case TYPE_USHORT:
		case TYPE_HALF:
			return 2;

		case TYPE_INT:
		case TYPE_UINT:
		case TYPE_FLOAT:
			return 4;
		
		case TYPE_LONG:
		case TYPE_ULONG:
		case TYPE_DOUBLE:
		case TYPE_POINTER:
		case TYPE_FUNCTION:
			return 8;
		
		/* Invalid types */
		default:
			return 0xFF;
	}
}

static type_glbl_kind s_globalize(foodtype *t)
{
	assert(t);
	switch (t->kind)
	{
		/*
			These are all integers types. These are
			all compatible between themselves by
			default.

			They can be converted to floats with some
			extra work, which is why the compiler must
			be notified.
		*/
		case TYPE_BOOL:
		case TYPE_BYTE:
		case TYPE_SBYTE:
		case TYPE_SHORT:
		case TYPE_USHORT:
		case TYPE_INT:
		case TYPE_UINT:
		case TYPE_LONG:
		case TYPE_ULONG:
			return TYPE_GLBL_INTEGER;

		/*
			These are the floating types. They are all
			compatible between themselves.

			They can be converted to integers, but the
			compiler must be notified.
		*/
		case TYPE_HALF:
		case TYPE_FLOAT:
		case TYPE_DOUBLE:
			return TYPE_GLBL_FLOATING;

		/*
			Pointers are very similar to integers, but
			aren't quite the same. Some operations cannot
			be done on pointers but can be done on integers.
		*/
		case TYPE_POINTER:
		case TYPE_FUNCTION:
			return TYPE_GLBL_POINTERS;

		/* The types remaining cannot be casted in any way. */
		default:
			return TYPE_GLBL_NEVER_COMPATIBLE;
	}
}

bool_t type_compatible(foodtype *left, foodtype *right)
{
	type_glbl_kind lGlblKind, rGlblKind;

	assert(left);
	assert(right);

	/* If both types are equal, no need to go further */
	if (left->kind == right->kind) {
		return TRUE;
	}

	/* Getting the global types involved is important. */
	lGlblKind = s_globalize(left);
	rGlblKind = s_globalize(right);

	/* TODO: Add support for casts */
	if (lGlblKind != rGlblKind) {
		return FALSE;
	}
	return TRUE;
}

void type_expression(foodtype *dest, foodtype *expected, foodtype *left, foodtype *right)
{
	if (expected) {
		/*
			You might see that below, we don't compare left and right.
			This is because if both are compatible with the expected type,
			then they must be compatible with eachother.
		*/
		if (type_compatible(left, expected) && (right != NULL && type_compatible(right, expected))) {
			memcpy(dest, expected, sizeof(foodtype));
			return;
		}

		printf("type expected is not compatible with expression\n");
		abort();
	}

	/*
		Unary operators only have one operand, which means that unless
		a type is expected or the operator isn't right, then there's no
		issue.
	*/
	if (right == NULL) {
		memcpy(dest, left, sizeof(foodtype));
		return;
	}

	if (type_compatible(left, right)) {
		/*
			We must find the widest type of the two types. To do that,
			we must get the maximal value of the two.
		*/
		uint8_t lSize, rSize;
		lSize = s_type_compatible_size(left->kind);
		rSize = s_type_compatible_size(right->kind);

		/*
			If both type sizes are compatible, get the type of the left
			branch.
		*/
		if (lSize == rSize) {
			memcpy(dest, left, sizeof(foodtype));
			return;
		}
		
		/*
			Getting the widest type. Originally, I was planning to use
			max() for that but then realized that there was no use for
			it.
		*/
		if (lSize > rSize) {
			memcpy(dest, left, sizeof(foodtype));
			return;
		} else {
			memcpy(dest, right, sizeof(foodtype));
			return;
		}
	}

	printf("left and right parts of the expression are not compatible\n");
	abort();
}

uint32_t kind_unary(uint64_t operator)
{
	uint32_t yield;
	switch (operator)
	{
		case OP2('+', '+'): yield = EXPRESSION_PREFIX_INCREMENT; break;
		case OP2('-', '-'): yield = EXPRESSION_PREFIX_DECREMENT; break;
		case '+': yield = EXPRESSION_POSTFIX_UNARY_PLUS; break;
		case '-': yield = EXPRESSION_POSTFIX_UNARY_MINUS; break;
		case '!': yield = EXPRESSION_POSTFIX_LOGICAL_NOT; break;
		case '~': yield = EXPRESSION_POSTFIX_BITWISE_NOT; break;
		case '*': yield = EXPRESSION_DEREFERENCE; break;
		case '&': yield = EXPRESSION_ADDRESS_OF; break;
		default: yield = 0; break;
	}
	return yield;
}

uint32_t kind_binary(uint64_t operator)
{
	uint32_t yield;
	switch (operator)
	{
		case OP2('|', '|'): yield = EXPRESSION_LOGICAL_OR; break;
		case OP2('&', '&'): yield = EXPRESSION_LOGICAL_AND; break;
		case '|': yield = EXPRESSION_BITWISE_OR; break;
		case '^': yield = EXPRESSION_BITWISE_XOR; break;
		case '&': yield = EXPRESSION_BITWISE_AND; break;
		case OP2('=', '='): yield = EXPRESSION_EQUAL; break;
		case OP2('!', '='): yield = EXPRESSION_NOT_EQUAL; break;
		case '<': yield = EXPRESSION_LOWER; break;
		case '>': yield = EXPRESSION_GREATER; break;
		case OP2('<', '='): yield = EXPRESSION_LOWER_OR_EQUAL; break;
		case OP2('>', '='): yield = EXPRESSION_GREATER_OR_EQUAL; break;
		case OP2('<', '<'): yield = EXPRESSION_LSHIFT; break;
		case OP2('>', '>'): yield = EXPRESSION_RSHIFT; break;
		case '+': yield = EXPRESSION_ADDITION; break;
		case '-': yield = EXPRESSION_SUBTRACTION; break;
		case '*': yield = EXPRESSION_MULTIPLY; break;
		case '/': yield = EXPRESSION_DIVISION; break;
		case '%': yield = EXPRESSION_MODULO; break;
		default: yield = 0; break;
	}
	return yield;
}

uint8_t prec_binary(uint64_t operator)
{
	switch (operator)
	{
		case OP2('|', '|'):
			return 1;

		case OP2('&', '&'):
			return 2;

		case '|':
			return 3;

		case '^':
			return 4;

		case '&':
			return 5;

		case OP2('=', '='):
		case OP2('!', '='):
			return 6;

		case '<':
		case '>':
		case OP2('<', '='):
		case OP2('>', '='):
			return 7;

		case OP2('<', '<'):
		case OP2('>', '>'):
			return 8;
		
		case '+':
		case '-':
			return 9;

		case '*':
		case '/':
		case '%':
			return 10;

		default:
			return 0;
	}
}

void expression_print(expression *expr, int indent)
{
	int i;
	assert(expr);

	for (i = 0; i < indent; i++) {
		printf("  ");
	}

	if (expr->kind == EXPRESSION_INTEGER_LITERAL) {
		printf("value: %lu\n", expr->token.value.u64);
	} else {
		printf("expression(%d):\n", expr->kind);
		if (expr->left) expression_print(expr->left, indent + 1);
		if (expr->right) expression_print(expr->right, indent + 1);
		if (expr->extra) expression_print(expr->extra, indent + 1);
	}
}
