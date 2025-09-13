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
	}
};

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
		z_uipad(&io, 0, 20);
		z_uitextfield(&io, "Input line", &p_panel.inputlinetf, O_MAXIOLINE);
		if (z_uibutton(&io, "Send input"))
		{
			ls_cprintf("ioenasrt ioarst nieoars toeinars tionearstoe nrst\n");
			// TODO: implement send input.
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
	
	ls_cprintf("%s:\n %u |", name, linum);
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
	
	ls_cprintf("  ");
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
	if (!p_panel.cgetlen)
	{
		return LS_CIGNORE;
	}
	
	if (p_panel.cgetidx >= p_panel.cgetlen)
	{
		p_panel.cgetidx = 0;
		p_panel.cgetlen = 0;
		return '\n';
	}
	
	return p_panel.cgetbuf[p_panel.cgetidx++];
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

void
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

void
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

void
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
	
	// TODO: finish implementing.
}

void
p_sema(void)
{
	// TODO: implement.
}

void
p_exec(void)
{
	// TODO: implement.
}
