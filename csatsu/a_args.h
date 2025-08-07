// SPDX-License-Identifier: BSD-3-Clause

typedef enum a_target
{
	A_EXEC = 0,
	A_LEX,
	A_PARSE,
} a_target_t;

typedef struct a_args
{
	char const *infile;
	FILE *infp;
	u8 target;
} a_args_t;

extern a_args_t a_args;

void a_proc(i32 argc, char *argv[]);
