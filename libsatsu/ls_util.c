// SPDX-License-Identifier: BSD-3-Clause

void
ls_destroyerr(ls_err_t *err)
{
	if (err->msg)
	{
		ls_free(err->msg);
	}
}

ls_err_t
ls_readfile(FILE *fp, char **outdata, uint32_t *outlen)
{
	fseek(fp, 0, SEEK_END);
	ssize_t len = ftell(fp);
	if (len < 0)
	{
		return (ls_err_t)
		{
			.code = 1,
			.msg = ls_strdup("failed to get file size")
		};
	}
	
	if (len >= UINT32_MAX)
	{
		return (ls_err_t)
		{
			.code = 1,
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
			.msg = ls_strdup("failed to read file")
		};
	}
	
	*outdata = data;
	*outlen = len;
	return (ls_err_t){0};
}

uint64_t
ls_fileid(char const *file, bool deref)
{
	int (*statfn[])(char const *restrict, struct stat *restrict) =
	{
		lstat,
		stat
	};
	
	struct stat stat;
	if (statfn[deref](file, &stat))
	{
		return 0;
	}
	
	return stat.st_ino;
}
