// SPDX-License-Identifier: BSD-3-Clause

int
e_cget(void)
{
	int c = fgetc(stdin);
	return c == EOF ? LS_CERR : c;
}

void
e_cput(int c)
{
	fputc(c, stdout);
}
