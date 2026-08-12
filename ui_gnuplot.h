#ifndef UI_GNUPLOT_H
#define UI_GNUPLOT_H

/** thx Joonas! **/

#ifdef __cplusplus

extern "C" {

enum pl_flags {
    PL_NOCLOSECMD = 1<<0,
    PL_ISPIPE = 1<<1
};

struct plotter_t {
    FILE      *cmd;		/* file to emit commands to. */
    pl_flags  flags;		/* flag: 0 iff cmd is not a pipe to gnuplot */
};

#else

typedef enum {
    PL_NOCLOSECMD = 1<<0,
    PL_ISPIPE = 1<<1
} pl_flags;

typedef struct {
    FILE      *cmd;		/* file to emit commands to. */
    pl_flags  flags;		/* flag: 0 iff cmd is not a pipe to gnuplot */
} plotter_t;

#endif /* __cplusplus */

#include <stdio.h>
#include <stdarg.h>

plotter_t *pl_open(FILE *cmdfile);
/* Open a plotter that writes to command file cmdfile. */

plotter_t *pl_popen();
/* Open a plotter that writes directly to gnuplot through a pipe. */

plotter_t *pl_fopen(const char *cmdfilename, const char *mode);
/* Open a plotter that writes to a command file given in commandfilename.
   mode must grant write access to the file. */

void pl_close(plotter_t *pl);
/* Close the plotter and command file. */


void pl_emit(const plotter_t *pl, const char *fmt, ...);
void pl_vemitf(const plotter_t *pl, const char *fmt, va_list ap);
/* Emit the formatted string fmt,... to the plotter pl. */

void pl_emitln(const plotter_t *pl, const char* fmt, ...);
/* Emit a formatted string fmt,... to the plotter, and a new line. */

void pl_cr(const plotter_t *pl);
/* Emit a new line to the plotter. */

#ifdef __cplusplus
} // extern "C"
#endif /* __cplusplus */

#endif /* UI_GNUPLOT_H */
