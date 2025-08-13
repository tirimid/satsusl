// SPDX-License-Identifier: BSD-3-Clause

// standard library.
#include <ctype.h>
#include <limits.h>
#include <string.h>

// system dependencies.
#include <sys/stat.h>
#include <unistd.h>

// project headers.
#include "internalutil.h"
#include "satsu.h"

// project source.
#include "ls_lex.c"
#include "ls_parse.c"
#include "ls_sema.c"
#include "ls_util.c"

void *(*ls_malloc)(size_t) = malloc;
void *(*ls_realloc)(void *, size_t) = realloc;
void (*ls_free)(void *) = free;
char *(*ls_strdup)(char const *) = strdup;
