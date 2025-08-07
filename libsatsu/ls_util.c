// SPDX-License-Identifier: BSD-3-Clause

void
ls_destroyerr(ls_err_t *err)
{
	if (err->src)
	{
		ls_free(err->src);
	}
	if (err->msg)
	{
		ls_free(err->msg);
	}
}

void
ls_destroylex(ls_lex_t *l)
{
	if (l->toks)
	{
		ls_free(l->toks);
	}
	if (l->types)
	{
		ls_free(l->types);
	}
}
