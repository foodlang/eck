#include "def.h"
#include <stdlib.h>
#include <string.h>

static void s_realloc(string_builder *builder)
{
	const size_t previous = builder->max;
	builder->max += builder->blklen + 1;
	builder->storage = realloc(builder->storage, builder->max);
	memset((builder->storage + previous), 0, builder->blklen + 1);
}

void strbuilder_alloc(string_builder *builder, size_t block_size)
{
	builder->length = 0;          /* None of the string builder is used */
	builder->blklen = block_size; /* Block size */
	builder->max = block_size;    /* By default, it can contain only 16 characters (can scale up) */
	builder->storage = malloc(block_size + 1);   /* Allocates the storage */
	memset(builder->storage, 0, block_size + 1); /* Sets the storage to zero */
}

void strbuilder_append_char(string_builder *builder, char append)
{
	if (builder->length + 1 > builder->max) /* Check for room */
		s_realloc(builder);
	
	builder->storage[builder->length++] = append;
}

void strbuilder_append_string(string_builder *builder, char *string)
{
	const size_t stringLength = strlen(string);

	if (builder->length + stringLength > builder->max)
		s_realloc(builder);
	
	memcpy((builder->storage + builder->length), string, stringLength);
	builder->length += stringLength;
}

void strbuilder_free(string_builder *builder)
{
	builder->blklen = 0;
	builder->length = 0;
	builder->max = 0;
	free(builder->storage);
	builder->storage = 0;
}

void *memorize_raw(void *item, size_t size)
{
	void *yield = malloc(size);
	memcpy(yield, item, size);
	return yield;
}

void delete_tree(expression *expr)
{
	if (expr->left) delete_tree(expr->left);
	if (expr->right) delete_tree(expr->right);
	if (expr->extra) delete_tree(expr->extra);
	free(expr);
}

uint8_t min_u8(uint8_t a, uint8_t b)
{
	return b + ((a - b) & (a - b) >> 7);
}

uint8_t max_u8(uint8_t a, uint8_t b)
{
	return a - ((a - b) & (a - b) >> 7);
}