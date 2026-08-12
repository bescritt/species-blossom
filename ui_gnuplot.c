#include <stdio.h>
#include <stdlib.h>
#include "ui_gnuplot.h"

/** thx Joonas! **/

FILE *popen(const char *command, const char *type);
int pclose(FILE *stream);

static plotter_t *
pl_alloc()
{
    plotter_t *pl = (plotter_t *)malloc(sizeof(plotter_t));
    if (pl==NULL) return NULL;
    pl->cmd = NULL;
    pl->flags = 0;
    return pl;
}

void
pl_free(plotter_t *pl)
{
    free(pl);
}


plotter_t *
pl_open(FILE *cmdfile) {
    plotter_t *pl;
    if (( pl = pl_alloc() )) {
	pl->cmd = cmdfile;
	pl->flags |= PL_NOCLOSECMD;
	setvbuf(pl->cmd, NULL, _IOLBF, 0);
    }
    return pl;
}

plotter_t *
pl_popen()
{
    plotter_t *pl;
    if (( pl = pl_alloc() )) {
	fflush(stdout);
	if (!( pl->cmd = popen("gnuplot","w") )) {
	    perror("pl_popen(): can't open pipe to gnuplot");
	    pl_free(pl); pl = NULL;
	} else {
	    pl->flags |= PL_ISPIPE;
	    setvbuf(pl->cmd, NULL, _IOLBF, 0);
	}
    }
    return pl;
}

plotter_t *
pl_fopen(const char *cmdfilename, const char *mode)
{
    FILE *cmd;
    plotter_t *pl = NULL;
    if (( cmd = fopen(cmdfilename, mode) )) {
	if (( pl = pl_open(cmd) )) {
	    pl->flags &=~ PL_NOCLOSECMD;
	}
    } else {
	perror("pl_fopen(): can't open command file");
    }
    return pl;
}

void
pl_close(plotter_t *pl)
{
    if (!(pl->flags & PL_NOCLOSECMD)) {
	if (pl->flags & PL_ISPIPE) {
	    if (pclose(pl->cmd)) {
		perror("pl_close(): can't close command pipe");
	    }
	} else if ( fclose(pl->cmd) ) {
	    perror("pl_close(): can't close command file");
	}
    }
    pl_free(pl);
}

void
pl_vemitf(const plotter_t *pl, const char *fmt, va_list ap)
{
    vfprintf(pl->cmd, fmt, ap);
    if (ferror(pl->cmd)) {
	perror("pl_emit(): error writing to command file");
    }
}

void
pl_emit(const plotter_t *pl, const char *fmt, ...)
{
    va_list ap;
    va_start(ap, fmt);
    pl_vemitf(pl, fmt, ap);
    va_end(ap);
}

void
pl_cr(const plotter_t *pl)
{
    pl_emit(pl,"\n");
}

void
pl_emitln(const plotter_t *pl, const char *fmt, ...)
{
    va_list ap;
    va_start(ap, fmt);
    pl_vemitf(pl, fmt, ap);
    va_end(ap);
    pl_cr(pl);
}
