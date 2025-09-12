// SPDX-License-Identifier: BSD-3-Clause

typedef struct p_panel
{
	// data buffers.
	char inputfile[128];
	char outputfile[128];
	char modpaths[O_MAXMODPATHS][128];
	char inputline[128];
	char outputlines[O_NOUTPUTLINES][O_MAXIOLINE + 1];
	
	// persistent UI elements.
	z_tfdata_t inputfiletf;
	z_tfdata_t outputfiletf;
	z_tfdata_t modpathtfs[O_MAXMODPATHS];
	z_tfdata_t inputlinetf;
	
	// cget / cput interface data.
	char cgetbuf[128];
	char cputbuf[128];
	usize cgetlen;
	usize cgetidx;
	usize cputlen;
} p_panel_t;

extern p_panel_t p_panel;

void p_run(void);
void p_clearoutput(void);
void p_pushoutput(char const *line);
i32 p_cget(void);
void p_cput(i32 c);
void p_lex(void);
void p_parse(void);
void p_import(void);
void p_sema(void);
void p_exec(void);
