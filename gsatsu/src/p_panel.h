// SPDX-License-Identifier: BSD-3-Clause

typedef struct p_panel
{
	// data buffers.
	char inputfile[128];
	char outputfile[128];
	char modpaths[O_MAXMODPATHS][128];
	char inputline[O_MAXINPUT];
	char outputlines[O_NOUTPUTLINES][O_MAXIOLINE + 1];
	
	// persistent UI elements.
	z_tfdata_t inputfiletf;
	z_tfdata_t outputfiletf;
	z_tfdata_t modpathtfs[O_MAXMODPATHS];
	z_tfdata_t inputlinetf;
	
	// cget / cput interface data.
	pthread_mutex_t cgetmutex;
	char cgetbuf[O_MAXINPUT];
	char cputbuf[O_MAXIOLINE + 1];
	usize cgetlen;
	usize cgetidx;
	usize cputlen;
	
	// execution data.
	bool running;
} p_panel_t;

extern p_panel_t p_panel;

void p_run(void);
void p_clearoutput(void);
void p_pushoutput(char const *line);
void p_err(char const *fmt, ...);
void p_errfile(char const *name, char const *data, usize datalen, usize pos, usize len, char const *fmt, ...);
void p_showfile(char const *name, char const *data, usize datalen, usize pos, usize len);
i32 p_cget(void);
void p_cput(i32 c);
