// SPDX-License-Identifier: BSD-3-Clause

char const *ls_toknames[LS_TOKTYPE_END] =
{
	"identifier",
	
	// literals.
	"int literal",
	"real literal",
	"string literal",
	
	// keywords.
	"bool",
	"break",
	"continue",
	"else",
	"false",
	"for",
	"func",
	"if",
	"import",
	"int",
	"real",
	"return",
	"string",
	"true",
	"var",
	"while",
	
	// punctuation.
	"{",
	"}",
	",",
	";",
	"(",
	")",
	".",
	"-",
	"!",
	"$",
	"*",
	"/",
	"%",
	"+",
	"<",
	"<=",
	">",
	">=",
	"==",
	"!=",
	"&&",
	"||",
	"^^",
	"?",
	":",
	"=",
	"+=",
	"-=",
	"*=",
	"/=",
	"%="
};

ls_err_t
ls_lexfile(ls_lexfile_t *out, char const *file)
{
}

ls_err_t
ls_lex(ls_lexfile_t *out, char const *name, char const *data, size_t len)
{
}

char *
ls_readraw(ls_lexfile_t const *l, uint32_t pos, uint32_t len)
{
}

char *
ls_readstr(ls_lexfile_t const *l, uint32_t pos, uint32_t len)
{
}
