/*  Small compiler - code generation (unoptimized "assembler" code)
 *
 *  Copyright (c) ITB CompuPhase, 1997-1999
 *  This file may be freely used. No warranties of any kind.
 */
#include <assert.h>
#include <stdio.h>
#include <stdlib.h>     /* for _MAX_PATH */
#include <string.h>
#include "sc.h"

/*  writetrailer
 *  Not much left of this once important function.
 *
 *  Global references: stksize          (referred to only)
 */
void writetrailer(void)
{
  stgwrite("\nSTKSIZE ");     /* write stack size */
  outval(stksize, _yes);
}

/*
 *  Start (or restart) the CODE segment.
 *
 *  In fact, the code and data segment specifiers are purely informational;
 *  the "DUMP" instruction itself already specifies that the following values
 *  should go to the data segment. All otherinstructions go to the code
 *  segment.
 *
 *  Global references: curseg
 */
void begcseg(void)
{
  if (curseg!=_incseg) {
    stgwrite("\n");
    stgwrite("CODE\n");
    curseg=_incseg;
  } /* endif */
}

/*
 *  Start (or restart) the DATA segment.
 *
 *  Global references: curseg
 */
void begdseg(void)
{
  if (curseg!=_indseg) {
    stgwrite("\n");
    stgwrite("DATA\n");
    curseg=_indseg;
  } /* if */
}

cell nameincells(char *name)
{
  cell clen=(strlen(name)+sizeof(cell)) & ~(sizeof(cell)-1);
  return clen;
}

void setfile(char *name,int fileno)
{
  if ((debug & _symbolic)!=0) {
    begcseg();
    stgwrite("file ");
    stgwrite(name);
    stgwrite(" ");
    outval(fileno,_yes);
    /* calculate code length */
    code_idx+=opcodes(1)+opargs(2)+nameincells(name);
  } /* if */
}

void setline(int line,int fileno)
{
  if ((debug & _symbolic)!=0 || (debug & _chkbounds)!=0) {
    stgwrite("line ");
    outval(line,_no);
    stgwrite(" ");
    outval(fileno,_no);
    stgwrite("\t; ");
    outval(code_idx,_yes);
    code_idx+=opcodes(1)+opargs(2);
  } /* if */
}

/*  postlab2
 *
 *  Post a code label (specified as a number) into the code stream, (but not
 *  on a new line).
 */
static void postlab2(int number)
{
  stgwrite((char *)itoh(number));
}

/*  setlabel
 *
 *  Post a code label (specified as a number), on a new line.
 */
void setlabel(int number)
{
  assert(number>=0);
  stgwrite("l.");
  postlab2(number);
  stgwrite("\t; ");
  outval(code_idx,_yes);
}

/* Write a token that signifies the end of an expression. This
 * allows several simple optimizations by the peeohole optimizer.
 */
void endexpr(void)
{
  stgwrite("\t;^;\n");
}

/*  startfunc   - declare a CODE entry point (function start)
 *
 *  Global references: funcstatus  (referred to only)
 */
void startfunc(void)
{
  stgwrite("\tproc\t; ");
  outval(code_idx,_yes);
  code_idx+=opcodes(1);
}

/*  endfunc
 *
 *  Declare a CODE ending point (function end)
 */
void endfunc(void)
{
  stgwrite("\n");       /* skip a line */
}

/*  Define a variable or function
 */
void defsymbol(char *name,int ident,int vclass,cell offset)
{
  if ((debug & _symbolic)!=0) {
    begcseg();          /* symbol definition in code segment */
    stgwrite("symbol ");

    stgwrite(name);
    stgwrite(" ");

    outval(offset,_no);
    stgwrite(" ");

    outval(vclass,_no);
    stgwrite(" ");

    outval(ident,_yes);

    code_idx+=opcodes(1)+opargs(3)+nameincells(name);  /* class and ident encoded in "flags" */
  } /* if */
}

void symbolrange(int level,cell size)
{
  if ((debug & _symbolic)!=0) {
    begcseg();          /* symbol definition in code segment */
    stgwrite("srange ");
    outval(level,_no);
    stgwrite(" ");
    outval(size,_yes);
    code_idx+=opcodes(1)+opargs(2);
  } /* if */
}

/*  rvalue
 *
 *  Generate code to get the value of a symbol into "primary".
 */
void rvalue(value *lval)
{
  symbol *sym;

  sym=lval->sym;
  if (lval->ident==_arraycell) {
    /* indirect fetch, address already in PRI */
    stgwrite("\tload.i\n");
    code_idx+=opcodes(1);
  } else if (lval->ident==_arraychar) {
    /* indirect fetch of a character from a pack, address already in PRI */
    stgwrite("\tlodb.i ");
    outval(charbits/8,_yes);    /* read one or two bytes */
    code_idx+=opcodes(1)+opargs(1);
  } else if (lval->ident==_reference) {
    /* indirect fetch, but address not yet in PRI */
    assert(sym!=NULL);
    assert(sym->vclass==_local);    /* global references don't exist in Small */
    if (sym->vclass==_local)
      stgwrite("\tlref.s.pri ");
    else
      stgwrite("\tlref.pri ");
    outval(sym->addr,_yes);
    sym->usage|=_read;
    code_idx+=opcodes(1)+opargs(1);
  } else {
    /* direct or stack relative fetch */
    assert(sym!=NULL);
    if (sym->vclass==_local)
      stgwrite("\tload.s.pri ");
    else
      stgwrite("\tload.pri ");
    outval(sym->addr,_yes);
    sym->usage|=_read;
    code_idx+=opcodes(1)+opargs(1);
  } /* if */
}

/*
 *  Get the address of a symbol into the primary register (used for arrays,
 *  and for passing arguments by reference).
 */
void address(symbol *sym)
{
  assert(sym!=NULL);
  /* the symbol can be a local array, a global array, or an array
   * that is passed by reference.
   */
  if (sym->ident==_refarray) {
    /* reference to an array; currently this is always a local variable */
    stgwrite("\tload.s.pri ");
  } else {
    /* a local array or local variable */
    if (sym->vclass==_local)
      stgwrite("\taddr.pri ");
    else
      stgwrite("\tconst.pri ");
  } /* if */
  outval(sym->addr,_yes);
  sym->usage|=_read;
  code_idx+=opcodes(1)+opargs(1);
}

/*  store
 *
 *  Saves the contents of "primary" into a memory cell, either directly
 *  or indirectly (at the address given in the alternate register).
 */
void store(value *lval)
{
  symbol *sym;

  sym=lval->sym;
  if (lval->ident==_arraycell) {
    stgwrite("\tstor.i\n");
    code_idx+=opcodes(1);
  } else if (lval->ident==_arraychar) {
    stgwrite("\tstrb.i ");
    outval(charbits/8,_yes);    /* write one or two bytes */
    code_idx+=opcodes(1)+opargs(1);
  } else if (lval->ident==_reference) {
    assert(sym!=NULL);
    if (sym->vclass==_local)
      stgwrite("\tsref.s.pri ");
    else
      stgwrite("\tsref.pri ");
    outval(sym->addr,_yes);
    code_idx+=opcodes(1)+opargs(1);
  } else {
    assert(sym!=NULL);
    sym->usage|=_written;
    if (sym->vclass==_local)
      stgwrite("\tstor.s.pri ");
    else
      stgwrite("\tstor.pri ");
    outval(sym->addr,_yes);
    code_idx+=opcodes(1)+opargs(1);
  } /* if */
}

/* source must in PRI, destination address in ALT. The "size"
 * parameter is in bytes, not cells.
 */
void memcopy(cell size)
{
  stgwrite("\tmovs ");
  outval(size,_yes);

  code_idx+=opcodes(1)+opargs(1);
}

/* Address of the source must already have been loaded in PRI
 * "size" is the size in bytes (not cells).
 */
void copyarray(symbol *sym,cell size)
{
  assert(sym!=NULL);
  /* the symbol can be a local array, a global array, or an array
   * that is passed by reference.
   */
  if (sym->ident==_refarray) {
    /* reference to an array; currently this is always a local variable */
    assert(sym->vclass==_local);        /* symbol must be stack relative */
    stgwrite("\tload.s.alt ");
  } else {
    /* a local or global array */
    if (sym->vclass==_local)
      stgwrite("\taddr.alt ");
    else
      stgwrite("\tconst.alt ");
  } /* if */
  outval(sym->addr,_yes);
  sym->usage|=_written;

  code_idx+=opcodes(1)+opargs(1);
  memcopy(size);
}

void fillarray(symbol *sym,cell size,cell value)
{
  const1(value);    /* lad value in PRI */

  assert(sym!=NULL);
  /* the symbol can be a local array, a global array, or an array
   * that is passed by reference.
   */
  if (sym->ident==_refarray) {
    /* reference to an array; currently this is always a local variable */
    assert(sym->vclass==_local);        /* symbol must be stack relative */
    stgwrite("\tload.s.alt ");
  } else {
    /* a local or global array */
    if (sym->vclass==_local)
      stgwrite("\taddr.alt ");
    else
      stgwrite("\tconst.alt ");
  } /* if */
  outval(sym->addr,_yes);
  sym->usage|=_written;

  stgwrite("\tfill ");
  outval(size,_yes);

  code_idx+=opcodes(2)+opargs(2);
}

/*
 *  Instruction to get an immediate value into the primary register
 */
void const1(cell val)
{
  if (val==0) {
    stgwrite("\tzero.pri\n");
    code_idx+=opcodes(1);
  } else {
    stgwrite("\tconst.pri ");
    outval(val, _yes);
    code_idx+=opcodes(1)+opargs(1);
  } /* if */
}

/*
 *  Instruction to get an immediate value into the secondary register
 */
void const2(cell val)
{
  if (val==0) {
    stgwrite("\tzero.alt\n");
    code_idx+=opcodes(1);
  } else {
    stgwrite("\tconst.alt ");
    outval(val, _yes);
    code_idx+=opcodes(1)+opargs(1);
  } /* if */
}

/*
 *  Push primary register onto the stack
 */
void push1(void)
{
  stgwrite("\tpush.pri\n");
  code_idx+=opcodes(1);
}

void pushval(cell val)
{
  stgwrite("\tpush.c ");
  outval(val, _yes);
  code_idx+=opcodes(1)+opargs(1);
}

/*
 *  pop stack to the secondary register
 */
void pop2(void)
{
  stgwrite("\tpop.alt\n");
  code_idx+=opcodes(1);
}

/* Switch statements
 * The "switch" statement generates a "case" table using the "CASE" opcode.
 * The case table contains a list of records, each record holds a comparison
 * value and a label to branch to on a match. The very first record is an
 * exception: it holds the size of the table (excluding the first record) and
 * the label to branch to when none of the values in the case table match.
 * The case table is sorted on the comparison value. This allows more advanced
 * abstract machines to sift the case table with a binary search.
 */
void ffswitch(int label)
{
  stgwrite("\tswitch ");
  outval(label,_yes);           /* the label is the address of the case table */
  code_idx+=opcodes(1)+opargs(1);
}

void ffcase(cell value,char *labelname,int newtable)
{
  if (newtable) {
    stgwrite("\tcasetbl\n");
    code_idx+=opcodes(1);
  } /* if */
  stgwrite("\tcase ");
  outval(value,_no);
  stgwrite(" ");
  stgwrite(labelname);
  stgwrite("\n");
  code_idx+=opcodes(0)+opargs(2);
}

/*
 *  Call specified function
 */
void ffcall(symbol *sym,int numargs)
{
  assert(sym->ident==_functn);
  if ((sym->usage & _native)!=0) {
    /* reserve a SYSREQ id if called for the first time */
    if ((sym->usage & _refer)==0)
      sym->addr=ntv_funcid++;
    stgwrite("\tsysreq.c ");
    outval(sym->addr,_no);
    stgwrite("\t; ");
    stgwrite(sym->name);
    stgwrite("\n\tstack ");
    outval((numargs+1)*sizeof(cell), _yes);
    code_idx+=opcodes(2)+opargs(2);
  } else {
    /* normal function */
    stgwrite("\tcall ");
    stgwrite(sym->name);
    stgwrite("\n");
    code_idx+=opcodes(1)+opargs(1);
  } /* if */
}

/*  Return from function
 *
 *  Global references: funcstatus  (referred to only)
 */
void ffret(void)
{
  stgwrite("\tretn\n");
  code_idx+=opcodes(1);
}

void ffabort(int reason)
{
  stgwrite("\thalt ");
  outval(reason,_yes);
  code_idx+=opcodes(1)+opargs(1);
}

void ffbounds(cell size)
{
  if ((debug & _chkbounds)!=0) {
    stgwrite("\tbounds ");
    outval(size,_yes);
    code_idx+=opcodes(1)+opargs(1);
  } /* if */
}

/*
 *  Jump to local label number (the number is converted to a name)
 */
void jumplabel(int number)
{
  stgwrite("\tjump ");
  outval(number,_yes);
  code_idx+=opcodes(1)+opargs(1);
}

/*
 *   Define storage according to size
 */
void defstorage(void)
{
  stgwrite("dump ");
}

/*
 *  Inclrement/decrement stack pointer. Note that this routine does
 *  nothing if the delta is zero.
 */
void modstk(int delta)
{
  if (delta) {
    stgwrite("\tstack ");
    outval(delta, _yes);
    code_idx+=opcodes(1)+opargs(1);
  } /* if */
}

/* set the stack to a hard offset from the frame */
void setstk(cell value)
{
  stgwrite("\tlctrl 5\n");      /* get FRM */
  stgwrite("\tadd.c ");
  outval(value, _yes);          /* add (negative) offset */
  stgwrite("\tsctrl 4\n");      /* store in STK */
}

void modheap(int delta)
{
  if (delta) {
    stgwrite("\theap ");
    outval(delta, _yes);
    code_idx+=opcodes(1)+opargs(1);
  } /* if */
}

void setheap_pri(void)
{
  stgwrite("\theap ");          /* ALT = HEA++ */
  outval(sizeof(cell), _yes);
  stgwrite("\tstor.i\n");       /* store PRI (default value) at address ALT */
  stgwrite("\tmove.pri\n");     /* move ALT to PRI: PRI contains the address */
  code_idx+=opcodes(3)+opargs(1);
}

void setheap(cell value)
{
  stgwrite("\tconst.pri ");     /* load default value in PRI */
  outval(value, _yes);
  code_idx+=opcodes(1)+opargs(1);
  setheap_pri();
}

/*
 *  Convert a cell number to a "byte" address; i.e. double or quadruple
 *  the primary register.
 */
void cell2addr(void)
{
  #if defined(BIT16)
    stgwrite("\tshl.c.pri 1\n");
  #else
    stgwrite("\tshl.c.pri 2\n");
  #endif
  code_idx+=opcodes(1)+opargs(1);
}

/*
 *  Double or quadruple the alternate register.
 */
void cell2addr_alt(void)
{
  #if defined(BIT16)
    stgwrite("\tshl.c.alt 1\n");
  #else
    stgwrite("\tshl.c.alt 2\n");
  #endif
  code_idx+=opcodes(1)+opargs(1);
}

/*
 *  Convert "distance of addresses" to "number of cells" in between.
 *  Or convert a number of packed characters to the number of cells (with
 *  truncation).
 */
void addr2cell(void)
{
  #if defined(BIT16)
    stgwrite("\tshr.c.pri 1\n");
  #else
    stgwrite("\tshr.c.pri 2\n");
  #endif
  code_idx+=opcodes(1)+opargs(1);
}

/* Convert from character index to byte address. This routine does
 * nothing if a character has the size of a byte.
 */
void char2addr(void)
{
  if (charbits==16) {
    stgwrite("\tshl.c.pri 1\n");
    code_idx+=opcodes(1)+opargs(1);
  } /* if */
}

/* Align PRI (which should hold a character index) to an address.
 * The first character in a "pack" occupies the highest bits of
 * the cell. This is at the lower memory address on Big Endian
 * computers and on the higher address on Little Endian computers.
 * The ALIGN.pri/alt instructions must solve this machine dependence;
 * that is, on Big Endian computers, ALIGN.pri/alt shuold do nothing
 * and on Little Endian computers they should toggle the address.
 */
void charalign(void)
{
  stgwrite("\talign.pri ");
  outval(charbits/8,_yes);
  code_idx+=opcodes(1)+opargs(1);
}

/*
 *  Add a constant to the primary register.
 */
void addconst(cell value)
{
  stgwrite("\tadd.c ");
  outval(value,_yes);
  code_idx+=opcodes(1)+opargs(1);
}

/*
 *  signed multiply of primary and secundairy registers (result in primary)
 */
void os_mult(void)
{
  stgwrite("\tsmul\n");
  code_idx+=opcodes(1);
}

/*
 *  signed divide of alternate register by primary register (quotient in
 *  primary; remainder in alternate)
 */
void os_div(void)
{
  stgwrite("\tsdiv.alt\n");
  code_idx+=opcodes(1);
}

/*
 *  modulus of (alternate % primary), result in primary (signed)
 */
void os_mod(void)
{
  stgwrite("\tsdiv.alt\n");
  stgwrite("\tmove.pri\n");     /* move ALT to PRI */
  code_idx+=opcodes(2);
}

/*
 *  Add primary and alternate registers (result in primary).
 */
void ob_add(void)
{
  stgwrite("\tadd\n");
  code_idx+=opcodes(1);
}

/*
 *  subtract primary register from alternate register (result in primary)
 */
void ob_sub(void)
{
  stgwrite("\tsub.alt\n");
  code_idx+=opcodes(1);
}

/*
 *  arithmic shift left alternate register the number of bits
 *  given in the primary register (result in primary).
 *  There is no need for a "logical shift left" routine, since
 *  logical shift left is identical to arithmic shift left.
 */
void ob_sal(void)
{
  stgwrite("\txchg\n");
  stgwrite("\tshl\n");
  code_idx+=opcodes(2);
}

/*
 *  arithmic shift right alternate register the number of bits
 *  given in the primary register (result in primary).
 */
void os_sar(void)
{
  stgwrite("\txchg\n");
  stgwrite("\tsshr\n");
  code_idx+=opcodes(2);
}

/*
 *  logical (unsigned) shift right of the alternate register by the
 *  number of bits given in the primary register (result in primary).
 */
void ou_sar(void)
{
  stgwrite("\txchg\n");
  stgwrite("\tshr\n");
  code_idx+=opcodes(2);
}

/*
 *  inclusive "or" of primary and secondary registers (result in primary)
 */
void ob_or(void)
{
  stgwrite("\tor\n");
  code_idx+=opcodes(1);
}

/*
 *  "exclusive or" of primary and alternate registers (result in primary)
 */
void ob_xor(void)
{
  stgwrite("\txor\n");
  code_idx+=opcodes(1);
}

/*
 *  "and" of primary and secundairy registers (result in primary)
 */
void ob_and(void)
{
  stgwrite("\tand\n");
  code_idx+=opcodes(1);
}

/*
 *  test ALT==PRI; result in primary register (1 or 0).
 */
void ob_eq(void)
{
  stgwrite("\teq\n");
  code_idx+=opcodes(1);
}

/*
 *  test ALT!=PRI
 */
void ob_ne(void)
{
  stgwrite("\tneq\n");
  code_idx+=opcodes(1);
}

/* The abstract machine defines the relational instructions so that PRI is
 * on the left side and ALT on the right side of the operator. For example,
 * SLESS sets PRI to either 1 or 0 depending on whether the expression
 * "PRI < ALT" is true.
 *
 * The compiler generates comparisons with ALT on the left side of the
 * relational operator and PRI on the right side. Therefore, it has to
 * transcode "ALT < PRI" to a SGRTR instruction. (Note that the compiler
 * does not create "logical inverted" comparisons, it just swaps the
 * operands.)
 */

/*
 *  test ALT<PRI (signed)
 */
void os_lt(void)
{
  stgwrite("\tsgrtr\n");
  code_idx+=opcodes(1);
}

/*
 *  test ALT<=PRI (signed)
 */
void os_le(void)
{
  stgwrite("\tsgeq\n");
  code_idx+=opcodes(1);
}

/*
 *  test ALT>PRI (signed)
 */
void os_gt(void)
{
  stgwrite("\tsless\n");
  code_idx+=opcodes(1);
}

/*
 *  test ALT>=PRI (signed)
 */
void os_ge(void)
{
  stgwrite("\tsleq\n");
  code_idx+=opcodes(1);
}

/*
 *  logical negation of primary register
 */
void lneg(void)
{
  stgwrite("\tnot\n");
  code_idx+=opcodes(1);
}

/*
 *  two's complement primary register
 */
void neg(void)
{
  stgwrite("\tneg\n");
  code_idx+=opcodes(1);
}

/*
 *  one's complement of primary register
 */
void invert(void)
{
  stgwrite("\tinvert\n");
  code_idx+=opcodes(1);
}

/*  increment symbol
 */
void inc(value *lval)
{
  symbol *sym;

  sym=lval->sym;
  if (lval->ident==_arraycell) {
    /* indirect increment, address already in PRI */
    stgwrite("\tinc.i\n");
    code_idx+=opcodes(1);
  } else if (lval->ident==_arraychar) {
    /* indirect increment of single character, address already in PRI */
    stgwrite("\tpush.pri\n");
    stgwrite("\tpush.alt\n");
    stgwrite("\tmove.alt\n");   /* copy address */
    stgwrite("\tlodb.i ");      /* read from PRI into PRI */
    outval(charbits/8,_yes);    /* read one or two bytes */
    stgwrite("\tinc.pri\n");
    stgwrite("\tstrb.i ");      /* write PRI to ALT */
    outval(charbits/8,_yes);    /* write one or two bytes */
    stgwrite("\tpop.alt\n");
    stgwrite("\tpop.pri\n");
    code_idx+=opcodes(8)+opargs(2);
  } else if (lval->ident==_reference) {
    assert(sym!=NULL);
    stgwrite("\tpush.pri\n");
    /* load dereferenced value */
    assert(sym->vclass==_local);    /* global references don't exist in Small */
    if (sym->vclass==_local)
      stgwrite("\tlref.s.pri ");
    else
      stgwrite("\tlref.pri ");
    outval(sym->addr,_yes);
    /* increment */
    stgwrite("\tinc.pri\n");
    /* store dereferenced value */
    if (sym->vclass==_local)
      stgwrite("\tsref.s.pri ");
    else
      stgwrite("\tsref.pri ");
    outval(sym->addr,_yes);
    stgwrite("\tpop.pri\n");
    code_idx+=opcodes(5)+opargs(2);
  } else {
    /* local or global variable */
    assert(sym!=NULL);
    if (sym->vclass==_local)
      stgwrite("\tinc.s ");
    else
      stgwrite("\tinc ");
    outval(sym->addr,_yes);
    code_idx+=opcodes(1)+opargs(1);
  } /* if */
}

/*  decrement symbol
 *
 *  in case of an integer pointer, the symbol must be incremented by 2.
 */
void dec(value *lval)
{
  symbol *sym;

  sym=lval->sym;
  if (lval->ident==_arraycell) {
    /* indirect decrement, address already in PRI */
    stgwrite("\tdec.i\n");
    code_idx+=opcodes(1);
  } else if (lval->ident==_arraychar) {
    /* indirect decrement of single character, address already in PRI */
    stgwrite("\tpush.pri\n");
    stgwrite("\tpush.alt\n");
    stgwrite("\tmove.alt\n");   /* copy address */
    stgwrite("\tlodb.i ");      /* read from PRI into PRI */
    outval(charbits/8,_yes);    /* read one or two bytes */
    stgwrite("\tdec.pri\n");
    stgwrite("\tstrb.i ");      /* write PRI to ALT */
    outval(charbits/8,_yes);    /* write one or two bytes */
    stgwrite("\tpop.alt\n");
    stgwrite("\tpop.pri\n");
    code_idx+=opcodes(8)+opargs(2);
  } else if (lval->ident==_reference) {
    assert(sym!=NULL);
    stgwrite("\tpush.pri\n");
    /* load dereferenced value */
    assert(sym->vclass==_local);    /* global references don't exist in Small */
    if (sym->vclass==_local)
      stgwrite("\tlref.s.pri ");
    else
      stgwrite("\tlref.pri ");
    outval(sym->addr,_yes);
    /* decrement */
    stgwrite("\tdec.pri\n");
    /* store dereferenced value */
    if (sym->vclass==_local)
      stgwrite("\tsref.s.pri ");
    else
      stgwrite("\tsref.pri ");
    outval(sym->addr,_yes);
    stgwrite("\tpop.pri\n");
    code_idx+=opcodes(5)+opargs(2);
  } else {
    /* local or global variable */
    assert(sym!=NULL);
    if (sym->vclass==_local)
      stgwrite("\tdec.s ");
    else
      stgwrite("\tdec ");
    outval(sym->addr,_yes);
    code_idx+=opcodes(1)+opargs(1);
  } /* if */
}

/*
 *  Jumps to "label" if PRI != 0
 */
void jmp_ne0(int number)
{
  stgwrite("\tjnz ");
  outval(number,_yes);
  code_idx+=opcodes(1)+opargs(1);
}

/*
 *  Jumps to "label" if PRI == 0
 */
void jmp_eq0(int number)
{
  stgwrite("\tjzer ");
  outval(number,_yes);
  code_idx+=opcodes(1)+opargs(1);
}

/* write a value in hexadecimal; optionally adds a newline */
void outval(cell val,int newline)
{
  stgwrite(itoh(val));
  if (newline)
    stgwrite("\n");
}

