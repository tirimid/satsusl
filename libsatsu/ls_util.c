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

ls_err_t
ls_readfile(FILE *fp, char const *name, char **outdata, size_t *outlen)
{
	fseek(fp, 0, SEEK_END);
	ssize_t len = ftell(fp);
	if (len < 0)
	{
		return (ls_err_t)
		{
			.code = 1,
			.src = ls_strdup(name),
			.msg = ls_strdup("failed to get file size")
		};
	}
	
	if (len >= UINT32_MAX)
	{
		return (ls_err_t)
		{
			.code = 1,
			.src = ls_strdup(name),
			.msg = ls_strdup("file is too big to lex")
		};
	}
	
	char *data = ls_calloc(len, 1);
	fseek(fp, 0, SEEK_SET);
	if (fread(data, 1, len, fp) != (size_t)len)
	{
		ls_free(data);
		return (ls_err_t)
		{
			.code = 1,
			.src = ls_strdup(name),
			.msg = ls_strdup("failed to read file")
		};
	}
	
	*outdata = data;
	*outlen = len;
	return (ls_err_t){0};
}
