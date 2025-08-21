// SPDX-License-Identifier: BSD-3-Clause

typedef enum ls_execaction
{
	LS_NOACTION = 0,
	LS_RETURNVALUE,
	LS_STOPITER,
	LS_NEXTITER
} ls_execaction_t;

typedef struct ls_exec
{
	ls_module_t const *m;
	ls_sysfns_t const *sf;
	ls_symtab_t const *globalst;
	FILE *logfp;
	
	void *buf;
	ls_symtab_t *localsts;
	uint16_t *scopes;
	uint32_t *mods;
	uint32_t fndepth, fndepthcap;
} ls_exec_t;

static ls_exec_t ls_createexec(ls_module_t const *m, ls_sysfns_t const *sf, ls_symtab_t const *globalst);
static void ls_destroyexec(ls_exec_t *e);
static void ls_pushexecfn(ls_exec_t *e, uint32_t mod);
static void ls_popexecfn(ls_exec_t *e);
static ls_val_t ls_sysprint(ls_exec_t *e, ls_val_t args[LS_MAXSYSARGS]);
static ls_val_t ls_sysreadln(ls_exec_t *e, ls_val_t args[LS_MAXSYSARGS]);
static ls_val_t ls_sysshell(ls_exec_t *e, ls_val_t args[LS_MAXSYSARGS]);
static ls_execaction_t ls_execfuncdecl(ls_val_t *out, ls_exec_t *e, uint32_t node);
static ls_execaction_t ls_execlocaldecl(ls_val_t *out, ls_exec_t *e, uint32_t node);
static ls_execaction_t ls_execreturn(ls_val_t *out, ls_exec_t *e, uint32_t node);
static ls_execaction_t ls_execctree(ls_val_t *out, ls_exec_t *e, uint32_t node);
static ls_execaction_t ls_execwhile(ls_val_t *out, ls_exec_t *e, uint32_t node);
static ls_execaction_t ls_execfor(ls_val_t *out, ls_exec_t *e, uint32_t node);
static ls_execaction_t ls_execbreak(ls_val_t *out, ls_exec_t *e, uint32_t node);
static ls_execaction_t ls_execcontinue(ls_val_t *out, ls_exec_t *e, uint32_t node);
static ls_execaction_t ls_execblock(ls_val_t *out, ls_exec_t *e, uint32_t node);
static ls_execaction_t ls_execeatom(ls_val_t *out, ls_exec_t *e, uint32_t node);
static ls_execaction_t ls_execesystem(ls_val_t *out, ls_exec_t *e, uint32_t node);
static ls_execaction_t ls_exececall(ls_val_t *out, ls_exec_t *e, uint32_t node);
static ls_execaction_t ls_execeaccess(ls_val_t *out, ls_exec_t *e, uint32_t node);
static ls_execaction_t ls_execeneg(ls_val_t *out, ls_exec_t *e, uint32_t node);
static ls_execaction_t ls_execenot(ls_val_t *out, ls_exec_t *e, uint32_t node);
static ls_execaction_t ls_exececast(ls_val_t *out, ls_exec_t *e, uint32_t node);
static ls_execaction_t ls_execemul(ls_val_t *out, ls_exec_t *e, uint32_t node);
static ls_execaction_t ls_execediv(ls_val_t *out, ls_exec_t *e, uint32_t node);
static ls_execaction_t ls_execemod(ls_val_t *out, ls_exec_t *e, uint32_t node);
static ls_execaction_t ls_execeadd(ls_val_t *out, ls_exec_t *e, uint32_t node);
static ls_execaction_t ls_execesub(ls_val_t *out, ls_exec_t *e, uint32_t node);
static ls_execaction_t ls_execeless(ls_val_t *out, ls_exec_t *e, uint32_t node);
static ls_execaction_t ls_execelequal(ls_val_t *out, ls_exec_t *e, uint32_t node);
static ls_execaction_t ls_execegreater(ls_val_t *out, ls_exec_t *e, uint32_t node);
static ls_execaction_t ls_execegrequal(ls_val_t *out, ls_exec_t *e, uint32_t node);
static ls_execaction_t ls_execeequal(ls_val_t *out, ls_exec_t *e, uint32_t node);
static ls_execaction_t ls_execenequal(ls_val_t *out, ls_exec_t *e, uint32_t node);
static ls_execaction_t ls_execeand(ls_val_t *out, ls_exec_t *e, uint32_t node);
static ls_execaction_t ls_execeor(ls_val_t *out, ls_exec_t *e, uint32_t node);
static ls_execaction_t ls_execexor(ls_val_t *out, ls_exec_t *e, uint32_t node);
static ls_execaction_t ls_execeternary(ls_val_t *out, ls_exec_t *e, uint32_t node);
static ls_execaction_t ls_execeassign(ls_val_t *out, ls_exec_t *e, uint32_t node);
static ls_execaction_t ls_execeaddassign(ls_val_t *out, ls_exec_t *e, uint32_t node);
static ls_execaction_t ls_execesubassign(ls_val_t *out, ls_exec_t *e, uint32_t node);
static ls_execaction_t ls_execemulassign(ls_val_t *out, ls_exec_t *e, uint32_t node);
static ls_execaction_t ls_execedivassign(ls_val_t *out, ls_exec_t *e, uint32_t node);
static ls_execaction_t ls_execemodassign(ls_val_t *out, ls_exec_t *e, uint32_t node);

static ls_execaction_t (*ls_execfns[LS_NODETYPE_END])(ls_val_t *, ls_exec_t *, uint32_t) =
{
	// structure nodes.
	[LS_NULL] = NULL,
	[LS_ROOT] = NULL,
	[LS_IMPORT] = NULL,
	[LS_FUNCDECL] = ls_execfuncdecl,
	[LS_GLOBALDECL] = NULL,
	[LS_LOCALDECL] = ls_execlocaldecl,
	[LS_ARGLIST] = NULL,
	[LS_ARG] = NULL,
	[LS_RETURN] = ls_execreturn,
	[LS_CTREE] = ls_execctree,
	[LS_WHILE] = ls_execwhile,
	[LS_FOR] = ls_execfor,
	[LS_BREAK] = ls_execbreak,
	[LS_CONTINUE] = ls_execcontinue,
	[LS_BLOCK] = ls_execblock,
	
	// type nodes.
	[LS_TYPE] = NULL,
	
	// expression nodes.
	[LS_EATOM] = ls_execeatom,
	[LS_ESYSTEM] = ls_execesystem,
	[LS_ECALL] = ls_exececall,
	[LS_EACCESS] = ls_execeaccess,
	[LS_ENEG] = ls_execeneg,
	[LS_ENOT] = ls_execenot,
	[LS_ECAST] = ls_exececast,
	[LS_EMUL] = ls_execemul,
	[LS_EDIV] = ls_execediv,
	[LS_EMOD] = ls_execemod,
	[LS_EADD] = ls_execeadd,
	[LS_ESUB] = ls_execesub,
	[LS_ELESS] = ls_execeless,
	[LS_ELEQUAL] = ls_execelequal,
	[LS_EGREATER] = ls_execegreater,
	[LS_EGREQUAL] = ls_execegrequal,
	[LS_EEQUAL] = ls_execeequal,
	[LS_ENEQUAL] = ls_execenequal,
	[LS_EAND] = ls_execeand,
	[LS_EOR] = ls_execeor,
	[LS_EXOR] = ls_execexor,
	[LS_ETERNARY] = ls_execeternary,
	[LS_EASSIGN] = ls_execeassign,
	[LS_EADDASSIGN] = ls_execeaddassign,
	[LS_ESUBASSIGN] = ls_execesubassign,
	[LS_EMULASSIGN] = ls_execemulassign,
	[LS_EDIVASSIGN] = ls_execedivassign,
	[LS_EMODASSIGN] = ls_execemodassign,
};

ls_val_t
ls_defaultval(ls_primtype_t type)
{
	ls_val_t v =
	{
		.type = type
	};
	
	if (type == LS_INT)
	{
		v.data.int_ = 0;
	}
	else if (type == LS_REAL)
	{
		v.data.real = 0.0;
	}
	else if (type == LS_STRING)
	{
		v.data.string = ls_strdup("");
	}
	else if (type == LS_BOOL)
	{
		v.data.bool_ = false;
	}
	
	return v;
}

void
ls_destroyval(ls_val_t *v)
{
	if (v->type == LS_STRING)
	{
		ls_free(v->data.string);
	}
}

ls_sysfns_t
ls_emptysysfns(void)
{
	ls_sysfns_t sf =
	{
		.fncap = 1
	};
	
	ls_allocbatch_t allocs[] =
	{
		{(void **)&sf.names, 1, sizeof(char *)},
		{(void **)&sf.callbacks, 1, sizeof(ls_val_t (*)(ls_val_t[LS_MAXSYSARGS]))},
		{(void **)&sf.rettypes, 1, sizeof(uint8_t)},
		{(void **)&sf.argtypes, 1, sizeof(uint8_t[LS_MAXSYSARGS])},
		{(void **)&sf.nargs, 1, sizeof(uint8_t)}
	};
	sf.buf = ls_allocbatch(allocs, ARRSIZE(allocs));
	
	return sf;
}

ls_sysfns_t
ls_basesysfns(void)
{
	ls_sysfns_t sf = ls_emptysysfns();
	
	ls_primtype_t types[LS_MAXSYSARGS];
	
	// system void print(int, string).
	types[0] = LS_INT;
	types[1] = LS_STRING;
	ls_pushsysfn(&sf, "print", ls_sysprint, LS_VOID, types, 2);
	
	// system string readln(int).
	types[0] = LS_INT;
	ls_pushsysfn(&sf, "readln", ls_sysreadln, LS_STRING, types, 1);
	
	// system int shell(string).
	types[0] = LS_STRING;
	ls_pushsysfn(&sf, "shell", ls_sysshell, LS_STRING, types, 1);
	
	return sf;
}

void
ls_destroysysfns(ls_sysfns_t *sf)
{
	for (uint32_t i = 0; i < sf->fncap; ++i)
	{
		ls_free(sf->names[i]);
	}
	ls_free(sf->buf);
}

int64_t
ls_findsysfn(ls_sysfns_t const *sf, char const *sysfn)
{
	for (size_t i = 0; i < sf->nfns; ++i)
	{
		if (!strcmp(sf->names[i], sysfn))
		{
			return i;
		}
	}
	return -1;
}

// *sf takes ownership of name[0:strlen(name)].
void
ls_pushsysfn(
	ls_sysfns_t *sf,
	char *name,
	ls_val_t (*callback)(ls_exec_t *, ls_val_t[LS_MAXSYSARGS]),
	ls_primtype_t rettype,
	ls_primtype_t argtypes[LS_MAXSYSARGS],
	uint8_t nargs
)
{
	if (sf->nfns >= sf->fncap)
	{
		ls_reallocbatch_t reallocs[] =
		{
			{(void **)&sf->names, sf->fncap, 2 * sf->fncap, sizeof(char *)},
			{(void **)&sf->callbacks, sf->fncap, 2 * sf->fncap, sizeof(ls_val_t (*)(ls_val_t[LS_MAXSYSARGS]))},
			{(void **)&sf->rettypes, sf->fncap, 2 * sf->fncap, sizeof(uint8_t)},
			{(void **)&sf->argtypes, sf->fncap, 2 * sf->fncap, sizeof(uint8_t[LS_MAXSYSARGS])},
			{(void **)&sf->nargs, sf->fncap, 2 * sf->fncap, sizeof(uint8_t)}
		};
		
		sf->fncap *= 2;
		sf->buf = ls_reallocbatch(sf->buf, reallocs, ARRSIZE(reallocs));
	}
	
	sf->names[sf->nfns] = name;
	sf->callbacks[sf->nfns] = callback;
	sf->rettypes[sf->nfns] = rettype;
	for (size_t i = 0; i < LS_MAXSYSARGS; ++i)
	{
		sf->argtypes[sf->nfns][i] = argtypes[i];
	}
	sf->nargs[sf->nfns] = nargs;
	++sf->nfns;
}

// very little error checking is performed during execution as it is assumed
// that semantic analysis has already caught most potential errors.
ls_err_t
ls_exec(
	ls_module_t const *m,
	FILE *logfp,
	ls_sysfns_t const *sf,
	char const *entry
)
{
	ls_symtab_t globalst;
	ls_globalsymtab(&globalst, m);
	
	for (uint32_t i = 0; i < globalst.nsyms; ++i)
	{
		if (globalst.types[i] != LS_FUNC)
		{
			// TODO: fixup values.
		}
	}
	
	int64_t entryfn = ls_findsym(&globalst, entry);
	if (entryfn == -1)
	{
		return (ls_err_t)
		{
			.code = 1,
			.msg = ls_strdup("did not find entry function in program")
		};
	}
	
	// TODO: implement most of ls_exec().
	
	return (ls_err_t){0};
}

static ls_exec_t
ls_createexec(
	ls_module_t const *m,
	ls_sysfns_t const *sf,
	ls_symtab_t const *globalst
)
{
	ls_exec_t e =
	{
		.m = m,
		.sf = sf,
		.globalst = globalst,
		.fndepthcap = 1
	};
	
	ls_allocbatch_t allocs[] =
	{
		{(void **)&e.localsts, 1, sizeof(ls_symtab_t)},
		{(void **)&e.scopes, 1, sizeof(uint16_t)},
		{(void **)&e.mods, 1, sizeof(uint32_t)}
	};
	e.buf = ls_allocbatch(allocs, ARRSIZE(allocs));
	
	return e;
}

static void
ls_destroyexec(ls_exec_t *e)
{
	for (size_t i = 0; i < e->fndepth; ++i)
	{
		ls_destroysymtab(&e->localsts[i]);
	}
	free(e->buf);
}

static void
ls_pushexecfn(ls_exec_t *e, uint32_t mod)
{
	if (e->fndepth >= e->fndepthcap)
	{
		ls_reallocbatch_t reallocs[] =
		{
			{(void **)&e->localsts, e->fndepthcap, 2 * e->fndepthcap, sizeof(ls_symtab_t)},
			{(void **)&e->scopes, e->fndepthcap, 2 * e->fndepthcap, sizeof(uint16_t)},
			{(void **)&e->mods, e->fndepthcap, 2 * e->fndepthcap, sizeof(uint32_t)}
		};
		
		e->fndepthcap *= 2;
		e->buf = ls_reallocbatch(e->buf, reallocs, ARRSIZE(reallocs));
	}
	
	e->localsts[e->fndepth] = ls_createsymtab();
	e->scopes[e->fndepth] = 0;
	e->mods[e->fndepth] = mod;
	++e->fndepth;
}

static void
ls_popexecfn(ls_exec_t *e)
{
	ls_destroysymtab(&e->localsts[e->fndepth]);
	--e->fndepth;
}

static ls_val_t
ls_sysprint(ls_exec_t *e, ls_val_t args[LS_MAXSYSARGS])
{
	char const *s = args[1].data.string;
	if (dprintf(args[0].data.int_, "%s", s) != strlen(s))
	{
		fprintf(e->logfp, "err: failure on dprintf() in system print!\n");
	}
	return ls_defaultval(LS_VOID);
}

static ls_val_t
ls_sysreadln(ls_exec_t *e, ls_val_t args[LS_MAXSYSARGS])
{
	char buf[LS_MAXSTRING + 1] = {0};
	size_t len = 0;
	
	while (len < LS_MAXSTRING)
	{
		char ch;
		if (read(args[0].data.int_, &ch, 1) == -1)
		{
			fprintf(e->logfp, "err: failure on read() in system readln!\n");
			return ls_defaultval(LS_STRING);
		}
		
		if (ch == '\n')
		{
			break;
		}
		
		buf[len++] = ch;
	}
	
	return (ls_val_t)
	{
		.type = LS_STRING,
		.data.string = ls_strdup(buf)
	};
}

static ls_val_t
ls_sysshell(ls_exec_t *e, ls_val_t args[LS_MAXSYSARGS])
{
	// TODO: implement ls_sysshell().
}

static ls_execaction_t
ls_execfuncdecl(ls_val_t *out, ls_exec_t *e, uint32_t node)
{
	// TODO: implement.
}

static ls_execaction_t
ls_execlocaldecl(ls_val_t *out, ls_exec_t *e, uint32_t node)
{
	// TODO: implement.
}

static ls_execaction_t
ls_execreturn(ls_val_t *out, ls_exec_t *e, uint32_t node)
{
	// TODO: implement.
}

static ls_execaction_t
ls_execctree(ls_val_t *out, ls_exec_t *e, uint32_t node)
{
	// TODO: implement.
}

static ls_execaction_t
ls_execwhile(ls_val_t *out, ls_exec_t *e, uint32_t node)
{
	// TODO: implement.
}

static ls_execaction_t
ls_execfor(ls_val_t *out, ls_exec_t *e, uint32_t node)
{
	// TODO: implement.
}

static ls_execaction_t
ls_execbreak(ls_val_t *out, ls_exec_t *e, uint32_t node)
{
	// TODO: implement.
}

static ls_execaction_t
ls_execcontinue(ls_val_t *out, ls_exec_t *e, uint32_t node)
{
	// TODO: implement.
}

static ls_execaction_t
ls_execblock(ls_val_t *out, ls_exec_t *e, uint32_t node)
{
	// TODO: implement.
}

static ls_execaction_t
ls_execeatom(ls_val_t *out, ls_exec_t *e, uint32_t node)
{
	// TODO: implement.
}

static ls_execaction_t
ls_execesystem(ls_val_t *out, ls_exec_t *e, uint32_t node)
{
	// TODO: implement.
}

static ls_execaction_t
ls_exececall(ls_val_t *out, ls_exec_t *e, uint32_t node)
{
	// TODO: implement.
}

static ls_execaction_t
ls_execeaccess(ls_val_t *out, ls_exec_t *e, uint32_t node)
{
	// TODO: implement.
}

static ls_execaction_t
ls_execeneg(ls_val_t *out, ls_exec_t *e, uint32_t node)
{
	// TODO: implement.
}

static ls_execaction_t
ls_execenot(ls_val_t *out, ls_exec_t *e, uint32_t node)
{
	// TODO: implement.
}

static ls_execaction_t
ls_exececast(ls_val_t *out, ls_exec_t *e, uint32_t node)
{
	// TODO: implement.
}

static ls_execaction_t
ls_execemul(ls_val_t *out, ls_exec_t *e, uint32_t node)
{
	// TODO: implement.
}

static ls_execaction_t
ls_execediv(ls_val_t *out, ls_exec_t *e, uint32_t node)
{
	// TODO: implement.
}

static ls_execaction_t
ls_execemod(ls_val_t *out, ls_exec_t *e, uint32_t node)
{
	// TODO: implement.
}

static ls_execaction_t
ls_execeadd(ls_val_t *out, ls_exec_t *e, uint32_t node)
{
	// TODO: implement.
}

static ls_execaction_t
ls_execesub(ls_val_t *out, ls_exec_t *e, uint32_t node)
{
	// TODO: implement.
}

static ls_execaction_t
ls_execeless(ls_val_t *out, ls_exec_t *e, uint32_t node)
{
	// TODO: implement.
}

static ls_execaction_t
ls_execelequal(ls_val_t *out, ls_exec_t *e, uint32_t node)
{
	// TODO: implement.
}

static ls_execaction_t
ls_execegreater(ls_val_t *out, ls_exec_t *e, uint32_t node)
{
	// TODO: implement.
}

static ls_execaction_t
ls_execegrequal(ls_val_t *out, ls_exec_t *e, uint32_t node)
{
	// TODO: implement.
}

static ls_execaction_t
ls_execeequal(ls_val_t *out, ls_exec_t *e, uint32_t node)
{
	// TODO: implement.
}

static ls_execaction_t
ls_execenequal(ls_val_t *out, ls_exec_t *e, uint32_t node)
{
	// TODO: implement.
}

static ls_execaction_t
ls_execeand(ls_val_t *out, ls_exec_t *e, uint32_t node)
{
	// TODO: implement.
}

static ls_execaction_t
ls_execeor(ls_val_t *out, ls_exec_t *e, uint32_t node)
{
	// TODO: implement.
}

static ls_execaction_t
ls_execexor(ls_val_t *out, ls_exec_t *e, uint32_t node)
{
	// TODO: implement.
}

static ls_execaction_t
ls_execeternary(ls_val_t *out, ls_exec_t *e, uint32_t node)
{
	// TODO: implement.
}

static ls_execaction_t
ls_execeassign(ls_val_t *out, ls_exec_t *e, uint32_t node)
{
	// TODO: implement.
}

static ls_execaction_t
ls_execeaddassign(ls_val_t *out, ls_exec_t *e, uint32_t node)
{
	// TODO: implement.
}

static ls_execaction_t
ls_execesubassign(ls_val_t *out, ls_exec_t *e, uint32_t node)
{
	// TODO: implement.
}

static ls_execaction_t
ls_execemulassign(ls_val_t *out, ls_exec_t *e, uint32_t node)
{
	// TODO: implement.
}

static ls_execaction_t
ls_execedivassign(ls_val_t *out, ls_exec_t *e, uint32_t node)
{
	// TODO: implement.
}

static ls_execaction_t
ls_execemodassign(ls_val_t *out, ls_exec_t *e, uint32_t node)
{
	// TODO: implement.
}
