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

// enumeration values.
typedef enum ls_toktype
{
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
ls_lexfile_t ls_lex(char const *file);
ls_lexfile_t ls_lexdirect(char const *name, char const *data, size_t len);
char *ls_readraw(ls_lexfile_t const *l, uint32_t pos, uint32_t len);
char *ls_readstr(ls_lexfile_t const *l, uint32_t pos, uint32_t len);

// parse procedures.
ls_ast_t ls_parse(ls_lexfile_t const *l);

#ifdef __cplusplus
}
#endif

#endif
