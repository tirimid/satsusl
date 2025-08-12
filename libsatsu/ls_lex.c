// SPDX-License-Identifier: BSD-3-Clause

char const *ls_toknames[LS_TOKTYPE_END] =
{
	"null",
	
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
	"void",
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

static ls_err_t ls_lexword(ls_lex_t *l, char const *name, char const *data, uint32_t len, size_t *i);
static ls_err_t ls_lexstr(ls_lex_t *l, char const *name, char const *data, uint32_t len, size_t *i);
static ls_err_t ls_lexnum(ls_lex_t *l, char const *name, char const *data, uint32_t len, size_t *i);
static void ls_lexcomment(char const *data, uint32_t len, size_t *i);

ls_err_t
ls_lex(ls_lex_t *out, char const *name, char const *data, uint32_t len)
{
	ls_lex_t l =
	{
		.toks = ls_calloc(1, sizeof(ls_tok_t)),
		.types = ls_calloc(1, 1),
		.tokcap = 1
	};
	ls_addtok(&l, LS_NULL, 0, 0);
	
	for (size_t i = 0; i < len; ++i)
	{
		if (isspace(data[i]))
		{
			continue;
		}
		else if (isalpha(data[i]) || data[i] == '_')
		{
			ls_err_t err = ls_lexword(&l, name, data, len, &i);
			if (err.code)
			{
				ls_destroylex(&l);
				return err;
			}
		}
		else if (data[i] == '"')
		{
			ls_err_t err = ls_lexstr(&l, name, data, len, &i);
			if (err.code)
			{
				ls_destroylex(&l);
				return err;
			}
		}
		else if (isdigit(data[i]))
		{
			ls_err_t err = ls_lexnum(&l, name, data, len, &i);
			if (err.code)
			{
				ls_destroylex(&l);
				return err;
			}
		}
		else if (!strncmp(&data[i], "//", 2))
		{
			ls_lexcomment(data, len, &i);
		}
		else if (data[i] == '(')
		{
			ls_addtok(&l, LS_LPAREN, i, 1);
		}
		else if (data[i] == ')')
		{
			ls_addtok(&l, LS_RPAREN, i, 1);
		}
		else if (data[i] == '.')
		{
			ls_addtok(&l, LS_PERIOD, i, 1);
		}
		else if (!strncmp(&data[i], "-=", 2))
		{
			ls_addtok(&l, LS_MINUSEQUAL, i, 2);
			++i;
		}
		else if (data[i] == '-')
		{
			ls_addtok(&l, LS_MINUS, i, 1);
		}
		else if (!strncmp(&data[i], "!=", 2))
		{
			ls_addtok(&l, LS_BANGEQUAL, i, 2);
			++i;
		}
		else if (data[i] == '!')
		{
			ls_addtok(&l, LS_BANG, i, 1);
		}
		else if (!strncmp(&data[i], "*=", 2))
		{
			ls_addtok(&l, LS_STAREQUAL, i, 2);
			++i;
		}
		else if (data[i] == '*')
		{
			ls_addtok(&l, LS_STAR, i, 1);
		}
		else if (!strncmp(&data[i], "/=", 2))
		{
			ls_addtok(&l, LS_SLASHEQUAL, i, 2);
			++i;
		}
		else if (data[i] == '/')
		{
			ls_addtok(&l, LS_SLASH, i, 1);
		}
		else if (!strncmp(&data[i], "%=", 2))
		{
			ls_addtok(&l, LS_PERCENTEQUAL, i, 2);
			++i;
		}
		else if (data[i] == '%')
		{
			ls_addtok(&l, LS_PERCENT, i, 1);
		}
		else if (!strncmp(&data[i], "+=", 2))
		{
			ls_addtok(&l, LS_PLUSEQUAL, i, 2);
			++i;
		}
		else if (data[i] == '+')
		{
			ls_addtok(&l, LS_PLUS, i, 1);
		}
		else if (!strncmp(&data[i], "<=", 2))
		{
			ls_addtok(&l, LS_LESSEQUAL, i, 2);
			++i;
		}
		else if (data[i] == '<')
		{
			ls_addtok(&l, LS_LESS, i, 1);
		}
		else if (!strncmp(&data[i], ">=", 2))
		{
			ls_addtok(&l, LS_GREATEREQUAL, i, 2);
			++i;
		}
		else if (data[i] == '>')
		{
			ls_addtok(&l, LS_GREATER, i, 1);
		}
		else if (!strncmp(&data[i], "==", 2))
		{
			ls_addtok(&l, LS_2EQUAL, i, 2);
			++i;
		}
		else if (data[i] == '=')
		{
			ls_addtok(&l, LS_EQUAL, i, 1);
		}
		else if (!strncmp(&data[i], "&&", 2))
		{
			ls_addtok(&l, LS_2AMPERSAND, i, 2);
			++i;
		}
		else if (!strncmp(&data[i], "||", 2))
		{
			ls_addtok(&l, LS_2PIPE, i, 2);
			++i;
		}
		else if (!strncmp(&data[i], "^^", 2))
		{
			ls_addtok(&l, LS_2CARET, i, 2);
			++i;
		}
		else if (data[i] == '?')
		{
			ls_addtok(&l, LS_QUESTION, i, 1);
		}
		else if (data[i] == ':')
		{
			ls_addtok(&l, LS_COLON, i, 1);
		}
		else if (data[i] == ';')
		{
			ls_addtok(&l, LS_SEMICOLON, i, 1);
		}
		else if (data[i] == '{')
		{
			ls_addtok(&l, LS_LBRACE, i, 1);
		}
		else if (data[i] == '}')
		{
			ls_addtok(&l, LS_RBRACE, i, 1);
		}
		else if (data[i] == ',')
		{
			ls_addtok(&l, LS_COMMA, i, 1);
		}
		else
		{
			ls_destroylex(&l);
			return (ls_err_t)
			{
				.code = 1,
				.pos = i,
				.len = 1,
				.msg = ls_strdup("unrecognized character")
			};
		}
	}
	
	*out = l;
	return (ls_err_t){0};
}

void
ls_addtok(ls_lex_t *l, ls_toktype_t type, uint32_t pos, uint32_t len)
{
	if (l->ntoks >= l->tokcap)
	{
		l->tokcap *= 2;
		l->toks = ls_reallocarray(l->toks, l->tokcap, sizeof(ls_tok_t)),
		l->types = ls_reallocarray(l->types, l->tokcap, 1);
	}
	
	l->toks[l->ntoks] = (ls_tok_t)
	{
		.pos = pos,
		.len = len
	};
	l->types[l->ntoks] = type;
	++l->ntoks;
}

void
ls_readtokraw(char out[], char const *data, ls_tok_t const *t)
{
	memcpy(out, &data[t->pos], t->len);
}

void
ls_readtokstr(char out[], char const *data, ls_tok_t const *t)
{
	// TODO: implement ls_readtokstr().
}

void
ls_printtok(FILE *fp, ls_tok_t tok, ls_toktype_t type)
{
	fprintf(fp, "%-14s%u+%u\n", ls_toknames[type], tok.pos, tok.len);
}

void
ls_destroylex(ls_lex_t *l)
{
	if (l->toks)
	{
		ls_free(l->toks);
	}
	if (l->types)
	{
		ls_free(l->types);
	}
}

static ls_err_t
ls_lexword(ls_lex_t *l, char const *name, char const *data, uint32_t len, size_t *i)
{
	size_t begin = *i;
	
	size_t end = begin;
	while (end < len && (isalnum(data[end]) || data[end] == '_'))
	{
		++end;
	}
	
	for (size_t kw = LS_KWBOOL; kw <= LS_KWWHILE; ++kw)
	{
		size_t kwlen = strlen(ls_toknames[kw]);
		
		if (kwlen != end - begin)
		{
			continue;
		}
		
		if (strncmp(&data[begin], ls_toknames[kw], kwlen))
		{
			continue;
		}
		
		ls_addtok(l, kw, begin, kwlen);
		*i = end - 1;
		return (ls_err_t){0};
	}
	
	if (end - begin > LS_MAXIDENT)
	{
		return (ls_err_t)
		{
			.code = 1,
			.pos = begin,
			.len = end - begin,
			.msg = ls_strdup("identifier exceeds " LS_STRINGIFY(LS_MAXIDENT) " characters")
		};
	}
	
	ls_addtok(l, LS_IDENT, begin, end - begin);
	*i = end - 1;
	return (ls_err_t){0};
}

static ls_err_t
ls_lexstr(
	ls_lex_t *l,
	char const *name,
	char const *data,
	uint32_t len,
	size_t *i
)
{
	size_t begin = *i;
	
	size_t end = begin + 1;
	while (end < len && data[end] != '"')
	{
		if (data[end] == '\\')
		{
			if (end + 1 >= len)
			{
				return (ls_err_t)
				{
					.code = 1,
					.pos = end + 1,
					.len = 1,
					.msg = ls_strdup("expected an escape sequence")
				};
			}
			
			++end;
		}
		
		++end;
	}
	--begin;
	
	if (end >= len)
	{
		return (ls_err_t)
		{
			.code = 1,
			.pos = begin,
			.len = end - begin,
			.msg = ls_strdup("unterminated string")
		};
	}
	
	ls_addtok(l, LS_LITSTR, begin, end - begin + 1);
	*i = end;
	return (ls_err_t){0};
}

static ls_err_t
ls_lexnum(
	ls_lex_t *l,
	char const *name,
	char const *data,
	uint32_t len,
	size_t *i
)
{
	size_t begin = *i;
	
	uint32_t ndp = 0;
	size_t end = begin;
	while (end < len && (isdigit(data[end]) || data[end] == '.'))
	{
		ndp += data[end] == '.';
		++end;
	}
	
	if (!ndp)
	{
		ls_addtok(l, LS_LITINT, begin, end - begin);
		*i = end - 1;
		return (ls_err_t){0};
	}
	
	if (ndp > 1)
	{
		return (ls_err_t)
		{
			.code = 1,
			.pos = begin,
			.len = end - begin,
			.msg = ls_strdup("real literal contains more than one decimal point")
		};
	}
	
	ls_addtok(l, LS_LITREAL, begin, end - begin);
	*i = end - 1;
	return (ls_err_t){0};
}

static void
ls_lexcomment(char const *data, uint32_t len, size_t *i)
{
	size_t end = *i;
	while (end < len && data[end] != '\n')
	{
		++end;
	}
	
	*i = end;
}
