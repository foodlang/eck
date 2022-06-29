#include "common/def.h"

#include <stdlib.h>
#include <string.h>
#include <assert.h>

static scope base;
static scope *head = &base;

void scope_enter(void)
{
	scope *new;

	/*
		1. Create a new scope
	*/
	new = calloc(1, sizeof(scope));
	new->parent = head; /* The new scope is under the current scope. */

	/* 2. Adding a new child to the list */
	if (head->childcount == 0) {
		/* If the current node has no children */
		head->children = malloc(sizeof(scope *));
		head->childcount++;
		head->children[0] = new;
	} else {
		/* If the current node has children */
		head->children = realloc(
			head->children,
			sizeof(scope *) * (head->childcount + 1)
		);
		head->children[head->childcount++] = new;
	}

	head = new;
}

void scope_leave(void)
{
	if (head->parent) {
		head = head->parent;
	}
	/*
		There's always a possiblity this function gets called by error.
		Because I don't know if it will have any drawbacks, for now, it
		will stay like this.
	*/
}

void destroy_scopes(scope *s)
{
	size_t i;
	if (s == NULL) {
		/* Destroy base. Annihilate. */
		for (i = 0; i < base.childcount; i++) {
			destroy_scopes(base.children[i]);
		}
		if (base.symbols) free(base.symbols);
		if (base.children) free(base.children);
	} else {
		for (i = 0; i < s->childcount; i++) {
			destroy_scopes(s->children[i]);
		}
		if (s->symbols) free(s->symbols);
		if (s->children) free(s->children);
	}
}

static bool_t internal_declared(scope *s, const char *name)
{
	size_t i;

	assert(name);

	/* 1. First, search in current scope */
	for (i = 0; i < s->symbolcount; i++) {
		assert(s->symbols[i].name);
		if (!strcmp(s->symbols[i].name, name))
			return TRUE;
	}

	if (s->parent)
		return internal_declared(s->parent, name);
	return FALSE;
}

bool_t declared(const char *name)
{
	return internal_declared(head, name);
}

bool_t decl(const char *name, foodtype *t)
{
	if (declared(name))
		return FALSE; /* Shadowing is not allowed */
	
	/* 1. Allocating space */
	if (head->symbolcount == 0) {
		/* No symbols yet? No problem */
		head->symbolcount++;
		head->symbols = malloc(sizeof(symbol));
	} else {
		/* Resizing array to allow for more symbols */
		head->symbolcount++;
		head->symbols = realloc(head->symbols, sizeof(symbol) * head->symbolcount);
	}
	head->symbols[head->symbolcount - 1].name = malloc(strlen(name));
	strcpy((char *)head->symbols[head->symbolcount - 1].name, name);
	memcpy(&(head->symbols[head->symbolcount - 1].t), t, sizeof(foodtype));
	return TRUE;
}

bool_t decltype(foodtype *dest, const char *name)
{
	size_t i;
	
	for (i = 0; i < head->symbolcount; i++) {
		if (!strcmp(head->symbols[i].name, name)) {
			memcpy(dest, &(head->symbols[i].t), sizeof(foodtype));
			return TRUE;
		}
	}
	return FALSE;
}

static void dump_node(scope *s, int indent)
{
	size_t i;
	int j;
	for (i = 0; i < s->symbolcount; i++) {
		for (j = 0; j < indent; j++) {
			printf("  ");
		}
		tprint(&(s->symbols[i].t), 0);
		printf(" %s;\n", s->symbols[i].name);
	}
	for (i = 0; i < s->childcount; i++) {
		dump_node(s->children[i], indent + 1);
	}
}

void dump_all(void)
{
	dump_node(&base, 0);
}

size_t required_size_for_scope(void)
{
	size_t required = 0;
	size_t i;
	for (i = 0; i < head->symbolcount; i++) {
		size_t objsize = rsizeof(&head->symbols[i].t);
		required += objsize;
	}
	
	if (required % 16 != 0)
		required += 16 - required % 16;
	return required;
}
