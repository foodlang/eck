#include "common/def.h"

#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <stdarg.h>

static bool_t sclean = TRUE;

void dinfo(lex_token *site, const char *fmt, ...)
{
	size_t r, c;
	va_list v;
	assert(site);
	lex_site(site, &r, &c);

	va_start(v, fmt);
	fprintf(stderr, "(%ld, %ld)\x1B[36m info: ", r, c);
	vfprintf(stderr, fmt, v);
	va_end(v);
	fprintf(stderr, "\x1B[0m\n");
}

void dwarn(lex_token *site, const char *fmt, ...)
{
	size_t r, c;
	va_list v;
	assert(site);
	lex_site(site, &r, &c);

	va_start(v, fmt);
	fprintf(stderr, "(%ld, %ld)\x1B[33m warn: ", r, c);
	vfprintf(stderr, fmt, v);
	va_end(v);
	fprintf(stderr, "\x1B[0m\n");
}

void derror(lex_token *site, const char *fmt, ...)
{
	size_t r, c;
	va_list v;
	assert(site);
	lex_site(site, &r, &c);

	va_start(v, fmt);
	fprintf(stderr, "(%ld, %ld)\x1B[31m err: ", r, c);
	vfprintf(stderr, fmt, v);
	va_end(v);
	fprintf(stderr, "\x1B[0m\n");
	sclean = FALSE;
}

void dfatal(const char *fmt, ...)
{
	va_list v;
	va_start(v, fmt);
	fprintf(stderr, "(internal)\x1B[31m fatal: ");
	vfprintf(stderr, fmt, v);
	va_end(v);
	fprintf(stderr, "\x1B[0m\n");
	abort();
}

bool_t is_clean(void)
{
	return sclean;
}

void reset_diags(void)
{
	sclean = FALSE;
}
