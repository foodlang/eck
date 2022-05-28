/*
	Lexer for eck
	See header
*/
#include "lex.h"

#include <stdlib.h>
#include <ctype.h>

static FILE *s_fstream;
static uint64_t s_fstreamPos;

/* Returns current character. */
static char s_getc(void)
{
	char result = fgetc(s_fstream);
	ungetc(result, s_fstream);
	return result;
}

/* Goes forward then returns the next character */
static char s_advance(void)
{
	s_fstreamPos++;
	return fgetc(s_fstream);
}

/* Skips all the spaces */
static void s_skip_spaces(void)
{
	while (isspace(s_getc())) {
		(void)s_advance();
	}
}

/* Converts a hex digit to an integer value */
static uint8_t s_hex_to_int(char hex)
{
	if (isdigit(hex)) return hex - '0';
	else if (hex >= 'a' && hex <= 'f') return hex - 'a' + 10;
	else return hex - 'A' + 10;
}

/* Raises 10 to an exponent */
static long double s_pow10(int8_t raiseTo)
{
	long double accumulator = 10;
	int i;
	for (i = 0; i < raiseTo; i++)
		accumulator *= 10;
	return accumulator;
}

/* Parses a number */
static int s_parse_number(lex_value *yield)
{
	uint64_t result = 0;
	char c = s_advance();

	/* Octal, binary and hex number parsing */
	if (c == '0') {
		c = s_advance();
		/* Binary parsing */
		if (c == 'b' || c == 'B') {
			c = s_advance();
			while (c == '0' || c == '1') {
				result = result * 2 + c - '0';
				c = s_advance();
			}
			yield->u64 = result;
			return 0;
		/* Hex parsing */
		} else if (c == 'x' || c == 'X') {
			c = s_advance();
			while (isdigit(c) || (c >= 'a' && c <= 'f') || (c >= 'A' && c <= 'F')) {
				result = result * 16 + s_hex_to_int(c);
				c = s_advance();
			}
			yield->u64 = result;
			return 0;
		/* Octal parsing */
		} else {
			while (c >= '0' && c <= '7') {
				result = result * 8 + c - '0';
				c = s_advance();
			}
			yield->u64 = result;
			return 0;
		}
	}

	/* Base 10 parsing */
	while (isdigit(c)) {
		result = result * 10 + c - '0';
		c = s_advance();
	}

	/* IEE 754 */
	if (c == '.') {
		long double ldResult = result;
		long double scale = 1;
		c = s_advance();
		while (isdigit(c)) {
			scale /= 10;
			ldResult = ldResult + (long double)(c - '0') * scale;
			c = s_advance();
		}
		/* Exponential notation */
		if (c == 'e' || c == 'E') {
			int8_t raiseTo = 1;
			uint8_t expScale = 1;
			c = s_advance();
			/* Negate */
			if (c == '-') {
				c = s_advance();
				raiseTo = -raiseTo;
			}
			while (isdigit(c)) {
				uint64_t resultVal = raiseTo + (c - '0') * expScale;
				if (resultVal > UINT8_MAX) {
					fprintf(stderr, "lex number parser: TODO error handling, exponent out of bounds\n");
					abort();
				}
				raiseTo = resultVal;
				expScale *= 10;
				c = s_advance();
			}
			ldResult *= s_pow10(raiseTo);
		}
		yield->long_double = ldResult;
		return 1;
	}
	yield->u64 = result;
	return 0;
}

/*
	Usage:
	lex_setup(stream) where stream is a valid file stream with read permissions.
*/
void lex_setup(FILE *stream)
{
	/* null check. In Food, this is done with the [not_null] attribute. */
	if (stream == NULL) {
		fprintf(stderr, "lex_setup, fatal: invalid stream passed\n");
		abort();
	}

	s_fstream = stream;
	s_fstreamPos = 0;
}

int lex_fetch(lex_token *tokenBuffer)
{
	char c;
	lex_token tokenInstance;
	/* null check. In Food, this is done with the [not_null] attribute. */
	if (tokenBuffer == NULL) {
		fprintf(stderr, "lex_fetch, fatal: invalid token buffer passed\n");
		abort();
	}

	s_skip_spaces();
	c = s_getc();
	if (c == EOF) {
		return 0;
	} else if (isdigit(c)) {
		int isFloat;
		
		tokenInstance.pos = s_fstreamPos;
		isFloat = s_parse_number(&tokenInstance.value);
		tokenInstance.kind = isFloat ? '0' << 8 | '.' : '0';
		*tokenBuffer = tokenInstance;
		return 1;
	}
	return 0;
}
