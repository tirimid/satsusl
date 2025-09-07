// SPDX-License-Identifier: BSD-3-Clause

#ifndef TGL_TGL_H
#define TGL_TGL_H

// standard library.
#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>
#include <stdio.h>

// system dependencies.
#include <SDL.h>
#include <SDL_ttf.h>

//--------------------------//
// library meta-information //
//--------------------------//

#define TGL_VERMAJOR 1
#define TGL_VERMINOR 0
#define TGL_VERPATCH 0

//--------//
// macros //
//--------//

// profiling.
#ifdef TGL_PROFILE
#define TGL_NEWTIMER(timer) uint64_t timer
#define TGL_BEGINTIMER(timer) tgl_begintimer(name)
#define TGL_ENDTIMER(timer, name) tgl_endtimer(timer, name)
#else
#define TGL_NEWTIMER(timer)
#define TGL_BEGINTIMER(timer)
#define TGL_ENDTIMER(timer, name)
#endif

// util constants.
#define TGL_PI 3.141592f
#define TGL_TAU (2.0f * TGL_PI)

//--------------------//
// enumeration values //
//--------------------//

typedef enum tgl_color
{
	TGL_PANELCOLOR = 0,
	TGL_LABELTEXTCOLOR,
	TGL_BUTTONCOLOR,
	TGL_BUTTONPRESSCOLOR,
	TGL_BUTTONHOVERCOLOR,
	TGL_BUTTONTEXTCOLOR,
	TGL_BUTTONTEXTPRESSCOLOR,
	TGL_BUTTONTEXTHOVERCOLOR
} tgl_color_t;

typedef enum tgl_uitype
{
	TGL_LABEL = 0,
	TGL_BUTTON
} tgl_uitype_t;

//-----------------//
// data structures //
//-----------------//

typedef struct tgl_conf
{
	// library function config.
	FILE *log;
	char const *errtitle;
	uint64_t tickus;
	int32_t uipad;
	
	// rendering call config.
	void (*renderrect)(int32_t, int32_t, int32_t, int32_t, tgl_color_t);
	void (*rendertext)(int32_t, int32_t, int32_t, int32_t, char const *, tgl_color_t);
} tgl_conf_t;

typedef union tgl_uielem
{
	// all UI elements have at least these properties.
	struct
	{
		uint8_t type;
		int32_t x, y;
		int32_t w, h;
	} any;
	
	struct
	{
		uint8_t type;
		int32_t x, y;
		int32_t w, h;
		char const *text;
	} label;
	
	struct
	{
		uint8_t type;
		int32_t x, y;
		int32_t w, h;
		char const *text;
	} button;
} tgl_uielem_t;

typedef struct tgl_ui
{
	tgl_uielem_t *elems;
	size_t nelems, elemcap;
	int32_t x, y;
	TTF_Font *font;
	SDL_Window const *wnd;
} tgl_ui_t;

//-----------------------//
// library configuration //
//-----------------------//

extern tgl_conf_t tgl_conf; // should be set before library use.

//------------//
// procedures //
//------------//

// input.
void tgl_handleinput(SDL_Event const *e);
void tgl_prepareinput(void);
bool tgl_kdown(SDL_Keycode k);
bool tgl_kpressed(SDL_Keycode k);
bool tgl_kreleased(SDL_Keycode k);
void tgl_mpos(SDL_Window const *wnd, int32_t *outx, int32_t *outy);
bool tgl_mdown(int32_t btn);
bool tgl_mpressed(int32_t btn);
bool tgl_mreleased(int32_t btn);

// ui.
tgl_ui_t tgl_beginui(tgl_uielem_t elems[], size_t elemcap, int32_t x, int32_t y, TTF_Font *font, SDL_Window const *wnd);
void tgl_renderui(tgl_ui_t const *u);
void tgl_uipad(tgl_ui_t *u, int32_t dx, int32_t dy);
void tgl_uilabel(tgl_ui_t *u, char const *text);
bool tgl_uibutton(tgl_ui_t *u, char const *text);

// util.
void tgl_err(char const *fmt, ...);
uint64_t tgl_unixus(void);
void tgl_begintick(void);
void tgl_endtick(void);
void tgl_begintimer(uint64_t *outtimer);
void tgl_endtimer(uint64_t timer, char const *name);
float tgl_interpangle(float a, float b, float t);
float tgl_shortestangle(float a, float b);
float tgl_r2d(float r);
float tgl_d2r(float d);

#endif
#ifdef TGL_IMPLEMENTATION
#ifndef TGL_IMPLINCLUDED
#define TGL_IMPLINCLUDED
// SPDX-License-Identifier: BSD-3-Clause

// standard library.
#include <limits.h>
#include <stdarg.h>
#include <string.h>
#include <time.h>

// system dependencies.
#include <sys/time.h>

tgl_conf_t tgl_conf;
// SPDX-License-Identifier: BSD-3-Clause

// assume that 1024 represents a big enough number of keystates.
static uint8_t tgl_kdownstates[1024 / 8];
static uint8_t tgl_kpressstates[1024 / 8], tgl_kreleasestates[1024 / 8];

// SDL2 supports 5 mouse buttons.
static bool tgl_mdownstates[5];
static bool tgl_mpressstates[5], tgl_mreleasestates[5];

void
tgl_handleinput(SDL_Event const *e)
{
	if (e->type == SDL_KEYDOWN || e->type == SDL_KEYUP)
	{
		if (e->key.repeat)
		{
			return;
		}
		
		bool state = e->type == SDL_KEYDOWN;
		
		SDL_Keycode k = e->key.keysym.sym;
		if (k & 1 << 30)
		{
			k &= ~(1 << 30);
			k += 128;
		}
		
		size_t byte = k / 8, bit = k % 8;
		
		if (state)
		{
			tgl_kdownstates[byte] |= 1 << bit;
			tgl_kpressstates[byte] |= 1 << bit;
		}
		else
		{
			tgl_kdownstates[byte] &= ~(1 << bit);
			tgl_kreleasestates[byte] |= 1 << bit;
		}
	}
	else if (e->type == SDL_MOUSEBUTTONDOWN || e->type == SDL_MOUSEBUTTONUP)
	{
		bool state = e->type == SDL_MOUSEBUTTONDOWN;
		
		if (state)
		{
			tgl_mdownstates[e->button.button] = true;
			tgl_mpressstates[e->button.button] = true;
		}
		else
		{
			tgl_mdownstates[e->button.button] = false;
			tgl_mreleasestates[e->button.button] = true;
		}
	}
}

void
tgl_prepareinput(void)
{
	memset(tgl_kpressstates, 0, sizeof(tgl_kpressstates));
	memset(tgl_kreleasestates, 0, sizeof(tgl_kreleasestates));
	memset(tgl_mpressstates, 0, sizeof(tgl_mpressstates));
	memset(tgl_mreleasestates, 0, sizeof(tgl_mreleasestates));
}

bool
tgl_kdown(SDL_Keycode k)
{
	if (k & 1 << 30)
	{
		k &= ~(1 << 30);
		k += 128;
	}
	size_t byte = k / 8, bit = k % 8;
	return tgl_kdownstates[byte] & 1 << bit;
}

bool
tgl_kpressed(SDL_Keycode k)
{
	if (k & 1 << 30)
	{
		k &= ~(1 << 30);
		k += 128;
	}
	size_t byte = k / 8, bit = k % 8;
	return tgl_kpressstates[byte] & 1 << bit;
}

bool
tgl_kreleased(SDL_Keycode k)
{
	if (k & 1 << 30)
	{
		k &= ~(1 << 30);
		k += 128;
	}
	size_t byte = k / 8, bit = k % 8;
	return tgl_kreleasestates[byte] & 1 << bit;
}

void
tgl_mpos(SDL_Window const *wnd, int32_t *outx, int32_t *outy)
{
	if (SDL_GetMouseFocus() != wnd)
	{
		*outx = 0;
		*outy = 0;
		return;
	}
	
	SDL_GetMouseState(outx, outy);
}

bool
tgl_mdown(int32_t btn)
{
	return tgl_mdownstates[btn];
}

bool
tgl_mpressed(int32_t btn)
{
	return tgl_mpressstates[btn];
}

bool
tgl_mreleased(int32_t btn)
{
	return tgl_mreleasestates[btn];
}
// SPDX-License-Identifier: BSD-3-Clause

tgl_ui_t
tgl_beginui(
	tgl_uielem_t elems[],
	size_t elemcap,
	int32_t x,
	int32_t y,
	TTF_Font *font,
	SDL_Window const *wnd
)
{
	return (tgl_ui_t)
	{
		.elems = elems,
		.elemcap = elemcap,
		.x = x,
		.y = y,
		.font = font,
		.wnd = wnd
	};
}

void
tgl_renderui(tgl_ui_t const *u)
{
	if (!u->nelems)
	{
		return;
	}
	
	// find render boundaries to draw panel.
	int32_t minx = INT32_MAX, miny = INT32_MAX;
	int32_t maxx = INT32_MIN, maxy = INT32_MIN;
	for (size_t i = 0; i < u->nelems; ++i)
	{
		int32_t x = u->elems[i].any.x, y = u->elems[i].any.y;
		int32_t w = u->elems[i].any.w, h = u->elems[i].any.h;
		
		minx = x < minx ? x : minx;
		miny = y < miny ? y : miny;
		maxx = x + w > maxx ? x + w : maxx;
		maxy = y + h > maxy ? y + h : maxy;
	}
	
	int32_t pad = tgl_conf.uipad;
	
	// draw panel.
	tgl_conf.renderrect(
		minx - pad,
		miny - pad,
		maxx - minx + 2 * pad,
		maxy - miny + 2 * pad,
		TGL_PANELCOLOR
	);
	
	// draw UI elements.
	int32_t mx, my;
	tgl_mpos(u->wnd, &mx, &my);
	for (size_t i = 0; i < u->nelems; ++i)
	{
		int32_t x = u->elems[i].any.x, y = u->elems[i].any.y;
		int32_t w = u->elems[i].any.w, h = u->elems[i].any.h;
		
		if (u->elems[i].any.type == TGL_LABEL)
		{
			tgl_conf.rendertext(
				x,
				y,
				w,
				h,
				u->elems[i].button.text,
				TGL_LABELTEXTCOLOR
			);
		}
		else // button.
		{
			tgl_color_t bcol = TGL_BUTTONCOLOR, tcol = TGL_BUTTONTEXTCOLOR;
			if (mx >= x && my >= y && mx < x + w && my < y + h)
			{
				if (tgl_mdown(SDL_BUTTON_LEFT))
				{
					bcol = TGL_BUTTONPRESSCOLOR;
					tcol = TGL_BUTTONTEXTPRESSCOLOR;
				}
				else
				{
					bcol = TGL_BUTTONHOVERCOLOR;
					tcol = TGL_BUTTONTEXTPRESSCOLOR;
				}
			}
			
			tgl_conf.renderrect(x, y, w, h, bcol);
			tgl_conf.rendertext(
				x + pad,
				y + pad,
				w - 2 * pad,
				h - 2 * pad,
				u->elems[i].button.text,
				tcol
			);
		}
	}
}

void
tgl_uipad(tgl_ui_t *u, int32_t dx, int32_t dy)
{
	u->x += dx;
	u->y += dy;
}

void
tgl_uilabel(tgl_ui_t *u, char const *text)
{
	if (u->nelems >= u->elemcap)
	{
		return;
	}
	
	int32_t w, h;
	TTF_SizeText(u->font, text, &w, &h);
	
	u->elems[u->nelems++] = (tgl_uielem_t)
	{
		.label =
		{
			.type = TGL_LABEL,
			.x = u->x,
			.y = u->y,
			.w = w,
			.h = h,
			.text = text
		}
	};
	u->y += h;
}

bool
tgl_uibutton(tgl_ui_t *u, char const *text)
{
	if (u->nelems >= u->elemcap)
	{
		return false;
	}
	
	bool state = false;
	
	int32_t w, h;
	TTF_SizeText(u->font, text, &w, &h);
	w += 2 * tgl_conf.uipad;
	h += 2 * tgl_conf.uipad;
	
	int32_t mx, my;
	tgl_mpos(u->wnd, &mx, &my);
	
	if (tgl_mreleased(SDL_BUTTON_LEFT)
		&& mx >= u->x
		&& my >= u->y
		&& mx < u->x + w
		&& my < u->y + h)
	{
		state = true;
	}
	
	u->elems[u->nelems++] = (tgl_uielem_t)
	{
		.button =
		{
			.type = TGL_BUTTON,
			.x = u->x,
			.y = u->y,
			.w = w,
			.h = h,
			.text = text
		}
	};
	u->y += h;
	
	return state;
}
// SPDX-License-Identifier: BSD-3-Clause

static uint64_t tgl_tickstart;

void
tgl_err(char const *fmt, ...)
{
	va_list args;
	va_start(args, fmt);
	
	char msg[512];
	vsnprintf(msg, sizeof(msg), fmt, args);
	
	if (SDL_ShowSimpleMessageBox(SDL_MESSAGEBOX_ERROR, tgl_conf.errtitle, msg, NULL))
	{
		fprintf(tgl_conf.log, "\x1b[1;31merr\x1b[0m: %s\n", msg);
	}
	
	va_end(args);
}

uint64_t
tgl_unixus(void)
{
	struct timeval tv;
	gettimeofday(&tv, NULL);
	return (uint64_t)tv.tv_sec * 1000000 + (uint64_t)tv.tv_usec;
}

void
tgl_begintick(void)
{
	tgl_tickstart = tgl_unixus();
}

void
tgl_endtick(void)
{
	uint64_t tickend = tgl_unixus();
	int64_t timeleft = tgl_conf.tickus - tickend + tgl_tickstart;
	timeleft *= (timeleft > 0);
	
	struct timespec ts =
	{
		.tv_nsec = 1000 * timeleft
	};
	nanosleep(&ts, NULL);
}

void
tgl_begintimer(uint64_t *outtimer)
{
	*outtimer = tgl_unixus();
}

void
tgl_endtimer(uint64_t timer, char const *name)
{
	uint64_t d = tgl_unixus() - timer;
	fprintf(
		tgl_conf.log,
		"\x1b[1;33mprofile\x1b[0m: %s: %llu\n",
		name,
		(unsigned long long)d
	);
}

float
tgl_interpangle(float a, float b, float t)
{
	return a + tgl_shortestangle(a, b) * t;
}

float
tgl_shortestangle(float a, float b)
{
	float d = fmod(b - a, TGL_TAU);
	float shortest = fmod(2.0f * d, TGL_TAU) - d;
	return shortest;
}

float
tgl_r2d(float r)
{
	return r / TGL_PI * 180.0f;
}

float
tgl_d2r(float d)
{
	return d / 180.0f * TGL_PI;
}
#endif
#endif
