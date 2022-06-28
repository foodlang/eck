#include "../common/def.h"

#include <assert.h>
#include <stdarg.h>

typedef const char *regname;

#define REG_COUNT 11

static regname r64[REG_COUNT] = { "rbx", "rcx", "rdx", "r8", "r9", "r10", "r11", "r12", "r13", "r14", "r15", };
static regname r32[REG_COUNT] = { "ebx", "ecx", "edx", "r8d", "r9d", "r10d", "r11d", "r12d", "r13d", "r14d", "r15d", };
static regname r16[REG_COUNT] = { "bx", "cx", "dx", "r8w", "r9w", "r10w", "r11w", "r12w", "r13w", "r14w", "r15w" };
static regname r8[REG_COUNT]  = { "bl", "cl", "dl", "r8b", "r9b", "r10b", "r11b", "r12b", "r13b", "r14b", "r15b" };
static bool_t rmsk[REG_COUNT] = { 0,    0,    0,    0,     0,     0,      0,      0,      0,      0,      0      };
static size_t label_count = 0;

static size_t label(void)
{
	return label_count++;
}

static int ralloc(void)
{
	int i;
	for (i = 0; i < REG_COUNT; i++) {
		if (!rmsk[i]) {
			rmsk[i] = 1;
			return i;
		}
	}
	dfatal("ran out of registers");
	return 0;
}

static regname rget(int reg, size_t size)
{
	regname *getter;
	assert(reg < REG_COUNT);
	getter = (size == 8) ? r64 : (size == 4) ? r32 : (size == 2) ? r16 : r8;
	return getter[reg];
}

static regname racc(size_t size)
{
	return (size == 8) ? "rax" : (size == 4) ? "eax" : (size == 2) ? "ax" : "al";
}

void rfree(int reg)
{
	assert(reg < REG_COUNT);
	rmsk[reg] = 0;
}

FILE *asm_target;

static void code(const char *fmt, ...)
{
	va_list v;
	va_start(v, fmt);
	fprintf(asm_target, "\t");
	vfprintf(asm_target, fmt, v);
	fprintf(asm_target, "\n");
	va_end(v);
}

static size_t rsizeof(foodtype *t)
{
	switch (t->kind)
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
		case TYPE_REFERENCE:
		case TYPE_FUNCTION:
			return 8;
		
		default:
			return 0;
	}
}

static int g_primary(expression *tree)
{
	size_t size;
	int reg;
	size = rsizeof(&tree->type);
	reg = ralloc();
	if (tree->token.value.u64) {
		code("mov %s, %lu ; primary(size = %d)", rget(reg, size), tree->token.value.u64, size);
	} else {
		code("xor %s, %s ; zero(size = %d)", rget(reg, size), rget(reg, size), size);
	}
	return reg;
}

static void g_binary(expression_kind e, int l, int r, int size, bool_t u)
{
	(void)u;
	switch (e)
	{
		case EXPRESSION_ADDITION:
			code("add %s, %s ; add", rget(l, size), rget(r, size));
			break;
		
		case EXPRESSION_SUBTRACTION:
			code("sub %s, %s ; sub", rget(l, size), rget(r, size));
			break;

		case EXPRESSION_MULTIPLY:
			code("imul %s, %s ; mul", rget(l, size), rget(r, size));
			break;
		
		case EXPRESSION_DIVISION:
			code("; div");
			code("mov %s, %s", racc(size), rget(l, size));
			code("%s", (size == 8) ? "cqo" : (size == 4) ? "cdq" : "cwd");
			code("idiv %s", rget(r, size)); /* yes. lsize, because division size must match here */
			code("mov %s, %s", rget(l, size), racc(size));
			break;

		case EXPRESSION_MODULO:
			code("; mod");
			if (rmsk[2] && l != 2 && r != 2) {
				code("push rdx ; saving data register");
			}
			code("mov %s, %s", racc(size), rget(l, size));
			code("%s", (size == 8) ? "cqo" : (size == 4) ? "cdq" : "cwd");
			code("idiv %s", rget(r, size)); /* yes. lsize, because division size must match here */
			if (rmsk[2] && l != 2 && r != 2) {
				code("mov %s, %s", rget(l, size), rget(2, size));
				code("pop rdx ; saving data register");
			} else if (l == 2) {
			} else {
				code("mov %s, %s", rget(l, size), rget(2, size));
			}
			break;

		case EXPRESSION_BITWISE_AND:
			code("and %s, %s ; bitwise and", rget(l, size), rget(r, size));
			break;
		
		case EXPRESSION_BITWISE_OR:
			code("or %s, %s ; bitwise or", rget(l, size), rget(r, size));
			break;
		
		case EXPRESSION_BITWISE_XOR:
			code("xor %s, %s ; bitwise xor", rget(l, size), rget(r, size));
			break;
		
		case EXPRESSION_LOGICAL_OR:
			code("; logical or");
			code("or %s, %s", rget(l, size), rget(r, size));
			code("setne %s", rget(l, 1));
			break;
		
		case EXPRESSION_LOGICAL_AND:
			code("; logical and");
			code("test %s, %s", rget(l, size), rget(l, size));
			code("setne %s", rget(l, 1));
			code("test %s, %s", rget(r, size), rget(r, size));
			code("setne %s", rget(r, 1));
			code("and %s, %s", rget(l, 1), rget(r, 1));
			break;
		
		case EXPRESSION_LOWER:
			code("; compare(lower)");
			code("cmp %s, %s", rget(l, size), rget(r, size));
			code("setl %s", rget(l, 1));
			break;

		case EXPRESSION_LOWER_OR_EQUAL:
			code("; compare(lower)");
			code("cmp %s, %s", rget(l, size), rget(r, size));
			code("setle %s", rget(l, 1));
			break;

		case EXPRESSION_GREATER:
			code("; compare(lower)");
			code("cmp %s, %s", rget(l, size), rget(r, size));
			code("setg %s", rget(l, 1));
			break;

		case EXPRESSION_GREATER_OR_EQUAL:
			code("; compare(lower)");
			code("cmp %s, %s", rget(l, size), rget(r, size));
			code("setge %s", rget(l, 1));
			break;

		case EXPRESSION_EQUAL:
			code("; compare(lower)");
			code("cmp %s, %s", rget(l, size), rget(r, size));
			code("sete %s", rget(l, 1));
			break;

		case EXPRESSION_NOT_EQUAL:
			code("; compare(lower)");
			code("cmp %s, %s", rget(l, size), rget(r, size));
			code("setne %s", rget(l, 1));
			break;

		case EXPRESSION_LSHIFT:
			code("sal %s, %s ; left shift", rget(l, size), rget(r, 1));
			break;

		case EXPRESSION_RSHIFT:
			code("shr %s, %s ; right shift", rget(l, size), rget(r, 1));
			break;

		default:
			dfatal("unsupported");
			break;
	}
}

int g_ternary(expression *tree)
{
	int e, l, r, true_label, exit_label;
	size_t size = rsizeof(&tree->type);
	e = g_expression(tree->extra);
	true_label = label();
	exit_label = label();

	/* Result is stored in right operand c in (a:b:c) */
	code("; ternary expression");
	code("test %s, %s", rget(e, 1), rget(e, 1));
	code("jne .L%ld", true_label);
	r = g_expression(tree->right);
	code("jmp .L%ld", exit_label);
	code("\r.L%ld:", true_label);
	l = g_expression(tree->left);
	code("mov %s, %s", rget(r, size), rget(l, size));
	code("\r.L%ld:", exit_label);
	return r;
}

int g_expression(expression *tree)
{
	int l = 0xFF, r = 0xFF;
	int size = rsizeof(&tree->type);

	if (tree->kind == EXPRESSION_INTEGER_LITERAL
	 || tree->kind == EXPRESSION_FLOATING_LITERAL
	 || tree->kind == EXPRESSION_BOOLEAN_LITERAL) {

		return g_primary(tree);
	}

	if (is_binary(tree)) {
		size_t lw, rw;
		lw = eweight(tree->left);
		rw = eweight(tree->right);
		if (lw > rw || is_sequence_point(tree->kind)) {
			l = g_expression(tree->left);
			r = g_expression(tree->right);
		} else {
			r = g_expression(tree->right);
			l = g_expression(tree->left);
		}
		g_binary(tree->kind, l, r, size, is_unsigned(&tree->type));
		rfree(r);
		return l;
	} else if (tree->kind == EXPRESSION_TERNARY_CONDITIONAL) {
		return g_ternary(tree);

	} else {
		l = g_expression(tree->left);
	}
	return 0;
}
