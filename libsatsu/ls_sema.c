// SPDX-License-Identifier: BSD-3-Clause

typedef struct ls_sema
{
	ls_module_t const *m;
	ls_symtab_t *st;
	uint32_t mod;
	ls_primtype_t rettype;
	uint16_t loopdepth;
	uint16_t scope;
} ls_sema_t;

typedef struct ls_typeof
{
	ls_module_t const *m;
	ls_symtab_t const *st;
	uint32_t mod;
} ls_typeof_t;

char const *ls_primtypenames[LS_PRIMTYPE_END] =
{
	"null",
	
	"int",
	"real",
	"string",
	"bool",
	"void",
	"func"
};

ls_primtype_t ls_toktoprim[LS_TOKTYPE_END] =
{
	[LS_KWINT] = LS_INT,
	[LS_KWREAL] = LS_REAL,
	[LS_KWSTRING] = LS_STRING,
	[LS_KWBOOL] = LS_BOOL,
	[LS_KWVOID] = LS_VOID
};

static ls_err_t ls_redefinition(ls_module_t const *m, uint32_t mod, ls_symtab_t const *st, ls_tok_t cur, int64_t prev);
static ls_err_t ls_semafuncdecl(ls_sema_t *s, uint32_t node);
static ls_err_t ls_semadecl(ls_sema_t *s, uint32_t node);
static ls_err_t ls_semareturn(ls_sema_t *s, uint32_t node);
static ls_err_t ls_semactree(ls_sema_t *s, uint32_t node);
static ls_err_t ls_semawhile(ls_sema_t *s, uint32_t node);
static ls_err_t ls_semafor(ls_sema_t *s, uint32_t node);
static ls_err_t ls_semabreak(ls_sema_t *s, uint32_t node);
static ls_err_t ls_semacontinue(ls_sema_t *s, uint32_t node);
static ls_err_t ls_semablock(ls_sema_t *s, uint32_t node);
static ls_err_t ls_semaeatom(ls_sema_t *s, uint32_t node);
static ls_err_t ls_semaecall(ls_sema_t *s, uint32_t node);
static ls_err_t ls_semaeneg(ls_sema_t *s, uint32_t node);
static ls_err_t ls_semaenot(ls_sema_t *s, uint32_t node);
static ls_err_t ls_semaecast(ls_sema_t *s, uint32_t node);
static ls_err_t ls_semaemul(ls_sema_t *s, uint32_t node);
static ls_err_t ls_semaediv(ls_sema_t *s, uint32_t node);
static ls_err_t ls_semaemod(ls_sema_t *s, uint32_t node);
static ls_err_t ls_semaeadd(ls_sema_t *s, uint32_t node);
static ls_err_t ls_semaesub(ls_sema_t *s, uint32_t node);
static ls_err_t ls_semaeless(ls_sema_t *s, uint32_t node);
static ls_err_t ls_semaelequal(ls_sema_t *s, uint32_t node);
static ls_err_t ls_semaegreater(ls_sema_t *s, uint32_t node);
static ls_err_t ls_semaegrequal(ls_sema_t *s, uint32_t node);
static ls_err_t ls_semaeequal(ls_sema_t *s, uint32_t node);
static ls_err_t ls_semaenequal(ls_sema_t *s, uint32_t node);
static ls_err_t ls_semaeand(ls_sema_t *s, uint32_t node);
static ls_err_t ls_semaeor(ls_sema_t *s, uint32_t node);
static ls_err_t ls_semaexor(ls_sema_t *s, uint32_t node);
static ls_err_t ls_semaeternary(ls_sema_t *s, uint32_t node);
static ls_err_t ls_semaeassign(ls_sema_t *s, uint32_t node);
static ls_err_t ls_semaeaddassign(ls_sema_t *s, uint32_t node);
static ls_err_t ls_semaesubassign(ls_sema_t *s, uint32_t node);
static ls_err_t ls_semaemulassign(ls_sema_t *s, uint32_t node);
static ls_err_t ls_semaedivassign(ls_sema_t *s, uint32_t node);
static ls_err_t ls_semaemodassign(ls_sema_t *s, uint32_t node);
static ls_primtype_t ls_typeofeatom(ls_typeof_t const *t, uint32_t node);
static ls_primtype_t ls_typeofecall(ls_typeof_t const *t, uint32_t node);
static ls_primtype_t ls_typeofeneg(ls_typeof_t const *t, uint32_t node);
static ls_primtype_t ls_typeofenot(ls_typeof_t const *t, uint32_t node);
static ls_primtype_t ls_typeofecast(ls_typeof_t const *t, uint32_t node);
static ls_primtype_t ls_typeofemul(ls_typeof_t const *t, uint32_t node);
static ls_primtype_t ls_typeofediv(ls_typeof_t const *t, uint32_t node);
static ls_primtype_t ls_typeofemod(ls_typeof_t const *t, uint32_t node);
static ls_primtype_t ls_typeofeadd(ls_typeof_t const *t, uint32_t node);
static ls_primtype_t ls_typeofesub(ls_typeof_t const *t, uint32_t node);
static ls_primtype_t ls_typeofeless(ls_typeof_t const *t, uint32_t node);
static ls_primtype_t ls_typeofelequal(ls_typeof_t const *t, uint32_t node);
static ls_primtype_t ls_typeofegreater(ls_typeof_t const *t, uint32_t node);
static ls_primtype_t ls_typeofegrequal(ls_typeof_t const *t, uint32_t node);
static ls_primtype_t ls_typeofeequal(ls_typeof_t const *t, uint32_t node);
static ls_primtype_t ls_typeofenequal(ls_typeof_t const *t, uint32_t node);
static ls_primtype_t ls_typeofeand(ls_typeof_t const *t, uint32_t node);
static ls_primtype_t ls_typeofeor(ls_typeof_t const *t, uint32_t node);
static ls_primtype_t ls_typeofexor(ls_typeof_t const *t, uint32_t node);
static ls_primtype_t ls_typeofeternary(ls_typeof_t const *t, uint32_t node);
static ls_primtype_t ls_typeofeassign(ls_typeof_t const *t, uint32_t node);
static ls_primtype_t ls_typeofeaddassign(ls_typeof_t const *t, uint32_t node);
static ls_primtype_t ls_typeofesubassign(ls_typeof_t const *t, uint32_t node);
static ls_primtype_t ls_typeofemulassign(ls_typeof_t const *t, uint32_t node);
static ls_primtype_t ls_typeofedivassign(ls_typeof_t const *t, uint32_t node);
static ls_primtype_t ls_typeofemodassign(ls_typeof_t const *t, uint32_t node);

static ls_err_t (*ls_semafns[LS_NODETYPE_END])(ls_sema_t *, uint32_t) =
{
	// structure nodes.
	[LS_NULL] = NULL,
	[LS_ROOT] = NULL,
	[LS_IMPORT] = NULL,
	[LS_FUNCDECL] = ls_semafuncdecl,
	[LS_DECL] = ls_semadecl,
	[LS_ARGLIST] = NULL,
	[LS_ARG] = NULL,
	[LS_RETURN] = ls_semareturn,
	[LS_CTREE] = ls_semactree,
	[LS_WHILE] = ls_semawhile,
	[LS_FOR] = ls_semafor,
	[LS_BREAK] = ls_semabreak,
	[LS_CONTINUE] = ls_semacontinue,
	[LS_BLOCK] = ls_semablock,
	
	// type nodes.
	[LS_TYPE] = NULL,
	
	// expression nodes.
	[LS_EATOM] = ls_semaeatom,
	[LS_ECALL] = ls_semaecall,
	[LS_ENEG] = ls_semaeneg,
	[LS_ENOT] = ls_semaenot,
	[LS_ECAST] = ls_semaecast,
	[LS_EMUL] = ls_semaemul,
	[LS_EDIV] = ls_semaediv,
	[LS_EMOD] = ls_semaemod,
	[LS_EADD] = ls_semaeadd,
	[LS_ESUB] = ls_semaesub,
	[LS_ELESS] = ls_semaeless,
	[LS_ELEQUAL] = ls_semaelequal,
	[LS_EGREATER] = ls_semaegreater,
	[LS_EGREQUAL] = ls_semaegrequal,
	[LS_EEQUAL] = ls_semaeequal,
	[LS_ENEQUAL] = ls_semaenequal,
	[LS_EAND] = ls_semaeand,
	[LS_EOR] = ls_semaeor,
	[LS_EXOR] = ls_semaexor,
	[LS_ETERNARY] = ls_semaeternary,
	[LS_EASSIGN] = ls_semaeassign,
	[LS_EADDASSIGN] = ls_semaeaddassign,
	[LS_ESUBASSIGN] = ls_semaesubassign,
	[LS_EMULASSIGN] = ls_semaemulassign,
	[LS_EDIVASSIGN] = ls_semaedivassign,
	[LS_EMODASSIGN] = ls_semaemodassign
};

static ls_primtype_t (*ls_typeoffns[LS_NODETYPE_END])(ls_typeof_t const *, uint32_t) =
{
	// expression nodes.
	[LS_EATOM] = ls_typeofeatom,
	[LS_ECALL] = ls_typeofecall,
	[LS_ENEG] = ls_typeofeneg,
	[LS_ENOT] = ls_typeofenot,
	[LS_ECAST] = ls_typeofecast,
	[LS_EMUL] = ls_typeofemul,
	[LS_EDIV] = ls_typeofediv,
	[LS_EMOD] = ls_typeofemod,
	[LS_EADD] = ls_typeofeadd,
	[LS_ESUB] = ls_typeofesub,
	[LS_ELESS] = ls_typeofeless,
	[LS_ELEQUAL] = ls_typeofelequal,
	[LS_EGREATER] = ls_typeofegreater,
	[LS_EGREQUAL] = ls_typeofegrequal,
	[LS_EEQUAL] = ls_typeofeequal,
	[LS_ENEQUAL] = ls_typeofenequal,
	[LS_EAND] = ls_typeofeand,
	[LS_EOR] = ls_typeofeor,
	[LS_EXOR] = ls_typeofexor,
	[LS_ETERNARY] = ls_typeofeternary,
	[LS_EASSIGN] = ls_typeofeassign,
	[LS_EADDASSIGN] = ls_typeofeaddassign,
	[LS_ESUBASSIGN] = ls_typeofesubassign,
	[LS_EMULASSIGN] = ls_typeofemulassign,
	[LS_EDIVASSIGN] = ls_typeofedivassign,
	[LS_EMODASSIGN] = ls_typeofemodassign
};

// *out takes ownership of *a, *l, name[0:strlen(name)], and data[0:len].
ls_module_t
ls_createmodule(
	ls_ast_t *a,
	ls_lex_t *l,
	char *name,
	uint64_t id,
	char *data,
	uint32_t len
)
{
	ls_module_t m =
	{
		.modcap = 1
	};
	
	ls_allocbatch_t allocs[] =
	{
		{(void **)&m.names, 1, sizeof(char *)},
		{(void **)&m.ids, 1, sizeof(uint64_t)},
		{(void **)&m.data, 1, sizeof(char *)},
		{(void **)&m.lens, 1, sizeof(uint32_t)},
		{(void **)&m.lexes, 1, sizeof(ls_lex_t)},
		{(void **)&m.asts, 1, sizeof(ls_ast_t)}
	};
	m.buf = ls_allocbatch(allocs, ARRSIZE(allocs));
	
	ls_pushmodule(&m, a, l, name, id, data, len);
	
	return m;
}

ls_err_t
ls_resolveimports(ls_module_t *m, char const *paths[], size_t npaths)
{
	for (size_t i = 0; i < m->asts[0].nnodes; ++i)
	{
		if (m->asts[0].types[i] != LS_IMPORT)
		{
			continue;
		}
		
		ls_tok_t tok = m->lexes[0].toks[m->asts[0].nodes[i].tok];
		
		char importname[LS_MAXIDENT] = {0};
		ls_readtokraw(importname, m->data[0], tok);
		
		char importpath[PATH_MAX] = {0};
		uint64_t importid = 0;
		for (size_t j = 0; j < npaths; ++j)
		{
			char const *path = paths[j];
			
			if (path[strlen(path) - 1] == '/')
			{
				snprintf(importpath, PATH_MAX, "%s%s.ssu", path, importname);
			}
			else
			{
				snprintf(importpath, PATH_MAX, "%s/%s.ssu", path, importname);
			}
			
			uint64_t id = ls_fileid(importpath, true);
			if (id)
			{
				importid = id;
				break;
			}
		}
		
		if (!importid)
		{
			return (ls_err_t)
			{
				.code = 1,
				.pos = tok.pos,
				.len = tok.len,
				.msg = ls_strdup("could not resolve import")
			};
		}
		
		bool found = false;
		for (size_t j = 0; j < m->nmods; ++j)
		{
			if (m->ids[j] == importid)
			{
				found = true;
				break;
			}
		}
		
		if (found)
		{
			continue;
		}
		
		FILE *fp = fopen(importpath, "rb");
		if (!fp)
		{
			return (ls_err_t)
			{
				.code = 1,
				.pos = tok.pos,
				.len = tok.len,
				.msg = ls_strdup("failed to open imported file")
			};
		}
		
		char *data;
		uint32_t len;
		ls_err_t e = ls_readfile(fp, &data, &len);
		fclose(fp);
		if (e.code)
		{
			e.pos = tok.pos;
			e.len = tok.len;
			return e;
		}
		
		ls_lex_t lex;
		e = ls_lex(&lex, data, len);
		if (e.code)
		{
			char msg[GENMSGLEN];
			sprintf(msg, "failed to lex file at %u+%u - %s", e.pos, e.len, e.msg);
			
			ls_destroyerr(&e);
			ls_free(data);
			
			return (ls_err_t)
			{
				.code = 1,
				.pos = tok.pos,
				.len = tok.len,
				.msg = ls_strdup(msg)
			};
		}
		
		ls_ast_t ast;
		e = ls_parse(&ast, &lex);
		if (e.code)
		{
			char msg[GENMSGLEN];
			sprintf(msg, "failed to parse file at %u+%u - %s", e.pos, e.len, e.msg);
			
			ls_destroyerr(&e);
			ls_destroylex(&lex);
			ls_free(data);
			
			return (ls_err_t)
			{
				.code = 1,
				.pos = tok.pos,
				.len = tok.len,
				.msg = ls_strdup(msg)
			};
		}
		
		ls_pushmodule(m, &ast, &lex, ls_strdup(importpath), importid, data, len);
	}
	
	return (ls_err_t){0};
}

// *m takes ownership of *a, *l, name[0:strlen(name)], and data[0:len].
void
ls_pushmodule(
	ls_module_t *m,
	ls_ast_t *a,
	ls_lex_t *l,
	char *name,
	uint64_t id,
	char *data,
	uint32_t len
)
{
	if (m->nmods >= m->modcap)
	{
		ls_reallocbatch_t reallocs[] =
		{
			{(void **)&m->names, m->modcap, 2 * m->modcap, sizeof(char *)},
			{(void **)&m->ids, m->modcap, 2 * m->modcap, sizeof(uint64_t)},
			{(void **)&m->data, m->modcap, 2 * m->modcap, sizeof(char *)},
			{(void **)&m->lens, m->modcap, 2 * m->modcap, sizeof(uint32_t)},
			{(void **)&m->lexes, m->modcap, 2 * m->modcap, sizeof(ls_lex_t)},
			{(void **)&m->asts, m->modcap, 2 * m->modcap, sizeof(ls_ast_t)}
		};
		
		m->modcap *= 2;
		m->buf = ls_reallocbatch(m->buf, reallocs, ARRSIZE(reallocs));
	}
	
	m->names[m->nmods] = name;
	m->ids[m->nmods] = id;
	m->data[m->nmods] = data;
	m->lens[m->nmods] = len;
	m->lexes[m->nmods] = *l;
	m->asts[m->nmods] = *a;
	++m->nmods;
}

void
ls_printmodule(FILE *fp, ls_module_t const *m)
{
	for (size_t i = 0; i < m->nmods; ++i)
	{
		fprintf(fp, "[%s]\n", m->names[i]);
		ls_printast(fp, &m->asts[i], &m->lexes[i]);
	}
}

void
ls_destroymodule(ls_module_t *m)
{
	for (size_t i = 0; i < m->nmods; ++i)
	{
		free(m->names[i]);
		free(m->data[i]);
		ls_destroylex(&m->lexes[i]);
		ls_destroyast(&m->asts[i]);
	}
	free(m->buf);
}

ls_err_t
ls_globalsymtab(ls_symtab_t *out, ls_module_t const *m)
{
	ls_symtab_t st = ls_createsymtab();
	
	for (size_t i = 0; i < m->nmods; ++i)
	{
		for (size_t j = 0; j < m->asts[i].nnodes; ++j)
		{
			if (m->asts[i].types[j] != LS_FUNCDECL)
			{
				continue;
			}
			
			ls_tok_t tok = m->lexes[i].toks[m->asts[i].nodes[j].tok];
			
			char sym[LS_MAXIDENT + 1] = {0};
			ls_readtokraw(sym, m->data[i], tok);
			
			int64_t prev = ls_findsym(&st, sym);
			if (prev != -1)
			{
				ls_err_t e = ls_redefinition(m, i, &st, tok, prev);
				ls_destroysymtab(&st);
				return e;
			}
			
			ls_pushsym(&st, ls_strdup(sym), LS_FUNC, i, j, 0);
		}
	}
	
	*out = st;
	return (ls_err_t){0};
}

ls_err_t
ls_sema(ls_module_t const *m)
{
	ls_symtab_t st;
	ls_err_t e = ls_globalsymtab(&st, m);
	if (e.code)
	{
		return e;
	}
	
	for (size_t i = 0; i < m->nmods; ++i)
	{
		for (size_t j = 0; j < m->asts[i].nnodes; ++j)
		{
			if (m->asts[i].types[j] != LS_FUNCDECL)
			{
				continue;
			}
			
			ls_sema_t s =
			{
				.m = m,
				.st = &st,
				.mod = i
			};
			
			e = ls_semafuncdecl(&s, j);
			if (e.code)
			{
				ls_destroysymtab(&st);
				return e;
			}
		}
	}
	
	return (ls_err_t){0};
}

ls_symtab_t
ls_createsymtab(void)
{
	ls_symtab_t st =
	{
		.symcap = 1
	};
	
	ls_allocbatch_t allocs[] =
	{
		{(void **)&st.syms, 1, sizeof(char *)},
		{(void **)&st.types, 1, sizeof(uint8_t)},
		{(void **)&st.mods, 1, sizeof(uint32_t)},
		{(void **)&st.nodes, 1, sizeof(uint32_t)},
		{(void **)&st.scopes, 1, sizeof(uint16_t)}
	};
	st.buf = ls_allocbatch(allocs, ARRSIZE(allocs));
	
	return st;
}

int64_t
ls_findsym(ls_symtab_t const *st, char const *sym)
{
	for (size_t i = 0; i < st->nsyms; ++i)
	{
		if (!strcmp(st->syms[i], sym))
		{
			return i;
		}
	}
	return -1;
}

void
ls_pushsym(
	ls_symtab_t *st,
	char *sym,
	ls_primtype_t type,
	uint32_t mod,
	uint32_t node,
	uint16_t scope
)
{
	if (st->nsyms >= st->symcap)
	{
		ls_reallocbatch_t reallocs[] =
		{
			{(void **)&st->syms, st->symcap, 2 * st->symcap, sizeof(char *)},
			{(void **)&st->types, st->symcap, 2 * st->symcap, sizeof(uint8_t)},
			{(void **)&st->mods, st->symcap, 2 * st->symcap, sizeof(uint32_t)},
			{(void **)&st->nodes, st->symcap, 2 * st->symcap, sizeof(uint32_t)},
			{(void **)&st->scopes, st->symcap, 2 * st->symcap, sizeof(uint16_t)}
		};
		
		st->buf = ls_reallocbatch(st->buf, reallocs, ARRSIZE(reallocs));
		st->symcap *= 2;
	}
	
	st->syms[st->nsyms] = sym;
	st->types[st->nsyms] = type;
	st->mods[st->nsyms] = mod;
	st->nodes[st->nsyms] = node;
	st->scopes[st->nsyms] = scope;
	++st->nsyms;
}

void
ls_destroysymtab(ls_symtab_t *st)
{
	for (size_t i = 0; i < st->nsyms; ++i)
	{
		ls_free(st->syms[i]);
	}
	ls_free(st->buf);
}

void
ls_popsymscope(ls_symtab_t *st, uint16_t scope)
{
	while (st->nsyms && st->scopes[st->nsyms - 1] >= scope)
	{
		--st->nsyms;
		ls_free(st->syms[st->nsyms]);
	}
}

// ls_typeof() assumes that you are getting the type of a node which has already
// been semantically analyzed; the function only handles the happy path.
ls_primtype_t
ls_typeof(
	ls_module_t const *m,
	uint32_t mod,
	ls_symtab_t const *st,
	uint32_t node
)
{
	ls_typeof_t t =
	{
		.m = m,
		.st = st,
		.mod = mod
	};
	
	ls_nodetype_t type = m->asts[mod].types[node];
	return ls_typeoffns[type](&t, node);
}

static ls_err_t
ls_redefinition(
	ls_module_t const *m,
	uint32_t mod,
	ls_symtab_t const *st,
	ls_tok_t cur,
	int64_t prev
)
{
	size_t prevmod = st->mods[prev], prevnode = st->nodes[prev];
	ls_tok_t prevtok = m->lexes[prevmod].toks[m->asts[prevmod].nodes[prevnode].tok];
	
	char msg[GENMSGLEN];
	sprintf(msg, "previously defined at %u+%u in %s", prevtok.pos, prevtok.len, m->names[prevmod]);
	
	return (ls_err_t)
	{
		.code = 1,
		.src = mod,
		.pos = cur.pos,
		.len = cur.len,
		.msg = ls_strdup(msg)
	};
}

static ls_err_t
ls_semafuncdecl(ls_sema_t *s, uint32_t node)
{
	++s->scope;
	
	ls_lex_t const *l = &s->m->lexes[s->mod];
	ls_ast_t const *a = &s->m->asts[s->mod];
	
	uint32_t ntype = a->nodes[node].children[0];
	uint32_t narglist = a->nodes[node].children[1];
	uint32_t nbody = a->nodes[node].children[2];
	
	for (size_t i = 0; i < a->nodes[narglist].nchildren; ++i)
	{
		uint32_t narg = a->nodes[narglist].children[i];
		ls_tok_t argtok = l->toks[a->nodes[narg].tok];
		
		uint32_t nargtype = a->nodes[narg].children[0];
		ls_toktype_t argtypetok = l->types[a->nodes[nargtype].tok];
		
		char sym[LS_MAXIDENT + 1] = {0};
		ls_readtokraw(sym, s->m->data[s->mod], argtok);
		
		int64_t prev = ls_findsym(s->st, sym);
		if (prev != -1)
		{
			return ls_redefinition(s->m, s->mod, s->st, argtok, prev);
		}
		
		ls_primtype_t primtype = ls_toktoprim[argtypetok];
		if (primtype == LS_VOID)
		{
			return (ls_err_t)
			{
				.code = 1,
				.src = s->mod,
				.pos = argtok.pos,
				.len = argtok.len,
				.msg = ls_strdup("function arguments cannot be void")
			};
		}
		
		ls_pushsym(s->st, ls_strdup(sym), primtype, s->mod, narg, s->scope);
	}
	
	ls_toktype_t typetok = l->types[a->nodes[ntype].tok];
	s->rettype = ls_toktoprim[typetok];
	
	ls_err_t e = ls_semafns[a->types[nbody]](s, nbody);
	if (e.code)
	{
		return e;
	}
	
	s->rettype = LS_NULL;
	ls_popsymscope(s->st, s->scope--);
	return (ls_err_t){0};
}

static ls_err_t
ls_semadecl(ls_sema_t *s, uint32_t node)
{
	ls_lex_t const *l = &s->m->lexes[s->mod];
	ls_ast_t const *a = &s->m->asts[s->mod];
	
	uint32_t ntype = a->nodes[node].children[0];
	uint32_t nval = a->nodes[node].children[1];
	
	ls_tok_t tok = l->toks[a->nodes[node].tok];
	ls_toktype_t toktype = l->types[a->nodes[ntype].tok];
	
	char sym[LS_MAXIDENT + 1] = {0};
	ls_readtokraw(sym, s->m->data[s->mod], tok);
	
	int64_t prev = ls_findsym(s->st, sym);
	if (prev != -1)
	{
		return ls_redefinition(s->m, s->mod, s->st, tok, prev);
	}
	
	ls_primtype_t primtype = ls_toktoprim[toktype];
	if (primtype == LS_VOID)
	{
		return (ls_err_t)
		{
			.code = 1,
			.src = s->mod,
			.pos = tok.pos,
			.len = tok.len,
			.msg = ls_strdup("declaration type cannot be void")
		};
	}
	
	ls_err_t e = ls_semafns[a->types[nval]](s, nval);
	if (e.code)
	{
		return e;
	}
	
	ls_primtype_t valprimtype = ls_typeof(s->m, s->mod, s->st, nval);
	if (valprimtype != primtype)
	{
		char msg[GENMSGLEN];
		sprintf(msg, "declaration is type %s but value is type %s", ls_primtypenames[primtype], ls_primtypenames[valprimtype]);
		
		return (ls_err_t)
		{
			.code = 1,
			.src = s->mod,
			.pos = tok.pos,
			.len = tok.len,
			.msg = ls_strdup(msg)
		};
	}
	
	ls_pushsym(s->st, ls_strdup(sym), primtype, s->mod, node, s->scope);
	
	return (ls_err_t){0};
}

static ls_err_t
ls_semareturn(ls_sema_t *s, uint32_t node)
{
	ls_lex_t const *l = &s->m->lexes[s->mod];
	ls_ast_t const *a = &s->m->asts[s->mod];
	
	ls_tok_t rettok = l->toks[a->nodes[node].tok];
	
	if (s->rettype != LS_VOID && !a->nodes[node].nchildren)
	{
		return (ls_err_t)
		{
			.code = 1,
			.src = s->mod,
			.pos = rettok.pos,
			.len = rettok.len,
			.msg = ls_strdup("function returning non-void cannot have empty return")
		};
	}
	
	if (s->rettype == LS_VOID && a->nodes[node].nchildren)
	{
		return (ls_err_t)
		{
			.code = 1,
			.src = s->mod,
			.pos = rettok.pos,
			.len = rettok.len,
			.msg = ls_strdup("function returning void cannot have value return")
		};
	}
	
	if (s->rettype == LS_VOID)
	{
		return (ls_err_t){0};
	}
	
	uint32_t nval = a->nodes[node].children[0];
	
	ls_err_t e = ls_semafns[a->types[nval]](s, nval);
	if (e.code)
	{
		return e;
	}
	
	ls_primtype_t primtype = ls_typeof(s->m, s->mod, s->st, nval);
	if (primtype != s->rettype)
	{
		char msg[GENMSGLEN];
		sprintf(msg, "return with value of type %s in function returning %s", ls_primtypenames[primtype], ls_primtypenames[s->rettype]);
		
		return (ls_err_t)
		{
			.code = 1,
			.src = s->mod,
			.pos = rettok.pos,
			.len = rettok.len,
			.msg = ls_strdup(msg)
		};
	}
	
	return (ls_err_t){0};
}

static ls_err_t
ls_semactree(ls_sema_t *s, uint32_t node)
{
	// TODO: implement.
	
	(void)s; (void)node;
	return (ls_err_t){0};
}

static ls_err_t
ls_semawhile(ls_sema_t *s, uint32_t node)
{
	++s->scope;
	++s->loopdepth;
	
	ls_lex_t const *l = &s->m->lexes[s->mod];
	ls_ast_t const *a = &s->m->asts[s->mod];
	
	uint32_t ncond = a->nodes[node].children[0];
	uint32_t nbody = a->nodes[node].children[1];
	
	ls_err_t e = ls_semafns[a->types[ncond]](s, ncond);
	if (e.code)
	{
		return e;
	}
	
	ls_tok_t tok = l->toks[a->nodes[node].tok];
	
	if (ls_typeof(s->m, s->mod, s->st, ncond) != LS_BOOL)
	{
		return (ls_err_t)
		{
			.code = 1,
			.src = s->mod,
			.pos = tok.pos,
			.len = tok.len,
			.msg = ls_strdup("condition must be of type bool")
		};
	}
	
	e = ls_semafns[a->types[nbody]](s, nbody);
	if (e.code)
	{
		return e;
	}
	
	--s->loopdepth;
	ls_popsymscope(s->st, s->scope--);
	return (ls_err_t){0};
}

static ls_err_t
ls_semafor(ls_sema_t *s, uint32_t node)
{
	++s->scope;
	++s->loopdepth;
	
	ls_lex_t const *l = &s->m->lexes[s->mod];
	ls_ast_t const *a = &s->m->asts[s->mod];
	
	uint32_t ninit = a->nodes[node].children[0];
	uint32_t ncond = a->nodes[node].children[1];
	uint32_t ninc = a->nodes[node].children[2];
	uint32_t nbody = a->nodes[node].children[3];
	
	ls_err_t e = ls_semafns[a->types[ninit]](s, ninit);
	if (e.code)
	{
		return e;
	}
	
	e = ls_semafns[a->types[ncond]](s, ncond);
	if (e.code)
	{
		return e;
	}
	
	ls_tok_t tok = l->toks[a->nodes[node].tok];
	
	if (ls_typeof(s->m, s->mod, s->st, ncond) != LS_BOOL)
	{
		return (ls_err_t)
		{
			.code = 1,
			.src = s->mod,
			.pos = tok.pos,
			.len = tok.len,
			.msg = ls_strdup("condition must be of type bool")
		};
	}
	
	e = ls_semafns[a->types[ninc]](s, ninc);
	if (e.code)
	{
		return e;
	}
	
	e = ls_semafns[a->types[nbody]](s, nbody);
	if (e.code)
	{
		return e;
	}
	
	--s->loopdepth;
	ls_popsymscope(s->st, s->scope--);
	return (ls_err_t){0};
}

static ls_err_t
ls_semabreak(ls_sema_t *s, uint32_t node)
{
	if (!s->loopdepth)
	{
		ls_lex_t const *l = &s->m->lexes[s->mod];
		ls_ast_t const *a = &s->m->asts[s->mod];
		
		ls_tok_t tok = l->toks[a->nodes[node].tok];
		
		return (ls_err_t)
		{
			.code = 1,
			.src = s->mod,
			.pos = tok.pos,
			.len = tok.len,
			.msg = ls_strdup("break statements cannot appear outside of loops")
		};
	}
	
	return (ls_err_t){0};
}

static ls_err_t
ls_semacontinue(ls_sema_t *s, uint32_t node)
{
	if (!s->loopdepth)
	{
		ls_lex_t const *l = &s->m->lexes[s->mod];
		ls_ast_t const *a = &s->m->asts[s->mod];
		
		ls_tok_t tok = l->toks[a->nodes[node].tok];
		
		return (ls_err_t)
		{
			.code = 1,
			.src = s->mod,
			.pos = tok.pos,
			.len = tok.len,
			.msg = ls_strdup("continue statements cannot appear outside of loops")
		};
	}
	
	return (ls_err_t){0};
}

static ls_err_t
ls_semablock(ls_sema_t *s, uint32_t node)
{
	++s->scope;
	
	ls_ast_t const *a = &s->m->asts[s->mod];
	
	for (size_t i = 0; i < a->nodes[node].nchildren; ++i)
	{
		uint32_t nstmt = a->nodes[node].children[i];
		
		ls_err_t e = ls_semafns[a->types[nstmt]](s, nstmt);
		if (e.code)
		{
			return e;
		}
	}
	
	ls_popsymscope(s->st, s->scope--);
	return (ls_err_t){0};
}

static ls_err_t
ls_semaeatom(ls_sema_t *s, uint32_t node)
{
	// TODO: implement.
	
	(void)s; (void)node;
	return (ls_err_t){0};
}

static ls_err_t
ls_semaecall(ls_sema_t *s, uint32_t node)
{
	// TODO: implement.
	
	(void)s; (void)node;
	return (ls_err_t){0};
}

static ls_err_t
ls_semaeneg(ls_sema_t *s, uint32_t node)
{
	// TODO: implement.
	
	(void)s; (void)node;
	return (ls_err_t){0};
}

static ls_err_t
ls_semaenot(ls_sema_t *s, uint32_t node)
{
	// TODO: implement.
	
	(void)s; (void)node;
	return (ls_err_t){0};
}

static ls_err_t
ls_semaecast(ls_sema_t *s, uint32_t node)
{
	// TODO: implement.
	
	(void)s; (void)node;
	return (ls_err_t){0};
}

static ls_err_t
ls_semaemul(ls_sema_t *s, uint32_t node)
{
	// TODO: implement.
	
	(void)s; (void)node;
	return (ls_err_t){0};
}

static ls_err_t
ls_semaediv(ls_sema_t *s, uint32_t node)
{
	// TODO: implement.
	
	(void)s; (void)node;
	return (ls_err_t){0};
}

static ls_err_t
ls_semaemod(ls_sema_t *s, uint32_t node)
{
	// TODO: implement.
	
	(void)s; (void)node;
	return (ls_err_t){0};
}

static ls_err_t
ls_semaeadd(ls_sema_t *s, uint32_t node)
{
	// TODO: implement.
	
	(void)s; (void)node;
	return (ls_err_t){0};
}

static ls_err_t
ls_semaesub(ls_sema_t *s, uint32_t node)
{
	// TODO: implement.
	
	(void)s; (void)node;
	return (ls_err_t){0};
}

static ls_err_t
ls_semaeless(ls_sema_t *s, uint32_t node)
{
	// TODO: implement.
	
	(void)s; (void)node;
	return (ls_err_t){0};
}

static ls_err_t
ls_semaelequal(ls_sema_t *s, uint32_t node)
{
	// TODO: implement.
	
	(void)s; (void)node;
	return (ls_err_t){0};
}

static ls_err_t
ls_semaegreater(ls_sema_t *s, uint32_t node)
{
	// TODO: implement.
	
	(void)s; (void)node;
	return (ls_err_t){0};
}

static ls_err_t
ls_semaegrequal(ls_sema_t *s, uint32_t node)
{
	// TODO: implement.
	
	(void)s; (void)node;
	return (ls_err_t){0};
}

static ls_err_t
ls_semaeequal(ls_sema_t *s, uint32_t node)
{
	// TODO: implement.
	
	(void)s; (void)node;
	return (ls_err_t){0};
}

static ls_err_t
ls_semaenequal(ls_sema_t *s, uint32_t node)
{
	// TODO: implement.
	
	(void)s; (void)node;
	return (ls_err_t){0};
}

static ls_err_t
ls_semaeand(ls_sema_t *s, uint32_t node)
{
	// TODO: implement.
	
	(void)s; (void)node;
	return (ls_err_t){0};
}

static ls_err_t
ls_semaeor(ls_sema_t *s, uint32_t node)
{
	// TODO: implement.
	
	(void)s; (void)node;
	return (ls_err_t){0};
}

static ls_err_t
ls_semaexor(ls_sema_t *s, uint32_t node)
{
	// TODO: implement.
	
	(void)s; (void)node;
	return (ls_err_t){0};
}

static ls_err_t
ls_semaeternary(ls_sema_t *s, uint32_t node)
{
	// TODO: implement.
	
	(void)s; (void)node;
	return (ls_err_t){0};
}

static ls_err_t
ls_semaeassign(ls_sema_t *s, uint32_t node)
{
	// TODO: implement.
	
	(void)s; (void)node;
	return (ls_err_t){0};
}

static ls_err_t
ls_semaeaddassign(ls_sema_t *s, uint32_t node)
{
	// TODO: implement.
	
	(void)s; (void)node;
	return (ls_err_t){0};
}

static ls_err_t
ls_semaesubassign(ls_sema_t *s, uint32_t node)
{
	// TODO: implement.
	
	(void)s; (void)node;
	return (ls_err_t){0};
}

static ls_err_t
ls_semaemulassign(ls_sema_t *s, uint32_t node)
{
	// TODO: implement.
	
	(void)s; (void)node;
	return (ls_err_t){0};
}

static ls_err_t
ls_semaedivassign(ls_sema_t *s, uint32_t node)
{
	// TODO: implement.
	
	(void)s; (void)node;
	return (ls_err_t){0};
}

static ls_err_t
ls_semaemodassign(ls_sema_t *s, uint32_t node)
{
	// TODO: implement.
	
	(void)s; (void)node;
	return (ls_err_t){0};
}

static ls_primtype_t
ls_typeofeatom(ls_typeof_t const *t, uint32_t node)
{
	ls_lex_t const *l = &t->m->lexes[t->mod];
	ls_ast_t const *a = &t->m->asts[t->mod];
	
	ls_primtype_t toktoprim[] =
	{
		[LS_LITINT] = LS_INT,
		[LS_LITREAL] = LS_REAL,
		[LS_LITSTR] = LS_STRING,
		[LS_KWTRUE] = LS_BOOL,
		[LS_KWFALSE] = LS_BOOL,
		[LS_IDENT] = LS_NULL
	};
	
	ls_toktype_t toktype = l->types[a->nodes[node].tok];
	if (toktoprim[toktype])
	{
		return toktoprim[toktype];
	}
	
	ls_tok_t tok = l->toks[a->nodes[node].tok];
	
	char sym[LS_MAXIDENT + 1] = {0};
	ls_readtokraw(sym, t->m->data[t->mod], tok);
	
	return t->st->types[ls_findsym(t->st, sym)];
}

static ls_primtype_t
ls_typeofecall(ls_typeof_t const *t, uint32_t node)
{
	ls_lex_t const *l = &t->m->lexes[t->mod];
	ls_ast_t const *a = &t->m->asts[t->mod];
	
	uint32_t nident = a->nodes[node].children[0];
	ls_tok_t identtok = l->toks[a->nodes[nident].tok];
	
	char sym[LS_MAXIDENT + 1] = {0};
	ls_readtokraw(sym, t->m->data[t->mod], identtok);
	
	int64_t decl = ls_findsym(t->st, sym);
	
	uint32_t funcmod = t->st->mods[decl];
	
	uint32_t nfunc = t->st->nodes[decl];
	uint32_t nfunctype = t->m->asts[funcmod].nodes[nfunc].children[0];
	ls_node_t functypenode = t->m->asts[funcmod].nodes[nfunctype];
	
	ls_toktype_t functoktype = t->m->lexes[funcmod].types[functypenode.tok];
	return ls_toktoprim[functoktype];
}

static ls_primtype_t
ls_typeofeneg(ls_typeof_t const *t, uint32_t node)
{
	// TODO: implement.
	
	(void)t; (void)node;
	return LS_NULL;
}

static ls_primtype_t
ls_typeofecast(ls_typeof_t const *t, uint32_t node)
{
	ls_lex_t const *l = &t->m->lexes[t->mod];
	ls_ast_t const *a = &t->m->asts[t->mod];
	
	uint32_t ntype = a->nodes[node].children[1];
	
	ls_toktype_t typetok = l->types[a->nodes[ntype].tok];
	
	return ls_toktoprim[typetok];
}

static ls_primtype_t
ls_typeofenot(ls_typeof_t const *t, uint32_t node)
{
	return LS_BOOL;
}

static ls_primtype_t
ls_typeofemul(ls_typeof_t const *t, uint32_t node)
{
	// TODO: implement.
	
	(void)t; (void)node;
	return LS_NULL;
}

static ls_primtype_t
ls_typeofediv(ls_typeof_t const *t, uint32_t node)
{
	// TODO: implement.
	
	(void)t; (void)node;
	return LS_NULL;
}

static ls_primtype_t
ls_typeofemod(ls_typeof_t const *t, uint32_t node)
{
	// TODO: implement.
	
	(void)t; (void)node;
	return LS_NULL;
}

static ls_primtype_t
ls_typeofeadd(ls_typeof_t const *t, uint32_t node)
{
	// TODO: implement.
	
	(void)t; (void)node;
	return LS_NULL;
}

static ls_primtype_t
ls_typeofesub(ls_typeof_t const *t, uint32_t node)
{
	return LS_BOOL;
}

static ls_primtype_t
ls_typeofeless(ls_typeof_t const *t, uint32_t node)
{
	return LS_BOOL;
}

static ls_primtype_t
ls_typeofelequal(ls_typeof_t const *t, uint32_t node)
{
	return LS_BOOL;
}

static ls_primtype_t
ls_typeofegreater(ls_typeof_t const *t, uint32_t node)
{
	return LS_BOOL;
}

static ls_primtype_t
ls_typeofegrequal(ls_typeof_t const *t, uint32_t node)
{
	return LS_BOOL;
}

static ls_primtype_t
ls_typeofeequal(ls_typeof_t const *t, uint32_t node)
{
	return LS_BOOL;
}

static ls_primtype_t
ls_typeofenequal(ls_typeof_t const *t, uint32_t node)
{
	return LS_BOOL;
}

static ls_primtype_t
ls_typeofeand(ls_typeof_t const *t, uint32_t node)
{
	return LS_BOOL;
}

static ls_primtype_t
ls_typeofeor(ls_typeof_t const *t, uint32_t node)
{
	return LS_BOOL;
}

static ls_primtype_t
ls_typeofexor(ls_typeof_t const *t, uint32_t node)
{
	return LS_BOOL;
}

static ls_primtype_t
ls_typeofeternary(ls_typeof_t const *t, uint32_t node)
{
	// TODO: implement.
	
	(void)t; (void)node;
	return LS_NULL;
}

static ls_primtype_t
ls_typeofeassign(ls_typeof_t const *t, uint32_t node)
{
	// TODO: implement.
	
	(void)t; (void)node;
	return LS_NULL;
}

static ls_primtype_t
ls_typeofeaddassign(ls_typeof_t const *t, uint32_t node)
{
	// TODO: implement.
	
	(void)t; (void)node;
	return LS_NULL;
}

static ls_primtype_t
ls_typeofesubassign(ls_typeof_t const *t, uint32_t node)
{
	// TODO: implement.
	
	(void)t; (void)node;
	return LS_NULL;
}

static ls_primtype_t
ls_typeofemulassign(ls_typeof_t const *t, uint32_t node)
{
	// TODO: implement.
	
	(void)t; (void)node;
	return LS_NULL;
}

static ls_primtype_t
ls_typeofedivassign(ls_typeof_t const *t, uint32_t node)
{
	// TODO: implement.
	
	(void)t; (void)node;
	return LS_NULL;
}

static ls_primtype_t
ls_typeofemodassign(ls_typeof_t const *t, uint32_t node)
{
	// TODO: implement.
	
	(void)t; (void)node;
	return LS_NULL;
}
