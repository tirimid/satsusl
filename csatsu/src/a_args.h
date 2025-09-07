// SPDX-License-Identifier: BSD-3-Clause

#define A_MAXPATHS 32

typedef enum a_target
{
	A_EXEC = 0,
	A_LEX,
	A_PARSE,
	A_IMPORT,
	A_SEMA
} a_target_t;

typedef struct a_args
{
	char const *infile;
	FILE *infp;
	char const *paths[A_MAXPATHS];
	usize npaths;
	u8 target;
} a_args_t;

extern a_args_t a_args;

void a_proc(i32 argc, char *argv[]);
