#include "../common/def.h"
#include <string.h>

static void rfree(gbranch_ref b)
{
	(void)b;
	fprintf(stdout, "\tpop eax\n");
}

static gbranch_ref primary(lex_value v)
{
	fprintf(stdout, "\tpush dword %d\n", v.u32);
	return NULL;
}

static gbranch_ref add(gbranch_ref l, gbranch_ref r)
{
	(void)l;
	(void)r;

	fprintf(stdout, "\tpop eax\n");
	fprintf(stdout, "\tpop ebx\n");
	fprintf(stdout, "\tadd eax, ebx\n");
	fprintf(stdout, "\tpush eax\n");
	return NULL;
}

static gbranch_ref sub(gbranch_ref l, gbranch_ref r)
{
	(void)l;
	(void)r;

	fprintf(stdout, "\tpop eax\n");
	fprintf(stdout, "\tpop ebx\n");
	fprintf(stdout, "\tsub eax, ebx\n");
	fprintf(stdout, "\tpush eax\n");
	return NULL;
}

static gbranch_ref mul(gbranch_ref l, gbranch_ref r)
{
	(void)l;
	(void)r;

	fprintf(stdout, "\tpop eax\n");
	fprintf(stdout, "\tpop ebx\n");
	fprintf(stdout, "\timul ebx\n");
	fprintf(stdout, "\tpush eax\n");
	return NULL;
}

static gbranch_ref div(gbranch_ref l, gbranch_ref r)
{
	(void)l;
	(void)r;

	fprintf(stdout, "\tpop eax\n");
	fprintf(stdout, "\tpop ebx\n");
	fprintf(stdout, "\tcqo\n");
	fprintf(stdout, "\timul ebx\n");
	fprintf(stdout, "\tpush eax\n");
	return NULL;
}

static gbranch_ref mod(gbranch_ref l, gbranch_ref r)
{
	(void)l;
	(void)r;

	fprintf(stdout, "\tpop eax\n");
	fprintf(stdout, "\tpop ebx\n");
	fprintf(stdout, "\tcqo\n");
	fprintf(stdout, "\timul ebx\n");
	fprintf(stdout, "\tpush edx\n");
	return NULL;
}

generator x86_64 =
{
	rfree,
	primary,
	add,
	sub,
	mul,
	div,
	mod
};
