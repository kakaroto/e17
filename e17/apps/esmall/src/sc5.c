/*  Small compiler - Error message system
 *  In fact a very simple system, using only 'panic mode'.
 *
 *  Copyright (c) ITB CompuPhase, 1997-1999
 *  This file may be freely used. No warranties of any kind.
 */
#include <assert.h>
#include <sys/io.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>     /* ANSI standardized variable argument list functions */
#include <string.h>
#include "sc.h"

static char *errmsg[] = {
/*001*/  "expected token: \"%s\", but found \"%s\"\n",
/*002*/  "local variables not allowed within \"switch\"\n",
/*003*/  "local variables disallowed between \"goto\" and its label\n",
/*004*/  "function \"%s\" not defined\n",
/*005*/  "function may not have arguments\n",
/*006*/  "must be assigned to an array\n",
/*007*/  "assertion failed\n",
/*008*/  "must be a constant expression; assumed zero\n",
/*009*/  "invalid array size (negative or zero)\n",
/*010*/  "invalid function or declaration\n",
/*011*/  "invalid outside functions\n",
/*012*/  "invalid function call, not a valid address\n",
/*013*/  "no entry point (no public functions)\n",
/*014*/  "invalid statement; not in switch\n",
/*015*/  "\"default\" case must be the last case in switch statement\n",
/*016*/  "multiple defaults in \"switch\"\n",
/*017*/  "undefined symbol \"%s\"\n",
/*018*/  "initialization data exceeds declared size\n",
/*019*/  "not a label: \"%s\"\n",
/*020*/  "invalid symbol name \"%s\"\n",
/*021*/  "symbol already defined: \"%s\"\n",
/*022*/  "must be lvalue\n",
/*023*/  "array assignment must be simple assignment\n",
/*024*/  "\"break\" or \"continue\" is out of context\n",
/*025*/  "function heading differs from prototype\n",
/*026*/  "no matching \"#if...\"\n",
/*027*/  "invalid character constant\n",
/*028*/  "cannot subscript, not an array\n",
/*029*/  "invalid expression, assumed zero\n",
/*030*/  "compound statement not closed at the end of file\n",
/*031*/  "unknown directive\n",
/*032*/  "array index out of bounds (variable \"%s\")\n",
/*033*/  "array must be indexed (variable \"%s\")\n",
/*034*/  "argument does not have a default value (argument %d)\n",
/*035*/  "argument type mismatch (argument %d)\n",
/*036*/  "empty statement\n",
/*037*/  "invalid string (possibly non-terminated string)\n",
/*038*/  "extra characters on line\n",
/*039*/  "constant symbol has no size\n",
/*040*/  "duplicate \"case\" label (value %d)\n",
/*041*/  "invalid ellipsis, array size is not known\n",
/*042*/  "invalid combination of class specifiers\n",
/*043*/  "character constant exceeds range for packed string\n",
/*044*/  "mixing named and positional parameters\n",
/*045*/  "too many function arguments\n",
/*046*/  "unknown array size (variable \"%s\")\n",
/*047*/  "array sizes must match\n",
/*048*/  "array dimensions must match\n",
/*049*/  "invalid line continuation\n",
/*050*/  "invalid range\n",
/*051*/  "invalid subscript, use \"[ ]\" operators on major dimensions\n",
/*052*/  "only the last dimension may be variable length\n",
/*053*/  "exceeding maximum number of dimensions\n",
/*054*/  "unmatched closing brace\n",
/*055*/  "start of function body without function header\n"
       };

static char *fatalmsg[] = {
/*100*/  "cannot read from file: \"%s\"\n",
/*101*/  "cannot write to file: \"%s\"\n",
/*102*/  "table overflow: \"%s\"\n",
          /* table can be: loop table
           *               staging buffer
           *               parser stack (recursive include?)
           *               option table (response file)
           */
/*103*/  "insufficient memory\n",
/*104*/  "invalid assembler instruction \"%s\"\n",
/*105*/  "numeric overflow, exceeding capacity\n"
       };

static char *warnmsg[] = {
/*200*/  "symbol is truncated to %d characters\n",
/*201*/  "redefinition of constant (symbol \"%s\")\n",
/*202*/  "number of arguments does not match definition\n",
/*203*/  "symbol is never used: \"%s\"\n",
/*204*/  "symbol is assigned a value that is never used: \"%s\"\n",
/*205*/  "redundant code: constant expression is zero\n",
/*206*/  "redundant test: constant expression is non-zero\n",
/*207*/  "unknown #pragma\n",
/*208*/  "function uses both \"return;\" and \"return <value>;\"\n",
/*209*/  "function should return a value\n",
/*210*/  "possible use of symbol before initialization: \"%s\"\n",
/*211*/  "possibly unintended assignment\n",
/*212*/  "possibly unintended bitwise operation\n",
/*213*/  "tagname mismatch\n",
/*214*/  "ambiguous mix of operators, use parentheses\n",
/*215*/  "expression has no effect\n",
/*216*/  "nested comment\n",
/*217*/  "loose indentation\n",
/*218*/  "old style prototypes used with optional semicolumns\n",
/*219*/  "local variable \"%s\" shadows a variable at a preceding level\n"
       };

/*  error
 *
 *  Outputs an error message (note: msg is passed optionally).
 *  If an error is found, the variable "errflag" is set an subsequent
 *  errors are ignored until lex() finds a semicolumn (lex() resets
 *  "errflag" in that case).
 *
 *  Global references: inpfname   (reffered to only)
 *                     lineno     (reffered to only)
 *                     verbose    (reffered to only)
 *                     lptr,pline (reffered to only)
 *                     errflag    (altered)
 */
int error(int number,...)
{
static char *prefix[3]={ "Error", "Fatal", "Warning" };
  char *msg, *pre;
  int column,i;
  va_list argptr;

  /* errflag is reset on each semicolon; fatal errors may never be ignored */
  if (errflag>0 && (number<100 || number>=200))
    return 0;

  column= (int)(lptr-pline)-1;  /* error probably occurred before current position */
  if (column<1)
    column=1;

  if (number<100){
    msg=errmsg[number-1];
    pre=prefix[0];
    errflag=1;          /* set errflag (skip rest of erroneous expression) */
    errnum++;
  } else if (number<200){
    msg=fatalmsg[number-100];
    pre=prefix[1];
  } else {
    msg=warnmsg[number-200];
    pre=prefix[2];
    warnnum++;
  } /* if */

  assert(!(verbose && strlen(errfname)>0));
  if (verbose){         /* write erroneous line and move a pointer ^ to... */
    printf("%s",pline); /* ...the error position */
    i=0;
    while (i++<column)
      putchar(' ');
    puts("^");
  } /* if */
  if (strlen(errfname)==0) {
    printf("%s(%d) %s [%d]: ",inpfname,fline,pre,number);
    va_start(argptr,number);
    vprintf(msg,argptr);
  } else {
    FILE *fp=fopen(errfname,"at");
    if (fp!=NULL) {
      fprintf(fp,"%s(%d) %s [%d]: ",inpfname,fline,pre,number);
      va_start(argptr,number);
      vfprintf(fp,msg,argptr);
      fclose(fp);
    } /* if */
  } /* if */

  if (number>=100 && number<200 || errnum>25){
    if (strlen(errfname)==0)
      puts("\nCompilation aborted.");
    unlink(outfname);
    exit(2);      /* fatal error, quit */
  } /* if */
  return 0;
}

