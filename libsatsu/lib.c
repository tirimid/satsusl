// SPDX-License-Identifier: BSD-3-Clause

// global util macros.
#define LS_STRINGIFY(s) #s

// standard library.
#include <ctype.h>
#include <limits.h>
#include <string.h>

// system dependencies.
#include <sys/stat.h>
#include <unistd.h>

// project headers.
#include "satsu.h"

// project source.
#include "ls_lex.c"
#include "ls_parse.c"
#include "ls_sema.c"
#include "ls_util.c"

void *(*ls_calloc)(size_t, size_t) = calloc;
void *(*ls_reallocarray)(void *, size_t, size_t) = reallocarray;
void (*ls_free)(void *) = free;
char *(*ls_strdup)(char const *) = strdup;
