// SPDX-License-Identifier: BSD-3-Clause

#ifndef LS_SATSU_H
#define LS_SATSU_H

#include <stdbool.h>
#include <stddef.h>
#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>

//--------------------------//
// library meta-information //
//--------------------------//

#define LS_VERMAJOR 1
#define LS_VERMINOR 0
#define LS_VERPATCH 0

//--------//
// macros //
//--------//

#define LS_NULL 0
#define LS_MAXIDENT 63
#define LS_MAXSTRING 1023
#define LS_MAXINT 63
#define LS_MAXREAL 63
#define LS_BATCHALIGN 16
#define LS_MAXREALLOCBATCH 32
#define LS_MAXSYSARGS 6
#define LS_CIGNORE 0x1fffffff
#define LS_CERR 0x2fffffff

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
	LS_KWNEW,
	LS_KWREAL,
	LS_KWRETURN,
	LS_KWSTRING,
	LS_KWSYSTEM,
	LS_KWTRUE,
	LS_KWVOID,
	LS_KWWHILE,
	
	// punctuation.
	LS_LBRACE,
	LS_RBRACE,
	LS_COMMA,
	LS_SEMICOLON,
	LS_LPAREN,
	LS_RPAREN,
	LS_LBRACKET,
	LS_RBRACKET,
	LS_MINUS,
	LS_BANG,
	LS_EQUALGREATER,
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
	LS_FUNCDECL,
	LS_GLOBALDECL,
	LS_LOCALDECL,
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
	LS_ESYSTEM,
	LS_ECALL,
	LS_EACCESS,
	LS_ENEG,
	LS_ENOT,
	LS_ECAST,
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
	LS_FUNC,
	
	LS_PRIMTYPE_END
} ls_primtype_t;

typedef enum ls_valuetype
{
	LS_LVALUE = 1,
	LS_RVALUE
} ls_valuetype_t;

//----------------//
// internal types //
//----------------//

struct ls_exec;

//-----------------//
// data structures //
//-----------------//

typedef struct ls_err
{
	int16_t code;
	uint16_t src;
	uint32_t pos, len;
	char *msg;
} ls_err_t;

typedef struct ls_allocbatch
{
	void **ptr;
	size_t n, size;
} ls_allocbatch_t;

typedef struct ls_reallocbatch
{
	void **ptr;
	size_t oldn, newn, size;
} ls_reallocbatch_t;

typedef struct ls_tok
{
	uint32_t pos, len;
} ls_tok_t;

typedef struct ls_lex
{
	void *buf;
	ls_tok_t *toks;
	uint8_t *types; // ls_toktype_t.
	uint32_t ntoks, tokcap;
} ls_lex_t;

typedef struct ls_node
{
	uint32_t *children;
	uint32_t tok;
	uint16_t nchildren, childcap;
} ls_node_t;

typedef struct ls_ast
{
	void *buf;
	ls_node_t *nodes;
	uint8_t *types; // ls_nodetype_t.
	uint32_t nnodes, nodecap;
} ls_ast_t;

typedef struct ls_module
{
	void *buf;
	char **names;
	uint64_t *ids;
	char **data;
	uint32_t *lens;
	ls_lex_t *lexes;
	ls_ast_t *asts;
	uint32_t nmods, modcap;
} ls_module_t;

typedef struct ls_val
{
	union
	{
		int64_t int_;
		double real;
		char *string;
		bool bool_;
	} data;
	uint8_t type; // ls_primtype_t.
} ls_val_t;

typedef struct ls_symtab
{
	void *buf;
	char **syms;
	uint8_t *types; // ls_primtype_t.
	uint32_t *mods, *nodes;
	uint16_t *scopes;
	ls_val_t *vals;
	uint32_t nsyms, symcap;
} ls_symtab_t;

typedef struct ls_sysfns
{
	void *buf;
	char const **names;
	ls_val_t (**callbacks)(struct ls_exec *, ls_val_t[LS_MAXSYSARGS]);
	uint8_t *rettypes; // ls_primtype_t.
	uint8_t (*argtypes)[LS_MAXSYSARGS]; // ls_primtype_t.
	uint8_t *nargs;
	uint32_t nfns, fncap;
} ls_sysfns_t;

typedef struct ls_conf
{
	int (*cget)(void);
	void (*cput)(int);
} ls_conf_t;

//-----------------------//
// library configuration //
//-----------------------//

// memory.
extern void *(*ls_malloc)(size_t);
extern void *(*ls_realloc)(void *, size_t);
extern void *(*ls_calloc)(size_t, size_t);
extern void *(*ls_reallocarray)(void *, size_t, size_t);
extern void (*ls_free)(void *);
extern char *(*ls_strdup)(char const *);
extern void *(*ls_memcpy)(void *, void const *, size_t);
extern void *(*ls_memmove)(void *, void const *, size_t);

// should be set before library use.
extern ls_conf_t ls_conf;

//-------------//
// data tables //
//-------------//

extern char const *ls_toknames[LS_TOKTYPE_END];
extern char const *ls_nodenames[LS_NODETYPE_END];
extern char const *ls_primtypenames[LS_PRIMTYPE_END];
extern ls_primtype_t ls_toktoprim[LS_TOKTYPE_END];

//------------//
// procedures //
//------------//

// util.
void ls_destroyerr(ls_err_t *err);
ls_err_t ls_readfile(FILE *fp, char **outdata, uint32_t *outlen);
uint64_t ls_fileid(char const *file, bool deref);
uint64_t ls_alignbatch(uint64_t n);
void *ls_allocbatch(ls_allocbatch_t *allocs, size_t nallocs);
void *ls_reallocbatch(void *p, ls_reallocbatch_t *reallocs, size_t nreallocs);
int32_t ls_cprintf(char const *fmt, ...);

// lex.
ls_err_t ls_lex(ls_lex_t *out, char const *data, uint32_t len);
void ls_addtok(ls_lex_t *l, ls_toktype_t type, uint32_t pos, uint32_t len);
void ls_readtokraw(char out[], char const *data, ls_tok_t tok);
int64_t ls_readtokint(char const *data, ls_tok_t tok);
double ls_readtokreal(char const *data, ls_tok_t tok);
void ls_readtokstr(char out[], char const *data, ls_tok_t tok);
void ls_printtok(FILE *fp, ls_tok_t tok, ls_toktype_t type);
void ls_cprinttok(ls_tok_t tok, ls_toktype_t type);
void ls_destroylex(ls_lex_t *l);

// parse.
ls_err_t ls_parse(ls_ast_t *out, ls_lex_t const *l);
uint32_t ls_addnode(ls_ast_t *a, ls_nodetype_t type);
void ls_parentnode(ls_ast_t *a, uint32_t parent, uint32_t child);
void ls_printast(FILE *fp, ls_ast_t const *ast, ls_lex_t const *lex);
void ls_cprintast(ls_ast_t const *ast, ls_lex_t const *lex);
void ls_printnode(FILE *fp, ls_ast_t const *ast, ls_lex_t const *lex, uint32_t n, uint32_t depth);
void ls_cprintnode(ls_ast_t const *ast, ls_lex_t const *lex, uint32_t n, uint32_t depth);
void ls_destroyast(ls_ast_t *a);

// sema.
ls_module_t ls_createmodule(ls_ast_t *a, ls_lex_t *l, char *name, uint64_t id, char *data, uint32_t len);
ls_err_t ls_resolveimports(ls_module_t *m, char const *paths[], size_t npaths);
void ls_pushmodule(ls_module_t *m, ls_ast_t *a, ls_lex_t *l, char *name, uint64_t id, char *data, uint32_t len);
void ls_printmodule(FILE *fp, ls_module_t const *m);
void ls_cprintmodule(ls_module_t const *m);
void ls_destroymodule(ls_module_t *m);
ls_err_t ls_globalsymtab(ls_symtab_t *out, ls_module_t const *m);
ls_err_t ls_sema(ls_module_t const *m);
ls_symtab_t ls_createsymtab(void);
int64_t ls_findsym(ls_symtab_t const *st, char const *sym);
void ls_pushsym(ls_symtab_t *st, char *sym, ls_primtype_t type, uint32_t mod, uint32_t node, uint16_t scope);
void ls_destroysymtab(ls_symtab_t *st);
void ls_popsymscope(ls_symtab_t *st, uint16_t scope);
ls_primtype_t ls_typeof(ls_module_t const *m, uint32_t mod, ls_symtab_t const *st, uint32_t node);
ls_valuetype_t ls_valuetypeof(ls_module_t const *m, uint32_t mod, ls_symtab_t const *st, uint32_t node);
void ls_printsymtab(FILE *fp, ls_symtab_t const *st);
void ls_cprintsymtab(ls_symtab_t const *st);

// exec.
ls_val_t ls_defaultval(ls_primtype_t type);
ls_val_t ls_copyval(ls_val_t const *v);
void ls_destroyval(ls_val_t *v);
ls_sysfns_t ls_emptysysfns(void);
ls_sysfns_t ls_basesysfns(void);
void ls_destroysysfns(ls_sysfns_t *sf);
int64_t ls_findsysfn(ls_sysfns_t const *sf, char const *sysfn);
void ls_pushsysfn(ls_sysfns_t *sf, char const *name, ls_val_t (*callback)(struct ls_exec *, ls_val_t[LS_MAXSYSARGS]), ls_primtype_t rettype, ls_primtype_t argtypes[LS_MAXSYSARGS], uint8_t nargs);
ls_err_t ls_exec(ls_module_t const *m, FILE *logfp, ls_sysfns_t const *sf, char const *entry);

#endif
