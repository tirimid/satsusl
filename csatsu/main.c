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
	u32 filelen;
	ls_err_t e = ls_readfile(a_args.infp, &filedata, &filelen);
	if (e.code)
	{
		err("main: failed to read file %s - %s!", a_args.infile, e.msg);
	}
	
	ls_lex_t lex;
	e = ls_lex(&lex, filedata, filelen);
	if (e.code)
	{
		errfile(a_args.infile, filedata, filelen, e.pos, e.len, "main: lex failed - %s!", e.msg);
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
	e = ls_parse(&ast, &lex);
	if (e.code)
	{
		errfile(a_args.infile, filedata, filelen, e.pos, e.len, "main: parse failed - %s!", e.msg);
	}
	
	if (a_args.target == A_PARSE)
	{
		ls_printast(stdout, &ast, &lex);
		return 0;
	}
	
	ls_module_t mod = ls_createmodule(
		&ast,
		&lex,
		strdup(a_args.infile),
		ls_fileid(a_args.infile, true),
		filedata,
		filelen
	);
	
	e = ls_resolveimports(&mod, a_args.paths, a_args.npaths);
	if (e.code)
	{
		errfile(a_args.infile, filedata, filelen, e.pos, e.len, "main: import resolution failed - %s!", e.msg);
	}
	
	if (a_args.target == A_IMPORT)
	{
		ls_printmoduleasts(stdout, &mod);
		return 0;
	}
	
	return 0;
}
