#ifndef ECK_COMMON_DEF_H
#define ECH_COMMON_DEF_H

/*
	Common type definitions and such for ECK
*/

#include <stdint.h>
#include <stddef.h>

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

typedef struct string_builder
{
	char *storage;  /* Where to store the string (heap-allocated string) */
	size_t length;  /* The length of the string. */
	size_t max;     /* The current maximum length of the string. */
	size_t blklen;  /* Block length */
} string_builder;

/* Allocates & prepares a basic string builder. */
void strbuilder_alloc(string_builder *builder, size_t block_size);

/* Appends a char at the end of a string builder. */
void strbuilder_append_char(string_builder *builder, char append);

/* Appends a string at the end of a string builder. */
void strbuilder_append_string(string_builder *builder, char *string);

/* Frees up all of the resources used by a string builder. */
void strbuilder_free(string_builder *builder);

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

/* Key pair */
#define KEYPAIR_T(K, V) struct { K key; V value; }

/*
	Keyword Enum

	Here lies a classic moment of doing witchcraft.
	We don't want the keywords to overlap with keywords,
	so I put the keywords at a very high base value to
	prevent issues.
*/
typedef enum keyword
{
	KEYWORD_ATOMIC = 0x100000,
	KEYWORD_BREAK,
	KEYWORD_BOOL,
	KEYWORD_BYTE,
	KEYWORD_CASE,
	KEYWORD_CONST,
	KEYWORD_CONTINUE,
	KEYWORD_DEFAULT,
	KEYWORD_DO,
	KEYWORD_DOUBLE,
	KEYWORD_DYNAMIC,
	KEYWORD_ELSE,
	KEYWORD_ENUM,
	KEYWORD_EXTERN,
	KEYWORD_FLOAT,
	KEYWORD_FOR,
	KEYWORD_FUNCTION,
	KEYWORD_GOTO,
	KEYWORD_HALF,
	KEYWORD_IF,
	KEYWORD_INT,
	KEYWORD_LONG,
	KEYWORD_NAMESPACE,
	KEYWORD_PUBLIC,
	KEYWORD_RECORD,
	KEYWORD_RESTRICT,
	KEYWORD_RETURN,
	KEYWORD_SBYTE,
	KEYWORD_SHORT,
	KEYWORD_SIZEOF,
	KEYWORD_STATIC,
	KEYWORD_STRUCT,
	KEYWORD_SWITCH,
	KEYWORD_UNION,
	KEYWORD_UINT,
	KEYWORD_ULONG,
	KEYWORD_USHORT,
	KEYWORD_VOID,
	KEYWORD_VOLATILE,
	KEYWORD_WHILE,
	KEYWORD_TRUE,
	KEYWORD_FALSE,
	KEYWORD_USING,
	KEYWORD_NEW,
	KEYWORD_NULL,
	KEYWORD_START,
	KEYWORD_END,
	KEYWORD_CLASS,
	KEYWORD_STRING,
	KEYWORD_LENGTHOF
} keyword;

#endif