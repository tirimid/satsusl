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
		}
		z_uipad(&main, 0, 20);
		if (z_uibutton(&main, "Lex"))
		{
			p_pushoutput("tried to lex");
		}
		if (z_uibutton(&main, "Parse"))
		{
			p_pushoutput("tried to parse");
		}
		if (z_uibutton(&main, "Import"))
		{
			p_pushoutput("tried to import");
		}
		if (z_uibutton(&main, "Sema"))
		{
			p_pushoutput("tried to sema");
		}
		
		// do module path UI panel.
		z_uielem_t modelems[64];
		z_ui_t mod = z_beginui(
			modelems,
			sizeof(modelems),
			30,
			420,
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
			260,
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
		}
		
		// do program status panel.
		z_uielem_t statuselems[64];
		z_ui_t status = z_beginui(
			statuselems,
			sizeof(statuselems),
			260,
			660,
			r_fonts[R_VCROSDMONO],
			r_wnd
		);
		
		z_uilabel(&status, "Program status");
		z_uipad(&status, 0, 20);
		z_uilabel(&status, "{program status}");
		
		// render.
		SDL_SetRenderDrawColor(r_rend, O_BGCOLOR);
		SDL_RenderClear(r_rend);
		z_renderui(&main);
		z_renderui(&mod);
		z_renderui(&io);
		z_renderui(&status);
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
