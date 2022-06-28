#include "../common/def.h"

#include <assert.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>

/* Returns the size of a compatible type, in bytes. */
uint8_t type_compatible_size(uint8_t type)
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

type_glbl_kind type_globalize(foodtype *t)
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
			return TYPE_GLBL_NONE_IMPLICIT_CAST;
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
	lGlblKind = type_globalize(left);
	rGlblKind = type_globalize(right);

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
		lSize = type_compatible_size(left->kind);
		rSize = type_compatible_size(right->kind);

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

	printf("left and right parts of the expression are not compatible, %d != %d\n", left->kind, right->kind);
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

uint64_t eval(expression *tree, bool_t *failed)
{
	*failed = FALSE;
	switch (tree->kind)
	{
		case EXPRESSION_INTEGER_LITERAL:
			return tree->token.value.u64;
		
		case EXPRESSION_ADDITION:
			return eval(tree->left, failed) + eval(tree->right, failed);
		
		case EXPRESSION_SUBTRACTION:
			return eval(tree->left, failed) - eval(tree->right, failed);
		
		case EXPRESSION_MULTIPLY:
			return eval(tree->left, failed) * eval(tree->right, failed);
		
		case EXPRESSION_DIVISION:
			return eval(tree->left, failed) / eval(tree->right, failed);
		
		case EXPRESSION_MODULO:
			return eval(tree->left, failed) % eval(tree->right, failed);
		
		case EXPRESSION_BITWISE_AND:
			return eval(tree->left, failed) & eval(tree->right, failed);
		
		case EXPRESSION_BITWISE_OR:
			return eval(tree->left, failed) | eval(tree->right, failed);
		
		case EXPRESSION_BITWISE_XOR:
			return eval(tree->left, failed) ^ eval(tree->right, failed);

		case EXPRESSION_LSHIFT:
			/* % 64 is to prevent overflow */
			return eval(tree->left, failed) << eval(tree->right, failed) % 64;
		
		case EXPRESSION_RSHIFT:
			return eval(tree->left, failed) >> eval(tree->right, failed) % 64;
		
		case EXPRESSION_LOWER:
			return eval(tree->left, failed) < eval(tree->right, failed);
		
		case EXPRESSION_LOWER_OR_EQUAL:
			return eval(tree->left, failed) <= eval(tree->right, failed);
		
		case EXPRESSION_GREATER:
			return eval(tree->left, failed) > eval(tree->right, failed);
		
		case EXPRESSION_GREATER_OR_EQUAL:
			return eval(tree->left, failed) >= eval(tree->right, failed);
		
		case EXPRESSION_POSTFIX_BITWISE_NOT:
			return ~eval(tree->left, failed);
		
		case EXPRESSION_POSTFIX_LOGICAL_NOT:
			return !eval(tree->left, failed);
		
		case EXPRESSION_LOGICAL_AND:
			return eval(tree->left, failed) && eval(tree->right, failed);

		case EXPRESSION_LOGICAL_OR:
			return eval(tree->left, failed) || eval(tree->right, failed);
		
		case EXPRESSION_TERNARY_CONDITIONAL:
			return eval(tree->extra, failed) ? eval(tree->left, failed) : eval(tree->right, failed);
		
		case EXPRESSION_BOOLEAN_LITERAL:
			if (tree->token.kind == KEYWORD_TRUE) return TRUE;
			else return FALSE;

		default:
			*failed = TRUE;
			return 0;
	}
}

static void simplify_node(expression **node, uint64_t v)
{
	expression *discard;
	expression *new;
	
	new = calloc(1, sizeof(expression));
	memcpy(&new->type, &(*node)->type, sizeof(foodtype));
	new->kind = EXPRESSION_INTEGER_LITERAL;
	new->token.pos = (*node)->token.pos;
	new->token.kind = '0';
	new->token.value.u64 = v;
	discard = *node;
	*node = new;

	delete_tree(discard);
}

void esimple(expression **tree)
{
	bool_t fail_status = FALSE;
	uint64_t simplified;
	assert(tree && *tree);

	/* 1. Simplifying the left branch */
	if ((*tree)->left) {
		simplified = eval((*tree)->left, &fail_status);
		if (!fail_status) {
			simplify_node(&(*tree)->left, simplified);
		}
	}

	/* 2. Simplifying the right branch */
	if ((*tree)->right) {
		simplified = eval((*tree)->right, &fail_status);
		if (!fail_status) {
			simplify_node(&(*tree)->right, simplified);
		}
	}

	/* 3. Simplifying the extra branch */
	if ((*tree)->extra) {
		simplified = eval((*tree)->extra, &fail_status);
		if (!fail_status) {
			simplify_node(&(*tree)->extra, simplified);
		}
	}

	/*
		4. Final simplification
		Once all children branches have been simplified,
		we can simplify the full branch. It is important
		to do this step to prevent a case like this:
		(2 * 4) + (3 * 9) -> 8 + 27

		We would want to optimize 8 + 27, right?
		This is what the lines below do.
	*/
	simplified = eval(*tree, &fail_status);
	if (!fail_status) {
		simplify_node(tree, simplified);
	}
}

bool_t is_unsigned(foodtype *t)
{
	if (t->kind == TYPE_BOOL
	 || t->kind == TYPE_BYTE
	 || t->kind == TYPE_USHORT
	 || t->kind == TYPE_UINT
	 || t->kind == TYPE_ULONG
	 || t->kind == TYPE_POINTER
	 || t->kind == TYPE_REFERENCE)
		return TRUE;
	return FALSE;
}

bool_t is_binary(expression *e)
{
	if (e->kind >= EXPRESSION_MULTIPLY
	 && e->kind <= EXPRESSION_LOGICAL_OR)
		return TRUE;
	return FALSE;
}

size_t eweight(expression *tree)
{
	size_t yield = 1;
	if (!tree) return 0;

	yield += eweight(tree->left);
	yield += eweight(tree->right);
	yield += eweight(tree->extra);

	return yield;
}

bool_t is_sequence_point(expression_kind kind)
{
	if (kind == EXPRESSION_LOGICAL_AND
	 || kind == EXPRESSION_LOGICAL_OR
	 || kind == EXPRESSION_TERNARY_CONDITIONAL
	 || kind == EXPRESSION_ASSIGN
	 || kind == EXPRESSION_SUM_ASSIGN
	 || kind == EXPRESSION_DIFFERENCE_ASSIGN
	 || kind == EXPRESSION_PRODUCT_ASSIGN
	 || kind == EXPRESSION_QUOTIENT_ASSIGN
	 || kind == EXPRESSION_AND_ASSIGN
	 || kind == EXPRESSION_XOR_ASSIGN
	 || kind == EXPRESSION_OR_ASSIGN
	 || kind == EXPRESSION_LSHIFT_ASSIGN
	 || kind == EXPRESSION_RSHIFT_ASSIGN
	 || kind == EXPRESSION_COMPOUND) {

		return TRUE;
	}
	return FALSE;
}
