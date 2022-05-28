#ifndef ECK_COMMON_DEF_H
#define ECH_COMMON_DEF_H

/*
	Common type definitions and such for ECK
*/

#include <stdint.h>

typedef union lex_value
{
	uint8_t u8;
	uint16_t u16;
	uint32_t u32;
	uint64_t u64;
	float single_float;
	double double_float;
	long double long_double;
	char *str;
} lex_value;

typedef struct lex_token
{
	uint64_t kind;   /* The kind of the token */
	uint64_t pos;    /* The position of the token */
	lex_value value; /* The value passed with the token */
} lex_token;

typedef signed char bool_t;
#ifndef NULL
	#define NULL (void *)0
#endif
#ifndef TRUE
	#define TRUE 1
#endif
#ifndef FALSE
	#define FALSE 0
#endif

#endif