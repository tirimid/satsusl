// SPDX-License-Identifier: BSD-3-Clause

a_args_t a_args;

static void a_usage(char const *name);

void
a_proc(i32 argc, char *argv[])
{
	i32 ch;
	while (ch = getopt(argc, (char *const *)argv, "hm:t:"), ch != -1)
	{
		switch (ch)
		{
		case 'h':
			a_usage(argv[0]);
			exit(0);
		case 'm':
			if (a_args.npaths >= A_MAXPATHS)
			{
				err("args: cannot have more than " STRINGIFY(A_MAXPATHS) " module paths!");
			}
			// TODO: verify that optarg is non-empty.
			a_args.paths[a_args.npaths++] = optarg;
			break;
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
			else if (!strcmp(optarg, "import"))
			{
				a_args.target = A_IMPORT;
			}
			else
			{
				err("args: unknown argument for -t - %s!", optarg);
			}
			break;
		default:
			exit(1);
		}
	}
	
	if (optind + 1 != argc)
	{
		err("args: missing required positional argument!");
	}
	
	a_args.infile = argv[optind];
	a_args.infp = openread(argv[optind]);
	if (!a_args.infp)
	{
		err("args: file cannot be read - %s!", argv[optind]);
	}
}

static void
a_usage(char const *name)
{
	// TODO: implement usage text.
}
