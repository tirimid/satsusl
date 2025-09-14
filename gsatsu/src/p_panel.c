// SPDX-License-Identifier: BSD-3-Clause

p_panel_t p_panel =
{
	.inputfiletf =
	{
		.buf = p_panel.inputfile,
		.cap = sizeof(p_panel.inputfile)
	},
	.outputfiletf =
	{
		.buf = p_panel.outputfile,
		.cap = sizeof(p_panel.outputfile)
	},
	.inputlinetf =
	{
		.buf = p_panel.inputline,
		.cap = sizeof(p_panel.inputline)
	},
	.cgetmutex = PTHREAD_MUTEX_INITIALIZER
};

static void p_lex(void);
static void p_parse(void);
static void p_import(void);
static void p_sema(void);
static void p_exec(void);
static void *p_execmodule(void *vpmod);

void
p_run(void)
{
	for (usize i = 0; i < O_MAXMODPATHS; ++i)
	{
		p_panel.modpathtfs[i] = (z_tfdata_t)
		{
			.buf = p_panel.modpaths[i],
			.cap = sizeof(p_panel.modpaths[i]) - 1
		};
	}
	
	for (;;)
	{
		z_begintick();
		
		// handle events.
		z_prepareinput();
		SDL_Event e;
		while (SDL_PollEvent(&e))
		{
			z_handleinput(&e);
			if (e.type == SDL_QUIT)
			{
				return;
			}
		}
		
		// do main UI panel.
		z_uielem_t mainelems[64];
		z_ui_t main = z_beginui(
			mainelems,
			sizeof(mainelems),
			30,
			30,
			r_fonts[R_VCROSDMONO],
			r_wnd
		);
		
		z_uilabel(&main, "Control panel");
		z_uiactive(&main, !p_panel.running);
		z_uipad(&main, 0, 20);
		z_uitextfield(&main, "Input file", &p_panel.inputfiletf, 12);
		z_uitextfield(&main, "Output file", &p_panel.outputfiletf, 12);
		z_uipad(&main, 0, 20);
		if (z_uibutton(&main, "Execute"))
		{
			p_clearoutput();
			p_exec();
		}
		z_uipad(&main, 0, 20);
		if (z_uibutton(&main, "Lex"))
		{
			p_clearoutput();
			p_lex();
		}
		if (z_uibutton(&main, "Parse"))
		{
			p_clearoutput();
			p_parse();
		}
		if (z_uibutton(&main, "Import"))
		{
			p_clearoutput();
			p_import();
		}
		if (z_uibutton(&main, "Sema"))
		{
			p_clearoutput();
			p_sema();
		}
		
		// do module path UI panel.
		z_uielem_t modelems[64];
		z_ui_t mod = z_beginui(
			modelems,
			sizeof(modelems),
			30,
			400,
			r_fonts[R_VCROSDMONO],
			r_wnd
		);
		
		z_uilabel(&mod, "Module paths");
		z_uiactive(&mod, !p_panel.running);
		z_uipad(&mod, 0, 20);
		for (i32 i = 0; i < O_MAXMODPATHS; ++i)
		{
			z_uitextfield(&mod, "Module path", &p_panel.modpathtfs[i], 12);
		}
		
		// do input / output UI panel.
		z_uielem_t ioelems[64];
		z_ui_t io = z_beginui(
			ioelems,
			sizeof(ioelems),
			240,
			30,
			r_fonts[R_VCROSDMONO],
			r_wnd
		);
		
		z_uilabel(&io, "Input / output terminal");
		z_uipad(&io, 0, 20);
		for (i32 i = O_NOUTPUTLINES - 1; i >= 0; --i)
		{
			z_uilabel(&io, p_panel.outputlines[i]);
		}
		z_uiactive(&io, p_panel.running);
		z_uipad(&io, 0, 20);
		z_uitextfield(&io, "Input line", &p_panel.inputlinetf, O_MAXIOLINE);
		if (z_uibutton(&io, "Send input"))
		{
			pthread_mutex_lock(&p_panel.cgetmutex);
			
			memcpy(p_panel.cgetbuf, p_panel.inputline, O_MAXINPUT);
			p_panel.cgetlen = p_panel.inputlinetf.len;
			p_panel.cgetidx = 0;
			
			ls_cprintf("%s\n", p_panel.inputline);
			memset(p_panel.inputline, 0, sizeof(p_panel.inputline));
			p_panel.inputlinetf = (z_tfdata_t)
			{
				.buf = p_panel.inputline,
				.cap = sizeof(p_panel.inputline)
			};
			
			pthread_mutex_unlock(&p_panel.cgetmutex);
		}
		
		// render.
		SDL_SetRenderDrawColor(r_rend, O_BGCOLOR);
		SDL_RenderClear(r_rend);
		z_renderui(&main);
		z_renderui(&mod);
		z_renderui(&io);
		SDL_RenderPresent(r_rend);
		
		z_endtick();
	}
}

void
p_clearoutput(void)
{
	for (usize i = 0; i < O_NOUTPUTLINES; ++i)
	{
		memset(&p_panel.outputlines[i], 0, sizeof(p_panel.outputlines[i]));
	}
}

void
p_pushoutput(char const *line)
{
	memmove(
		&p_panel.outputlines[1],
		&p_panel.outputlines[0],
		sizeof(p_panel.outputlines[0]) * (O_NOUTPUTLINES - 1)
	);
	
	memset(&p_panel.outputlines[0], 0, sizeof(p_panel.outputlines[0]));
	for (usize i = 0; i < O_MAXIOLINE && line[i]; ++i)
	{
		p_panel.outputlines[0][i] = line[i];
	}
}

void
p_err(char const *fmt, ...)
{
	va_list args;
	va_start(args, fmt);
	
	char msg[512] = "err: ";
	vsnprintf(&msg[5], sizeof(msg) - 5, fmt, args);
	ls_cprintf("%s\n", msg);
	
	va_end(args);
}

void
p_errfile(
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
	
	char msg[512] = "err: ";
	vsnprintf(&msg[5], sizeof(msg) - 5, fmt, args);
	ls_cprintf("%s\n", msg);
	
	va_end(args);
	
	p_showfile(name, data, datalen, pos, len);
}

void
p_showfile(
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
	
	ls_cprintf("%s:\n %u |", name, line);
	for (usize i = lbegin; i < lend; ++i)
	{
		if (data[i] == '\t')
		{
			ls_cprintf("  ");
		}
		else
		{
			p_cput(data[i]);
		}
	}
	
	ls_cprintf("\n ");
	for (usize i = 0; i <= linumlen; ++i)
	{
		p_cput(' ');
	}
	p_cput('|');
	for (usize i = lbegin; i < pos; ++i)
	{
		ls_cprintf("%s", data[i] == '\t' ? "  " : " ");
	}
	p_cput('^');
	for (usize i = 1; i < len; ++i)
	{
		p_cput('~');
	}
	p_cput('\n');
}

i32
p_cget(void)
{
	pthread_mutex_lock(&p_panel.cgetmutex);
	
	// may need to first flush cput buffer (otherwise prompts won't be visible).
	if (p_panel.cputlen)
	{
		p_panel.cputbuf[p_panel.cputlen] = 0;
		p_panel.cputlen = 0;
		ls_cprintf("%s\n", p_panel.cputbuf);
	}
	
	if (!p_panel.cgetlen)
	{
		pthread_mutex_unlock(&p_panel.cgetmutex);
		return LS_CIGNORE;
	}
	
	if (p_panel.cgetidx >= p_panel.cgetlen)
	{
		p_panel.cgetidx = 0;
		p_panel.cgetlen = 0;
		pthread_mutex_unlock(&p_panel.cgetmutex);
		return '\n';
	}
	
	i32 c = p_panel.cgetbuf[p_panel.cgetidx++];
	pthread_mutex_unlock(&p_panel.cgetmutex);
	return c;
}

void
p_cput(i32 c)
{
	// TODO: also write characters to the output file.
	
	if (c == '\n')
	{
		p_panel.cputbuf[p_panel.cputlen] = 0;
		p_panel.cputlen = 0;
		p_pushoutput(p_panel.cputbuf);
		return;
	}
	
	if (p_panel.cputlen >= O_MAXIOLINE)
	{
		p_panel.cputbuf[p_panel.cputlen] = 0;
		p_panel.cputlen = 0;
		p_pushoutput(p_panel.cputbuf);
	}
	
	p_panel.cputbuf[p_panel.cputlen++] = c;
}

static void
p_lex(void)
{
	FILE *fp = fopen(p_panel.inputfile, "rb");
	if (!fp)
	{
		p_err("lex: failed to open file %s!", p_panel.inputfile);
		return;
	}
	
	char *filedata;
	u32 filelen;
	ls_err_t e = ls_readfile(fp, &filedata, &filelen);
	fclose(fp);
	if (e.code)
	{
		p_err("lex: failed to read file %s - %s!", p_panel.inputfile, e.msg);
		ls_destroyerr(&e);
		return;
	}
	
	ls_lex_t lex;
	e = ls_lex(&lex, filedata, filelen);
	if (e.code)
	{
		p_errfile(p_panel.inputfile, filedata, filelen, e.pos, e.len, "lex: lex failed - %s!", e.msg);
		ls_destroyerr(&e);
		free(filedata);
		return;
	}
	
	for (usize i = 0; i < lex.ntoks; ++i)
	{
		ls_cprinttok(lex.toks[i], lex.types[i]);
	}
	
	ls_destroylex(&lex);
	free(filedata);
}

static void
p_parse(void)
{
	FILE *fp = fopen(p_panel.inputfile, "rb");
	if (!fp)
	{
		p_err("parse: failed to open file %s!", p_panel.inputfile);
		return;
	}
	
	char *filedata;
	u32 filelen;
	ls_err_t e = ls_readfile(fp, &filedata, &filelen);
	fclose(fp);
	if (e.code)
	{
		p_err("parse: failed to read file %s - %s!", p_panel.inputfile, e.msg);
		ls_destroyerr(&e);
		return;
	}
	
	ls_lex_t lex;
	e = ls_lex(&lex, filedata, filelen);
	if (e.code)
	{
		p_errfile(p_panel.inputfile, filedata, filelen, e.pos, e.len, "parse: lex failed - %s!", e.msg);
		ls_destroyerr(&e);
		free(filedata);
		return;
	}
	
	ls_ast_t ast;
	e = ls_parse(&ast, &lex);
	if (e.code)
	{
		p_errfile(p_panel.inputfile, filedata, filelen, e.pos, e.len, "parse: parse failed - %s!", e.msg);
		ls_destroyerr(&e);
		ls_destroylex(&lex);
		free(filedata);
		return;
	}
	
	ls_cprintast(&ast, &lex);
	
	ls_destroyast(&ast);
	ls_destroylex(&lex);
	free(filedata);
}

static void
p_import(void)
{
	FILE *fp = fopen(p_panel.inputfile, "rb");
	if (!fp)
	{
		p_err("import: failed to open file %s!", p_panel.inputfile);
		return;
	}
	
	char *filedata;
	u32 filelen;
	ls_err_t e = ls_readfile(fp, &filedata, &filelen);
	fclose(fp);
	if (e.code)
	{
		p_err("import: failed to read file %s - %s!", p_panel.inputfile, e.msg);
		ls_destroyerr(&e);
		return;
	}
	
	ls_lex_t lex;
	e = ls_lex(&lex, filedata, filelen);
	if (e.code)
	{
		p_errfile(p_panel.inputfile, filedata, filelen, e.pos, e.len, "import: lex failed - %s!", e.msg);
		ls_destroyerr(&e);
		free(filedata);
		return;
	}
	
	ls_ast_t ast;
	e = ls_parse(&ast, &lex);
	if (e.code)
	{
		p_errfile(p_panel.inputfile, filedata, filelen, e.pos, e.len, "import: parse failed - %s!", e.msg);
		ls_destroyerr(&e);
		ls_destroylex(&lex);
		free(filedata);
		return;
	}
	
	ls_module_t mod = ls_createmodule(
		&ast,
		&lex,
		strdup(p_panel.inputfile),
		ls_fileid(p_panel.inputfile, true),
		filedata,
		filelen
	);
	
	usize nmodpaths = 0;
	char *modpaths[O_MAXMODPATHS];
	for (usize i = 0; i < O_MAXMODPATHS; ++i)
	{
		if (p_panel.modpaths[i][0])
		{
			modpaths[nmodpaths++] = strdup(p_panel.modpaths[i]);
		}
	}
	
	e = ls_resolveimports(&mod, (char const **)modpaths, nmodpaths);
	for (usize i = 0; i < nmodpaths; ++i)
	{
		free(modpaths[i]);
	}
	if (e.code)
	{
		p_errfile(p_panel.inputfile, filedata, filelen, e.pos, e.len, "import: import resolution failed - %s!", e.msg);
		ls_destroyerr(&e);
		ls_destroymodule(&mod);
		return;
	}
	
	ls_cprintmodule(&mod);
	
	ls_destroymodule(&mod);
}

static void
p_sema(void)
{
	FILE *fp = fopen(p_panel.inputfile, "rb");
	if (!fp)
	{
		p_err("sema: failed to open file %s!", p_panel.inputfile);
		return;
	}
	
	char *filedata;
	u32 filelen;
	ls_err_t e = ls_readfile(fp, &filedata, &filelen);
	fclose(fp);
	if (e.code)
	{
		p_err("sema: failed to read file %s - %s!", p_panel.inputfile, e.msg);
		ls_destroyerr(&e);
		return;
	}
	
	ls_lex_t lex;
	e = ls_lex(&lex, filedata, filelen);
	if (e.code)
	{
		p_errfile(p_panel.inputfile, filedata, filelen, e.pos, e.len, "sema: lex failed - %s!", e.msg);
		ls_destroyerr(&e);
		free(filedata);
		return;
	}
	
	ls_ast_t ast;
	e = ls_parse(&ast, &lex);
	if (e.code)
	{
		p_errfile(p_panel.inputfile, filedata, filelen, e.pos, e.len, "sema: parse failed - %s!", e.msg);
		ls_destroyerr(&e);
		ls_destroylex(&lex);
		free(filedata);
		return;
	}
	
	ls_module_t mod = ls_createmodule(
		&ast,
		&lex,
		strdup(p_panel.inputfile),
		ls_fileid(p_panel.inputfile, true),
		filedata,
		filelen
	);
	
	usize nmodpaths = 0;
	char *modpaths[O_MAXMODPATHS];
	for (usize i = 0; i < O_MAXMODPATHS; ++i)
	{
		if (p_panel.modpaths[i][0])
		{
			modpaths[nmodpaths++] = strdup(p_panel.modpaths[i]);
		}
	}
	
	e = ls_resolveimports(&mod, (char const **)modpaths, nmodpaths);
	for (usize i = 0; i < nmodpaths; ++i)
	{
		free(modpaths[i]);
	}
	if (e.code)
	{
		p_errfile(p_panel.inputfile, filedata, filelen, e.pos, e.len, "sema: import resolution failed - %s!", e.msg);
		ls_destroyerr(&e);
		ls_destroymodule(&mod);
		return;
	}
	
	e = ls_sema(&mod);
	if (e.code)
	{
		p_errfile(mod.names[e.src], mod.data[e.src], mod.lens[e.src], e.pos, e.len, "sema: semantic analysis failed - %s!", e.msg);
		ls_destroyerr(&e);
		ls_destroymodule(&mod);
		return;
	}
	
	p_pushoutput("sema: finished successfully");
	
	ls_destroymodule(&mod);
}

static void
p_exec(void)
{
	FILE *fp = fopen(p_panel.inputfile, "rb");
	if (!fp)
	{
		p_err("exec: failed to open file %s!", p_panel.inputfile);
		return;
	}
	
	char *filedata;
	u32 filelen;
	ls_err_t e = ls_readfile(fp, &filedata, &filelen);
	fclose(fp);
	if (e.code)
	{
		p_err("exec: failed to read file %s - %s!", p_panel.inputfile, e.msg);
		ls_destroyerr(&e);
		return;
	}
	
	ls_lex_t lex;
	e = ls_lex(&lex, filedata, filelen);
	if (e.code)
	{
		p_errfile(p_panel.inputfile, filedata, filelen, e.pos, e.len, "exec: lex failed - %s!", e.msg);
		ls_destroyerr(&e);
		free(filedata);
		return;
	}
	
	ls_ast_t ast;
	e = ls_parse(&ast, &lex);
	if (e.code)
	{
		p_errfile(p_panel.inputfile, filedata, filelen, e.pos, e.len, "exec: parse failed - %s!", e.msg);
		ls_destroyerr(&e);
		ls_destroylex(&lex);
		free(filedata);
		return;
	}
	
	ls_module_t mod = ls_createmodule(
		&ast,
		&lex,
		strdup(p_panel.inputfile),
		ls_fileid(p_panel.inputfile, true),
		filedata,
		filelen
	);
	
	usize nmodpaths = 0;
	char *modpaths[O_MAXMODPATHS];
	for (usize i = 0; i < O_MAXMODPATHS; ++i)
	{
		if (p_panel.modpaths[i][0])
		{
			modpaths[nmodpaths++] = strdup(p_panel.modpaths[i]);
		}
	}
	
	e = ls_resolveimports(&mod, (char const **)modpaths, nmodpaths);
	for (usize i = 0; i < nmodpaths; ++i)
	{
		free(modpaths[i]);
	}
	if (e.code)
	{
		p_errfile(p_panel.inputfile, filedata, filelen, e.pos, e.len, "exec: import resolution failed - %s!", e.msg);
		ls_destroyerr(&e);
		ls_destroymodule(&mod);
		return;
	}
	
	e = ls_sema(&mod);
	if (e.code)
	{
		p_errfile(mod.names[e.src], mod.data[e.src], mod.lens[e.src], e.pos, e.len, "exec: semantic analysis failed - %s!", e.msg);
		ls_destroyerr(&e);
		ls_destroymodule(&mod);
		return;
	}
	
	ls_module_t *pmod = malloc(sizeof(ls_module_t));
	*pmod = mod;
	
	p_panel.running = true;
	pthread_t execthread;
	i32 rc = pthread_create(&execthread, NULL, p_execmodule, pmod);
	if (rc)
	{
		p_err("exec: failed to create execution thread!");
		ls_destroyerr(&e);
		ls_destroymodule(pmod);
		free(pmod);
		return;
	}
	
	// rest is handled across the UI code and p_execmodule().
}

static void *
p_execmodule(void *vpmod)
{
	ls_module_t *pmod = vpmod;
	ls_sysfns_t sysfns = ls_basesysfns();
	
	ls_err_t e = ls_exec(pmod, stderr, &sysfns, "start");
	
	if (e.code)
	{
		p_err("exec: execution failed - %s!", e.msg);
		ls_destroyerr(&e);
		ls_destroysysfns(&sysfns);
		ls_destroymodule(pmod);
		free(pmod);
		p_panel.running = false;
		return NULL;
	}
	
	p_pushoutput("exec: finished successfully");
	ls_destroysysfns(&sysfns);
	ls_destroymodule(pmod);
	free(pmod);
	p_panel.running = false;
	return NULL;
}
