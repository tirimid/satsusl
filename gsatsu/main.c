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

// project headers.
#include "o_options.h"
#include "util.h"
#include "r_render.h"
#include "i_input.h"
#include "u_ui.h"

// project source.
#include "i_input.c"
#include "r_render.c"
#include "u_ui.c"
#include "util.c"

int
main(int argc, char *argv[])
{
	(void)argc;
	(void)argv;
	
	// initialize external systems.
	if (SDL_Init(O_SDLFLAGS))
	{
		err("main: failed to init SDL!");
		return 1;
	}
	atexit(SDL_Quit);
	
	if (TTF_Init())
	{
		err("main: failed to init SDL TTF!");
		return 1;
	}
	atexit(TTF_Quit);
	
	// initialize program systems.
	if (r_init())
	{
		return 1;
	}
	
	return 0;
}
