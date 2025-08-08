// SPDX-License-Identifier: BSD-3-Clause

// standard library.
#include <stdarg.h>
#include <stddef.h>
#include <stdint.h>
#include <string.h>

// system dependencies.
#include <satsu.h>
#include <sys/stat.h>
#include <unistd.h>

// project headers.
#include "util.h"
#include "a_args.h"

// project source.
#include "a_args.c"
#include "util.c"

int
main(int argc, char *argv[])
{
	a_proc(argc, argv);
	
	char *filedata;
	size_t filelen;
	ls_err_t e = ls_readfile(a_args.infp, a_args.infile, &filedata, &filelen);
	if (e.code)
	{
		err("main: failed to read file %s - %s!", e.src, e.msg);
	}
	
	ls_lex_t lex;
	e = ls_lex(&lex, a_args.infile, filedata, filelen);
	if (e.code)
	{
		errfile(e.src, filedata, e.pos, e.len, "main: lex failed - %s!", e.msg);
	}
	
	if (a_args.target == A_LEX)
	{
		for (usize i = 0; i < lex.ntoks; ++i)
		{
			ls_printtok(stdout, lex.toks[i], lex.types[i]);
		}
		return 0;
	}
	
	ls_ast_t ast;
	e = ls_parse(&ast, &lex, a_args.infile);
	if (e.code)
	{
		errfile(e.src, filedata, e.pos, e.len, "main: parse failed - %s!", e.msg);
	}
	
	if (a_args.target == A_PARSE)
	{
		ls_printast(stdout, &ast, &lex);
		return 0;
	}
	
	return 0;
}
