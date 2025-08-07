// SPDX-License-Identifier: BSD-3-Clause

#ifndef LS_SATSU_H
#define LS_SATSU_H

#ifdef __cplusplus
extern "C"
{
#endif

#include <stddef.h>
#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>

//--------------------//
// enumeration values //
//--------------------//

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

//-----------------//
// data structures //
//-----------------//

typedef struct ls_err
{
	int32_t code;
	uint32_t pos, len;
	char *src;
	char *msg;
} ls_err_t;

typedef struct ls_tok
{
	uint32_t pos, len;
} ls_tok_t;

typedef struct ls_lex
{
	ls_tok_t *toks;
	uint8_t *types;
	size_t ntoks, tokcap;
} ls_lex_t;

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

//-----------------------//
// library configuration //
//-----------------------//

extern void *(*ls_calloc)(size_t, size_t);
extern void *(*ls_reallocarray)(void *, size_t, size_t);
extern void (*ls_free)(void *);
extern char *(*ls_strdup)(char const *);

//-------------//
// data tables //
//-------------//

extern char const *ls_toknames[LS_TOKTYPE_END];
extern char const *ls_nodenames[LS_NODETYPE_END];

//------------//
// procedures //
//------------//

// util.
void ls_destroyerr(ls_err_t *err);
void ls_destroylex(ls_lex_t *l);

// lex.
ls_err_t ls_lexfile(ls_lex_t *out, FILE *fp, char const *name);
ls_err_t ls_lex(ls_lex_t *out, char const *name, char const *data, uint32_t len);
void ls_addtok(ls_lex_t *l, ls_toktype_t type, uint32_t pos, uint32_t len);

// parse.
ls_err_t ls_parse(ls_ast_t *out, ls_lex_t const *l);

#ifdef __cplusplus
}
#endif

#endif
