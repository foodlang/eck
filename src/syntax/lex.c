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
	uint8_t ret = 0;
	if (isdigit(hex)) ret = hex - '0';
	else if (hex >= 'a' && hex <= 'f') ret = hex - 'a' + 10;
	else ret = hex - 'A' + 10;
	return ret;
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
static bool_t s_parse_number(lex_value *yield)
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
		/* Hex parsing */
		} else if (c == 'x' || c == 'X') {
			c = s_advance();
			while (isdigit(c) || (c >= 'a' && c <= 'f') || (c >= 'A' && c <= 'F')) {
				result = result * 16 + s_hex_to_int(c);
				c = s_advance();
			}
		/* Octal parsing */
		} else {
			while (c >= '0' && c <= '7') {
				result = result * 8 + c - '0';
				c = s_advance();
			}
		}
		yield->u64 = result;
		return FALSE;
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
		return TRUE;
	}
	yield->u64 = result;
	return FALSE;
}

/* Parses an escape sequence. c is the original character. */
static char s_parse_escape_sequence(char c)
{
	if (c == '\\') {
		switch (s_getc()) {
		/* Alert https://en.wikipedia.org/wiki/Bell_character */
			case 'a':
			case 'A':
				(void)s_advance();
				c = 0x07;
				break;
		/* Backspace https://en.wikipedia.org/wiki/Backspace */
			case 'b':
			case 'B':
				(void)s_advance();
				c = 0x08;
				break;
		/* Escape Character https://en.wikipedia.org/wiki/Escape_character */
			case 'e':
			case 'E':
				(void)s_advance();
				c = 0x1B;
				break;
		/* Form Feed https://en.wikipedia.org/wiki/Formfeed */
			case 'f':
			case 'F':
				(void)s_advance();
				c = 0x0C;
				break;
		/* Line Feed https://en.wikipedia.org/wiki/Newline */
			case 'n':
			case 'N':
				(void)s_advance();
				c = 0x0A;
				break;
		/* Carriage Return https://en.wikipedia.org/wiki/Carriage_Return */
			case 'r':
			case 'R':
				(void)s_advance();
				c = 0x0D;
				break;
		/* Horizontal Tab https://en.wikipedia.org/wiki/Horizontal_Tab */
			case 't':
			case 'T':
				(void)s_advance();
				c = 0x09;
				break;
		/* Vertical Tab https://en.wikipedia.org/wiki/Vertical_Tab */
			case 'v':
			case 'V':
				(void)s_advance();
				c = 0x0B;
				break;
		/* Backslash Character */
			case '\\':
				(void)s_advance();
				c = 0x5C;
				break;
		/* Apostrophe */
			case '\'':
				(void)s_advance();
				c = 0x27;
				break;
		/* Quotation Mark */
			case '"':
				(void)s_advance();
				c = 0x22;
				break;
			default: {
				c = s_getc();
				/* \000 - \777:  */
				if (c >= '0' && c <= '7') {
					uint64_t resultC = 0;
					register int counter = 0;
					c = s_advance();
					while (c >= '0' && c <= '7') {
						if (counter > 3) {
							break;
						}
						resultC = resultC * 8 + c - '0';
						c = s_advance();
						counter++;
					}
					ungetc(c, s_fstream);
					c = resultC > UINT8_MAX ? UINT8_MAX : resultC;
					break;
				} else if (c == 'x' || c == 'X') {
					uint64_t resultC = 0;
					register int counter = 0;
					c = s_advance(); /* skipping x or X */
					c = s_advance(); /* fetching first character */
					while (isdigit(c) || (c >= 'a' && c <= 'f') || (c >= 'A' && c <= 'F')) {
						if (counter > 2) {
							break;
						}
						resultC = resultC * 16 + s_hex_to_int(c);
						c = s_advance();
						counter++;
					}
					ungetc(c, s_fstream);
					c = resultC > UINT8_MAX ? UINT8_MAX : resultC;
					break;
				} else {
					fprintf(stderr, "lexer, escape sequence parser: unknown escape sequence \\%c\n", c);
					abort();
				}
			}
		}
	}
	return c;
}

/* Parses a character literal. */
static void s_parse_character_literal(lex_value *yield)
{
	uint64_t result = 0;
	char c = '\0';
	while (s_getc() != '\'') {
		c = s_advance();
		/* escape sequence */
		c = s_parse_escape_sequence(c);
		result = result << 8 | c;
	}
	if (c == EOF) {
		fprintf(stderr, "lexer, character literal parser: character literal does not end\n");
		abort();
	}
	(void)s_advance();
	yield->u64 = result;
	return;
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

bool_t lex_fetch(lex_token *tokenBuffer)
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
		return FALSE;
	} else if (isdigit(c)) {
		bool_t isFloat;

		tokenInstance.pos = s_fstreamPos;
		isFloat = s_parse_number(&tokenInstance.value);
		tokenInstance.kind = isFloat ? '0' << 8 | '.' : '0';
		*tokenBuffer = tokenInstance;
		return TRUE;
	} else if (c == '\'') {
		c = s_advance();
		tokenInstance.pos = s_fstreamPos;
		s_parse_character_literal(&tokenInstance.value);
		tokenInstance.kind = '0';
		*tokenBuffer = tokenInstance;
		return TRUE;
	}
	return FALSE;
}
