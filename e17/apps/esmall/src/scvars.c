/*  Small compiler
 *
 *  Global (cross-module) variables.
 *
 *  Copyright (c) ITB CompuPhase, 1997-1999
 *  This file may be freely used. No warranties of any kind.
 */
#include <stdio.h>
#include <stdlib.h>     /* for _MAX_PATH */
#include "sc.h"

/*  global variables
 *
 *  All global variables that are shared amongst the compiler files are
 *  declared here.
 */
symbol loctab;          /* local symbol table */
symbol glbtab;          /* global symbol table */
cell *litq;             /* the literal queue */
char pline[_linemax+1]; /* the line read from the input file */
char *lptr;             /* points to the current position in "pline" */
constval libname_tab = { "", 0, NULL};  /* library table (#pragma library "..." syntax) */
char *inpfname;         /* pointer to name of the file currently read from */
char outfname[_MAX_PATH];       /* output file name */
char errfname[_MAX_PATH];       /* error file name */
char includepath[_MAX_PATH];    /* directory for system include files */

char ctrlchar = CTRL_CHAR;      /* the control character (or escape character)*/
int litidx    = 0;      /* index to literal table */
int litmax    = _def_litmax;    /* current size of the literal table */
int stgidx    = 0;      /* index to the staging buffer */
int labnum    = 0;      /* number of (internal) labels */
int staging   = 0;      /* true if staging output */
cell declared = 0;      /* number of local cells declared */
cell glb_declared=0;    /* number of global bytes declared */
cell code_idx = 0;      /* number of bytes with generated code */
int ntv_funcid= 0;      /* incremental number of native function */
int errflag   = 0;      /* 1 after first error in statement, -1 if no... */
                        /* ...error on current line, but errors were found */
int errnum    = 0;      /* number of errors */
int warnnum   = 0;      /* number of warnings */
int verbose   = _no;    /* display extra information while compiling? */
int debug     = _chkbounds;     /* by default: bounds checking+assertions */
int charbits  = 8;      /* a "char" is 8 bits */
int packstr   = _no;    /* strings are packed by default? */
int needsemicolon=_yes; /* semicolon required to terminate expressions? */
int curseg    = 0;      /* 1 if currently parsing CODE, 2 if parsing DATA */
cell stksize  = 2048;   /* default stack size */
int freading  = _no;    /* Is there an input file ready for reading? */
int fline     = 0;      /* the line number in the current file */
int fnumber   = 0;      /* the file number in the file table (debugging) */
int fcurrent  = 0;      /* current file being processed (debugging) */
int intest    = 0;      /* true if inside a test */
int sideeffect= 0;      /* true if an expression causes a side-effect */
int stmtindent= 0;      /* current indent of the statement */
int indent_nowarn=0;    /* skip warning "217 loose indentation" */

FILE *inpf    = NULL;   /* file read from */
FILE *outf    = NULL;   /* file written to */
