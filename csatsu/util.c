// SPDX-License-Identifier: BSD-3-Clause

void
err(char const *fmt, ...)
{
	va_list args;
	va_start(args, fmt);
	
	char msg[512] = "\x1b[1;31merr\x1b[0m: ";
	vsnprintf(&msg[16], sizeof(msg) - 16, fmt, args);
	fprintf(stderr, "%s\n", msg);
	
	va_end(args);
	
	exit(1);
}

void
errfile(
	char const *name,
	char const *data,
	usize datalen,
	usize pos,
	usize len,
	char const *fmt,
	...
)
{
	va_list args;
	va_start(args, fmt);
	
	char msg[512] = "\x1b[1;31merr\x1b[0m: ";
	vsnprintf(&msg[16], sizeof(msg) - 16, fmt, args);
	fprintf(stderr, "%s\n", msg);
	
	va_end(args);
	
	showfile(stderr, name, data, datalen, pos, len);
	
	exit(1);
}

void
showfile(
	FILE *fp,
	char const *name,
	char const *data,
	usize datalen,
	usize pos,
	usize len
)
{
	u32 line = 1;
	for (usize i = 0; i < pos; ++i)
	{
		line += data[i] == '\n';
	}
	
	usize lbegin = pos;
	while (lbegin && data[lbegin - 1] != '\n')
	{
		--lbegin;
	}
	
	usize lend = pos;
	while (lend < datalen && data[lend] != '\n')
	{
		++lend;
	}
	
	char linum[32] = {0};
	sprintf(linum, "%u", line);
	usize linumlen = strlen(linum);
	
	fprintf(fp, "%s:\n %u |\x1b[0;36m", name, line);
	for (usize i = lbegin; i < lend; ++i)
	{
		if (data[i] == '\t')
		{
			fprintf(fp, "  ");
		}
		else
		{
			fprintf(fp, "%c", data[i]);
		}
	}
	
	fprintf(fp, "\x1b[0m\n ");
	for (usize i = 0; i <= linumlen; ++i)
	{
		fprintf(fp, " ");
	}
	fprintf(fp, "|");
	for (usize i = lbegin; i < pos; ++i)
	{
		fprintf(fp, "%s", data[i] == '\t' ? "  " : " ");
	}
	fprintf(fp, "\x1b[1;35m^");
	for (usize i = 1; i < len; ++i)
	{
		fprintf(fp, "~");
	}
	fprintf(fp, "\x1b[0m\n");
}

FILE *
openread(char const *file)
{
	struct stat s;
	if (stat(file, &s) || !S_ISREG(s.st_mode))
	{
		return NULL;
	}
	return fopen(file, "rb");
}
