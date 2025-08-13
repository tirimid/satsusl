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
ls_printmoduleasts(FILE *fp, ls_module_t const *m)
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
