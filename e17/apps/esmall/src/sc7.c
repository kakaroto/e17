/*  Small compiler - Staging buffer and optimizer
 *
 *  The staging buffer
 *  ------------------
 *  The staging buffer allows buffered output of generated code, deletion
 *  of redundant code, optimization by a tinkering process and reversing
 *  the ouput of evaluated expressions (which is used for the reversed
 *  evaluation of arguments in functions).
 *  Initially, stgwrite() writes to the file directly, but after a call to
 *  stgset(_yes), output is redirected to the buffer. After a call to
 *  stgset(_no), stgwrite()'s output is directed to the file again. Thus
 *  only one routine is used for writing to the output, which can be
 *  buffered output or direct output.
 *
 *  staging buffer variables:   stgbuf  - the buffer
 *                              stgidx  - current index in the staging buffer
 *                              staging - if true, write to the staging buffer;
 *                                        if false, write to file directly.
 *
 *  Copyright (c) ITB CompuPhase, 1997-1999
 *  This file may be freely used. No warranties of any kind.
 */  
#include <assert.h>
#include <stdio.h>
#include <stdlib.h>     /* for atoi() */
#include <string.h>
#include <ctype.h>
#include "sc.h"

static void stgstring(char *start, char *end);

static void stgopt(char *start, char *end);

void writef(char *st);



static char stgbuf[_stgmax + 1];

/* the variables "stgidx" and "staging" are declared in "scvars.c" */ 

/*  stgmark
 *
 *  Copies a mark into the staging buffer. At this moment there are three
 *  possible marks:
 *     _startreorder    identifies the beginning of a series of expression
 *                      strings that must be written to the output file in
 *                      reordered order
 *    _endreorder       identifies the end of 'reverse evaluation'
 *    _exprstart + idx  only valid within a block that is evaluated in
 *                      reordered order, it identifies the start of an
 *                      expression; the "idx" value is the argument position
 *
 *  Global references: stgidx  (altered)
 *                     stgbuf  (altered)
 *                     staging (referred to only)
 */ 
void stgmark(char mark) 
{
	
		if (staging) {
		
			if (stgidx < _stgmax) {
			
				stgbuf[stgidx] = mark;
			
				stgidx += 1;
			
		} else {
			
				error(102, "staging buffer");	/* staging buffer overflow */
			
		}						/* if */
		
	}							/* if */
	
}


/*  stgwrite
 *
 *  Writes the string "st" to the staging buffer or to the output file. In the
 *  case of writing to the staging buffer, the terminating byte of zero is
 *  copied too, but... the optimizer can only work on complete lines (not on
 *  fractions of it. Therefore if the string is staged, if the last character
 *  written to the buffer is a '\0' and the previous-to-last is not a '\n',
 *  the string is concatenated to the last string in the buffer (the '\0' is
 *  overwritten). This also means an '\n' used in the middle of a string isn't
 *  recognized and could give wrong results with the optimizer.
 *  Even when writing to the output file directly, all strings are buffered
 *  until a whole line is complete.
 *
 *  Global references: stgidx  (altered)
 *                     stgbuf  (altered)
 *                     staging (referred to only)
 */ 
void stgwrite(char *st) 
{
	
		int len;
	
		
		if (staging) {
		
			if (stgidx >= 2 && stgbuf[stgidx - 1] == '\0' && stgbuf[stgidx - 2] != '\n')
			
				stgidx -= 1;	/* overwrite last '\0' */
		
			while (*st && stgidx < _stgmax) {	/* copy to staging buffer */
			
				stgbuf[stgidx] = *st;
			
				st += 1;
			
				stgidx += 1;
			
		}						/* endwhile */
		
			if (*st) {			/* check buffer overflow (is entire string written?) */
			
				error(102, "staging buffer");	/* staging buffer overflow */
			
		} else {				/* terminate string */
			
				stgbuf[stgidx] = '\0';
			
				stgidx += 1;
			
		}						/* endif */
		
	} else {
		
			strcat(stgbuf, st);
		
			len = strlen(stgbuf);
		
			if (len > 0 && stgbuf[len - 1] == '\n') {
			
				writef(stgbuf);
			
				stgbuf[0] = '\0';
			
		}						/* endif */
		
	}							/* endif */
	
}


/*  stgout
 *
 *  Writes the staging buffer to the output file via stgstring() (for
 *  reversing expressions in the buffer) and stgopt() (for optimizing). It
 *  resets "stgidx".
 *
 *  Global references: stgidx  (altered)
 *                     stgbuf  (referred to only)
 *                     staging (referred to only)
 */ 
void stgout(int index) 
{
	
		if (!staging)
		
			return;
	
		stgstring(&stgbuf[index], &stgbuf[stgidx]);
	
		stgidx = index;
	
}


typedef struct {
	
	char *start, *end;
	
} argstack;



/*  stgstring
 *
 *  Analyses whether code strings should be output to the file as they appear
 *  in the staging buffer or whether portions of it should be re-ordered.
 *  Re-ordering takes place in function argument lists; Small passes arguments
 *  to functions from right to left. When arguments are "named" rather than
 *  positional, the order in the source stream is indeterminate.
 *  This function calls itself recursively in case it needs to re-order code
 *  strings, and it uses a private stack (or list) to mark the start and the
 *  end of expressions in their correct (reversed) order.
 *  In any case, stgstring() sends a block as large as possible to the
 *  optimizer stgopt().
 *
 *  In "reorder" mode, each set of code strings must start with the token
 *  _exprstart, even the first. If the token _startreorder is represented
 *  by '[', _endreorder by ']' and _exprstart by '|' the following applies:
 *     '[]...'     valid, but useless; no output
 *     '[|...]     valid, but useless; only one string
 *     '[|...|...] valid and usefull
 *     '[...|...]  invalid, first string doesn't start with '|'
 *     '[|...|]    invalid
 */ 
static void stgstring(char *start, char *end) 
{
	
		char *ptr;
	
		int nest, argc, arg;
	
		argstack * stack;
	
		
		while (start < end) {
		
			if (*start == _startreorder) {
			
				start += 1;		/* skip token */
			
			/* allocate a argstack with _maxargs items */ 
				stack = (argstack *) malloc(_maxargs * sizeof(argstack));
			
				if (stack == NULL)
				
					error(103);	/* insufficient memory */
			
				nest = 1;		/* nesting counter */
			
				argc = 0;		/* argument counter */
			
				arg = -1;		/* argument index; no valid argument yet */
			
				do {
				
					switch (*start) {
						
					case _startreorder:
						
							nest++;
						
							start++;
						
							break;
						
					case _endreorder:
						
							nest--;
						
							start++;
						
							break;
						
					default:
						
							if ((*start & _exprstart) == _exprstart) {
							
								if (nest == 1) {
								
									if (arg >= 0)
									
										stack[arg].end = start - 1;		/* finish previous argument */
								
									arg = (unsigned char) *start - _exprstart;
								
									stack[arg].start = start + 1;
								
									if (arg >= argc)
									
										argc = arg + 1;
								
							}	/* if */
							
								start++;
							
						} else {
							
								start += strlen(start) + 1;
							
						}		/* if */
						
				}				/* switch */
				
			} while (nest);		/* enddo */
			
				if (arg >= 0)
				
					stack[arg].end = start - 1;		/* finish previous argument */
			
				while (argc > 0) {
				
					argc--;
				
					stgstring(stack[argc].start, stack[argc].end);
				
			}					/* while */
			
				free(stack);
			
		} else {
			
				ptr = start;
			
				while (*ptr != _startreorder && ptr < end)
				
					ptr += strlen(ptr) + 1;
			
				stgopt(start, ptr);
			
				start = ptr;
			
		}						/* if */
		
	}							/* while */
	
}


/*  stgdel
 *
 *  Scraps code from the staging buffer by resetting "stgidx" to "index".
 *
 *  Global references: stgidx (altered)
 *                     staging (reffered to only)
 */ 
void stgdel(int index, cell code_index) 
{
	
		if (staging) {
		
			stgidx = index;
		
			code_idx = code_index;
		
	}							/* if */
	
}


int stgget(int *index, cell * code_index) 
{
	
		if (staging) {
		
			*index = stgidx;
		
			*code_index = code_idx;
		
	}							/* if */
	
		return staging;
	
}


/*  stgset
 *
 *  Sets staging on or off. If it's turned off, the staging buffer must be
 *  initialized to an empty string. If it's turned on, the routine makes sure
 *  the index ("stgidx") is set to 0 (it should already be 0).
 *
 *  Global references: staging  (altered)
 *                     stgidx   (altered)
 *                     stgbuf   (contents altered)
 */ 
void stgset(int onoff) 
{
	
		staging = onoff;
	
		if (staging) {
		
			assert(stgidx == 0);
		
			stgidx = 0;
		
			if (strlen(stgbuf) > 0)		/* write any contents that may be put in the */
			
				writef(stgbuf);	/* buffer by stgwrite() when "staging" was 0 */
		
	}							/* endif */
	
		stgbuf[0] = '\0';
	
}


/*  stgopt
 *
 *  Optimizes the staging buffer by checking for series of instructions that
 *  can be coded more compact. The routine expects the lines in the staging
 *  buffer to be separated with a newline character.
 *
 *  The longest sequences must be checked first.
 */ 
#define seqsize(o,p)    (opcodes(o)+opargs(p))
typedef struct {
	
	char *find;
	
	char *replace;
	
	int savesize;				/* number of bytes saved (in bytecode) */
	
} SEQUENCE;


static SEQUENCE sequences[] =
{
#if !defined NO_OPTIMIZE
  /* A very common sequence in four varieties
   *    load.s.pri n1           load.s.alt n1
   *    push.pri                load.s.pri n2
   *    load.s.pri n2           -
   *    pop.alt                 -
   *    --------------------------------------
   *    load.pri n1             load.alt n1
   *    push.pri                load.s.pri n2
   *    load.s.pri n2           -
   *    pop.alt                 -
   *    --------------------------------------
   *    load.s.pri n1           load.s.alt n1
   *    push.pri                load.pri n2
   *    load.pri n2             -
   *    pop.alt                 -
   *    --------------------------------------
   *    load.pri n1             load.alt n1
   *    push.pri                load.pri n2
   *    load.pri n2             -
   *    pop.alt                 -
   */ 
 {"load.s.pri %1!push.pri!load.s.pri %2!pop.alt!", 
  "load.s.alt %1!load.s.pri %2!", 
  seqsize(4, 2) - seqsize(2, 2) 
 }, 
 {"load.pri %1!push.pri!load.s.pri %2!pop.alt!", 
  "load.alt %1!load.s.pri %2!", 
  seqsize(4, 2) - seqsize(2, 2) 
 }, 
 {"load.s.pri %1!push.pri!load.pri %2!pop.alt!", 
  "load.s.alt %1!load.pri %2!", 
  seqsize(4, 2) - seqsize(2, 2) 
 }, 
 {"load.pri %1!push.pri!load.pri %2!pop.alt!", 
  "load.alt %1!load.pri %2!", 
  seqsize(4, 2) - seqsize(2, 2) 
 }, 
  /* (#1#) The above also occurs with "addr.pri" (array
   * indexing) as the first line; so that adds 2 cases.
   */ 
 {"addr.pri %1!push.pri!load.s.pri %2!pop.alt!", 
  "addr.alt %1!load.s.pri %2!", 
  seqsize(4, 2) - seqsize(2, 2) 
 }, 
 {"addr.pri %1!push.pri!load.pri %2!pop.alt!", 
  "addr.alt %1!load.pri %2!", 
  seqsize(4, 2) - seqsize(2, 2) 
 }, 
  /* And the same sequence with const.pri as either the first
   * or the second load instruction: four more cases.
   */ 
 {"const.pri %1!push.pri!load.s.pri %2!pop.alt!", 
  "const.alt %1!load.s.pri %2!", 
  seqsize(4, 2) - seqsize(2, 2) 
 }, 
 {"const.pri %1!push.pri!load.pri %2!pop.alt!", 
  "const.alt %1!load.pri %2!", 
  seqsize(4, 2) - seqsize(2, 2) 
 }, 
 {"load.s.pri %1!push.pri!const.pri %2!pop.alt!", 
  "load.s.alt %1!const.pri %2!", 
  seqsize(4, 2) - seqsize(2, 2) 
 }, 
 {"load.pri %1!push.pri!const.pri %2!pop.alt!", 
  "load.alt %1!const.pri %2!", 
  seqsize(4, 2) - seqsize(2, 2) 
 }, 
  /* The same as above, but now with "addr.pri" (array
   * indexing) on the first line and const.pri on
   * the second.
   */ 
 {"addr.pri %1!push.pri!const.pri %2!pop.alt!", 
  "addr.alt %1!const.pri %2!", 
  seqsize(4, 2) - seqsize(2, 2) 
 }, 
  /* ??? add references */ 
  /* Array indexing can merit from special instructions.
   * Simple indexed array lookup can be optimized quite
   * a bit.
   *    addr.pri n1             addr.alt n1
   *    push.pri                load.s.pri n2
   *    load.s.pri n2           bounds n3
   *    bounds n3               lidx.b n4
   *    shl.c.pri n4            -
   *    pop.alt                 -
   *    add                     -
   *    load.i                  -
   *
   * And to prepare for storing a value in an array
   *    addr.pri n1             addr.alt n1
   *    push.pri                load.s.pri n2
   *    load.s.pri n2           bounds n3
   *    bounds n3               idxaddr.b n4
   *    shl.c.pri n4            -
   *    pop.alt                 -
   *    add                     -
   *
   * Notes (additional cases):
   * 1. instruction addr.pri can also be const.pri (for
   *    global arrays)
   * 2. the bounds instruction can be absent
   * 3. when "n4" (the shift value) is the 2 (with 32-bit cels), use the
   *    even more optimal instructions LIDX and IDDXADDR
   *
   * If the array index is more complex, one can only optimize
   * the last four instructions:
   *    shl.c.pri n1            pop.alt
   *    pop.alt                 lidx.b n1
   *    add                     -
   *    loadi                   -
   *    --------------------------------------
   *    shl.c.pri n1            pop.alt
   *    pop.alt                 idxaddr.b n1
   *    add                     -
   */ 
  /* loading from array, "cell" shifted */ 
#if !defined BIT16
 {"addr.pri %1!push.pri!load.s.pri %2!bounds %3!shl.c.pri 2!pop.alt!add!load.i!", 
  "addr.alt %1!load.s.pri %2!bounds %3!lidx!", 
  seqsize(8, 4) - seqsize(4, 3) 
 }, 
 {"const.pri %1!push.pri!load.s.pri %2!bounds %3!shl.c.pri 2!pop.alt!add!load.i!", 
  "const.alt %1!load.s.pri %2!bounds %3!lidx!", 
  seqsize(8, 4) - seqsize(4, 3) 
 }, 
 {"addr.pri %1!push.pri!load.s.pri %2!shl.c.pri 2!pop.alt!add!load.i!", 
  "addr.alt %1!load.s.pri %2!lidx!", 
  seqsize(7, 3) - seqsize(3, 2) 
 }, 
 {"const.pri %1!push.pri!load.s.pri %2!shl.c.pri 2!pop.alt!add!load.i!", 
  "const.alt %1!load.s.pri %2!lidx!", 
  seqsize(7, 3) - seqsize(3, 2) 
 }, 
#endif	/*  */
  /* loading from array, not "cell" shifted */ 
 {"addr.pri %1!push.pri!load.s.pri %2!bounds %3!shl.c.pri %4!pop.alt!add!load.i!", 
  "addr.alt %1!load.s.pri %2!bounds %3!lidx.b %4!", 
  seqsize(8, 4) - seqsize(4, 4) 
 }, 
 {"const.pri %1!push.pri!load.s.pri %2!bounds %3!shl.c.pri %4!pop.alt!add!load.i!", 
  "const.alt %1!load.s.pri %2!bounds %3!lidx.b %4!", 
  seqsize(8, 4) - seqsize(4, 4) 
 }, 
 {"addr.pri %1!push.pri!load.s.pri %2!shl.c.pri %3!pop.alt!add!load.i!", 
  "addr.alt %1!load.s.pri %2!lidx.b %3!", 
  seqsize(7, 3) - seqsize(3, 3) 
 }, 
 {"const.pri %1!push.pri!load.s.pri %2!shl.c.pri %3!pop.alt!add!load.i!", 
  "const.alt %1!load.s.pri %2!lidx.b %3!", 
  seqsize(7, 3) - seqsize(3, 3) 
 }, 
#if !defined BIT16
  /* array index calculation for storing a value, "cell" aligned */ 
 {"addr.pri %1!push.pri!load.s.pri %2!bounds %3!shl.c.pri 2!pop.alt!add!", 
  "addr.alt %1!load.s.pri %2!bounds %3!idxaddr!", 
  seqsize(7, 4) - seqsize(4, 3) 
 }, 
 {"const.pri %1!push.pri!load.s.pri %2!bounds %3!shl.c.pri 2!pop.alt!add!", 
  "const.alt %1!load.s.pri %2!bounds %3!idxaddr!", 
  seqsize(7, 4) - seqsize(4, 3) 
 }, 
 {"addr.pri %1!push.pri!load.s.pri %2!shl.c.pri 2!pop.alt!add!", 
  "addr.alt %1!load.s.pri %2!idxaddr!", 
  seqsize(6, 3) - seqsize(3, 2) 
 }, 
 {"const.pri %1!push.pri!load.s.pri %2!shl.c.pri 2!pop.alt!add!", 
  "const.alt %1!load.s.pri %2!idxaddr!", 
  seqsize(6, 3) - seqsize(3, 2) 
 }, 
#endif	/*  */
  /* array index calculation for storing a value, not "cell" packed */ 
 {"addr.pri %1!push.pri!load.s.pri %2!bounds %3!shl.c.pri %4!pop.alt!add!", 
  "addr.alt %1!load.s.pri %2!bounds %3!idxaddr.b %4!", 
  seqsize(7, 4) - seqsize(4, 4) 
 }, 
 {"const.pri %1!push.pri!load.s.pri %2!bounds %3!shl.c.pri %4!pop.alt!add!", 
  "const.alt %1!load.s.pri %2!bounds %3!idxaddr.b %4!", 
  seqsize(7, 4) - seqsize(4, 4) 
 }, 
 {"addr.pri %1!push.pri!load.s.pri %2!shl.c.pri %3!pop.alt!add!", 
  "addr.alt %1!load.s.pri %2!idxaddr.b %3!", 
  seqsize(6, 3) - seqsize(3, 3) 
 }, 
 {"const.pri %1!push.pri!load.s.pri %2!shl.c.pri %3!pop.alt!add!", 
  "const.alt %1!load.s.pri %2!idxaddr.b %3!", 
  seqsize(6, 3) - seqsize(3, 3) 
 }, 
  /* the shorter array indexing sequences, see above for comments */ 
#if !defined BIT16
 {"shl.c.pri 2!pop.alt!add!loadi!", 
  "pop.alt!lidx!", 
  seqsize(4, 1) - seqsize(2, 0) 
 }, 
 {"shl.c.pri 2!pop.alt!add!", 
  "pop.alt!idxaddr!", 
  seqsize(3, 1) - seqsize(2, 0) 
 }, 
#endif	/*  */
 {"shl.c.pri %1!pop.alt!add!loadi!", 
  "pop.alt!lidx.b %1!", 
  seqsize(4, 1) - seqsize(2, 1) 
 }, 
 {"shl.c.pri %1!pop.alt!add!", 
  "pop.alt!idxaddr.b %1!", 
  seqsize(3, 1) - seqsize(2, 1) 
 }, 
  /* For packed arrays, there is another case (packed arrays
   * do not take advantage of the LIDX or IDXADDR instructions).
   *    addr.pri n1             addr.alt n1
   *    push.pri                load.s.pri n2
   *    load.s.pri n2           bounds n3
   *    bounds n3               -
   *    pop.alt                 -
   *
   * Notes (additional cases):
   * 1. instruction addr.pri can also be const.pri (for
   *    global arrays)
   * 2. the bounds instruction can be absent, but that
   *    case is already handled (see #1#)
   */ 
 {"addr.pri %1!push.pri!load.s.pri %2!bounds %3!pop.alt!", 
  "addr.alt %1!load.s.pri %2!bounds %3!", 
  seqsize(5, 3) - seqsize(3, 3) 
 }, 
 {"const.pri %1!push.pri!load.s.pri %2!bounds %3!pop.alt!", 
  "const.alt %1!load.s.pri %2!bounds %3!", 
  seqsize(5, 3) - seqsize(3, 3) 
 }, 
  /* During a calculation, the intermediate result must sometimes
   * be moved from PRI to ALT, like in:
   *    push.pri                move.alt
   *    load.s.pri n1           load.s.pri n1
   *    pop.alt                 -
   *
   * The above also accurs for "load.pri" and for "const.pri",
   * so add another two cases. Also note that "const.pri 0"
   * can be optimized (which adds another case).
   */ 
 {"push.pri!load.s.pri %1!pop.alt!", 
  "move.alt!load.s.pri %1!", 
  seqsize(3, 1) - seqsize(2, 1) 
 }, 
 {"push.pri!load.pri %1!pop.alt!", 
  "move.alt!load.pri %1!", 
  seqsize(3, 1) - seqsize(2, 1) 
 }, 
 {"push.pri!const.pri 0!pop.alt!", 
  "move.alt!zero.pri!", 
  seqsize(3, 1) - seqsize(2, 0) 
 }, 
 {"push.pri!const.pri %1!pop.alt!", 
  "move.alt!const.pri %1!", 
  seqsize(3, 1) - seqsize(2, 1) 
 }, 
 {"push.pri!zero.pri!pop.alt!", 
  "move.alt!zero.pri!", 
  seqsize(3, 0) - seqsize(2, 0) 
 }, 
  /* saving PRI and then loading from its address
   * occurs when indexing a multi-dimensional array
   */ 
 {"push.pri!load.i!pop.alt!", 
  "move.alt!load.i!", 
  seqsize(3, 0) - seqsize(2, 0) 
 }, 
  /* An even simpler PUSH/POP optimization (occurs in
   * switch statements):
   *    push.pri                move.alt
   *    pop.alt                 -
   */ 
 {"push.pri!pop.alt!", 
  "move.alt!", 
  seqsize(2, 0) - seqsize(1, 0) 
 }, 
  /* Functions with many parameters with the same default
   * value have sequences like:
   *    const.pri n1            const.pri n1
   *    push.pri                push.r.pri n2   ; where n2 is the number of pushes
   *    conts.pri n1            -
   *    push.pri                -
   *    etc.                    etc.
   * We start with sequences of 5, assuming that longer sequences
   * will be rare. The shortest matched sequence is 3, because a sequence
   * of two can also be optimized as two "push.c n1" instructions.
   */ 
 {"const.pri %1!push.pri!const.pri %1!push.pri!const.pri %1!push.pri!" 
  "const.pri %1!push.pri!const.pri %1!push.pri!", 
  "const.pri %1!push.r.pri 5!", 
  seqsize(10, 5) - seqsize(2, 2) 
 }, 
 {"const.pri %1!push.pri!const.pri %1!push.pri!const.pri %1!push.pri!" 
  "const.pri %1!push.pri!", 
  "const.pri %1!push.r.pri 4!", 
  seqsize(8, 4) - seqsize(2, 2) 
 }, 
 {"const.pri %1!push.pri!const.pri %1!push.pri!const.pri %1!push.pri!", 
  "const.pri %1!push.r.pri 3!", 
  seqsize(6, 3) - seqsize(2, 2) 
 }, 
  /* Function calls (parameters are passed on the stack)
   *    load.s.pri n1           push.s n1
   *    push.pri                -
   *    --------------------------------------
   *    load.pri n1             push n1
   *    push.pri                -
   *    --------------------------------------
   *    const.pri n1            push.c n1
   *    push.pri                -
   *    --------------------------------------
   *    zero.pri                push.c 0
   *    push.pri                -
   *
   * However, PRI must not be needed after this instruction
   * if this shortcut is used. Check for LOAD.I first.
   */ 
 {"load.s.pri %1!push.pri!load.i!", 
  "load.s.pri %1!push.pri!load.i!", 0 
 }, 
 {"load.pri %1!push.pri!load.i!", 
  "load.pri %1!push.pri!load.i!", 0 
 }, 
 {"const.pri %1!push.pri!load.i!", 
  "const.pri %1!push.pri!load.i!", 0 
 }, 
 {"zero.pri!push.pri!load.i!", 
  "zero.pri!push.pri!load.i!", 0 
 }, 
 {"load.s.pri %1!push.pri!", 
  "push.s %1!", 
  seqsize(2, 1) - seqsize(1, 1) 
 }, 
 {"load.pri %1!push.pri!", 
  "push %1!", 
  seqsize(2, 1) - seqsize(1, 1) 
 }, 
 {"const.pri %1!push.pri!", 
  "push.c %1!", 
  seqsize(2, 1) - seqsize(1, 1) 
 }, 
 {"zero.pri!push.pri!", 
  "push.c 0!", 
  seqsize(2, 0) - seqsize(1, 1) 
 }, 
  /* References with a default value generate new cells on the heap
   * dynamically. That code often ends with:
   *    move.pri                push.alt
   *    push.pri                -
   */ 
 {"move.pri!push.pri!", 
  "push.alt!", 
  seqsize(2, 0) - seqsize(1, 0) 
 }, 
  /* Simple arithmetic operations on constants. Noteworthy is the
   * subtraction of a constant, since it is converted to the addition
   * of the inverse value.
   *    const.alt n1            add.c n1
   *    add                     -
   *    --------------------------------------
   *    const.alt n1            add.c -n1
   *    sub                     -
   *    --------------------------------------
   *    const.alt n1            smul.c n1
   *    smul                    -
   *    --------------------------------------
   *    const.alt n1            eq.c.pri n1
   *    eq                      -
   *    --------------------------------------
   */ 
 {"const.alt %1!add!", 
  "add.c %1!", 
  seqsize(2, 1) - seqsize(1, 1) 
 }, 
 {"const.alt %1!sub!", 
  "add.c -%1!", 
  seqsize(2, 1) - seqsize(1, 1) 
 }, 
 {"const.alt %1!smul!", 
  "smul.c %1!", 
  seqsize(2, 1) - seqsize(1, 1) 
 }, 
 {"const.alt %1!eq!", 
  "eq.c.pri %1!", 
  seqsize(2, 1) - seqsize(1, 1) 
 }, 
  /* Compare and jump
   *    eq                      jneq n1
   *    jzer n1                 -
   *    --------------------------------------
   *    eq                      jeq n1
   *    jnz n1                  -
   *    --------------------------------------
   *    neq                     jeq n1
   *    jzer n1                 -
   *    --------------------------------------
   *    neq                     jneq n1
   *    jnz n1                  -
   * Compares followed by jzer occur much more
   * often than compares followed with jnz. So we
   * take the easy route here.
   *    less                    jgeq n1
   *    jzer n1                 -
   *    --------------------------------------
   *    leq                     jgrtr n1
   *    jzer n1                 -
   *    --------------------------------------
   *    grtr                    jleq n1
   *    jzer n1                 -
   *    --------------------------------------
   *    geq                     jless n1
   *    jzer n1                 -
   *    --------------------------------------
   *    sless                   jsgeq n1
   *    jzer n1                 -
   *    --------------------------------------
   *    sleq                    jsgrtr n1
   *    jzer n1                 -
   *    --------------------------------------
   *    sgrtr                   jsleq n1
   *    jzer n1                 -
   *    --------------------------------------
   *    sgeq                    jsless n1
   *    jzer n1                 -
   */ 
 {"eq!jzer %1!", 
  "jneq %1!", 
  seqsize(2, 1) - seqsize(1, 1) 
 }, 
 {"eq!jnz %1!", 
  "jeq %1!", 
  seqsize(2, 1) - seqsize(1, 1) 
 }, 
 {"neq!jzer %1!", 
  "jeq %1!", 
  seqsize(2, 1) - seqsize(1, 1) 
 }, 
 {"neq!jnz %1!", 
  "jneq %1!", 
  seqsize(2, 1) - seqsize(1, 1) 
 }, 
 {"less!jzer %1!", 
  "jgeq %1!", 
  seqsize(2, 1) - seqsize(1, 1) 
 }, 
 {"leq!jzer %1!", 
  "jgrtr %1!", 
  seqsize(2, 1) - seqsize(1, 1) 
 }, 
 {"grtr!jzer %1!", 
  "jleq %1!", 
  seqsize(2, 1) - seqsize(1, 1) 
 }, 
 {"geq!jzer %1!", 
  "jless %1!", 
  seqsize(2, 1) - seqsize(1, 1) 
 }, 
 {"sless!jzer %1!", 
  "jsgeq %1!", 
  seqsize(2, 1) - seqsize(1, 1) 
 }, 
 {"sleq!jzer %1!", 
  "jsgrtr %1!", 
  seqsize(2, 1) - seqsize(1, 1) 
 }, 
 {"sgrtr!jzer %1!", 
  "jsleq %1!", 
  seqsize(2, 1) - seqsize(1, 1) 
 }, 
 {"sgeq!jzer %1!", 
  "jsless %1!", 
  seqsize(2, 1) - seqsize(1, 1) 
 }, 
  /* Test for zero (common case, especially for strings)
   * E.g. the test expression of: "for (i=0; str{i}!=0; ++i)"
   *
   *    zero.alt                jzer n1
   *    jeq n1                  -
   *    --------------------------------------
   *    zero.alt                jnz n1
   *    jneq n1                 -
   */ 
 {"zero.alt!jeq %1!", 
  "jzer %1!", 
  seqsize(2, 1) - seqsize(1, 1) 
 }, 
 {"zero.alt!jneq %1!", 
  "jnz %1!", 
  seqsize(2, 1) - seqsize(1, 1) 
 }, 
  /* Incrementing and decrementing leaves a value in
   * in PRI which may not be used (for example, as the
   * third expression in a "for" loop).
   *    inc n1                  inc n1  ; ++n
   *    load.pri n1             -
   *    ;^;                     -
   *    --------------------------------------
   *    load.pri n1             inc n1  ; n++, e.g. "for (n=0; n<10; n++)"
   *    inc n1                  -
   *    ;^;                     -
   * Plus the varieties for stack relative increments
   * and decrements.
   */ 
 {"inc %1!load.pri %1!;^;!", 
  "inc %1!", 
  seqsize(2, 2) - seqsize(1, 1) 
 }, 
 {"load.pri %1!inc %1!;^;!", 
  "inc %1!", 
  seqsize(2, 2) - seqsize(1, 1) 
 }, 
 {"inc.s %1!load.s.pri %1!;^;!", 
  "inc.s %1!", 
  seqsize(2, 2) - seqsize(1, 1) 
 }, 
 {"load.s.pri %1!inc.s %1!;^;!", 
  "inc.s %1!", 
  seqsize(2, 2) - seqsize(1, 1) 
 }, 
 {"dec %1!load.pri %1!;^;!", 
  "dec %1!", 
  seqsize(2, 2) - seqsize(1, 1) 
 }, 
 {"load.pri %1!dec %1!;^;!", 
  "dec %1!", 
  seqsize(2, 2) - seqsize(1, 1) 
 }, 
 {"dec.s %1!load.s.pri %1!;^;!", 
  "dec.s %1!", 
  seqsize(2, 2) - seqsize(1, 1) 
 }, 
 {"load.s.pri %1!dec.s %1!;^;!", 
  "dec.s %1!", 
  seqsize(2, 2) - seqsize(1, 1) 
 }, 
  /* ??? the same (increments and decrements) for references */ 
  /* Loading the constant zero has a special opcode.
   *    const.pri 0             zero n1
   *    stor.pri n1             -
   *    ;^;                     -
   *    --------------------------------------
   *    const.pri 0             zero.s n1
   *    stor.s.pri n1           -
   *    ;^;                     -
   *    --------------------------------------
   *    zero.pri                zero n1
   *    stor.pri n1             -
   *    ;^;                     -
   *    --------------------------------------
   *    zero.pri                zero.s n1
   *    stor.s.pri n1           -
   *    ;^;                     -
   *    --------------------------------------
   *    const.pri 0             zero.pri
   *    --------------------------------------
   *    const.alt 0             zero.alt
   * The last two alternatives save more memory than they save
   * time, but anyway...
   */ 
 {"const.pri 0!stor.pri %1!;^;!", 
  "zero %1!", 
  seqsize(2, 2) - seqsize(1, 1) 
 }, 
 {"const.pri 0!stor.s.pri %1!;^;!", 
  "zero.s %1!", 
  seqsize(2, 2) - seqsize(1, 1) 
 }, 
 {"zero.pri!stor.pri %1!;^;!", 
  "zero %1!", 
  seqsize(2, 1) - seqsize(1, 1) 
 }, 
 {"zero.pri!stor.s.pri %1!;^;!", 
  "zero.s %1!", 
  seqsize(2, 1) - seqsize(1, 1) 
 }, 
 {"const.pri 0!", 
  "zero.pri!", 
  seqsize(1, 1) - seqsize(1, 0) 
 }, 
 {"const.alt 0!", 
  "zero.alt!", 
  seqsize(1, 1) - seqsize(1, 0) 
 }, 
#endif	/*  */
  /* ----- */ 
 {NULL, NULL, 0} 
};


#define _maxoptvars     4
#define _aliasmax       10      /* a 32-bit number can be represented in
                                 * 9 decimal digits */

static int matchsequence(char *start, char *end, char *pattern, 
						 char symbols[_maxoptvars][_aliasmax + 1], 
						 int *num) 
{
	
		int var, i;
	
		char str[_aliasmax + 1];
	
		
		*num = 0;
	
		for (var = 0; var < _maxoptvars; var++)
		
			symbols[var][0] = '\0';
	
		
		while (*start == '\t' || *start == ' ')
		
			start++;
	
		while (*pattern) {
		
			if (start >= end)
			
				return _no;
		
			switch (*pattern) {
				
			case '%':			/* new "symbol" */
				
					pattern++;
				
					assert(isdigit(*pattern));
				
					var = atoi(pattern) - 1;
				
					assert(var >= 0 && var < _maxoptvars);
				
					assert(alphanum(*start));
				
					for (i = 0; alphanum(*start); i++, start++) {
					
						assert(i <= _aliasmax);
					
						str[i] = *start;
					
				}				/* for */
				
					str[i] = '\0';
				
					if (symbols[var][0] != '\0') {
					
						if (strcmp(symbols[var], str) != 0)
						
							return _no;		/* symbols should be identical */
					
				} else {
					
						strcpy(symbols[var], str);
					
				}				/* if */
				
					break;
				
			case ' ':
				
					if (*start != '\t' && *start != ' ')
					
						return _no;
				
					while (*start == '\t' || *start == ' ')
					
						start++;
				
					break;
				
			case '!':
				
					while (*start == '\t' || *start == ' ')
					
						start++;	/* skip trailing white space */
				
					if (*start != '\n')
					
						return _no;
				
					assert(*(start + 1) == '\0');
				
					start += 2;	/* skip '\n' and '\0' */
				
					while (*start == '\t' || *start == ' ')
					
						start++;	/* skip leading white space */
				
					*num += 1;	/* one more matched line */
				
					break;
				
			default:
				
					if (tolower(*start) != tolower(*pattern))
					
						return _no;
				
					start++;
				
		}						/* switch */
		
			pattern++;
		
	}							/* while */
	
		
		return _yes;
	
}


static void replacesequence(char *pattern, char symbols[_maxoptvars][_aliasmax + 1]) 
{
	
		char line[60], *lptr;
	
		int var;
	
		
		lptr = line;
	
		*lptr++ = '\t';			/* the "replace" patterns do not have tabs */
	
		while (*pattern) {
		
			assert((int) (lptr - line) < 60);
		
			switch (*pattern) {
				
			case '%':
				
				/* write out what is accumulated for the line so far */ 
					*lptr = '\0';
				
					writef(line);
				
				/* write out the symbol */ 
					pattern++;
				
					assert(isdigit(*pattern));
				
					var = atoi(pattern) - 1;
				
					assert(var >= 0 && var < _maxoptvars);
				
					assert(symbols[var][0] != '\0');	/* variable should be defined */
				
					writef(symbols[var]);
				
				/* restart the line */ 
					lptr = line;
				
					break;
				
			case '!':
				
				/* write out what is accumulated for the line so far */ 
					*lptr++ = '\n';
				
					*lptr = '\0';
				
					writef(line);
				
					lptr = line;
				
					*lptr++ = '\t';
				
					break;
				
			default:
				
					*lptr++ = *pattern;
				
		}						/* switch */
		
			pattern++;
		
	}							/* while */
	
}


static void stgopt(char *start, char *end) 
{
	
		char symbols[_maxoptvars][_aliasmax + 1];
	
		int num, seq;
	
		
		while (start < end) {
		
			for (seq = 0; sequences[seq].find != NULL; seq++) {
			
				if (matchsequence(start, end, sequences[seq].find, symbols, &num)) {
				
					replacesequence(sequences[seq].replace, symbols);
				
				/* Skip the number if matched sequences, minus one. The
				 * last sequence is skipped after the "for" loop.
				 */ 
					while (num-- > 1) {
					
						start += strlen(start) + 1;
					
						assert(start < end);
					
				}				/* while */
				
					code_idx -= sequences[seq].savesize;
				
					break;		/* break out of "for" loop, do not try to match more */
				
			}					/* if */
			
		}						/* for */
		
			if (sequences[seq].find == NULL)	/* if not matched */
			
				writef(start);	/* transform line if needed */
		
			assert(start < end);
		
			start += strlen(start) + 1;		/* to next string */
		
	}							/* while */
	
}


/*  writef
 *
 *  writes a string to the output file
 *
 *  Global references: outf   (referred to only)
 */ 
void writef(char *st) 
{
	
		fputs(st, outf);
	
} 


