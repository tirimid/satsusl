// SPDX-License-Identifier: BSD-3-Clause

char const *ls_primtypenames[LS_PRIMTYPE_END] =
{
	"null",
	
	"int",
	"real",
	"string",
	"bool",
	"void"
};

// *out takes ownership of *a, *l, and data[0:len].
ls_err_t
ls_buildmodule(
	ls_module_t *out,
	ls_ast_t *a,
	ls_lex_t *l,
	char const *name,
	uint64_t id,
	char *data,
	uint32_t len,
	char const *paths[],
	size_t npaths
)
{
	ls_module_t m =
	{
		.names = ls_calloc(1, sizeof(char *)),
		.fileids = ls_calloc(1, sizeof(uint64_t)),
		.data = ls_calloc(1, sizeof(char *)),
		.lens = ls_calloc(1, sizeof(uint32_t)),
		.lexes = ls_calloc(1, sizeof(ls_lex_t)),
		.asts = ls_calloc(1, sizeof(ls_ast_t)),
		.modcap = 1
	};
	ls_pushmodule(&m, a, l, ls_strdup(name), id, data, len);
	
	// find modules requiring import.
	for (size_t i = 0; i < m.asts[0].nnodes; ++i)
	{
		if (m.asts[0].types[i] != LS_IMPORT)
		{
			continue;
		}
		
		char importname[LS_MAXIDENT] = {0};
		ls_readtokraw(
			importname,
			m.data[0],
			m.lexes[0].toks[m.asts[0].nodes[i].tok]
		);
		
		char importpath[PATH_MAX] = {0};
		uint64_t importid = 0;
		for (size_t j = 0; j < npaths; ++j)
		{
			// TODO: continue work.
		}
		
		// TODO: continue work.
	}
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
	if (m->nmods >= a->modcap)
	{
		m->nmods *= 2;
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
	m->lexes[m->nmods] = lex;
	m->asts[m->nmods] = ast;
	++m->nmods;
}

void
ls_printmodule(FILE *fp, ls_module_t const *m)
{
	// TODO: implement.
}

void
ls_destroymodule(ls_module_t *m)
{
	for (size_t i = 0; i < m->nmods; ++i)
	{
		free(n->names[i]);
		free(n->data[i]);
		free(n->lens[i]);
		ls_destroylex(n->lexes[i]);
		ls_destroyast(n->asts[i]);
	}
	
	if (m->names)
	{
		free(m->names);
	}
	if (m->ids)
	{
		free(m->ids);
	}
	if (m->data)
	{
		free(m->data);
	}
	if (m->lens)
	{
		free(m->lens);
	}
	if (m->lexes)
	{
		free(m->lexes);
	}
	if (m->asts)
	{
		free(m->asts);
	}
}
