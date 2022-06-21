#ifndef ECK_COMMON_DEF_H
#define ECH_COMMON_DEF_H

/*
	Common type definitions and such for ECK
	Also includes templates a string builder.
*/

#include <stdint.h>
#include <stddef.h>
#include <stdio.h>

/*
	A string builder is used to build
	strings that need a lot of write
	access.
*/
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

/* Copies an object to the heap. */
void *memorize_raw(void *item, size_t size);

/* Templated memorize. */
#define MEMORIZE(T, P) (T *)memorize_raw((void *)P, sizeof(T))

#define OP2(x, y) (((x) << 8) | (y))
#define OP3(x, y, z) (((x) << 16) | ((y) << 8) | (z))

/* C89 does not provide a bool type, like C99's _Bool. */
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

/*
	This simple *template* macro is used to
	represent a key/value pair.
*/
#define KEYPAIR_T(K, V) \
struct                  \
{                       \
	K key;              \
	V value;            \
}

typedef struct foodtype
{
	uint8_t qualifiers; /* The qualifiers of the type. */
	uint8_t kind;       /* The kind of the type. */
	void    *extra;     /* A pointer to extra data. */
} foodtype;

typedef enum type_kind
{
	TYPE_NULL = 0,  /* used for unknown/error types */
	TYPE_VOID,      /* void, size = 0 */
	TYPE_BOOL,      /* bool, size = 1 */
	TYPE_SBYTE,     /* sbyte, char, size = 1 */
	TYPE_BYTE,      /* byte, uchar, size = 1 */
	TYPE_SHORT,     /* short, size = 2 */
	TYPE_USHORT,    /* ushort, size = 2 */
	TYPE_HALF,      /* half, size = 2 */
	TYPE_INT,       /* int, size = 4 */
	TYPE_UINT,      /* uint, size = 4 */
	TYPE_FLOAT,     /* float, size = 4 */
	TYPE_LONG,      /* long, size = 8 */
	TYPE_ULONG,     /* ulong, size = 8 */
	TYPE_DOUBLE,    /* double, size = 8 */
	TYPE_POINTER,   /* T*, size = 8 */
	TYPE_FUNCTION,  /* function T(params), size = 8 */
	TYPE_REFERENCE, /* T&, size = 8 */
	TYPE_ARRAY,     /* T[length], size = sizeof(T) * length */
	TYPE_TUPLE,     /* tuple (T, U, ...), size = sizeof(T) + alignof(U) + sizeof(U), etc. */
	TYPE_STRUCTURE_LIKE /* structures, records, unions, size = ??? */
} type_kind;

/*  ===== LEXER DECL ===== */


/*
	Lexical tokens might need to store
	values to allow the parser to decipher
	the meaning of the text. This union
	represents it all.
*/
typedef union lex_value
{
	int8_t i8;
	int16_t i16;
	int32_t i32;
	int64_t i64;

	uint8_t u8;
	uint16_t u16;
	uint32_t u32;
	uint64_t u64;
	float single_float;
	double double_float;
	long double long_double;
	char *str;
} lex_value;

/*
	A lexical token is the most basic syntax
	unit that compiler knows. It represents
	a bit of text that cannot be broken down
	further.
*/
typedef struct lex_token
{
	uint64_t kind;   /* The kind of the token */
	uint64_t pos;    /* The position of the token */
	lex_value value; /* The value passed with the token */
} lex_token;

/*
	Keyword Enum

	Here lies a classic moment of doing witchcraft.
	We don't want the keywords to overlap with keywords,
	so I put the keywords at a very high base value to
	prevent issues.
*/
typedef enum keyword
{
	KEYWORD_ALIGNOF = 0x100000,
	KEYWORD_ATOMIC,
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

void lex_setup(FILE *stream);
bool_t lex_fetch(lex_token *tokenBuffer);
size_t lex_pos(void);
void lex_move(size_t pos);

/*  ===== PARSER DECL ===== */

/*
	This simple enum represents the possible kinds
	of expressions.
*/
typedef enum expression_kind
{
	EXPRESSION_NULL = 0,
	EXPRESSION_INTEGER_LITERAL,
	EXPRESSION_BOOLEAN_LITERAL,
	EXPRESSION_FLOATING_LITERAL,

	EXPRESSION_POSTFIX_INCREMENT,
	EXPRESSION_POSTFIX_DECREMENT,
	EXPRESSION_FUNCTION_CALL,
	EXPRESSION_ARRAY_SUBSCRIPT,
	EXPRESSION_DIRECT_MEMBER_ACCESS,
	EXPRESSION_POINTER_MEMBER_ACCESS,

	EXPRESSION_PREFIX_INCREMENT,
	EXPRESSION_PREFIX_DECREMENT,
	EXPRESSION_POSTFIX_UNARY_PLUS,
	EXPRESSION_POSTFIX_UNARY_MINUS,
	EXPRESSION_POSTFIX_LOGICAL_NOT,
	EXPRESSION_POSTFIX_BITWISE_NOT,
	EXPRESSION_CAST,
	EXPRESSION_DEREFERENCE,
	EXPRESSION_ADDRESS_OF,
	EXPRESSION_SIZEOF,
	EXPRESSION_ALIGNOF,
	EXPRESSION_TYPEOF,

	EXPRESSION_MULTIPLY,
	EXPRESSION_DIVISION,
	EXPRESSION_MODULO,

	EXPRESSION_ADDITION,
	EXPRESSION_SUBTRACTION,

	EXPRESSION_LSHIFT,
	EXPRESSION_RSHIFT,

	EXPRESSION_LOWER,
	EXPRESSION_LOWER_OR_EQUAL,
	EXPRESSION_GREATER,
	EXPRESSION_GREATER_OR_EQUAL,

	EXPRESSION_EQUAL,
	EXPRESSION_NOT_EQUAL,

	EXPRESSION_BITWISE_AND,

	EXPRESSION_BITWISE_XOR,

	EXPRESSION_BITWISE_OR,

	EXPRESSION_LOGICAL_AND,

	EXPRESSION_LOGICAL_OR,

	EXPRESSION_TERNARY_CONDITIONAL,

	EXPRESSION_ASSIGN,
	EXPRESSION_SUM_ASSIGN,
	EXPRESSION_DIFFERENCE_ASSIGN,
	EXPRESSION_PRODUCT_ASSIGN,
	EXPRESSION_QUOTIENT_ASSIGN,
	EXPRESSION_REMAINDER_ASSIGN,
	EXPRESSION_LSHIFT_ASSIGN,
	EXPRESSION_RSHIFT_ASSIGN,
	EXPRESSION_AND_ASSIGN,
	EXPRESSION_XOR_ASSIGN,
	EXPRESSION_OR_ASSIGN,

	EXPRESSION_COMPOUND
	
} expression_kind;

/*
	An expression is well, a mathematical expression.
	It is represented as a tree with multiple children
	nodes.
*/
typedef struct expression
{
	expression_kind kind;     /* The kind of the expression. */
	lex_token token;          /* The main token of the expression. */
	struct expression *left;  /* The left node of the expression. */
	struct expression *right; /* The right node of the expression. */
	struct expression *extra; /* An extra tree used as the condition of the conditional operator and such. */
	foodtype type;            /* The type of the expression. */
	bool_t isLValue;          /* Whether this expression is a LValue. */
} expression;

/* Parses an expression. */
expression *parse_expression(void);

/* Deletes an expression tree. */
void delete_tree(expression *expr);

#endif