// SPDX-License-Identifier: BSD-3-Clause

#ifndef LS_SATSU_H
#define LS_SATSU_H

#include <stdbool.h>
#include <stddef.h>
#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>

#define LS_NULL 0
#define LS_MAXIDENT 63

//--------------------//
// enumeration values //
//--------------------//

typedef enum ls_toktype
{
	LS_IDENT = 1,
	
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
	LS_KWVOID,
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
	// structure nodes.
	LS_ROOT = 1,
	LS_IMPORT,
	LS_FUNC,
	LS_DECLARATION,
	LS_ARGLIST,
	LS_ARG,
	LS_RETURN,
	LS_CTREE,
	LS_WHILE,
	LS_FOR,
	LS_BREAK,
	LS_CONTINUE,
	LS_BLOCK,
	
	// type nodes.
	LS_TYPE,
	
	// expression nodes.
	LS_EATOM,
	LS_ECALL,
	LS_EACCESS,
	LS_ENEG,
	LS_ENOT,
	LS_EMUL,
	LS_EDIV,
	LS_EMOD,
	LS_EADD,
	LS_ESUB,
	LS_ELESS,
	LS_ELEQUAL,
	LS_EGREATER,
	LS_EGREQUAL,
	LS_EEQUAL,
	LS_ENEQUAL,
	LS_EAND,
	LS_EOR,
	LS_EXOR,
	LS_ETERNARY,
	LS_EASSIGN,
	LS_EADDASSIGN,
	LS_ESUBASSIGN,
	LS_EMULASSIGN,
	LS_EDIVASSIGN,
	LS_EMODASSIGN,
	
	LS_NODETYPE_END
} ls_nodetype_t;

typedef enum ls_primtype
{
	LS_INT = 1,
	LS_REAL,
	LS_STRING,
	LS_BOOL,
	LS_VOID,
	
	LS_PRIMTYPE_END
} ls_primtype_t;

//-----------------//
// data structures //
//-----------------//

typedef struct ls_err
{
	int32_t code;
	uint32_t pos, len;
	size_t src;
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

typedef struct ls_module
{
	char **names;
	uint64_t *ids;
	char **data;
	uint32_t *lens;
	ls_lex_t *lexes;
	ls_ast_t *asts;
	size_t nmods, modcap;
} ls_module_t;

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
extern char const *ls_primtypenames[LS_PRIMTYPE_END];

//------------//
// procedures //
//------------//

// util.
void ls_destroyerr(ls_err_t *err);
ls_err_t ls_readfile(FILE *fp, char **outdata, uint32_t *outlen);
uint64_t ls_fileid(char const *file, bool deref);

// lex.
ls_err_t ls_lex(ls_lex_t *out, char const *data, uint32_t len);
void ls_addtok(ls_lex_t *l, ls_toktype_t type, uint32_t pos, uint32_t len);
void ls_readtokraw(char out[], char const *data, ls_tok_t tok);
void ls_readtokstr(char out[], size_t *outlen, char const *data, ls_tok_t tok);
void ls_printtok(FILE *fp, ls_tok_t tok, ls_toktype_t type);
void ls_destroylex(ls_lex_t *l);

// parse.
ls_err_t ls_parse(ls_ast_t *out, ls_lex_t const *l);
uint32_t ls_addnode(ls_ast_t *a, ls_nodetype_t type);
void ls_parentnode(ls_ast_t *a, uint32_t parent, uint32_t child);
void ls_printast(FILE *fp, ls_ast_t const *ast, ls_lex_t const *lex);
void ls_printnode(FILE *fp, ls_ast_t const *ast, ls_lex_t const *lex, uint32_t n, uint32_t depth);
void ls_destroyast(ls_ast_t *a);

// sema.
ls_module_t ls_createmodule(ls_ast_t *a, ls_lex_t *l, char *name, uint64_t id, char *data, uint32_t len);
ls_err_t ls_resolveimports(ls_module_t *m, char const *paths[], size_t npaths);
void ls_pushmodule(ls_module_t *m, ls_ast_t *a, ls_lex_t *l, char *name, uint64_t id, char *data, uint32_t len);
void ls_printmoduleasts(FILE *fp, ls_module_t const *m);
void ls_destroymodule(ls_module_t *m);

#endif
