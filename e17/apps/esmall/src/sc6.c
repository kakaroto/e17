/*  Small compiler - Binary code generation (the "assembler")
 *
 *  Copyright (c) ITB CompuPhase, 1997-1999
 *  This file may be freely used. No warranties of any kind.
 */
#include <assert.h>
#include <stdio.h>
#include <stdlib.h>		/* for macro max() */
#include <string.h>
#include <ctype.h>
#include "sc.h"
#include "amx.h"
typedef cell (*OPCODE_PROC) (FILE * fbin, char *params, cell opcode);
typedef struct
{
  cell opcode;
  char *name;
  int segment;			/* _incseg=parse in cseg, _indseg=parse in dseg */
  OPCODE_PROC func;
}
OPCODE;
static cell codeindex;		/* similar to "code_idx" */
static cell *lbltab;		/* label table */
/* apparently, strtol() does not work correctly on very large (unsigned)
 * hexadecimal values */
static unsigned long
hex2long (char *s, char **n)
{
  unsigned long result = 0L;
  int digit;
  /* ignore leading whitespace */
  while (*s == ' ' || *s == '\t')
    s++;
  /* NB. only works for lower case letters */
  for (;;)
    {
      assert (!(*s >= 'A' && *s <= 'Z'));	/* lower case only */
      if (*s >= '0' && *s <= '9')
	digit = *s - '0';
      else if (*s >= 'a' && *s <= 'f')
	digit = *s - 'a' + 10;
      else
	break;
      result = (result << 4) | digit;
      s++;
    }				/* for */
  if (n != NULL)
    *n = s;
  return result;
}
#if BYTE_ORDER == BIG_ENDIAN
static unsigned long *
align16 (unsigned short *v)
{
  unsigned char *s = (unsigned char *) v;
  unsigned char t;
  /* swap two bytes */
  t = s[0];
  s[0] = s[1];
  s[1] = t;
  return v;
}
static unsigned long *
align32 (unsigned long *v)
{
  unsigned char *s = (unsigned char *) v;
  unsigned char t;
  /* swap outer two bytes */
  t = s[0];
  s[0] = s[3];
  s[3] = t;
  /* swap inner two bytes */
  t = s[1];
  s[1] = s[2];
  s[2] = t;
  return v;
}
#if defined BIT16
#define aligncell(v)  align16(v)
#else /*  */
#define aligncell(v)  align32(v)
#endif /*  */
#else /*  */
#define align16(v)    (v)
#define align32(v)    (v)
#define aligncell(v)  (v)
#endif /*  */
static char *
skipwhitespace (char *str)
{
  while (isspace (*str))
    str++;
  return str;
}
static char *
stripcomment (char *str)
{
  char *ptr = strchr (str, ';');
  if (ptr != NULL)
    {
      *ptr++ = '\n';		/* terminate the line, but leave the '\n' */
      *ptr = '\0';
    }				/* if */
  return str;
}
#if defined __BORLANDC__ || defined __WATCOMC__
#pragma argsused
#endif /*  */
static cell
noop (FILE * fbin, char *params, cell opcode)
{
  return 0;
}
#if defined __BORLANDC__ || defined __WATCOMC__
#pragma argsused
#endif /*  */
static cell
parm0 (FILE * fbin, char *params, cell opcode)
{
  if (fbin != NULL)
    {
      assert ((ftell (fbin) % sizeof (cell)) == 0);
      fwrite (aligncell (&opcode), sizeof opcode, 1, fbin);
    }				/* if */
  return opcodes (1);
}
static cell
parm1 (FILE * fbin, char *params, cell opcode)
{
  ucell p = hex2long (params, NULL);
  if (fbin != NULL)
    {
      assert ((ftell (fbin) % sizeof (cell)) == 0);
      fwrite (aligncell (&opcode), sizeof opcode, 1, fbin);
      fwrite (aligncell (&p), sizeof p, 1, fbin);
    }				/* if */
  return opcodes (1) + opargs (1);
}
static cell
parm2 (FILE * fbin, char *params, cell opcode)
{
  ucell p1, p2;
  p1 = hex2long (params, &params);
  p2 = hex2long (params, NULL);
  if (fbin != NULL)
    {
      assert ((ftell (fbin) % sizeof (cell)) == 0);
      fwrite (aligncell (&opcode), sizeof opcode, 1, fbin);
      fwrite (aligncell (&p1), sizeof p1, 1, fbin);
      fwrite (aligncell (&p2), sizeof p2, 1, fbin);
    }				/* if */
  return opcodes (1) + opargs (2);
}
#if defined __BORLANDC__ || defined __WATCOMC__
#pragma argsused
#endif /*  */
static cell
do_dump (FILE * fbin, char *params, cell opcode)
{
  ucell p;
  int num = 0;
  while (*params != '\0')
    {
      p = hex2long (params, &params);
      if (fbin != NULL)
	{
	  assert ((ftell (fbin) % sizeof (cell)) == 0);
	  fwrite (aligncell (&p), sizeof p, 1, fbin);
	}			/* if */
      num++;
      while (isspace (*params))
	params++;
    }				/* while */
  return num * sizeof (cell);
}
static cell
do_call (FILE * fbin, char *params, cell opcode)
{
  char name[_namemax + 1];
  int i;
  symbol *sym;
  ucell p;
  for (i = 0; !isspace (*params); i++, params++)
    {
      assert (*params != '\0');
      assert (i < _namemax);
      name[i] = *params;
    }				/* for */
  name[i] = '\0';
  /* look up the function address */
  sym = findglb (name);
  assert (sym != NULL);
  assert (sym->ident == _functn || sym->ident == _reffunc);
  assert (sym->vclass == _global);
  p = sym->addr;
  if (fbin != NULL)
    {
      assert ((ftell (fbin) % sizeof (cell)) == 0);
      fwrite (aligncell (&opcode), sizeof opcode, 1, fbin);
      fwrite (aligncell (&p), sizeof p, 1, fbin);
    }				/* if */
  return opcodes (1) + opargs (1);
}
static cell
do_jump (FILE * fbin, char *params, cell opcode)
{
  int i;
  ucell p;
  i = (int) hex2long (params, NULL);
  assert (i >= 0 && i < _numlabels);
  if (fbin != NULL)
    {
      assert (lbltab != NULL);
      p = lbltab[i];
      assert ((ftell (fbin) % sizeof (cell)) == 0);
      fwrite (aligncell (&opcode), sizeof opcode, 1, fbin);
      fwrite (aligncell (&p), sizeof p, 1, fbin);
    }				/* if */
  return opcodes (1) + opargs (1);
}
static cell
do_file (FILE * fbin, char *params, cell opcode)
{
  char *endptr;
  ucell p, clen;
  int len;
  unsigned char zero = 0;	/* for padding */
  for (endptr = params; !isspace (*endptr) && endptr != '\0'; endptr++)
    /* nothing */ ;
  assert (*endptr == ' ');
  assert ((endptr - params) < 256);
  len = (int) (endptr - params);
  /* add one for zero-terminating byte, then round to multiple of cells */
  clen = (len + sizeof (cell)) & ~(sizeof (cell) - 1);
  clen += sizeof (cell);	/* add size of file ordinal */
  p = hex2long (endptr, NULL);
  if (fbin != NULL)
    {
      assert ((ftell (fbin) % sizeof (cell)) == 0);
      fwrite (aligncell (&opcode), sizeof opcode, 1, fbin);
      fwrite (aligncell (&clen), sizeof clen, 1, fbin);
      fwrite (aligncell (&p), sizeof p, 1, fbin);
      fwrite (params, 1, len, fbin);
      while (len++ < (int) (clen - sizeof (cell)))
	fwrite (&zero, 1, 1, fbin);	/* pad with zeros */
    }				/* if */
  return opcodes (1) + opargs (1) + clen;	/* other argument is in clen */
}
static cell
do_symbol (FILE * fbin, char *params, cell opcode)
{
  char *endptr;
  ucell offset, clen, flags;
  int len;
  unsigned char mclass, type;
  unsigned char zero = 0;	/* for padding */
  for (endptr = params; !isspace (*endptr) && endptr != '\0'; endptr++)
    /* nothing */ ;
  assert (*endptr == ' ');
  len = (int) (endptr - params);
  assert (len > 0 && len < _namemax);
  /* add one for zero-terminating byte, then round to multiple of cells */
  clen = (len + sizeof (cell)) & ~(sizeof (cell) - 1);
  clen += 2 * sizeof (cell);	/* add size of symbol address and flags */
  offset = hex2long (endptr, &endptr);
  mclass = (unsigned char) hex2long (endptr, &endptr);
  type = (unsigned char) hex2long (endptr, NULL);
  flags = type + 256 * mclass;
  if (fbin != NULL)
    {
      assert ((ftell (fbin) % sizeof (cell)) == 0);
      fwrite (aligncell (&opcode), sizeof opcode, 1, fbin);
      fwrite (aligncell (&clen), sizeof clen, 1, fbin);
      fwrite (aligncell (&offset), sizeof offset, 1, fbin);
      fwrite (aligncell (&flags), sizeof flags, 1, fbin);
      fwrite (params, 1, len, fbin);
      while (len++ < (int) (clen - 2 * sizeof (cell)))
	fwrite (&zero, 1, 1, fbin);	/* pad with zeros */
    }				/* if */
#if !defined NDEBUG
  /* function should start right after the symbolic information */
  if (fbin == NULL && mclass == 0 && type == _functn)
    assert (offset == codeindex + opcodes (1) + opargs (1) + clen);
#endif /*  */
  return opcodes (1) + opargs (1) + clen;	/* other 2 arguments are in clen */
}
static cell
do_switch (FILE * fbin, char *params, cell opcode)
{
  int i;
  ucell p;
  i = (int) hex2long (params, NULL);
  assert (i >= 0 && i < _numlabels);
  if (fbin != NULL)
    {
      assert (lbltab != NULL);
      p = lbltab[i];
      assert ((ftell (fbin) % sizeof (cell)) == 0);
      fwrite (aligncell (&opcode), sizeof opcode, 1, fbin);
      fwrite (aligncell (&p), sizeof p, 1, fbin);
    }				/* if */
  return opcodes (1) + opargs (1);
}
#if defined __BORLANDC__ || defined __WATCOMC__
#pragma argsused
#endif /*  */
static cell
do_case (FILE * fbin, char *params, cell opcode)
{
  int i;
  ucell p, v;
  v = hex2long (params, &params);
  i = (int) hex2long (params, NULL);
  assert (i >= 0 && i < _numlabels);
  if (fbin != NULL)
    {
      assert (lbltab != NULL);
      p = lbltab[i];
      assert ((ftell (fbin) % sizeof (cell)) == 0);
      fwrite (aligncell (&v), sizeof v, 1, fbin);
      fwrite (aligncell (&p), sizeof p, 1, fbin);
    }				/* if */
  return opcodes (0) + opargs (2);
}
static OPCODE opcodelist[] = {
  /* node for "invalid instruction" */
  {0, NULL, 0, noop},
  /* special "directives" (no opcodes) */
  {0, "code", 0, noop},
  {0, "data", 0, noop},
  {0, "stksize", 0, noop},
  {0, "dump", _indseg, do_dump},
  {0, "case", _incseg, do_case},
  /* loading from memory and storing into memory */
  {1, "load.pri", _incseg, parm1},
  {2, "load.alt", _incseg, parm1},
  {3, "load.s.pri", _incseg, parm1},
  {4, "load.s.alt", _incseg, parm1},
  {5, "lref.pri", _incseg, parm1},
  {6, "lref.alt", _incseg, parm1},
  {7, "lref.s.pri", _incseg, parm1},
  {8, "lref.s.alt", _incseg, parm1},
  {9, "load.i", _incseg, parm0},
  {10, "lodb.i", _incseg, parm1},
  {11, "const.pri", _incseg, parm1},
  {12, "const.alt", _incseg, parm1},
  {13, "addr.pri", _incseg, parm1},
  {14, "addr.alt", _incseg, parm1},
  {15, "stor.pri", _incseg, parm1},
  {16, "stor.alt", _incseg, parm1},
  {17, "stor.s.pri", _incseg, parm1},
  {18, "stor.s.alt", _incseg, parm1},
  {19, "sref.pri", _incseg, parm1},
  {20, "sref.alt", _incseg, parm1},
  {21, "sref.s.pri", _incseg, parm1},
  {22, "sref.s.alt", _incseg, parm1},
  {23, "stor.i", _incseg, parm0},
  {24, "strb.i", _incseg, parm1},
  {25, "lidx", _incseg, parm0},
  {26, "lidx.b", _incseg, parm1},
  {27, "idxaddr", _incseg, parm0},
  {28, "idxaddr.b", _incseg, parm1},
  {29, "align.pri", _incseg, parm1},
  {30, "align.alt", _incseg, parm1},
  {31, "lctrl", _incseg, parm1},
  {32, "sctrl", _incseg, parm1},
  {33, "move.pri", _incseg, parm0},
  {34, "move.alt", _incseg, parm0},
  {35, "xchg", _incseg, parm0},
  /* stack and heap manipulation */
  {36, "push.pri", _incseg, parm0},
  {37, "push.alt", _incseg, parm0},
  {38, "push.r", _incseg, parm1},
  {39, "push.c", _incseg, parm1},
  {40, "push", _incseg, parm1},
  {41, "push.s", _incseg, parm1},
  {42, "pop.pri", _incseg, parm0},
  {43, "pop.alt", _incseg, parm0},
  {44, "stack", _incseg, parm1},
  {45, "heap", _incseg, parm1},
  /* jumps, function calls (and returns) */
  {46, "proc", _incseg, parm0},
  {47, "ret", _incseg, parm0},
  {48, "retn", _incseg, parm0},
  {49, "call", _incseg, do_call},
  {50, "call.pri", _incseg, parm0},
  {51, "jump", _incseg, do_jump},
  {52, "jrel", _incseg, parm1},	/* always a number */
  {53, "jzer", _incseg, do_jump},
  {54, "jnz", _incseg, do_jump},
  {55, "jeq", _incseg, do_jump},
  {56, "jneq", _incseg, do_jump},
  {57, "jless", _incseg, do_jump},
  {58, "jleq", _incseg, do_jump},
  {59, "jgrtr", _incseg, do_jump},
  {60, "jgeq", _incseg, do_jump},
  {61, "jsless", _incseg, do_jump},
  {62, "jsleq", _incseg, do_jump},
  {63, "jsgrtr", _incseg, do_jump},
  {64, "jsgeq", _incseg, do_jump},
  /* shift instructions */
  {65, "shl", _incseg, parm0},
  {66, "shr", _incseg, parm0},
  {67, "sshr", _incseg, parm0},
  {68, "shl.c.pri", _incseg, parm1},
  {69, "shl.c.alt", _incseg, parm1},
  {70, "shr.c.pri", _incseg, parm1},
  {71, "shr.c.alt", _incseg, parm1},
  /* arithmetic and bitwise instructions */
  {72, "smul", _incseg, parm0},
  {73, "sdiv", _incseg, parm0},
  {74, "sdiv.alt", _incseg, parm0},
  {75, "umul", _incseg, parm0},
  {76, "udiv", _incseg, parm0},
  {77, "udiv.alt", _incseg, parm0},
  {78, "add", _incseg, parm0},
  {79, "sub", _incseg, parm0},
  {80, "sub.alt", _incseg, parm0},
  {81, "and", _incseg, parm0},
  {82, "or", _incseg, parm0},
  {83, "xor", _incseg, parm0},
  {84, "not", _incseg, parm0},
  {85, "neg", _incseg, parm0},
  {86, "invert", _incseg, parm0},
  {87, "add.c", _incseg, parm1},
  {88, "smul.c", _incseg, parm1},
  {89, "zero.pri", _incseg, parm0},
  {90, "zero.alt", _incseg, parm0},
  {91, "zero", _incseg, parm1},
  {92, "zero.s", _incseg, parm1},
  {93, "sign.pri", _incseg, parm0},
  {94, "sign.alt", _incseg, parm0},
  /* relational operators */
  {95, "eq", _incseg, parm0},
  {96, "neq", _incseg, parm0},
  {97, "less", _incseg, parm0},
  {98, "leq", _incseg, parm0},
  {99, "grtr", _incseg, parm0},
  {100, "geq", _incseg, parm0},
  {101, "sless", _incseg, parm0},
  {102, "sleq", _incseg, parm0},
  {103, "sgrtr", _incseg, parm0},
  {104, "sgeq", _incseg, parm0},
  {105, "eq.c.pri", _incseg, parm1},
  {106, "eq.c.alt", _incseg, parm1},
  /* increment/decrement */
  {107, "inc.pri", _incseg, parm0},
  {108, "inc.alt", _incseg, parm0},
  {109, "inc", _incseg, parm1},
  {110, "inc.s", _incseg, parm1},
  {111, "inc.i", _incseg, parm1},
  {112, "dec.pri", _incseg, parm0},
  {113, "dec.alt", _incseg, parm0},
  {114, "dec", _incseg, parm1},
  {115, "dec.s", _incseg, parm1},
  {116, "dec.i", _incseg, parm1},
  /* special instructions */
  {117, "movs", _incseg, parm1},
  {118, "cmps", _incseg, parm1},
  {119, "fill", _incseg, parm1},
  {120, "halt", _incseg, parm1},
  {121, "bounds", _incseg, parm1},
  {122, "sysreq.pri", _incseg, parm0},
  {123, "sysreq.c", _incseg, parm1},
  /* debugging opcodes */
  {124, "file", _incseg, do_file},
  {125, "line", _incseg, parm2},
  {126, "symbol", _incseg, do_symbol},
  {127, "srange", _incseg, parm2},	/* version 1 */
  {128, "jump.pri", _incseg, parm0},
  {129, "switch", _incseg, do_switch},
  {130, "casetbl", _incseg, parm0},
  /* terminator */
  {0, NULL, 0, noop}
};
typedef struct
{
  cell address;
  char name[_namemax + 1];
}
FUNCSTUB;
#define MAX_INSTR_LEN   30
static int
findopcode (char *instr, int maxlen)
{
  int i;
  char str[MAX_INSTR_LEN];
  if (maxlen >= MAX_INSTR_LEN)
    return 0;
  /* copy the instruction name, then adapt it to lower case so the assembler is
   * case insensitive to instructions (but case sensitive to symbols)
   */
  strncpy (str, instr, maxlen);
  str[maxlen] = '\0';		/* make sure the string is zero terminated */
  strlwr (str);
  /* look up the instruction */
  for (i = 1;
       opcodelist[i].name != NULL
       && strcasecmp (opcodelist[i].name, str) != 0; i++)
    /* nothing */ ;
  return i;
}
void
assemble (FILE * fout, FILE * fin)
{
  AMX_HEADER hdr;
  FUNCSTUB func;
  int numpublics, numnatives, numlibraries;
  char line[256], *instr, *params;
  int i, pass;
  symbol *sym, **nativelist;
  constval *constptr;
  cell mainaddr;
#if !defined NDEBUG
  int opcode;
#endif /*  */
  /* verify the opcode table */
#if !defined NDEBUG
  opcode = 0;
  for (i = 0; opcodelist[i].name != NULL; i++)
    if (opcodelist[i].opcode != 0)
      assert (opcodelist[i].opcode == ++opcode);
#endif /*  */
  numpublics = 0;
  numnatives = 0;
  mainaddr = 0;
  /* count number of public and native functions */
  for (sym = glbtab.next; sym != NULL; sym = sym->next)
    {
      if (sym->ident == _functn)
	{
	  if ((sym->usage & _native) != 0 && (sym->usage & _refer) != 0)
	    numnatives++;
	  if ((sym->usage & _public) != 0 && (sym->usage & _define) != 0)
	    numpublics++;
	  if (strcmp (sym->name, "main") == 0)
	    {
	      assert (sym->vclass == _global);
	      mainaddr = sym->addr;
	    }			/* if */
	}			/* if */
    }				/* for */
  assert (numnatives == ntv_funcid);
  /* count number of libraries */
  numlibraries = 0;
  for (constptr = libname_tab.next; constptr != NULL;
       constptr = constptr->next)
    if (strlen (constptr->name) > 0)
      numlibraries++;
  /* write the abstract machine header */
  memset (&hdr, 0, sizeof hdr);
  hdr.magic = (short) 0xF1E0;
  hdr.file_version = 1;
  hdr.amx_version = 1;
  hdr.flags = debug & _symbolic;
  if (charbits == 16)
    hdr.flags |= 1;
  hdr.defsize = _namemax + 1 + sizeof (cell);
  assert ((hdr.defsize % sizeof (cell)) == 0);
  hdr.num_publics = numpublics;
  hdr.num_natives = numnatives;
  hdr.num_libraries = numlibraries;
  hdr.publics = sizeof hdr;	/* public table starts right after the header */
  hdr.natives = hdr.publics + numpublics * sizeof (FUNCSTUB);
  hdr.libraries = hdr.natives + numnatives * sizeof (FUNCSTUB);
  hdr.cod = hdr.libraries + numlibraries * sizeof (FUNCSTUB);
  hdr.dat = hdr.cod + code_idx;
  hdr.hea = hdr.dat + glb_declared * sizeof (cell);
  hdr.stp = hdr.hea + stksize * sizeof (cell);
  hdr.cip = mainaddr;
  hdr.size = hdr.hea;
#if BYTE_ORDER == BIG_ENDIAN
  align32 (&hdr.size);
  align16 (&hdr.magic);
  align16 (&hdr.flags);;
  align16 (&hdr.defsize);
  align16 (&hdr.num_publics);
  align16 (&hdr.num_natives);
  align32 (&hdr.publics);
  align32 (&hdr.natives);
  align32 (&hdr.cod);
  align32 (&hdr.dat);
  align32 (&hdr.hea);
  align32 (&hdr.stp);
  align32 (&hdr.cip);
#endif /*  */
  fwrite (&hdr, 1, sizeof hdr, fout);
  /* write the public table */
  for (sym = glbtab.next; sym != NULL; sym = sym->next)
    {
      if (sym->ident == _functn
	  && (sym->usage & _public) != 0 && (sym->usage & _define) != 0)
	{
	  memset (&func, 0, sizeof func);
	  strcpy (func.name, sym->name);
	  assert (sym->vclass == _global);
	  func.address = sym->addr;
#if BYTE_ORDER == BIG_ENDIAN
	  aligncell (&func.address);
#endif /*  */
	  fwrite (&func, 1, sizeof func, fout);
	}			/* if */
    }				/* for */
  /* write the natives table */
  /* The native functions must be written in sorted order. (They are
   * sorted on their "id" not on their name). A nested loop to find
   * each successive function would be an O(n^2) operation. But we
   * do not really need to sort, because the native function id's
   * are sequential and there are no duplicates. So we first walk
   * through the complete symbol list and store a pointer to every
   * native function of interest in a temporary table, where its id
   * serves as the index in the table. Now we can walk the table and
   * have all native functions in sorted order.
   */
  if (numnatives > 0)
    {
      nativelist = (symbol **) malloc (numnatives * sizeof (symbol *));
      if (nativelist == NULL)
	error (103);		/* insufficient memory */
#if !defined NDEBUG
      memset (nativelist, 0, numnatives * sizeof (symbol *));	/* for NULL checking */
#endif /*  */
      for (sym = glbtab.next; sym != NULL; sym = sym->next)
	{
	  if (sym->ident == _functn && (sym->usage & _native) != 0
	      && (sym->usage & _refer) != 0)
	    {
	      assert (sym->addr < numnatives);
	      nativelist[(int) sym->addr] = sym;
	    }			/* if */
	}			/* for */
      for (i = 0; i < numnatives; i++)
	{
	  sym = nativelist[i];
	  assert (sym != NULL);
	  memset (&func, 0, sizeof func);
	  strcpy (func.name, sym->name);
	  assert (sym->vclass == _global);
	  func.address = 0;
#if BYTE_ORDER == BIG_ENDIAN
	  aligncell (&func.address);
#endif /*  */
	  fwrite (&func, 1, sizeof func, fout);
	}			/* for */
      free (nativelist);
    }				/* if */
  /* write the libraries table */
  for (constptr = libname_tab.next; constptr != NULL;
       constptr = constptr->next)
    {
      if (strlen (constptr->name) > 0)
	{
	  memset (&func, 0, sizeof func);
	  strcpy (func.name, constptr->name);
	  func.address = constptr->value;
#if BYTE_ORDER == BIG_ENDIAN
	  aligncell (&func.address);
#endif /*  */
	  fwrite (&func, 1, sizeof func, fout);
	}			/* if */
    }				/* for */
  /* First pass: relocate all labels */
  if (labnum == 0)
    labnum = 1;			/* only very short programs have zero labels; add
				   * a dummy one so malloc() succeeds */
  lbltab = (long *) malloc (labnum * sizeof (cell));
  if (lbltab == NULL)
    error (103);		/* insufficient memory */
  codeindex = 0;
  rewind (fin);
  while (fgets (line, 256, fin) != NULL)
    {
      stripcomment (line);
      instr = skipwhitespace (line);
      /* ignore empty lines */
      if (*instr == '\0')
	continue;
      if (tolower (*instr) == 'l' && *(instr + 1) == '.')
	{
	  int lindex = (int) hex2long (instr + 2, NULL);
	  assert (lindex < labnum);
	  lbltab[lindex] = codeindex;
	}
      else
	{
	  /* get to the end of the instruction (make use of the '\n' that fgets()
	   * added at the end of the line; this way we will *always* drop on a
	   * whitespace character) */
	  for (params = instr; !isspace (*params); params++)
	    /* nothing */ ;
	  assert (params > instr);
	  i = findopcode (instr, (int) (params - instr));
	  if (opcodelist[i].name == NULL)
	    error (104, instr);	/* invalid assembler instruction */
	  if (opcodelist[i].segment == _incseg)
	    codeindex +=
	      opcodelist[i].func (NULL, skipwhitespace (params),
				  opcodelist[i].opcode);
	}			/* if */
    }				/* while */
  /* Second pass */
  for (pass = _incseg; pass <= _indseg; pass++)
    {
      rewind (fin);
      while (fgets (line, 256, fin) != NULL)
	{
	  stripcomment (line);
	  instr = skipwhitespace (line);
	  /* ignore empty lines and labels (labels have a special syntax, so these
	   * must be parsed separately) */
	  if (*instr == '\0' || tolower (*instr) == 'l'
	      && *(instr + 1) == '.')
	    continue;
	  /* get to the end of the instruction (make use of the '\n' that fgets()
	   * added at the end of the line; this way we will *always* drop on a
	   * whitespace character) */
	  for (params = instr; !isspace (*params); params++)
	    /* nothing */ ;
	  assert (params > instr);
	  i = findopcode (instr, (int) (params - instr));
	  assert (opcodelist[i].name != NULL);
	  if (opcodelist[i].segment == pass)
	    opcodelist[i].func (fout, skipwhitespace (params),
				opcodelist[i].opcode);
	}			/* while */
    }				/* for */
  free (lbltab);
#if !defined NDEBUG
  lbltab = NULL;
#endif /*  */
}
#pragma pack()			/* reset default packing */
