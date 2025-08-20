// SPDX-License-Identifier: BSD-3-Clause

void
ls_destroyval(ls_val_t *v)
{
	if (v->type == LS_STRING)
	{
		ls_free(v->data.string.s);
	}
}
