// SPDX-License-Identifier: BSD-3-Clause

static char inputfile[128] = {0};
static tgl_tfdata_t inputfiletf =
{
	.buf = inputfile,
	.cap = sizeof(inputfile) - 1
};

static char outputfile[128] = {0};
static tgl_tfdata_t outputfiletf =
{
	.buf = outputfile,
	.cap = sizeof(outputfile) - 1
};

static char modpaths[O_MAXMODPATHS][128] = {{0}};
static tgl_tfdata_t modpathtfs[O_MAXMODPATHS];

static char inputline[128] = {0};
static tgl_tfdata_t inputlinetf =
{
	.buf = inputline,
	.cap = sizeof(inputline) - 1
};

void
p_run(void)
{
	for (size_t i = 0; i < O_MAXMODPATHS; ++i)
	{
		modpathtfs[i] = (tgl_tfdata_t)
		{
			.buf = modpaths[i],
			.cap = sizeof(modpaths[i]) - 1
		};
	}
	
	for (;;)
	{
		tgl_begintick();
		
		// handle events.
		tgl_prepareinput();
		SDL_Event e;
		while (SDL_PollEvent(&e))
		{
			tgl_handleinput(&e);
			if (e.type == SDL_QUIT)
			{
				return;
			}
		}
		
		// do main UI panel.
		tgl_uielem_t mainelems[64];
		tgl_ui_t main = tgl_beginui(
			mainelems,
			sizeof(mainelems),
			30,
			30,
			r_fonts[R_VCROSDMONO],
			r_wnd
		);
		
		tgl_uilabel(&main, "Control panel");
		tgl_uipad(&main, 0, 20);
		tgl_uitextfield(&main, "Input file", &inputfiletf, 12);
		tgl_uitextfield(&main, "Output file", &outputfiletf, 12);
		tgl_uipad(&main, 0, 20);
		if (tgl_uibutton(&main, "Execute"))
		{
		}
		tgl_uipad(&main, 0, 20);
		if (tgl_uibutton(&main, "Lex"))
		{
		}
		if (tgl_uibutton(&main, "Parse"))
		{
		}
		if (tgl_uibutton(&main, "Import"))
		{
		}
		if (tgl_uibutton(&main, "Sema"))
		{
		}
		
		// do module path UI panel.
		tgl_uielem_t modelems[64];
		tgl_ui_t mod = tgl_beginui(
			modelems,
			sizeof(modelems),
			30,
			420,
			r_fonts[R_VCROSDMONO],
			r_wnd
		);
		
		tgl_uilabel(&mod, "Module paths");
		tgl_uipad(&mod, 0, 20);
		for (i32 i = 0; i < O_MAXMODPATHS; ++i)
		{
			tgl_uitextfield(&mod, "Module path", &modpathtfs[i], 12);
		}
		
		// do input / output UI panel.
		tgl_uielem_t ioelems[64];
		tgl_ui_t io = tgl_beginui(
			ioelems,
			sizeof(ioelems),
			260,
			30,
			r_fonts[R_VCROSDMONO],
			r_wnd
		);
		
		tgl_uilabel(&io, "Input / output terminal");
		tgl_uipad(&io, 0, 20);
		for (i32 i = 0; i < O_MAXIOLINES; ++i)
		{
			tgl_uilabel(&io, "{output line}");
		}
		tgl_uipad(&io, 0, 20);
		tgl_uitextfield(&io, "Input line", &inputlinetf, 28);
		tgl_uibutton(&io, "Send input");
		
		// do program status panel.
		tgl_uielem_t statuselems[64];
		tgl_ui_t status = tgl_beginui(
			statuselems,
			sizeof(statuselems),
			260,
			660,
			r_fonts[R_VCROSDMONO],
			r_wnd
		);
		
		tgl_uilabel(&status, "Program status");
		tgl_uipad(&status, 0, 20);
		tgl_uilabel(&status, "{program status}");
		
		// render.
		SDL_SetRenderDrawColor(r_rend, O_BGCOLOR);
		SDL_RenderClear(r_rend);
		tgl_renderui(&main);
		tgl_renderui(&mod);
		tgl_renderui(&io);
		tgl_renderui(&status);
		SDL_RenderPresent(r_rend);
		
		tgl_endtick();
	}
}
