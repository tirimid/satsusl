// SPDX-License-Identifier: BSD-3-Clause

// standard library.
#include <stddef.h>
#include <stdint.h>

// system dependencies.
#include <satsu.h>

// project headers.
#include "util.h"
#include "a_args.h"

// project source.
#include "a_args.c"

int
main(int argc, char *argv[])
{
	a_proc(argc, argv);
	
	ls_lex_t lex;
	ls_err_t err = ls_lexfile(&lex, a_args.infp, a_args.infile);
	if (err.code)
	{
		// TODO: implement error display.
		fprintf(stderr, "fucked up\n");
		return 1;
	}
	
	if (a_args.target == A_LEX)
	{
		for (usize i = 0; i < lex.ntoks; ++i)
		{
			ls_toktype_t type = lex.types[i];
			ls_tok_t tok = lex.toks[i];
			printf("%-8s%-6d%-6d\n", ls_toknames[type], tok.pos, tok.len);
		}
		
		return 0;
	}
	
	return 0;
}
