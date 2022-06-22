#include "../common/def.h"
#include <assert.h>

gbranch_ref gen_expression(generator *g, expression *tree)
{
	gbranch_ref yield = NULL;
	assert(tree);

	switch (tree->kind)
	{
		case EXPRESSION_INTEGER_LITERAL:
			yield = g->primary(tree->token.value);
			break;
		
		case EXPRESSION_ADDITION:
			yield = g->add(
				gen_expression(g, tree->left),
				gen_expression(g, tree->right)
			);
			break;
		
		case EXPRESSION_SUBTRACTION:
			yield = g->sub(
				gen_expression(g, tree->left),
				gen_expression(g, tree->right)
			);
			break;
		
		case EXPRESSION_MULTIPLY:
			yield = g->mul(
				gen_expression(g, tree->left),
				gen_expression(g, tree->right)
			);
			break;
		
		case EXPRESSION_DIVISION:
			yield = g->div(
				gen_expression(g, tree->left),
				gen_expression(g, tree->right)
			);
			break;
		
		case EXPRESSION_MODULO:
			yield = g->mod(
				gen_expression(g, tree->left),
				gen_expression(g, tree->right)
			);
			break;
		default: dfatal("unsupported %d\n", tree->kind);
	}
	return yield;
}
