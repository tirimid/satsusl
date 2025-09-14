// SPDX-License-Identifier: BSD-3-Clause

// standard library.
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>

// system dependencies.
#include <pthread.h>
#include <SDL.h>
#include <SDL_ttf.h>
#include <sys/time.h>

// in-project dependencies.
#include <satsu.h>
#include <ztgl.h>

// project headers.
#include "o_options.h"
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
	z_conf = (z_conf_t)
	{
		.log = stderr,
		.errtitle = O_ERRWNDTITLE,
		.tickus = O_TICKUS,
		.uipad = O_UIPAD,
		.uitextfieldbar = O_UITEXTFIELDBAR,
		.renderrect = r_tglrenderrect,
		.rendertext = r_tglrendertext
	};
	
	ls_conf = (ls_conf_t)
	{
		.cget = p_cget,
		.cput = p_cput
	};
	
	if (SDL_Init(O_SDLFLAGS))
	{
		z_err("main: failed to init SDL!");
		return 1;
	}
	atexit(SDL_Quit);
	
	if (TTF_Init())
	{
		z_err("main: failed to init SDL TTF!");
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
