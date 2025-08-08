// SPDX-License-Identifier: BSD-3-Clause

typedef struct ls_pstate
{
	ls_lex_t const *lex;
	ls_ast_t *ast;
	uint32_t cur;
} ls_pstate_t;

typedef struct ls_pratt
{
	uint8_t nudleft, nudright;
	uint8_t ledleft, ledright;
	uint8_t nudtype, ledtype;
} ls_pratt_t;

char const *ls_nodenames[LS_NODETYPE_END] =
{
	"null",
	
	// structure nodes.
	"root",
	"import",
	"func",
	"declaration",
	"arglist",
	"arg",
	"return",
	"ctree",
	"while",
	"for",
	"break",
	"continue",
	"block",
	
	// type nodes.
	"type",
	
	// expression nodes.
	"eatom",
	"ecall",
	"eaccess",
	"eneg",
	"enot",
	"eenv",
	"emul",
	"ediv",
	"emod",
	"eadd",
	"esub",
	"eless",
	"elequal",
	"egreater",
	"egrequal",
	"eequal",
	"enequal",
	"eand",
	"eor",
	"exor",
	"eternary",
	"eassign",
	"eaddassign",
	"esubassign",
	"emulassign",
	"edivassign",
	"emodassign"
};

static ls_pratt_t ls_pratt[LS_TOKTYPE_END] =
{
	// left-to-right.
	[LS_LPAREN] = {0, 0, 19, 20, LS_NULL, LS_ECALL},
	[LS_PERIOD] = {0, 0, 19, 20, LS_NULL, LS_EACCESS},
	
	// right-to-left.
	[LS_BANG] = {18, 17, 0, 0, LS_ENOT, LS_NULL},
	[LS_DOLLAR] = {18, 17, 0, 0, LS_ENOT, LS_NULL},
	
	// left-to-right.
	[LS_STAR] = {0, 0, 15, 16, LS_NULL, LS_EMUL},
	[LS_SLASH] = {0, 0, 15, 16, LS_NULL, LS_EDIV},
	[LS_PERCENT] = {0, 0, 15, 16, LS_NULL, LS_EMOD},
	
	// left-to-right.
	[LS_PLUS] = {0, 0, 13, 14, LS_NULL, LS_EADD},
	[LS_MINUS] = {18, 17, 13, 14, LS_ENEG, LS_ESUB},
	
	// left-to-right.
	[LS_LESS] = {0, 0, 11, 12, LS_NULL, LS_ELESS},
	[LS_LESSEQUAL] = {0, 0, 11, 12, LS_NULL, LS_ELEQUAL},
	[LS_GREATER] = {0, 0, 11, 12, LS_NULL, LS_EGREATER},
	[LS_GREATEREQUAL] = {0, 0, 11, 12, LS_NULL, LS_EGREQUAL},
	[LS_2EQUAL] = {0, 0, 11, 12, LS_NULL, LS_EEQUAL},
	[LS_BANGEQUAL] = {0, 0, 11, 12, LS_NULL, LS_ENEQUAL},
	
	// left-to-right.
	[LS_2CARET] = {0, 0, 9, 10, LS_NULL, LS_EXOR},
	
	// left-to-right.
	[LS_2CARET] = {0, 0, 7, 8, LS_NULL, LS_EXOR},
	
	// left-to-right.
	[LS_2PIPE] = {0, 0, 5, 6, LS_NULL, LS_EOR},
	
	// right-to-left.
	[LS_QUESTION] = {0, 0, 4, 3, LS_NULL, LS_ETERNARY},
	
	// right-to-left.
	[LS_EQUAL] = {0, 0, 2, 1, LS_NULL, LS_EASSIGN},
	[LS_PLUSEQUAL] = {0, 0, 2, 1, LS_NULL, LS_EADDASSIGN},
	[LS_MINUSEQUAL] = {0, 0, 2, 1, LS_NULL, LS_ESUBASSIGN},
	[LS_STAREQUAL] = {0, 0, 2, 1, LS_NULL, LS_EMODASSIGN},
	[LS_SLASHEQUAL] = {0, 0, 2, 1, LS_NULL, LS_EDIVASSIGN},
	[LS_PERCENTEQUAL] = {0, 0, 2, 1, LS_NULL, LS_EMODASSIGN}
};

ls_err_t
ls_parse(ls_ast_t *out, ls_lex_t const *l)
{
}

uint32_t
ls_addnode(ls_ast_t *a, ls_nodetype_t type)
{
}

void
ls_parentnode(ls_ast_t *a, uint32_t parent, uint32_t child)
{
}

void
ls_printast(FILE *fp, ls_ast_t const *ast)
{
}

void
ls_printnode(FILE *fp, ls_ast_t const *ast, ls_node_t const *n, uint32_t depth)
{
}
