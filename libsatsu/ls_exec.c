// SPDX-License-Identifier: BSD-3-Clause

static ls_val_t ls_sysprint(ls_val_t args[LS_MAXSYSARGS]);
static ls_val_t ls_syseprint(ls_val_t args[LS_MAXSYSARGS]);
static ls_val_t ls_sysreadln(ls_val_t args[LS_MAXSYSARGS]);

void
ls_destroyval(ls_val_t *v)
{
	if (v->type == LS_STRING)
	{
		ls_free(v->data.string.s);
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
	
	// system void print(string).
	types[0] = LS_STRING;
	ls_pushsysfn(&sf, "print", ls_sysprint, LS_VOID, types, 1);
	
	// system void eprint(string).
	types[0] = LS_STRING;
	ls_pushsysfn(&sf, "eprint", ls_syseprint, LS_VOID, types, 1);
	
	// system string readln().
	ls_pushsysfn(&sf, "readln", ls_sysreadln, LS_STRING, types, 0);
	
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
	ls_val_t (*callback)(ls_val_t[LS_MAXSYSARGS]),
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

ls_err_t
ls_exec(
	ls_val_t *out,
	ls_module_t const *m,
	FILE *logfp,
	ls_sysfns_t const *sf,
	char const *entry
)
{
	// TODO: implement.
}

static ls_val_t
ls_sysprint(ls_val_t args[LS_MAXSYSARGS])
{
	// TODO: implement.
}

static ls_val_t
ls_syseprint(ls_val_t args[LS_MAXSYSARGS])
{
	// TODO: implement.
}

static ls_val_t
ls_sysreadln(ls_val_t args[LS_MAXSYSARGS])
{
	// TODO: implement.
}
