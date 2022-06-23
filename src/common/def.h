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

#define DISCARD(x) ((void)((x) + 1))

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

#define TYPE_CONST_BIT    1
#define TYPE_VOLATILE_BIT 2
#define TYPE_RESTRICT_BIT 4
#define TYPE_ATOMIC_BIT   8

typedef struct foodtype
{
	uint8_t qualifiers;   /* The qualifiers of the type. */
	uint8_t kind;         /* The kind of the type. */
	struct foodtype *sub; /* A pointer to a subtype. */
	void    *extra;       /* A pointer to extra data. */
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
	TYPE_STRING,    /* Pascal strings */
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

/* Figures out the kind of a unary operator. */
uint32_t kind_unary(uint64_t operator);

/* Figures out the kind of a binary operator. */
uint32_t kind_binary(uint64_t operator);

/* Gets the precedence of a binary operator. Returns null if this is not a binary operator. */
uint8_t prec_binary(uint64_t operator);

/*
	This function performs the complex task of giving a type to the expression.
	Casts are not handled here, however resizing is done. The expected type is the
	expected type of the expression, for example in the case of an assignment
	or function argument.
*/
void type_expression(foodtype *dest, foodtype *expected, foodtype *left, foodtype *right);

/* Parses a type. Returns false if failed. */
bool_t tparse(foodtype *dest);

/* Prints a type. */
void tprint(foodtype *t, int indent);

/* Checks whether two types are compatible. */
bool_t type_compatible(foodtype *left, foodtype *right);

uint8_t min_u8(uint8_t a, uint8_t b);
uint8_t max_u8(uint8_t a, uint8_t b);

/* Prints an expression. Useful for debugging. */
void expression_print(expression *expr, int indent);

typedef enum type_glbl_kind
{
	TYPE_GLBL_NONE_IMPLICIT_CAST,
	TYPE_GLBL_INTEGER,
	TYPE_GLBL_FLOATING,
	TYPE_GLBL_POINTERS
} type_glbl_kind;

/* Returns the size of a compatible type, in bytes. */
uint8_t type_compatible_size(uint8_t type);

/* Gets the type category for a certain type. */
type_glbl_kind type_globalize(foodtype *t);

/* Gets the weight of an expression (+1 for each node) */
size_t eweight(expression *tree);

/* Simplifies an expression. */
void esimple(expression **tree);

/* ===== DIAGNOSTICS ===== */

/* Triggers a diagnostic of type information. This diagnostic is usually benign. */
void dinfo(lex_token *site, const char *fmt, ...);

/* Warnings are diagnostics that indicate a possible bug. */
void dwarn(lex_token *site, const char *fmt, ...);

/* Errors are diagnostics that indicate a syntax error or something that prevents the compilation. */
void derror(lex_token *site, const char *fmt, ...);

/* Fatals are errors that come from the compiler itself. These crash the compiler. */
void dfatal(const char *fmt, ...);

/* Gets 2D coordinates for a token. */
void lex_site(lex_token *site, size_t *line, size_t *col);

typedef void *gbranch_ref;

typedef struct generator
{
	void (*clear)(gbranch_ref r); /* Deallocates a branch reference. */
	gbranch_ref (*primary)(lex_value v); /* Generates primary tree branches (literal values.) */
	gbranch_ref (*add)(gbranch_ref l, gbranch_ref r); /* Generates code for an addition */
	gbranch_ref (*sub)(gbranch_ref l, gbranch_ref r); /* Generates code for a subtraction */
	gbranch_ref (*mul)(gbranch_ref l, gbranch_ref r); /* Generates code for a multiplication */
	gbranch_ref (*div)(gbranch_ref l, gbranch_ref r); /* Generates code for a division. */
	gbranch_ref (*mod)(gbranch_ref l, gbranch_ref r); /* Generates code for a modulo. */

} generator;

/* Generates an expression. */
gbranch_ref gen_expression(generator *g, expression *tree);

/* x86_64 generator */
extern generator x86_64;

/* ===== SYMBOL RELATED ===== */

/* Represents a symbol. */
typedef struct symbol
{
	const char *name; /* The name of the symbol. */
	foodtype    t;    /* The type of the symbol. */

} symbol;

/* A scope of access. Contains a list of symbols. */
typedef struct scope
{
	struct scope *parent;    /* The parent scope. NULL if base scope. */ 
	struct scope **children; /* A pointer to an array of child scopes. */
	size_t childcount;       /* The number of children scopes in the scope. */
	symbol *symbols;         /* A pointer to an array of symbols. */
	size_t symbolcount;      /* The number of symbols in the scope. */

} scope;

/* Enters a new scope. */
void scope_enter(void);

/*
	Leaves the current scope and ascends to the parent one.
	Does nothing if performing on base.
*/
void scope_leave(void);

/* Destroys a specific scope. Leave the argument to null to destroy all. */
void destroy_scopes(scope *s);

/* Checks whether a symbol is declared. */
bool_t declared(const char *name);

/* Declares a symbol. Fails if already existing. */
bool_t decl(const char *name, foodtype *t);

/* Gets the type of a declaration. Fails if not found. */
bool_t decltype(foodtype *dest, const char *name);

#endif