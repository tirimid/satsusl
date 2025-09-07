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
	
	char *data = ls_malloc(len);
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

uint64_t
ls_alignbatch(uint64_t n)
{
	return n + LS_BATCHALIGN - n % LS_BATCHALIGN;
}

void *
ls_allocbatch(ls_allocbatch_t *allocs, size_t nallocs)
{
	size_t size = 0;
	for (size_t i = 0; i < nallocs; ++i)
	{
		size += allocs[i].n * allocs[i].size;
		size = ls_alignbatch(size);
	}
	
	uint8_t *ptr = ls_malloc(size);
	if (!ptr)
	{
		return NULL;
	}
	
	size_t offset = 0;
	for (size_t i = 0; i < nallocs; ++i)
	{
		*allocs[i].ptr = &ptr[offset];
		offset += allocs[i].n * allocs[i].size;
		offset = ls_alignbatch(offset);
	}
	
	return ptr;
}

// function assumes that nreallocs <= LS_MAXREALLOCBATCH.
void *
ls_reallocbatch(void *p, ls_reallocbatch_t *reallocs, size_t nreallocs)
{
	size_t oldoffsets[LS_MAXREALLOCBATCH], newoffsets[LS_MAXREALLOCBATCH];
	
	size_t newsize = 0, oldsize = 0;
	for (size_t i = 0; i < nreallocs; ++i)
	{
		newoffsets[i] = newsize;
		newsize += reallocs[i].newn * reallocs[i].size;
		newsize = ls_alignbatch(newsize);
		
		oldoffsets[i] = oldsize;
		oldsize += reallocs[i].oldn * reallocs[i].size;
		oldsize = ls_alignbatch(oldsize);
	}
	
	p = ls_realloc(p, newsize);
	if (!p)
	{
		return NULL;
	}
	
	uint8_t *up = p;
	for (ssize_t i = nreallocs - 1; i >= 0; --i)
	{
		size_t newbytes = reallocs[i].newn * reallocs[i].size;
		size_t oldbytes = reallocs[i].oldn * reallocs[i].size;
		size_t bytes = newbytes < oldbytes ? newbytes : oldbytes;
		
		ls_memmove(&up[newoffsets[i]], &up[oldoffsets[i]], bytes);
		*reallocs[i].ptr = &up[newoffsets[i]];
	}
	
	return p;
}
