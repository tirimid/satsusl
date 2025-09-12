// SPDX-License-Identifier: BSD-3-Clause

typedef enum r_font
{
	R_VCROSDMONO = 0,
	
	R_FONT_END
} r_font_t;

extern SDL_Window *r_wnd;
extern SDL_Renderer *r_rend;
extern TTF_Font *r_fonts[R_FONT_END];

i32 r_init(void);
SDL_Texture *r_rendertext(r_font_t font, char const *text, u8 r, u8 g, u8 b, u8 a);
void r_tglrenderrect(i32 x, i32 y, i32 w, i32 h, z_color_t col);
void r_tglrendertext(i32 x, i32 y, i32 w, i32 h, char const *text, z_color_t col);
