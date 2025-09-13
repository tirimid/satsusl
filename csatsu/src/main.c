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
#include "e_exec.h"

// project source.
#include "a_args.c"
#include "e_exec.c"
#include "util.c"

int
main(int argc, char *argv[])
{
	// superfluous code is included to destroy allocated objects, as address
	// sanitizer otherwise sometimes reports erroneous memory leaks.
	
	a_proc(argc, argv);
	
	ls_conf = (ls_conf_t)
	{
		.cget = e_cget,
		.cput = e_cput
	};
	
	char *filedata;
	u32 filelen;
	ls_err_t e = ls_readfile(a_args.infp, &filedata, &filelen);
	if (e.code)
	{
		err("main: failed to read file %s - %s!", a_args.infile, e.msg);
		ls_destroyerr(&e);
		return 1;
	}
	
	ls_lex_t lex;
	e = ls_lex(&lex, filedata, filelen);
	if (e.code)
	{
		errfile(a_args.infile, filedata, filelen, e.pos, e.len, "main: lex failed - %s!", e.msg);
		ls_destroyerr(&e);
		free(filedata);
		return 1;
	}
	
	if (a_args.target == A_LEX)
	{
		for (usize i = 0; i < lex.ntoks; ++i)
		{
			ls_printtok(stdout, lex.toks[i], lex.types[i]);
		}
		ls_destroylex(&lex);
		free(filedata);
		return 0;
	}
	
	ls_ast_t ast;
	e = ls_parse(&ast, &lex);
	if (e.code)
	{
		errfile(a_args.infile, filedata, filelen, e.pos, e.len, "main: parse failed - %s!", e.msg);
		ls_destroyerr(&e);
		ls_destroylex(&lex);
		free(filedata);
		return 1;
	}
	
	if (a_args.target == A_PARSE)
	{
		ls_printast(stdout, &ast, &lex);
		ls_destroyast(&ast);
		ls_destroylex(&lex);
		free(filedata);
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
		ls_destroyerr(&e);
		ls_destroymodule(&mod);
		return 1;
	}
	
	if (a_args.target == A_IMPORT)
	{
		ls_printmodule(stdout, &mod);
		ls_destroymodule(&mod);
		return 0;
	}
	
	e = ls_sema(&mod);
	if (e.code)
	{
		errfile(mod.names[e.src], mod.data[e.src], mod.lens[e.src], e.pos, e.len, "main: semantic analysis failed - %s!", e.msg);
		ls_destroyerr(&e);
		ls_destroymodule(&mod);
		return 1;
	}
	
	if (a_args.target == A_SEMA)
	{
		ls_destroymodule(&mod);
		return 0;
	}
	
	ls_sysfns_t sysfns = ls_basesysfns();
	
	e = ls_exec(&mod, stderr, &sysfns, "start");
	if (e.code)
	{
		err("main: execution failed - %s!", e.msg);
		ls_destroyerr(&e);
		ls_destroysysfns(&sysfns);
		ls_destroymodule(&mod);
		return 1;
	}
	
	ls_destroysysfns(&sysfns);
	ls_destroymodule(&mod);
	return 0;
}
