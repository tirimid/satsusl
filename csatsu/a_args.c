// SPDX-License-Identifier: BSD-3-Clause

a_args_t a_args;

static void a_usage(char const *name);

void
a_proc(i32 argc, char *argv[])
{
	i32 ch;
	while (ch = getopt(argc, (char *const *)argv, "ht:"), ch != -1)
	{
		switch (ch)
		{
		case 'h':
			a_usage(name);
			exit(0);
		case 't':
			if (!strcmp(optarg, "exec"))
			{
				a_args.target = A_EXEC;
			}
			else if (!strcmp(optarg, "lex"))
			{
				a_args.target = A_LEX;
			}
			else if (!strcmp(optarg, "parse"))
			{
				a_args.target = A_PARSE;
			}
			else
			{
				// TODO: maybe rewrite this error.
				fprintf(stderr, "you really done fucked up big time buddy\n");
				exit(1);
			}
			break;
		default:
			exit(1);
		}
	}
	
	if (optind + 1 != argc)
	{
		// TODO: rewrite error.
		fprintf(stderr, "fuck you\n");
		exit(1);
	}
	
	// TODO: get args for required arg.
}

static void
a_usage(char const *name)
{
	// TODO: implement usage text.
}
