// SPDX-License-Identifier: BSD-3-Clause

// standard library.
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>

// system dependencies.
#include <SDL.h>
#include <SDL_ttf.h>
#include <sys/time.h>

// in-project dependencies.
#define TGL_IMPLEMENTATION
#include <tgl.h>

// project headers.
#include "o_options.h"
#include "util.h"
#include "resources.h"
#include "r_render.h"
#include "p_panel.h"

// project source.
#include "p_panel.c"
#include "r_render.c"

int
main(int argc, char *argv[])
{
	(void)argc;
	(void)argv;
	
	// initialize external systems.
	tgl_conf = (tgl_conf_t)
	{
		.log = stderr,
		.errtitle = O_ERRWNDTITLE,
		.tickus = O_TICKUS,
		.uipad = O_UIPAD,
		.renderrect = r_tglrenderrect,
		.rendertext = r_tglrendertext
	};
	
	if (SDL_Init(O_SDLFLAGS))
	{
		tgl_err("main: failed to init SDL!");
		return 1;
	}
	atexit(SDL_Quit);
	
	if (TTF_Init())
	{
		tgl_err("main: failed to init SDL TTF!");
		return 1;
	}
	atexit(TTF_Quit);
	
	// initialize program systems.
	if (r_init())
	{
		return 1;
	}
	
	p_run();
	
	return 0;
}
