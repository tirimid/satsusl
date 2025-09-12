// SPDX-License-Identifier: BSD-3-Clause

#ifndef Z_ZTGL_H
#define Z_ZTGL_H

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

#define Z_VERMAJOR 1
#define Z_VERMINOR 2
#define Z_VERPATCH 0

//--------//
// macros //
//--------//

#define OUT
#define INOUT

// profiling.
#ifdef Z_PROFILE
#define Z_NEWTIMER(timer) u64 timer
#define Z_BEGINTIMER(timer) z_begintimer(name)
#define Z_ENDTIMER(timer, name) z_endtimer(timer, name)
#else
#define Z_NEWTIMER(timer)
#define Z_BEGINTIMER(timer)
#define Z_ENDTIMER(timer, name)
#endif

// util constants.
#define Z_PI 3.141592f
#define Z_TAU (2.0f * Z_PI)

// library data constants.
#define Z_MAXRESNAME 15
#define Z_BATCHALIGN 16
#define Z_MAXOPTIONKEY 127
#define Z_MAXOPTIONVALUE 127
#define Z_OPTIONSCAN "%127s = %127[^\r\n]"

//--------------//
// type aliases //
//--------------//

#ifndef Z_NOALIAS

typedef int8_t i8;
typedef int16_t i16;
typedef int32_t i32;
typedef int64_t i64;

typedef uint8_t u8;
typedef uint16_t u16;
typedef uint32_t u32;
typedef uint64_t u64;
typedef size_t usize;

typedef float f32;
typedef double f64;

#endif

//--------------------//
// enumeration values //
//--------------------//

typedef enum z_color
{
	Z_PANELCOLOR = 0,
	Z_LABELTEXTCOLOR,
	Z_BUTTONCOLOR,
	Z_BUTTONPRESSCOLOR,
	Z_BUTTONHOVERCOLOR,
	Z_BUTTONTEXTCOLOR,
	Z_BUTTONTEXTPRESSCOLOR,
	Z_BUTTONTEXTHOVERCOLOR,
	Z_SLIDERCOLOR,
	Z_SLIDERPRESSCOLOR,
	Z_SLIDERHOVERCOLOR,
	Z_SLIDERBARCOLOR,
	Z_SLIDERBARPRESSCOLOR,
	Z_SLIDERBARHOVERCOLOR,
	Z_SLIDERTEXTCOLOR,
	Z_SLIDERTEXTPRESSCOLOR,
	Z_SLIDERTEXTHOVERCOLOR,
	Z_TEXTFIELDCOLOR,
	Z_TEXTFIELDPRESSCOLOR,
	Z_TEXTFIELDHOVERCOLOR,
	Z_TEXTFIELDTEXTCOLOR,
	Z_TEXTFIELDTEXTPRESSCOLOR,
	Z_TEXTFIELDTEXTHOVERCOLOR,
	Z_TEXTFIELDBARCOLOR,
	Z_TEXTFIELDBARPRESSCOLOR,
	Z_TEXTFIELDBARHOVERCOLOR,
	Z_TEXTFIELDPROMPTCOLOR,
	Z_TEXTFIELDPROMPTPRESSCOLOR,
	Z_TEXTFIELDPROMPTHOVERCOLOR
} z_color_t;

typedef enum z_uitype
{
	Z_LABEL = 0,
	Z_BUTTON,
	Z_SLIDER,
	Z_TEXTFIELD
} z_uitype_t;

typedef enum z_restype
{
	Z_IMAGEFILE = 0,
	Z_SOUNDFILE,
	Z_MODELFILE,
	Z_FONTFILE,
	Z_MAPFILE
} z_restype_t;

//-----------------//
// data structures //
//-----------------//

typedef struct z_conf
{
	// library function config.
	FILE *log;
	char const *errtitle;
	u64 tickus;
	i32 uipad;
	i32 uitextfieldbar;
	
	// rendering call config.
	void (*renderrect)(i32, i32, i32, i32, z_color_t);
	void (*rendertext)(i32, i32, i32, i32, char const *, z_color_t);
} z_conf_t;

typedef struct z_tfdata
{
	char *buf;
	u32 len, cap;
	u32 csr, first;
	bool sel;
} z_tfdata_t;

typedef union z_uielem
{
	// all UI elements have at least these properties.
	struct
	{
		u8 type;
		i32 x, y;
		i32 w, h;
	} any;
	
	struct
	{
		u8 type;
		i32 x, y;
		i32 w, h;
		char const *text;
	} label;
	
	struct
	{
		u8 type;
		i32 x, y;
		i32 w, h;
		char const *text;
	} button;
	
	struct
	{
		u8 type;
		i32 x, y;
		i32 w, h;
		f32 val;
		char const *text;
	} slider;
	
	struct
	{
		u8 type;
		i32 x, y;
		i32 w, h;
		u32 ndraw;
		char const *text;
		z_tfdata_t const *tfdata;
	} textfield;
} z_uielem_t;

typedef struct z_ui
{
	z_uielem_t *elems;
	usize nelems, elemcap;
	i32 x, y;
	TTF_Font *font;
	SDL_Window const *wnd;
} z_ui_t;

typedef struct z_res
{
	u8 type;
	char name[Z_MAXRESNAME];
	u32 flags;
	u32 size;
	u8 *data; // invalidated upon pack update.
} z_res_t;

typedef struct z_pack
{
	void *buf;
	usize size;
	u8 *data;
	usize datasize;
	z_res_t *images, *sounds, *models, *fonts, *maps, *others;
	u32 nimages, nsounds, nmodels, nfonts, nmaps, nothers;
} z_pack_t;

typedef struct z_allocbatch
{
	void **ptr;
	usize n, size;
} z_allocbatch_t;

typedef struct z_reallocbatch
{
	void **ptr;
	usize nold, nnew, size;
} z_reallocbatch_t;

//-----------------------//
// library configuration //
//-----------------------//

extern z_conf_t z_conf; // should be set before library use.

//-------------//
// data tables //
//-------------//

extern u8 z_defaultcolors[][4];

//------------//
// procedures //
//------------//

// input.
void z_handleinput(SDL_Event const *e);
void z_prepareinput(void);
bool z_kdown(SDL_Keycode k);
bool z_kpressed(SDL_Keycode k);
bool z_kreleased(SDL_Keycode k);
void z_mpos(SDL_Window const *wnd, OUT i32 *x, OUT i32 *y);
bool z_mdown(i32 btn);
bool z_mpressed(i32 btn);
bool z_mreleased(i32 btn);
bool z_textinput(char ch);

// options.
i32 z_optraw(OUT char buf[], FILE *fp, char const *key);
i32 z_optkeycode(OUT SDL_Keycode *k, FILE *fp, char const *key);
i32 z_optfloat(OUT f64 *f, FILE *fp, char const *key);
i32 z_optint(OUT i64 *i, FILE *fp, char const *key);
i32 z_optbool(OUT bool *b, FILE *fp, char const *key);

// pack.
i32 z_readpack(OUT z_pack_t *p, u8 *pack, usize size);
i32 z_readpackfile(OUT z_pack_t *p, FILE *fp);
void z_writepack(OUT u8 buf[], OUT usize *size, z_pack_t const *p);
i32 z_writepackfile(FILE *fp, z_pack_t const *p);
void z_destroypack(z_pack_t *p);
i32 z_packadd(z_pack_t *p, z_res_t const *r);
i32 z_packinsert(z_pack_t *p, z_res_t const *r, u32 idx);
i32 z_packrm(z_pack_t *p, z_restype_t type, u32 idx);
bool z_packfind(OUT u32 *idx, z_pack_t const *p, z_restype_t type, char const *name);
void z_packranges(z_pack_t *p);

// ui.
z_ui_t z_beginui(z_uielem_t elems[], usize elemcap, i32 x, i32 y, TTF_Font *font, SDL_Window const *wnd);
void z_renderui(z_ui_t const *u);
void z_uipad(z_ui_t *u, i32 dx, i32 dy);
void z_uilabel(z_ui_t *u, char const *text);
bool z_uibutton(z_ui_t *u, char const *text);
bool z_uislider(z_ui_t *u, char const *text, INOUT f32 *val);
void z_uitextfield(z_ui_t *u, char const *text, INOUT z_tfdata_t *tfdata, u32 ndraw);

// util.
void z_err(char const *fmt, ...);
u64 z_unixus(void);
void z_begintick(void);
void z_endtick(void);
void z_begintimer(OUT u64 *timer);
void z_endtimer(u64 timer, char const *name);
f32 z_interpangle(f32 a, f32 b, f32 t);
f32 z_shortestangle(f32 a, f32 b);
f32 z_r2d(f32 r);
f32 z_d2r(f32 d);
void *z_allocbatch(INOUT z_allocbatch_t *allocs, usize nallocs);
void *z_reallocbatch(INOUT z_reallocbatch_t *reallocs, usize nreallocs);

#endif
#ifdef Z_IMPLEMENTATION
#ifndef Z_IMPLINCLUDED
#define Z_IMPLINCLUDED
// SPDX-License-Identifier: BSD-3-Clause

// standard library.
#include <ctype.h>
#include <limits.h>
#include <stdarg.h>
#include <string.h>
#include <time.h>

// system dependencies.
#include <sys/time.h>

z_conf_t z_conf;

u8 z_defaultcolors[][4] =
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
static u8 z_kdownstates[1024 / 8];
static u8 z_kpressstates[1024 / 8], z_kreleasestates[1024 / 8];

// SDL2 supports 5 mouse buttons.
static u8 z_mdownstates;
static u8 z_mpressstates, z_mreleasestates;

// ZTGL supports ASCII text input.
static u8 z_textinputstates[128 / 8];

void
z_handleinput(SDL_Event const *e)
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
		
		usize byte = k / 8, bit = k % 8;
		
		if (state)
		{
			z_kdownstates[byte] |= 1 << bit;
			z_kpressstates[byte] |= 1 << bit;
		}
		else
		{
			z_kdownstates[byte] &= ~(1 << bit);
			z_kreleasestates[byte] |= 1 << bit;
		}
	}
	else if (e->type == SDL_MOUSEBUTTONDOWN || e->type == SDL_MOUSEBUTTONUP)
	{
		bool state = e->type == SDL_MOUSEBUTTONDOWN;
		
		if (state)
		{
			z_mdownstates |= 1 << e->button.button;
			z_mpressstates |= 1 << e->button.button;
		}
		else
		{
			z_mdownstates &= ~(1 << e->button.button);
			z_mreleasestates |= 1 << e->button.button;
		}
	}
	else if (e->type == SDL_TEXTINPUT)
	{
		u8 ch = e->text.text[0];
		
		// disregard non-ASCII input.
		if (ch & 0x80)
		{
			return;
		}
		
		usize byte = ch / 8, bit = ch % 8;
		z_textinputstates[byte] |= 1 << bit;
	}
}

void
z_prepareinput(void)
{
	memset(z_kpressstates, 0, sizeof(z_kpressstates));
	memset(z_kreleasestates, 0, sizeof(z_kreleasestates));
	
	z_mpressstates = 0;
	z_mreleasestates = 0;
	
	memset(z_textinputstates, 0, sizeof(z_textinputstates));
}

bool
z_kdown(SDL_Keycode k)
{
	if (k & 1 << 30)
	{
		k &= ~(1 << 30);
		k += 128;
	}
	usize byte = k / 8, bit = k % 8;
	return z_kdownstates[byte] & 1 << bit;
}

bool
z_kpressed(SDL_Keycode k)
{
	if (k & 1 << 30)
	{
		k &= ~(1 << 30);
		k += 128;
	}
	usize byte = k / 8, bit = k % 8;
	return z_kpressstates[byte] & 1 << bit;
}

bool
z_kreleased(SDL_Keycode k)
{
	if (k & 1 << 30)
	{
		k &= ~(1 << 30);
		k += 128;
	}
	usize byte = k / 8, bit = k % 8;
	return z_kreleasestates[byte] & 1 << bit;
}

void
z_mpos(SDL_Window const *wnd, OUT i32 *x, OUT i32 *y)
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
z_mdown(i32 btn)
{
	return !!(z_mdownstates & 1 << btn);
}

bool
z_mpressed(i32 btn)
{
	return !!(z_mpressstates & 1 << btn);
}

bool
z_mreleased(i32 btn)
{
	return !!(z_mreleasestates & 1 << btn);
}

bool
z_textinput(char ch)
{
	usize byte = ch / 8, bit = ch % 8;
	return z_textinputstates[byte] & 1 << bit;
}
// SPDX-License-Identifier: BSD-3-Clause

i32
z_optraw(OUT char buf[], FILE *fp, char const *key)
{
	// TODO: implement.
	(void)buf, (void)fp, (void)key;
	__builtin_unreachable();
}

i32
z_optkeycode(OUT SDL_Keycode *k, FILE *fp, char const *key)
{
	// TODO: implement.
	(void)k, (void)fp, (void)key;
	__builtin_unreachable();
}

i32
z_optfloat(OUT f64 *f, FILE *fp, char const *key)
{
	// TODO: implement.
	(void)f, (void)fp, (void)key;
	__builtin_unreachable();
}

i32
z_optint(OUT i64 *i, FILE *fp, char const *key)
{
	// TODO: implement.
	(void)i, (void)fp, (void)key;
	__builtin_unreachable();
}

i32
z_optbool(OUT bool *b, FILE *fp, char const *key)
{
	// TODO: implement.
	(void)b, (void)fp, (void)key;
	__builtin_unreachable();
}
// SPDX-License-Identifier: BSD-3-Clause

i32
z_readpack(OUT z_pack_t *p, u8 *pack, usize size)
{
	// TODO: implement.
	(void)p, (void)pack, (void)size;
	__builtin_unreachable();
}

i32
z_readpackfile(OUT z_pack_t *p, FILE *fp)
{
	// TODO: implement.
	(void)p, (void)fp;
	__builtin_unreachable();
}

void
z_writepack(OUT u8 buf[], OUT usize *size, z_pack_t const *p)
{
	// TODO: implement.
	(void)buf, (void)size, (void)p;
	__builtin_unreachable();
}

i32
z_writepackfile(FILE *fp, z_pack_t const *p)
{
	// TODO: implement.
	(void)fp, (void)p;
	__builtin_unreachable();
}

void
z_destroypack(z_pack_t *p)
{
	// TODO: implement.
	(void)p;
	__builtin_unreachable();
}

i32
z_packadd(z_pack_t *p, z_res_t const *r)
{
	// TODO: implement.
	(void)p, (void)r;
	__builtin_unreachable();
}

i32
z_packinsert(z_pack_t *p, z_res_t const *r, u32 idx)
{
	// TODO: implement.
	(void)p, (void)r, (void)idx;
	__builtin_unreachable();
}

i32
z_packrm(z_pack_t *p, z_restype_t type, u32 idx)
{
	// TODO: implement.
	(void)p, (void)type, (void)idx;
	__builtin_unreachable();
}

bool
z_packfind(OUT u32 *idx, z_pack_t const *p, z_restype_t type, char const *name)
{
	// TODO: implement.
	(void)idx, (void)p, (void)type, (void)name;
	__builtin_unreachable();
}

void
z_packranges(z_pack_t *p)
{
	// TODO: implement.
	(void)p;
	__builtin_unreachable();
}
// SPDX-License-Identifier: BSD-3-Clause

z_ui_t
z_beginui(
	z_uielem_t elems[],
	usize elemcap,
	i32 x,
	i32 y,
	TTF_Font *font,
	SDL_Window const *wnd
)
{
	return (z_ui_t)
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
z_renderui(z_ui_t const *u)
{
	if (!u->nelems)
	{
		return;
	}
	
	// find render boundaries to draw panel.
	i32 minx = INT32_MAX, miny = INT32_MAX;
	i32 maxx = INT32_MIN, maxy = INT32_MIN;
	for (usize i = 0; i < u->nelems; ++i)
	{
		i32 x = u->elems[i].any.x, y = u->elems[i].any.y;
		i32 w = u->elems[i].any.w, h = u->elems[i].any.h;
		
		minx = x < minx ? x : minx;
		miny = y < miny ? y : miny;
		maxx = x + w > maxx ? x + w : maxx;
		maxy = y + h > maxy ? y + h : maxy;
	}
	
	i32 pad = z_conf.uipad;
	
	// draw panel.
	z_conf.renderrect(
		minx - pad,
		miny - pad,
		maxx - minx + 2 * pad,
		maxy - miny + 2 * pad,
		Z_PANELCOLOR
	);
	
	// draw UI elements.
	i32 mx, my;
	z_mpos(u->wnd, &mx, &my);
	for (usize i = 0; i < u->nelems; ++i)
	{
		i32 x = u->elems[i].any.x, y = u->elems[i].any.y;
		i32 w = u->elems[i].any.w, h = u->elems[i].any.h;
		
		if (u->elems[i].any.type == Z_LABEL)
		{
			z_conf.rendertext(
				x,
				y,
				w,
				h,
				u->elems[i].button.text,
				Z_LABELTEXTCOLOR
			);
		}
		else if (u->elems[i].any.type == Z_BUTTON)
		{
			z_color_t bcol = Z_BUTTONCOLOR;
			z_color_t tcol = Z_BUTTONTEXTCOLOR;
			if (mx >= x && my >= y && mx < x + w && my < y + h)
			{
				if (z_mdown(SDL_BUTTON_LEFT))
				{
					bcol = Z_BUTTONPRESSCOLOR;
					tcol = Z_BUTTONTEXTPRESSCOLOR;
				}
				else
				{
					bcol = Z_BUTTONHOVERCOLOR;
					tcol = Z_BUTTONTEXTPRESSCOLOR;
				}
			}
			
			z_conf.renderrect(x, y, w, h, bcol);
			z_conf.rendertext(
				x + pad,
				y + pad,
				w - 2 * pad,
				h - 2 * pad,
				u->elems[i].button.text,
				tcol
			);
		}
		else if (u->elems[i].any.type == Z_SLIDER)
		{
			z_color_t scol = Z_SLIDERCOLOR;
			z_color_t tcol = Z_SLIDERTEXTCOLOR;
			z_color_t bcol = Z_SLIDERBARCOLOR;
			if (mx >= x && my >= y && mx < x + w && my < y + h)
			{
				if (z_mdown(SDL_BUTTON_LEFT))
				{
					scol = Z_SLIDERPRESSCOLOR;
					tcol = Z_SLIDERTEXTPRESSCOLOR;
					bcol = Z_SLIDERBARPRESSCOLOR;
				}
				else
				{
					scol = Z_SLIDERHOVERCOLOR;
					tcol = Z_SLIDERTEXTPRESSCOLOR;
					bcol = Z_SLIDERBARHOVERCOLOR;
				}
			}
			
			z_conf.renderrect(x, y, w, h, scol);
			z_conf.renderrect(x, y, u->elems[i].slider.val * w, h, bcol);
			z_conf.rendertext(
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
			z_color_t tfcol = Z_TEXTFIELDCOLOR;
			z_color_t tftcol = Z_TEXTFIELDTEXTCOLOR;
			z_color_t tfbcol = Z_TEXTFIELDBARCOLOR;
			z_color_t tfpcol = Z_TEXTFIELDPROMPTCOLOR;
			if (mx >= x && my >= y && mx < x + w && my < y + h)
			{
				if (z_mdown(SDL_BUTTON_LEFT))
				{
					tfcol = Z_TEXTFIELDPRESSCOLOR;
					tftcol = Z_TEXTFIELDTEXTPRESSCOLOR;
					tfbcol = Z_TEXTFIELDBARPRESSCOLOR;
					tfpcol = Z_TEXTFIELDPROMPTPRESSCOLOR;
				}
				else
				{
					tfcol = Z_TEXTFIELDHOVERCOLOR;
					tftcol = Z_TEXTFIELDTEXTHOVERCOLOR;
					tfbcol = Z_TEXTFIELDBARHOVERCOLOR;
					tfpcol = Z_TEXTFIELDPROMPTHOVERCOLOR;
				}
			}
			
			z_conf.renderrect(x, y, w, h, tfcol);
			
			z_tfdata_t const *t = u->elems[i].textfield.tfdata;
			i32 chw = (w - 2 * pad) / u->elems[i].textfield.ndraw;
			i32 chh = h - 2 * pad;
			
			char const *text = t->len ? t->buf : u->elems[i].textfield.text;
			u32 textfirst = t->len ? t->first : 0;
			u32 textlen = t->len ? t->len : strlen(text);
			z_color_t textcol = t->len ? tftcol : tfpcol;
			
			i32 dx = 0;
			for (u32 j = textfirst; j < textlen; ++j)
			{
				if (dx >= w - 2 * pad)
				{
					break;
				}
				char render[] = {text[j], 0};
				z_conf.rendertext(x + pad + dx, y + pad, chw, chh, render, textcol);
				dx += chw;
			}
			
			if (t->sel)
			{
				z_conf.renderrect(
					x + pad + (t->csr - t->first) * chw,
					y + pad,
					z_conf.uitextfieldbar,
					chh,
					tfbcol
				);
			}
		}
	}
}

void
z_uipad(z_ui_t *u, i32 dx, i32 dy)
{
	u->x += dx;
	u->y += dy;
}

void
z_uilabel(z_ui_t *u, char const *text)
{
	if (u->nelems >= u->elemcap)
	{
		return;
	}
	
	i32 w, h;
	TTF_SizeText(u->font, text, &w, &h);
	
	u->elems[u->nelems++] = (z_uielem_t)
	{
		.label =
		{
			.type = Z_LABEL,
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
z_uibutton(z_ui_t *u, char const *text)
{
	if (u->nelems >= u->elemcap)
	{
		return false;
	}
	
	bool state = false;
	
	i32 w, h;
	TTF_SizeText(u->font, text, &w, &h);
	w += 2 * z_conf.uipad;
	h += 2 * z_conf.uipad;
	
	i32 mx, my;
	z_mpos(u->wnd, &mx, &my);
	
	if (z_mreleased(SDL_BUTTON_LEFT)
		&& mx >= u->x
		&& my >= u->y
		&& mx < u->x + w
		&& my < u->y + h)
	{
		state = true;
	}
	
	u->elems[u->nelems++] = (z_uielem_t)
	{
		.button =
		{
			.type = Z_BUTTON,
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
z_uislider(z_ui_t *u, char const *text, INOUT f32 *val)
{
	if (u->nelems >= u->elemcap)
	{
		return false;
	}
	
	bool state = false;
	
	i32 w, h;
	TTF_SizeText(u->font, text, &w, &h);
	w += 2 * z_conf.uipad;
	h += 2 * z_conf.uipad;
	
	i32 mx, my;
	z_mpos(u->wnd, &mx, &my);
	
	if (z_mreleased(SDL_BUTTON_LEFT)
		&& mx >= u->x
		&& my >= u->y
		&& mx < u->x + w
		&& my < u->y + h)
	{
		*val = (f64)(mx - u->x) / w;
		state = true;
	}
	
	*val = *val < 0.0f ? 0.0f : *val;
	*val = *val > 1.0f ? 1.0f : *val;
	
	u->elems[u->nelems++] = (z_uielem_t)
	{
		.slider =
		{
			.type = Z_SLIDER,
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
z_uitextfield(z_ui_t *u, char const *text, INOUT z_tfdata_t *tfdata, u32 ndraw)
{
	if (u->nelems >= u->elemcap)
	{
		return;
	}
	
	i32 chw, chh;
	TTF_SizeText(u->font, " ", &chw, &chh);
	
	i32 w = ndraw * chw + 2 * z_conf.uipad;
	i32 h = chh + 2 * z_conf.uipad;
	
	if (z_mreleased(SDL_BUTTON_LEFT))
	{
		i32 mx, my;
		z_mpos(u->wnd, &mx, &my);
		
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
		if (z_kpressed(SDLK_LEFT))
		{
			tfdata->csr -= tfdata->csr > 0;
			tfdata->first -= tfdata->csr < tfdata->first;
		}
		
		if (z_kpressed(SDLK_RIGHT))
		{
			tfdata->csr += tfdata->csr < tfdata->len;
			tfdata->first += tfdata->csr - tfdata->first >= ndraw;
		}
		
		if (z_kpressed(SDLK_UP))
		{
			tfdata->csr = 0;
			tfdata->first = 0;
		}
		
		if (z_kpressed(SDLK_DOWN))
		{
			tfdata->csr = tfdata->len;
			tfdata->first = 0;
			while (tfdata->csr - tfdata->first > ndraw)
			{
				++tfdata->first;
			}
		}
		
		for (u8 i = 0; i < 128; ++i)
		{
			if (tfdata->len >= tfdata->cap + 1)
			{
				break;
			}
			
			if (!isprint(i))
			{
				continue;
			}
			
			if (z_textinput(i))
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
		
		if (z_kpressed(SDLK_BACKSPACE) && tfdata->csr)
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
	
	u->elems[u->nelems++] = (z_uielem_t)
	{
		.textfield =
		{
			.type = Z_TEXTFIELD,
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

static u64 z_tickstart;

void
z_err(char const *fmt, ...)
{
	va_list args;
	va_start(args, fmt);
	
	char msg[512];
	vsnprintf(msg, sizeof(msg), fmt, args);
	
	if (SDL_ShowSimpleMessageBox(SDL_MESSAGEBOX_ERROR, z_conf.errtitle, msg, NULL))
	{
		fprintf(z_conf.log, "\x1b[1;31merr\x1b[0m: %s\n", msg);
	}
	
	va_end(args);
}

u64
z_unixus(void)
{
	struct timeval tv;
	gettimeofday(&tv, NULL);
	return (u64)tv.tv_sec * 1000000 + (u64)tv.tv_usec;
}

void
z_begintick(void)
{
	z_tickstart = z_unixus();
}

void
z_endtick(void)
{
	u64 tickend = z_unixus();
	i64 timeleft = z_conf.tickus - tickend + z_tickstart;
	timeleft *= (timeleft > 0);
	
	struct timespec ts =
	{
		.tv_nsec = 1000 * timeleft
	};
	nanosleep(&ts, NULL);
}

void
z_begintimer(OUT u64 *timer)
{
	*timer = z_unixus();
}

void
z_endtimer(u64 timer, char const *name)
{
	u64 d = z_unixus() - timer;
	fprintf(
		z_conf.log,
		"\x1b[1;33mprofile\x1b[0m: %s: %llu\n",
		name,
		(unsigned long long)d
	);
}

f32
z_interpangle(f32 a, f32 b, f32 t)
{
	return a + z_shortestangle(a, b) * t;
}

f32
z_shortestangle(f32 a, f32 b)
{
	f32 d = fmod(b - a, Z_TAU);
	f32 shortest = fmod(2.0f * d, Z_TAU) - d;
	return shortest;
}

f32
z_r2d(f32 r)
{
	return r / Z_PI * 180.0f;
}

f32
z_d2r(f32 d)
{
	return d / 180.0f * Z_PI;
}

void *
z_allocbatch(INOUT z_allocbatch_t *allocs, usize nallocs)
{
	// TODO: implement.
	(void)allocs, (void)nallocs;
	__builtin_unreachable();
}

void *
z_reallocbatch(INOUT z_reallocbatch_t *reallocs, usize nreallocs)
{
	// TODO: implement.
	(void)reallocs, (void)nreallocs;
	__builtin_unreachable();
}
#endif
#endif
