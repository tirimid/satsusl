// SPDX-License-Identifier: BSD-3-Clause

// standard library.
#include <ctype.h>
#include <limits.h>
#include <string.h>

// system dependencies.
#include <sys/stat.h>
#include <unistd.h>

// project headers.
#include "internal.h"
#include "satsu.h"

// project source.
#include "ls_lex.c"
#include "ls_parse.c"
#include "ls_sema.c"
#include "ls_util.c"

void *(*ls_malloc)(size_t) = malloc;
void *(*ls_realloc)(void *, size_t) = realloc;
void *(*ls_calloc)(size_t, size_t) = calloc;
void *(*ls_reallocarray)(void *, size_t, size_t) = reallocarray;
void (*ls_free)(void *) = free;
char *(*ls_strdup)(char const *) = strdup;
void *(*ls_memcpy)(void *, void const *, size_t) = memcpy;
void *(*ls_memmove)(void *, void const *, size_t) = memmove;
