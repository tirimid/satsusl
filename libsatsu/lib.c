// SPDX-License-Identifier: BSD-3-Clause

// standard library.
#include <ctype.h>
#include <string.h>

// system dependencies.
#include <unistd.h>

// project headers.
#include "satsu.h"

// project source.
#include "ls_lex.c"
#include "ls_parse.c"
#include "ls_util.c"

void *(*ls_calloc)(size_t, size_t) = calloc;
void *(*ls_reallocarray)(void *, size_t, size_t) = reallocarray;
void (*ls_free)(void *) = free;
char *(*ls_strdup)(char const *) = strdup;
