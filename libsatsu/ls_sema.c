// SPDX-License-Identifier: BSD-3-Clause

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
static ls_err_t ls_semafunc(ls_module_t *m, uint32_t mod, uint32_t node, ls_symtab_t *st);
static ls_err_t ls_semastmt(ls_module_t *m, uint32_t mod, uint32_t node, ls_symtab_t *st);

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
		.names = ls_calloc(1, sizeof(char *)),
		.ids = ls_calloc(1, sizeof(uint64_t)),
		.data = ls_calloc(1, sizeof(char *)),
		.lens = ls_calloc(1, sizeof(uint32_t)),
		.lexes = ls_calloc(1, sizeof(ls_lex_t)),
		.asts = ls_calloc(1, sizeof(ls_ast_t)),
		.modcap = 1
	};
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
			char msg[256];
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
			char msg[256];
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
		m->modcap *= 2;
		m->names = ls_reallocarray(m->names, m->modcap, sizeof(char *));
		m->ids = ls_reallocarray(m->ids, m->modcap, sizeof(uint64_t));
		m->data = ls_reallocarray(m->data, m->modcap, sizeof(char *));
		m->lens = ls_reallocarray(m->lens, m->modcap, sizeof(uint32_t));
		m->lexes = ls_reallocarray(m->lexes, m->modcap, sizeof(ls_lex_t));
		m->asts = ls_reallocarray(m->asts, m->modcap, sizeof(ls_ast_t));
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
	
	free(m->names);
	free(m->ids);
	free(m->data);
	free(m->lens);
	free(m->lexes);
	free(m->asts);
}

ls_err_t
ls_sema(ls_module_t *m)
{
	// set initial null typeinfo and create global symtab.
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
	
	// generate typeinfo for functions.
	for (size_t i = 0; i < m->nmods; ++i)
	{
		for (size_t j = 0; j < m->asts[i].nnodes; ++j)
		{
			if (m->asts[i].types[j] != LS_FUNCDECL)
			{
				continue;
			}
			
			ls_err_t e = ls_semafunc(m, i, j, &st);
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
	return (ls_symtab_t)
	{
		.syms = ls_calloc(1, sizeof(char *)),
		.types = ls_calloc(1, 1),
		.mods = ls_calloc(1, sizeof(uint32_t)),
		.nodes = ls_calloc(1, sizeof(uint32_t)),
		.scopes = ls_calloc(1, sizeof(uint16_t)),
		.symcap = 1
	};
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
		st->symcap *= 2;
		st->syms = ls_reallocarray(st->syms, st->symcap, sizeof(char *));
		st->types = ls_reallocarray(st->types, st->symcap, 1);
		st->mods = ls_reallocarray(st->mods, st->symcap, sizeof(uint32_t));
		st->nodes = ls_reallocarray(st->nodes, st->symcap, sizeof(uint32_t));
		st->scopes = ls_reallocarray(st->scopes, st->symcap, sizeof(uint16_t));
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
	
	ls_free(st->syms);
	ls_free(st->types);
	ls_free(st->mods);
	ls_free(st->nodes);
	ls_free(st->scopes);
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

uint16_t
ls_newscope(ls_symtab_t const *st)
{
	return st->nsyms ? st->scopes[st->nsyms - 1] + 1 : 0;
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
	
	char msg[256];
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
ls_semafunc(ls_module_t *m, uint32_t mod, uint32_t node, ls_symtab_t *st)
{
	uint16_t scope = ls_newscope(st);
	
	ls_lex_t *l = &m->lexes[mod];
	ls_ast_t *a = &m->asts[mod];
	
	uint32_t narglist = a->nodes[node].children[1];
	uint32_t nbody = a->nodes[node].children[2];
	
	for (size_t i = 0; i < a->nodes[narglist].nchildren; ++i)
	{
		uint32_t narg = a->nodes[narglist].children[i];
		uint32_t nargtype = a->nodes[narg].children[0];
		
		ls_tok_t argtok = l->toks[a->nodes[narg].tok];
		ls_toktype_t argtypetok = l->types[a->nodes[nargtype].tok];
		
		char sym[LS_MAXIDENT + 1] = {0};
		ls_readtokraw(sym, m->data[mod], argtok);
		
		int64_t prev = ls_findsym(st, sym);
		if (prev != -1)
		{
			return ls_redefinition(m, mod, st, argtok, prev);
		}
		
		ls_pushsym(st, ls_strdup(sym), ls_toktoprim[l->types[argtypetok]], mod, narg, scope);
	}
	
	ls_err_t e = ls_semastmt(m, mod, nbody, st);
	if (e.code)
	{
		return e;
	}
	
	ls_popsymscope(st, scope);
	return (ls_err_t){0};
}

static ls_err_t
ls_semastmt(ls_module_t *m, uint32_t mod, uint32_t node, ls_symtab_t *st)
{
	(void)m;
	(void)mod;
	(void)node;
	(void)st;
	
	// TODO: implement ls_semastmt().
	
	__builtin_unreachable();
}
