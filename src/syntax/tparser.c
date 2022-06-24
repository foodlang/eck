#include "../common/def.h"

bool_t tparse(foodtype *dest)
{
	lex_token tok;
	size_t base;
	uint8_t flags = 0;
	uint8_t type = 0;

	dest->extra = 0;
	dest->sub = 0;

	base = lex_pos();
	if (!lex_fetch(&tok)) {
		return FALSE;
	}
	
	/* 1. Parsing qualifiers */
	while (tok.kind == KEYWORD_CONST
	    || tok.kind == KEYWORD_VOLATILE
	    || tok.kind == KEYWORD_RESTRICT
	    || tok.kind == KEYWORD_ATOMIC
	) {
		if (tok.kind == KEYWORD_CONST && !(flags & TYPE_CONST_BIT)) {
			flags |= TYPE_CONST_BIT;
		} else if (tok.kind == KEYWORD_VOLATILE && !(flags & TYPE_VOLATILE_BIT)) {
			flags |= TYPE_VOLATILE_BIT;
		} else if (tok.kind == KEYWORD_RESTRICT && !(flags & TYPE_RESTRICT_BIT)) {
			flags |= TYPE_RESTRICT_BIT;
		} else if (tok.kind == KEYWORD_ATOMIC && !(flags & TYPE_ATOMIC_BIT)) {
			flags |= TYPE_ATOMIC_BIT;
		} else {
			derror(&tok, "duplicate type qualifier\n");
			return FALSE;
		}
		base = lex_pos();
		if (!lex_fetch(&tok)) {
			return FALSE;
		}
	}
	dest->qualifiers = flags;

	/* 2. Parsing the type. */
	switch (tok.kind) {
		case KEYWORD_BOOL:
			type = TYPE_BOOL;
			break;

		case KEYWORD_SBYTE:
			type = TYPE_SBYTE;
			break;
		
		case KEYWORD_BYTE:
			type = TYPE_BYTE;
			break;
		
		case KEYWORD_SHORT:
			type = TYPE_SHORT;
			break;

		case KEYWORD_USHORT:
			type = TYPE_USHORT;
			break;
		
		case KEYWORD_INT:
			type = TYPE_INT;
			break;
		
		case KEYWORD_UINT:
			type = TYPE_UINT;
			break;
		
		case KEYWORD_LONG:
			type = TYPE_LONG;
			break;
		
		case KEYWORD_ULONG:
			type = TYPE_ULONG;
			break;
		
		case KEYWORD_HALF:
			type = TYPE_HALF;
			break;
		
		case KEYWORD_FLOAT:
			type = TYPE_FLOAT;
			break;
		
		case KEYWORD_DOUBLE:
			type = TYPE_DOUBLE;
			break;
		
		case KEYWORD_STRING:
			type = TYPE_STRING;
			break;

		case KEYWORD_VOID:
			type = TYPE_VOID;
			break;
		
		case KEYWORD_FUNCTION:
			dfatal("function callbacks are not supported yet\n");
			break;
		
		default:
			return FALSE;
	}
	dest->kind = type;

	base = lex_pos();
	if (!lex_fetch(&tok)) {
		return TRUE;
	}

	/* 3. Taking care of pointers, references and arrays */
	while (tok.kind == '*' || tok.kind == '&'/* || '[' add support for arrays */) {
		dest->sub = MEMORIZE(foodtype, dest);
		dest->qualifiers = 0;
		switch (tok.kind)
		{
			/* Pointer */
			case '*':
				dest->kind = TYPE_POINTER;
				break;
			
			case '&':
				dest->kind = TYPE_REFERENCE;
				break;
		}

		base = lex_pos();
		if (!lex_fetch(&tok)) {
			lex_move(base);
			break;
		}

		/* 1. Parsing qualifiers */
		while (tok.kind == KEYWORD_CONST
		    || tok.kind == KEYWORD_VOLATILE
			|| tok.kind == KEYWORD_RESTRICT
			|| tok.kind == KEYWORD_ATOMIC) {
			
			if (tok.kind == KEYWORD_CONST && !(flags & TYPE_CONST_BIT)) {
				flags |= TYPE_CONST_BIT;
			} else if (tok.kind == KEYWORD_VOLATILE && !(flags & TYPE_VOLATILE_BIT)) {
				flags |= TYPE_VOLATILE_BIT;
			} else if (tok.kind == KEYWORD_RESTRICT && !(flags & TYPE_RESTRICT_BIT)) {
				flags |= TYPE_RESTRICT_BIT;
			} else if (tok.kind == KEYWORD_ATOMIC && !(flags & TYPE_ATOMIC_BIT)) {
				flags |= TYPE_ATOMIC_BIT;
			} else {
				derror(&tok, "duplicate type qualifier\n");
				return FALSE;
			}
			base = lex_pos();
			if (!lex_fetch(&tok)) {
				return TRUE;
			}
		}
		dest->qualifiers = flags;
	}
	lex_move(base);
	return TRUE;
}

#define TCASE(T) case T: printf("%s", #T); break;

void tprint(foodtype *t, int indent)
{
	int i;
	uint8_t flags = t->qualifiers;
	for (i = 0; i < indent; i++) {
		printf("  ");
	}

	for (i = 0; i < 8; i++) {
		if (flags & 128) {
			putc('1', stdout);
		} else {
			putc('0', stdout);
		}
		flags <<= 1;
	}
	printf(": ");

	switch (t->kind)
	{
		TCASE(TYPE_SBYTE)
		TCASE(TYPE_BYTE)
		TCASE(TYPE_SHORT)
		TCASE(TYPE_USHORT)
		TCASE(TYPE_INT)
		TCASE(TYPE_UINT)
		TCASE(TYPE_LONG)
		TCASE(TYPE_ULONG)
		TCASE(TYPE_HALF)
		TCASE(TYPE_FLOAT)
		TCASE(TYPE_DOUBLE)
		TCASE(TYPE_BOOL)
		TCASE(TYPE_VOID)
		
		case TYPE_POINTER:
			printf("Pointer ->");
			tprint(t->sub, indent + 1);
			break;
		
		case TYPE_REFERENCE:
			printf("Reference ->");
			tprint(t->sub, indent + 1);
			break;
		
		default:
			printf("(Unknown Type %d)", t->kind);
			break;
	}
}
