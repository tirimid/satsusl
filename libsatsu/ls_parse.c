// SPDX-License-Identifier: BSD-3-Clause

typedef struct ls_pstate
{
	ls_lex_t const *lex;
	ls_ast_t *ast;
	char const *name;
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

static ls_toktype_t ls_nexttok(ls_pstate_t *p);
static ls_err_t ls_requiretok(ls_pstate_t *p, ls_toktype_t *out);
static ls_err_t ls_expecttok(ls_pstate_t *p, ls_toktype_t type);
static ls_err_t ls_parseroot(ls_pstate_t *p, uint32_t *out);
static ls_err_t ls_parseimport(ls_pstate_t *p, uint32_t *out);
static ls_err_t ls_parsefunc(ls_pstate_t *p, uint32_t *out);
static ls_err_t ls_parsedeclaration(ls_pstate_t *p, uint32_t *out);
static ls_err_t ls_parsearglist(ls_pstate_t *p, uint32_t *out);
static ls_err_t ls_parsearg(ls_pstate_t *p, uint32_t *out);
static ls_err_t ls_parsereturn(ls_pstate_t *p, uint32_t *out);
static ls_err_t ls_parsectree(ls_pstate_t *p, uint32_t *out);
static ls_err_t ls_parsewhile(ls_pstate_t *p, uint32_t *out);
static ls_err_t ls_parsefor(ls_pstate_t *p, uint32_t *out);
static ls_err_t ls_parsebreak(ls_pstate_t *p, uint32_t *out);
static ls_err_t ls_parsecontinue(ls_pstate_t *p, uint32_t *out);
static ls_err_t ls_parseblock(ls_pstate_t *p, uint32_t *out);
static ls_err_t ls_parsestmt(ls_pstate_t *p, uint32_t *out);
static ls_err_t ls_parseexpr(ls_pstate_t *p, uint32_t *out, uint8_t const term[], size_t nterm, uint8_t minbp);
static ls_err_t ls_parseexprnud(ls_pstate_t *p, uint32_t *out, uint8_t const term[], size_t nterm);
static ls_err_t ls_parseexprled(ls_pstate_t *p, uint32_t *out, uint32_t lhs, uint8_t const term[], size_t nterm);
static ls_err_t ls_parsetype(ls_pstate_t *p, uint32_t *out);

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
	[LS_2AMPERSAND] = {0, 0, 9, 10, LS_NULL, LS_EXOR},
	
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
ls_parse(ls_ast_t *out, ls_lex_t const *l, char const *name)
{
	ls_ast_t a =
	{
		.nodes = ls_calloc(1, sizeof(ls_node_t)),
		.types = ls_calloc(1, 1),
		.nodecap = 1
	};
	
	ls_pstate_t p =
	{
		.lex = l,
		.ast = &a,
		.name = name,
		.cur = 0
	};
	
	uint32_t root;
	ls_err_t e = ls_parseroot(&p, &root);
	if (e.code)
	{
		ls_destroyast(&a);
		return e;
	}
	
	return (ls_err_t){0};
}

uint32_t
ls_addnode(ls_ast_t *a, ls_nodetype_t type)
{
	if (a->nnodes >= a->nodecap)
	{
		a->nodecap *= 2;
		a->nodes = ls_reallocarray(a->nodes, a->nodecap, sizeof(ls_node_t));
		a->types = ls_reallocarray(a->types, a->nodecap, 1);
	}
	
	a->nodes[a->nnodes] = (ls_node_t)
	{
		.children = ls_calloc(1, sizeof(uint32_t)),
		.childcap = 1
	};
	a->types[a->nnodes] = type;
	
	return a->nnodes++;
}

void
ls_parentnode(ls_ast_t *a, uint32_t parent, uint32_t child)
{
	ls_node_t *node = &a->nodes[parent];
	
	if (node->childcap >= node->nchildren)
	{
		node->childcap *= 2;
		node->children = ls_reallocarray(node->children, node->childcap, sizeof(uint32_t));
	}
	
	node->children[node->nchildren++] = child;
}

void
ls_printast(FILE *fp, ls_ast_t const *ast, ls_lex_t const *lex)
{
	ls_printnode(fp, ast, lex, 0, 0);
}

void
ls_printnode(
	FILE *fp,
	ls_ast_t const *ast,
	ls_lex_t const *lex,
	uint32_t n,
	uint32_t depth
)
{
	for (uint32_t i = 0; i < depth; ++i)
	{
		fprintf(fp, "  ");
	}
	
	ls_node_t node = ast->nodes[n];
	ls_tok_t tok = lex->toks[node.tok];
	fprintf(
		fp,
		"%s | %s %u+%u\n",
		ls_nodenames[ast->types[n]],
		ls_toknames[lex->types[node.tok]],
		tok.pos,
		tok.len
	);
	
	for (uint32_t i = 0; i < node.nchildren; ++i)
	{
		ls_printnode(fp, ast, lex, node.children[i], depth + 1);
	}
}

void
ls_destroyast(ls_ast_t *a)
{
	for (size_t i = 0; i < a->nnodes; ++i)
	{
		ls_free(a->nodes[i].children);
	}
	
	ls_free(a->nodes);
	ls_free(a->types);
}

static ls_toktype_t
ls_nexttok(ls_pstate_t *p)
{
	++p->cur;
	if (p->cur >= p->lex->ntoks)
	{
		return LS_NULL;
	}
	return p->lex->types[p->cur];
}

static ls_err_t
ls_requiretok(ls_pstate_t *p, ls_toktype_t *out)
{
	++p->cur;
	if (p->cur >= p->lex->ntoks)
	{
		ls_tok_t lasttok = p->lex->toks[p->lex->ntoks - 1];
		return (ls_err_t)
		{
			.code = 1,
			.pos = lasttok.pos + lasttok.len,
			.len = 1,
			.src = ls_strdup(p->name),
			.msg = ls_strdup("required a token, found EOF")
		};
	}
	*out = p->lex->types[p->cur];
	return (ls_err_t){0};
}

static ls_err_t
ls_expecttok(ls_pstate_t *p, ls_toktype_t type)
{
	++p->cur;
	if (p->cur >= p->lex->ntoks)
	{
		ls_tok_t lasttok = p->lex->toks[p->lex->ntoks - 1];
		return (ls_err_t)
		{
			.code = 1,
			.pos = lasttok.pos + lasttok.len,
			.len = 1,
			.src = ls_strdup(p->name),
			.msg = ls_strdup("expected a token, found EOF")
		};
	}
	
	ls_toktype_t actual = p->lex->types[p->cur];
	ls_tok_t tok = p->lex->toks[p->cur];
	if (actual != type)
	{
		char msg[128];
		sprintf(msg, "expected %s, found %s", ls_toknames[type], ls_toknames[actual]);
		return (ls_err_t)
		{
			.code = 1,
			.pos = tok.pos,
			.len = tok.len,
			.src = ls_strdup(p->name),
			.msg = ls_strdup(msg)
		};
	}
	
	return (ls_err_t){0};
}

static ls_err_t
ls_parseroot(ls_pstate_t *p, uint32_t *out)
{
	uint32_t root = ls_addnode(p->ast, LS_ROOT);
	
	for (ls_toktype_t type = ls_nexttok(p); type; type = ls_nexttok(p))
	{
		if (type == LS_KWIMPORT)
		{
			uint32_t imp;
			ls_err_t e = ls_parseimport(p, &imp);
			if (e.code)
			{
				return e;
			}
			ls_parentnode(p->ast, root, imp);
		}
		else if (type == LS_KWFUNC)
		{
			uint32_t fn;
			ls_err_t e = ls_parsefunc(p, &fn);
			if (e.code)
			{
				return e;
			}
			ls_parentnode(p->ast, root, fn);
		}
		else
		{
			ls_tok_t tok = p->lex->toks[p->cur];
			return (ls_err_t)
			{
				.code = 1,
				.pos = tok.pos,
				.len = tok.len,
				.src = ls_strdup(p->name),
				.msg = ls_strdup("expected a root element")
			};
		}
	}
	
	*out = root;
	return (ls_err_t){0};
}

static ls_err_t
ls_parseimport(ls_pstate_t *p, uint32_t *out)
{
	uint32_t imp = ls_addnode(p->ast, LS_IMPORT);
	
	ls_err_t e = ls_expecttok(p, LS_IDENT);
	if (e.code)
	{
		return e;
	}
	
	p->ast->nodes[imp].tok = p->cur;
	
	e = ls_expecttok(p, LS_SEMICOLON);
	if (e.code)
	{
		return e;
	}
	
	*out = imp;
	return (ls_err_t){0};
}

static ls_err_t
ls_parsefunc(ls_pstate_t *p, uint32_t *out)
{
	uint32_t fn = ls_addnode(p->ast, LS_FUNC);
	
	uint32_t type;
	ls_err_t e = ls_parsetype(p, &type);
	if (e.code)
	{
		return e;
	}
	ls_parentnode(p->ast, fn, type);
	
	e = ls_expecttok(p, LS_IDENT);
	if (e.code)
	{
		return e;
	}
	
	p->ast->nodes[fn].tok = p->cur;
	
	uint32_t arglist;
	e = ls_parsearglist(p, &arglist);
	if (e.code)
	{
		return e;
	}
	ls_parentnode(p->ast, fn, arglist);
	
	uint32_t body;
	e = ls_parsestmt(p, &body);
	if (e.code)
	{
		return e;
	}
	ls_parentnode(p->ast, fn, body);
	
	*out = fn;
	return (ls_err_t){0};
}

static ls_err_t
ls_parsedeclaration(ls_pstate_t *p, uint32_t *out)
{
}

static ls_err_t
ls_parsearglist(ls_pstate_t *p, uint32_t *out)
{
	uint32_t arglist = ls_addnode(p->ast, LS_ARGLIST);
	
	ls_err_t e = ls_expecttok(p, LS_LPAREN);
	if (e.code)
	{
		return e;
	}
	
	p->ast->nodes[arglist].tok = p->cur;
	
	if (ls_nexttok(p) == LS_RPAREN)
	{
		*out = arglist;
		return (ls_err_t){0};
	}
	--p->cur;
	
	for (;;)
	{
		uint32_t arg;
		e = ls_parsearg(p, &arg);
		if (e.code)
		{
			return e;
		}
		
		ls_parentnode(p->ast, arglist, arg);
		
		ls_toktype_t type;
		e = ls_requiretok(p, &type);
		if (e.code)
		{
			return e;
		}
		
		if (type == LS_RPAREN)
		{
			break;
		}
		else if (type == LS_COMMA)
		{
			continue;
		}
		else
		{
			ls_tok_t tok = p->lex->toks[p->cur];
			return (ls_err_t)
			{
				.code = 1,
				.pos = tok.pos,
				.len = tok.len,
				.src = ls_strdup(p->name),
				.msg = ls_strdup("expected ) or ,")
			};
		}
	}
	
	*out = arglist;
	return (ls_err_t){0};
}

static ls_err_t
ls_parsearg(ls_pstate_t *p, uint32_t *out)
{
	uint32_t arg = ls_addnode(p->ast, LS_ARG);
	
	uint32_t type;
	ls_err_t e = ls_parsetype(p, &type);
	if (e.code)
	{
		return e;
	}
	ls_parentnode(p->ast, arg, type);
	
	e = ls_expecttok(p, LS_IDENT);
	if (e.code)
	{
		return e;
	}
	
	p->ast->nodes[arg].tok = p->cur;
	
	*out = arg;
	return (ls_err_t){0};
}

static ls_err_t
ls_parsereturn(ls_pstate_t *p, uint32_t *out)
{
}

static ls_err_t
ls_parsectree(ls_pstate_t *p, uint32_t *out)
{
}

static ls_err_t
ls_parsewhile(ls_pstate_t *p, uint32_t *out)
{
}

static ls_err_t
ls_parsefor(ls_pstate_t *p, uint32_t *out)
{
}

static ls_err_t
ls_parsebreak(ls_pstate_t *p, uint32_t *out)
{
}

static ls_err_t
ls_parsecontinue(ls_pstate_t *p, uint32_t *out)
{
}

static ls_err_t
ls_parseblock(ls_pstate_t *p, uint32_t *out)
{
}

static ls_err_t
ls_parsestmt(ls_pstate_t *p, uint32_t *out)
{
}

static ls_err_t
ls_parseexpr(
	ls_pstate_t *p,
	uint32_t *out,
	uint8_t const term[],
	size_t nterm,
	uint8_t minbp
)
{
}

static ls_err_t
ls_parseexprnud(
	ls_pstate_t *p,
	uint32_t *out,
	uint8_t const term[],
	size_t nterm
)
{
}

static ls_err_t
ls_parseexprled(
	ls_pstate_t *p,
	uint32_t *out,
	uint32_t lhs,
	uint8_t const term[],
	size_t nterm
)
{
}

static ls_err_t
ls_parsetype(ls_pstate_t *p, uint32_t *out)
{
	uint32_t type = ls_addnode(p->ast, LS_TYPE);
	
	ls_toktype_t toktype;
	ls_err_t e = ls_requiretok(p, &toktype);
	if (e.code)
	{
		return e;
	}
	
	if (toktype != LS_KWINT
		&& toktype != LS_KWREAL
		&& toktype != LS_KWSTRING
		&& toktype != LS_KWBOOL
		&& toktype != LS_KWVOID)
	{
		ls_tok_t tok = p->lex->toks[p->cur];
		return (ls_err_t)
		{
			.code = 1,
			.pos = tok.pos,
			.len = tok.pos,
			.src = ls_strdup(p->name),
			.msg = ls_strdup("expected type")
		};
	}
	
	p->ast->nodes[type].tok = p->cur;
	
	*out = type;
	return (ls_err_t){0};
}
