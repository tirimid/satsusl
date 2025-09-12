// SPDX-License-Identifier: BSD-3-Clause

typedef struct p_panel
{
	char inputfile[128];
	z_tfdata_t inputfiletf;
	char outputfile[128];
	z_tfdata_t outputfiletf;
	char modpaths[O_MAXMODPATHS][128];
	z_tfdata_t modpathtfs[O_MAXMODPATHS];
	char inputline[128];
	z_tfdata_t inputlinetf;
	char outputlines[O_NOUTPUTLINES][O_MAXIOLINE + 1];
} p_panel_t;

extern p_panel_t p_panel;

void p_run(void);
void p_clearoutput(void);
void p_pushoutput(char const *line);
i32 p_cget(void);
void p_cput(i32 c);
