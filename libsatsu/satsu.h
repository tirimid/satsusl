// SPDX-License-Identifier: BSD-3-Clause

#ifndef LS_SATSU_H
#define LS_SATSU_H

#ifdef __cplusplus
extern "C"
{
#endif

#include <stddef.h>
#include <stdint.h>
#include <stdlib.h>

// utility types.
typedef int32_t ls_err_t;

// enumeration values.
typedef enum ls_toktype
{
	LS_IDENT = 0,
	
	// literals.
	LS_LITINT,
	LS_LITREAL,
	LS_LITSTR,
	
	// keywords.
	LS_KWBOOL,
	LS_KWBREAK,
	LS_KWCONTINUE,
	LS_KWELSE,
	LS_KWFALSE,
	LS_KWFOR,
	LS_KWFUNC,
	LS_KWIF,
	LS_KWIMPORT,
	LS_KWINT,
	LS_KWREAL,
	LS_KWRETURN,
	LS_KWSTRING,
	LS_KWTRUE,
	LS_KWVAR,
	LS_KWWHILE,
	
	// punctuation.
	LS_LBRACE,
	LS_RBRACE,
	LS_COMMA,
	LS_SEMICOLON,
	LS_LPAREN,
	LS_RPAREN,
	LS_PERIOD,
	LS_MINUS,
	LS_BANG,
	LS_DOLLAR,
	LS_STAR,
	LS_SLASH,
	LS_PERCENT,
	LS_PLUS,
	LS_LESS,
	LS_LESSEQUAL,
	LS_GREATER,
	LS_GREATEREQUAL,
	LS_2EQUAL,
	LS_BANGEQUAL,
	LS_2AMPERSAND,
	LS_2PIPE,
	LS_2CARET,
	LS_QUESTION,
	LS_COLON,
	LS_EQUAL,
	LS_PLUSEQUAL,
	LS_MINUSEQUAL,
	LS_STAREQUAL,
	LS_SLASHEQUAL,
	LS_PERCENTEQUAL,
	
	LS_TOKTYPE_END
} ls_toktype_t;

typedef enum ls_nodetype
{
	LS_NODETYPE_END
} ls_nodetype_t;

// data structures.
typedef struct ls_tok
{
	uint32_t pos, len;
} ls_tok_t;

typedef struct ls_lexfile
{
	char *name;
	char *conts;
	size_t len;
	ls_tok_t *toks;
	uint8_t *types;
	size_t ntoks, tokcap;
} ls_lexfile_t;

typedef struct ls_node
{
	uint32_t *children;
	uint32_t tok;
	uint16_t nchildren, childcap;
} ls_node_t;

typedef struct ls_ast
{
	ls_node_t *nodes;
	uint8_t *types;
	size_t nnodes, nodecap;
} ls_ast_t;

// library configuration.
extern void *(*ls_calloc)(size_t, size_t);
extern void *(*ls_reallocarray)(void *, size_t, size_t);
extern void (*ls_free)(void *);

// data tables.
extern char const *ls_toknames[LS_TOKTYPE_END];
extern char const *ls_nodenames[LS_NODETYPE_END];

// lex procedures.
ls_err_t ls_lexfile(ls_lexfile_t *out, char const *file);
ls_err_t ls_lex(l_lexfile_t *out, char const *name, char const *data, size_t len);
char *ls_readraw(ls_lexfile_t const *l, uint32_t pos, uint32_t len);
char *ls_readstr(ls_lexfile_t const *l, uint32_t pos, uint32_t len);

// parse procedures.
ls_err_t ls_parse(ls_ast_t *out, ls_lexfile_t const *l);

#ifdef __cplusplus
}
#endif

#endif
