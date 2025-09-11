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
#define TGL_VERMINOR 1
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

// library data constants.
#define TGL_MAXRESNAME 15
#define TGL_BATCHALIGN 16
#define TGL_MAXOPTIONKEY 127
#define TGL_MAXOPTIONVALUE 127
#define TGL_OPTIONSCAN "%127s = %127[^\r\n]"

// clarity.
#define TGL_OUT
#define TGL_INOUT

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
	TGL_BUTTONTEXTHOVERCOLOR,
	TGL_SLIDERCOLOR,
	TGL_SLIDERPRESSCOLOR,
	TGL_SLIDERHOVERCOLOR,
	TGL_SLIDERBARCOLOR,
	TGL_SLIDERBARPRESSCOLOR,
	TGL_SLIDERBARHOVERCOLOR,
	TGL_SLIDERTEXTCOLOR,
	TGL_SLIDERTEXTPRESSCOLOR,
	TGL_SLIDERTEXTHOVERCOLOR,
	TGL_TEXTFIELDCOLOR,
	TGL_TEXTFIELDPRESSCOLOR,
	TGL_TEXTFIELDHOVERCOLOR,
	TGL_TEXTFIELDTEXTCOLOR,
	TGL_TEXTFIELDTEXTPRESSCOLOR,
	TGL_TEXTFIELDTEXTHOVERCOLOR,
	TGL_TEXTFIELDBARCOLOR,
	TGL_TEXTFIELDBARPRESSCOLOR,
	TGL_TEXTFIELDBARHOVERCOLOR,
	TGL_TEXTFIELDPROMPTCOLOR,
	TGL_TEXTFIELDPROMPTPRESSCOLOR,
	TGL_TEXTFIELDPROMPTHOVERCOLOR
} tgl_color_t;

typedef enum tgl_uitype
{
	TGL_LABEL = 0,
	TGL_BUTTON,
	TGL_SLIDER,
	TGL_TEXTFIELD
} tgl_uitype_t;

typedef enum tgl_restype
{
	TGL_IMAGEFILE = 0,
	TGL_SOUNDFILE,
	TGL_MODELFILE,
	TGL_FONTFILE,
	TGL_MAPFILE
} tgl_restype_t;

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
	int32_t uitextfieldbar;
	
	// rendering call config.
	void (*renderrect)(int32_t, int32_t, int32_t, int32_t, tgl_color_t);
	void (*rendertext)(int32_t, int32_t, int32_t, int32_t, char const *, tgl_color_t);
} tgl_conf_t;

typedef struct tgl_tfdata
{
	char *buf;
	uint32_t len, cap;
	uint32_t csr, first;
	bool sel;
} tgl_tfdata_t;

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
	
	struct
	{
		uint8_t type;
		int32_t x, y;
		int32_t w, h;
		char const *text;
		float val;
	} slider;
	
	struct
	{
		uint8_t type;
		int32_t x, y;
		int32_t w, h;
		char const *text;
		tgl_tfdata_t const *tfdata;
		uint32_t ndraw;
	} textfield;
} tgl_uielem_t;

typedef struct tgl_ui
{
	tgl_uielem_t *elems;
	size_t nelems, elemcap;
	int32_t x, y;
	TTF_Font *font;
	SDL_Window const *wnd;
} tgl_ui_t;

typedef struct tgl_res
{
	uint8_t type;
	char name[TGL_MAXRESNAME];
	uint32_t flags;
	uint32_t size;
	uint8_t *data; // invalidated upon pack update.
} tgl_res_t;

typedef struct tgl_pack
{
	void *buf;
	size_t size;
	uint8_t *data;
	size_t datasize;
	tgl_res_t *images, *sounds, *models, *fonts, *maps, *others;
	uint32_t nimages, nsounds, nmodels, nfonts, nmaps, nothers;
} tgl_pack_t;

typedef struct tgl_allocbatch
{
	void **ptr;
	size_t n, size;
} tgl_allocbatch_t;

typedef struct tgl_reallocbatch
{
	void **ptr;
	size_t nold, nnew, size;
} tgl_reallocbatch_t;

//-----------------------//
// library configuration //
//-----------------------//

extern tgl_conf_t tgl_conf; // should be set before library use.

//-------------//
// data tables //
//-------------//

extern uint8_t tgl_defaultcolors[][4];

//------------//
// procedures //
//------------//

// input.
void tgl_handleinput(SDL_Event const *e);
void tgl_prepareinput(void);
bool tgl_kdown(SDL_Keycode k);
bool tgl_kpressed(SDL_Keycode k);
bool tgl_kreleased(SDL_Keycode k);
void tgl_mpos(SDL_Window const *wnd, TGL_OUT int32_t *x, TGL_OUT int32_t *y);
bool tgl_mdown(int32_t btn);
bool tgl_mpressed(int32_t btn);
bool tgl_mreleased(int32_t btn);
bool tgl_textinput(char ch);

// options.
int32_t tgl_optraw(TGL_OUT char buf[], FILE *fp, char const *key);
int32_t tgl_optkeycode(TGL_OUT SDL_Keycode *k, FILE *fp, char const *key);
int32_t tgl_optfloat(TGL_OUT double *f, FILE *fp, char const *key);
int32_t tgl_optint(TGL_OUT int64_t *i, FILE *fp, char const *key);
int32_t tgl_optbool(TGL_OUT bool *b, FILE *fp, char const *key);

// pack.
int32_t tgl_readpack(TGL_OUT tgl_pack_t *p, uint8_t *pack, size_t size);
int32_t tgl_readpackfile(TGL_OUT tgl_pack_t *p, FILE *fp);
void tgl_writepack(TGL_OUT uint8_t buf[], TGL_OUT size_t *size, tgl_pack_t const *p);
int32_t tgl_writepackfile(FILE *fp, tgl_pack_t const *p);
void tgl_destroypack(tgl_pack_t *p);
int32_t tgl_packadd(tgl_pack_t *p, tgl_res_t const *r);
int32_t tgl_packinsert(tgl_pack_t *p, tgl_res_t const *r, uint32_t idx);
int32_t tgl_packrm(tgl_pack_t *p, tgl_restype_t type, uint32_t idx);
bool tgl_packfind(TGL_OUT uint32_t *idx, tgl_pack_t const *p, tgl_restype_t type, char const *name);
void tgl_packranges(tgl_pack_t *p);

// ui.
tgl_ui_t tgl_beginui(tgl_uielem_t elems[], size_t elemcap, int32_t x, int32_t y, TTF_Font *font, SDL_Window const *wnd);
void tgl_renderui(tgl_ui_t const *u);
void tgl_uipad(tgl_ui_t *u, int32_t dx, int32_t dy);
void tgl_uilabel(tgl_ui_t *u, char const *text);
bool tgl_uibutton(tgl_ui_t *u, char const *text);
bool tgl_uislider(tgl_ui_t *u, char const *text, TGL_INOUT float *val);
void tgl_uitextfield(tgl_ui_t *u, char const *text, TGL_INOUT tgl_tfdata_t *tfdata, uint32_t ndraw);

// util.
void tgl_err(char const *fmt, ...);
uint64_t tgl_unixus(void);
void tgl_begintick(void);
void tgl_endtick(void);
void tgl_begintimer(TGL_OUT uint64_t *timer);
void tgl_endtimer(uint64_t timer, char const *name);
float tgl_interpangle(float a, float b, float t);
float tgl_shortestangle(float a, float b);
float tgl_r2d(float r);
float tgl_d2r(float d);
void *tgl_allocbatch(TGL_INOUT tgl_allocbatch_t *allocs, size_t nallocs);
void *tgl_reallocbatch(TGL_INOUT tgl_reallocbatch_t *reallocs, size_t nreallocs);

#endif
#ifdef TGL_IMPLEMENTATION
#ifndef TGL_IMPLINCLUDED
#define TGL_IMPLINCLUDED
// SPDX-License-Identifier: BSD-3-Clause

// standard library.
#include <ctype.h>
#include <limits.h>
#include <stdarg.h>
#include <string.h>
#include <time.h>

// system dependencies.
#include <sys/time.h>

tgl_conf_t tgl_conf;

uint8_t tgl_defaultcolors[][4] =
{
	{0, 0, 0, 128}, // panel.
	{255, 255, 255, 255}, // label text.
	{255, 255, 255, 128}, // button.
	{0, 0, 0, 255}, // button press.
	{128, 128, 128, 255}, // button hover.
	{255, 255, 255, 255}, // button text.
	{255, 255, 255, 255}, // button press text.
	{255, 255, 255, 255}, // button hover text.
	{0, 0, 0, 128}, // slider.
	{0, 0, 0, 255}, // slider press.
	{0, 0, 0, 255}, // slider hover.
	{128, 128, 128, 255}, // slider bar.
	{128, 128, 128, 255}, // slider press bar.
	{128, 128, 128, 255}, // slider hover bar.
	{255, 255, 255, 255}, // slider text.
	{255, 255, 255, 255}, // slider press text.
	{255, 255, 255, 255}, // slider hover text.
	{0, 0, 0, 128}, // textfield.
	{0, 0, 0, 255}, // textfield press.
	{0, 0, 0, 255}, // textfield hover.
	{255, 255, 255, 255}, // textfield text.
	{255, 255, 255, 255}, // textfield press text.
	{255, 255, 255, 255}, // textfield hover text.
	{255, 255, 255, 255}, // textfield bar.
	{255, 255, 255, 255}, // textfield press bar.
	{255, 255, 255, 255}, // textfield hover bar.
	{128, 128, 128, 255}, // textfield prompt.
	{128, 128, 128, 255}, // textfield press prompt.
	{128, 128, 128, 255} // textfield hover prompt.
};
// SPDX-License-Identifier: BSD-3-Clause

// assume that 1024 represents a big enough number of keystates.
static uint8_t tgl_kdownstates[1024 / 8];
static uint8_t tgl_kpressstates[1024 / 8], tgl_kreleasestates[1024 / 8];

// SDL2 supports 5 mouse buttons.
static bool tgl_mdownstates[5];
static bool tgl_mpressstates[5], tgl_mreleasestates[5];

// TGL supports ASCII text input.
static uint8_t tgl_textinputstates[128 / 8];

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
	else if (e->type == SDL_TEXTINPUT)
	{
		uint8_t ch = e->text.text[0];
		
		// disregard non-ASCII input.
		if (ch & 0x80)
		{
			return;
		}
		
		size_t byte = ch / 8, bit = ch % 8;
		tgl_textinputstates[byte] |= 1 << bit;
	}
}

void
tgl_prepareinput(void)
{
	memset(tgl_kpressstates, 0, sizeof(tgl_kpressstates));
	memset(tgl_kreleasestates, 0, sizeof(tgl_kreleasestates));
	memset(tgl_mpressstates, 0, sizeof(tgl_mpressstates));
	memset(tgl_mreleasestates, 0, sizeof(tgl_mreleasestates));
	memset(tgl_textinputstates, 0, sizeof(tgl_textinputstates));
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
tgl_mpos(SDL_Window const *wnd, TGL_OUT int32_t *x, TGL_OUT int32_t *y)
{
	if (SDL_GetMouseFocus() != wnd)
	{
		*x = 0;
		*y = 0;
		return;
	}
	
	SDL_GetMouseState(x, y);
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

bool
tgl_textinput(char ch)
{
	size_t byte = ch / 8, bit = ch % 8;
	return tgl_textinputstates[byte] & 1 << bit;
}
// SPDX-License-Identifier: BSD-3-Clause

int32_t
tgl_optraw(TGL_OUT char buf[], FILE *fp, char const *key)
{
	// TODO: implement.
}

int32_t
tgl_optkeycode(TGL_OUT SDL_Keycode *k, FILE *fp, char const *key)
{
	// TODO: implement.
}

int32_t
tgl_optfloat(TGL_OUT double *f, FILE *fp, char const *key)
{
	// TODO: implement.
}

int32_t
tgl_optint(TGL_OUT int64_t *i, FILE *fp, char const *key)
{
	// TODO: implement.
}

int32_t
tgl_optbool(TGL_OUT bool *b, FILE *fp, char const *key)
{
	// TODO: implement.
}
// SPDX-License-Identifier: BSD-3-Clause

int32_t
tgl_readpack(TGL_OUT tgl_pack_t *p, uint8_t *pack, size_t size)
{
	// TODO: implement.
}

int32_t
tgl_readpackfile(TGL_OUT tgl_pack_t *p, FILE *fp)
{
	// TODO: implement.
}

void
tgl_writepack(TGL_OUT uint8_t buf[], TGL_OUT size_t *size, tgl_pack_t const *p)
{
	// TODO: implement.
}

int32_t
tgl_writepackfile(FILE *fp, tgl_pack_t const *p)
{
	// TODO: implement.
}

void
tgl_destroypack(tgl_pack_t *p)
{
	// TODO: implement.
}

int32_t
tgl_packadd(tgl_pack_t *p, tgl_res_t const *r)
{
	// TODO: implement.
}

int32_t
tgl_packinsert(tgl_pack_t *p, tgl_res_t const *r, uint32_t idx)
{
	// TODO: implement.
}

int32_t
tgl_packrm(tgl_pack_t *p, tgl_restype_t type, uint32_t idx)
{
	// TODO: implement.
}

bool
tgl_packfind(
	TGL_OUT uint32_t *idx,
	tgl_pack_t const *p,
	tgl_restype_t type,
	char const *name
)
{
	// TODO: implement.
}

void
tgl_packranges(tgl_pack_t *p)
{
	// TODO: implement.
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
		else if (u->elems[i].any.type == TGL_BUTTON)
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
		else if (u->elems[i].any.type == TGL_SLIDER)
		{
			tgl_color_t scol = TGL_SLIDERCOLOR;
			tgl_color_t tcol = TGL_SLIDERTEXTCOLOR;
			tgl_color_t bcol = TGL_SLIDERBARCOLOR;
			if (mx >= x && my >= y && mx < x + w && my < y + h)
			{
				if (tgl_mdown(SDL_BUTTON_LEFT))
				{
					scol = TGL_SLIDERPRESSCOLOR;
					tcol = TGL_SLIDERTEXTPRESSCOLOR;
					bcol = TGL_SLIDERBARPRESSCOLOR;
				}
				else
				{
					scol = TGL_SLIDERHOVERCOLOR;
					tcol = TGL_SLIDERTEXTPRESSCOLOR;
					bcol = TGL_SLIDERBARHOVERCOLOR;
				}
			}
			
			tgl_conf.renderrect(x, y, w, h, scol);
			tgl_conf.renderrect(x, y, u->elems[i].slider.val * w, h, bcol);
			tgl_conf.rendertext(
				x + pad,
				y + pad,
				w - 2 * pad,
				h - 2 * pad,
				u->elems[i].slider.text,
				tcol
			);
		}
		else // textfield.
		{
			tgl_color_t tfcol = TGL_TEXTFIELDCOLOR;
			tgl_color_t tftcol = TGL_TEXTFIELDTEXTCOLOR;
			tgl_color_t tfbcol = TGL_TEXTFIELDBARCOLOR;
			tgl_color_t tfpcol = TGL_TEXTFIELDPROMPTCOLOR;
			if (mx >= x && my >= y && mx < x + w && my < y + h)
			{
				if (tgl_mdown(SDL_BUTTON_LEFT))
				{
					tfcol = TGL_TEXTFIELDPRESSCOLOR;
					tftcol = TGL_TEXTFIELDTEXTPRESSCOLOR;
					tfbcol = TGL_TEXTFIELDBARPRESSCOLOR;
					tfpcol = TGL_TEXTFIELDPROMPTPRESSCOLOR;
				}
				else
				{
					tfcol = TGL_TEXTFIELDHOVERCOLOR;
					tftcol = TGL_TEXTFIELDTEXTHOVERCOLOR;
					tfbcol = TGL_TEXTFIELDBARHOVERCOLOR;
					tfpcol = TGL_TEXTFIELDPROMPTHOVERCOLOR;
				}
			}
			
			tgl_conf.renderrect(x, y, w, h, tfcol);
			
			tgl_tfdata_t const *t = u->elems[i].textfield.tfdata;
			int32_t chw = (w - 2 * pad) / u->elems[i].textfield.ndraw;
			int32_t chh = h - 2 * pad;
			
			char const *text = t->len ? t->buf : u->elems[i].textfield.text;
			uint32_t textfirst = t->len ? t->first : 0;
			uint32_t textlen = t->len ? t->len : strlen(text);
			tgl_color_t textcol = t->len ? tftcol : tfpcol;
			
			int32_t dx = 0;
			for (uint32_t j = textfirst; j < textlen; ++j)
			{
				if (dx >= w - 2 * pad)
				{
					break;
				}
				char render[] = {text[j], 0};
				tgl_conf.rendertext(x + pad + dx, y + pad, chw, chh, render, textcol);
				dx += chw;
			}
			
			if (t->sel)
			{
				tgl_conf.renderrect(
					x + pad + (t->csr - t->first) * chw,
					y + pad,
					tgl_conf.uitextfieldbar,
					chh,
					tfbcol
				);
			}
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

bool
tgl_uislider(tgl_ui_t *u, char const *text, TGL_INOUT float *val)
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
		*val = (double)(mx - u->x) / w;
		state = true;
	}
	
	*val = *val < 0.0f ? 0.0f : *val;
	*val = *val > 1.0f ? 1.0f : *val;
	
	u->elems[u->nelems++] = (tgl_uielem_t)
	{
		.slider =
		{
			.type = TGL_SLIDER,
			.x = u->x,
			.y = u->y,
			.w = w,
			.h = h,
			.text = text,
			.val = *val
		}
	};
	u->y += h;
	
	return state;
}

// function assumes that the user is rendering UI with a monospacing font.
void
tgl_uitextfield(
	tgl_ui_t *u,
	char const *text,
	TGL_INOUT tgl_tfdata_t *tfdata,
	uint32_t ndraw
)
{
	if (u->nelems >= u->elemcap)
	{
		return;
	}
	
	int32_t chw, chh;
	TTF_SizeText(u->font, " ", &chw, &chh);
	
	int32_t w = ndraw * chw + 2 * tgl_conf.uipad;
	int32_t h = chh + 2 * tgl_conf.uipad;
	
	if (tgl_mreleased(SDL_BUTTON_LEFT))
	{
		int32_t mx, my;
		tgl_mpos(u->wnd, &mx, &my);
		
		if (mx >= u->x && my >= u->y && mx < u->x + w && my < u->y + h)
		{
			tfdata->sel = true;
		}
		else
		{
			tfdata->sel = false;
		}
	}
	
	if (tfdata->sel)
	{
		if (tgl_kpressed(SDLK_LEFT))
		{
			tfdata->csr -= tfdata->csr > 0;
			tfdata->first -= tfdata->csr < tfdata->first;
		}
		
		if (tgl_kpressed(SDLK_RIGHT))
		{
			tfdata->csr += tfdata->csr < tfdata->len;
			tfdata->first += tfdata->csr - tfdata->first >= ndraw;
		}
		
		if (tgl_kpressed(SDLK_UP))
		{
			tfdata->csr = 0;
			tfdata->first = 0;
		}
		
		if (tgl_kpressed(SDLK_DOWN))
		{
			tfdata->csr = tfdata->len;
			tfdata->first = 0;
			while (tfdata->csr - tfdata->first > ndraw)
			{
				++tfdata->first;
			}
		}
		
		for (uint8_t i = 0; i < 128; ++i)
		{
			if (tfdata->len >= tfdata->cap + 1)
			{
				break;
			}
			
			if (!isprint(i))
			{
				continue;
			}
			
			if (tgl_textinput(i))
			{
				memmove(
					&tfdata->buf[tfdata->csr + 1],
					&tfdata->buf[tfdata->csr],
					tfdata->len - tfdata->csr
				);
				
				++tfdata->csr;
				tfdata->first += tfdata->csr - tfdata->first >= ndraw;
				tfdata->buf[tfdata->csr - 1] = i;
				++tfdata->len;
				tfdata->buf[tfdata->len] = 0;
			}
		}
		
		if (tgl_kpressed(SDLK_BACKSPACE) && tfdata->csr)
		{
			memmove(
				&tfdata->buf[tfdata->csr - 1],
				&tfdata->buf[tfdata->csr],
				tfdata->len - tfdata->csr
			);
			
			--tfdata->csr;
			tfdata->first -= tfdata->csr < tfdata->first;
			
			--tfdata->len;
			tfdata->buf[tfdata->len] = 0;
		}
	}
	
	u->elems[u->nelems++] = (tgl_uielem_t)
	{
		.textfield =
		{
			.type = TGL_TEXTFIELD,
			.x = u->x,
			.y = u->y,
			.w = w,
			.h = h,
			.text = text,
			.tfdata = tfdata,
			.ndraw = ndraw
		}
	};
	u->y += h;
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
tgl_begintimer(TGL_OUT uint64_t *timer)
{
	*timer = tgl_unixus();
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

void *
tgl_allocbatch(TGL_INOUT tgl_allocbatch_t *allocs, size_t nallocs)
{
	// TODO: implement.
}

void *
tgl_reallocbatch(TGL_INOUT tgl_reallocbatch_t *reallocs, size_t nreallocs)
{
	// TODO: implement.
}
#endif
#endif
