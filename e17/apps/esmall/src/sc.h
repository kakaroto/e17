/*  Small compiler
 *
 *  Drafted after the Small-C compiler Version 2.01, originally created
 *  by Ron Cain, july 1980, and enhanced by James E. Hendrix. It is
 *  now adapted to a virtual machine.
 *
 *  This version comes close to a complete rewrite.
 *
 *  Copyright R. Cain, 1980
 *  Copyright J.E. Hendrix, 1982, 1983
 *  Copyright T. Riemersma, 1997-1999
 */

#if !defined CELL_TYPE
  #define CELL_TYPE
  #if defined(BIT16)
    typedef unsigned short int ucell;     /* only for type casting */
    typedef short int cell;
  #else
    typedef unsigned long int ucell;
    typedef long int cell;
  #endif
#endif

#define PUBLIC_CHAR '@'     /* character that defines a function "public" */
#define CTRL_CHAR   '^'     /* default control character */

#define _dimen_max     2    /* maximum number of array dimensions */
#define _def_litmax  500    /* initial size of the literal pool, in "cells" */
#define _numlabels  1000    /* maximum number of internal labels */
#define _linemax 511    /* input line length (in characters) */
#define _namemax  19    /* significant length of symbol name */
#define _stkmax   50    /* stack for nested #includes and other uses */
#define _stgmax  800    /* output staging buffer (size in characters) */
typedef void *stkitem;  /* type of items stored on the stack */

typedef struct {        /* function argument info */
  char name[_namemax+1];
  char ident;           /* _variable, _reference, _refarray or _varargs */
  int tag;              /* argument tag id. */
  int dim[_dimen_max];
  int numdim;           /* number of dimensions */
  char hasdefault;      /* is there a default value? */
  union {
    cell val;           /* default value */
    struct {
      cell *data;       /* values of default array */
      int size;         /* complete length of default array */
    } array;
  } defvalue;           /* default value, or pointer to default array */
} arginfo;

/*  Symbol table format
 *
 *  The symbol name read from the input file is stored in "name", the
 *  value of "addr" is written to the output file. The address in "addr"
 *  depends on the class of the symbol:
 *      global          offset into the data segment
 *      local           offset relative to the stack frame
 *      label           generated hexadecimal number
 *      function        offset into code segment
 */
typedef struct __symbol {
  char name[_namemax+1];
  cell addr;            /* address or offset (or value for constant) */
  char vclass;          /* _local if "addr" refers to a local symbol */
  char ident;
  char usage;
  int compound;         /* compound level (braces nesting level) */
  int tag;              /* tagname id */
  union {
    int declared;       /* label: how many local variables are declared */
    int idxtag;         /* array: tag of array indices */
  } x;                  /* 'x' for 'extra' */
  union {
    arginfo *arglist;   /* types of all parameters for functions */
    struct {
      cell length;      /* arrays: length (size) */
      short level;      /* number of dimensions below this level */
    } array;
  } dim;                /* for 'dimension', both functions and arrays */
  struct __symbol *parent;  /* hierarchical types (multi-dimensional arrays) */
  struct __symbol *next;
} symbol;


/*  Possible entries for "ident". These are used in the "symbol", "value"
 *  and arginfo structures. Not every constant is valid for every use.
 */
#define _label      0
#define _variable   1   /* cell that has an address and that can be fetched directly (lvalue) */
#define _reference  2   /* _variable, but must be dereferenced */
#define _array      3
#define _refarray   4   /* an array passed by reference (i.e. a pointer) */
#define _arraycell  5   /* array element, cell that must be fetched indirectly */
#define _arraychar  6   /* array element, character from cell from array */
#define _expression 7   /* expression result, has no address (rvalue) */
#define _constexpr  8   /* constant expression */
#define _functn     9
#define _reffunc    10  /* function passed as a parameter */
#define _varargs    11  /* function specified ... as argument(s) */

/*  Possible entries for "usage"
 *
 *  This byte is used as a serie of bits, the syntax is different for
 *  functions and other symbols:
 *
 *  VARIABLE
 *  bits: 0     set 1 if the variable is defined in the source file
 *        1     set 1 if the variable is read in the source file
 *        2     set 1 if the variable is altered (assigned a value)
 *
 *  FUNCTION
 *  bits: 0     set 1 if the function is defined in the source file
 *        1     set 1 if the function is referred to in the source file
 *        2     the function returns a value (or should return a value)
 *        3     function was prototyped
 *        4     function is public
 *        5     function is native
 */
#define _define   0x01
#define _read     0x02
#define _written  0x04
#define _refer    0x02  /* note: _refer == _read */
#define _retvalue 0x04  /* function returns (or should return) a value */
#define _prototyped 0x08
#define _public   0x10
#define _native   0x20
/* _retnone is not stored in the "usage" field of a symbol. It is
 * used during parsing a function, to detect a mix of "return;" and
 * "return value;" in a few special cases.
 */
#define _retnone  0x10

#define _global   0     /* global/local variable/constant class */
#define _local    1
#define _static   2     /* global life, local scope */

typedef struct {
  symbol *sym;          /* symbol in symbol table, NULL for (constant) expression */
  char ident;           /* _constexpr, _variable, _array, _elemarray,
                         * _expression or _reference */
  cell constval;        /* value of the constant expression (if ident==_constexpr)
                         * also used for the size of a literal array */
  int tag;              /* tagname id (of the expression) */
} value;

/*  Equate table  */
typedef struct __s_constval {
  char name[_namemax+1];
  cell value;
  struct __s_constval *next;
} constval;

/*  "while" statement queue (also used for "for" and "do - while" loops) */
enum {
  _wqbr,        /* used to restore stack for "break" */
  _wqcont,      /* used to restore stack for "continue" */
  _wqloop,      /* loop start label number */
  _wqexit,      /* loop exit label number (jump if false) */
  /* --- */
  _wqsiz        /* "while queue" size */
};
#define _wqtabsz (16*_wqsiz)    /* 16 nested loop statements */

typedef struct __s_linebuffer {
  char *line;
  struct __s_linebuffer *next;
} linebuffer;

/* macros for code generation */
#define opcodes(n)      ((n)*sizeof(cell))      /* opcode size */
#define opargs(n)       ((n)*sizeof(cell))      /* size of typical argument */

/*  Tokens recognized by lex()
 *  Some of these constants are assigned as well to the variable "lastst"
 */
#define _firsttok 256   /* value of first multi-character operator */
#define _midtok  279    /* value of last multi-character operator */
#define _lasttok 313    /* value of last multi-character match-able token */
/* multi-character operators */
#define __amult  256    /* *= */
#define __adiv   257    /* /= */
#define __amod   258    /* %= */
#define __aadd   259    /* += */
#define __asub   260    /* -= */
#define __ashl   261    /* <<= */
#define __ashru  262    /* >>>= */
#define __ashr   263    /* >>= */
#define __aand   264    /* &= */
#define __axor   265    /* ^= */
#define __aor    266    /* |= */
#define __lor    267    /* || */
#define __land   268    /* && */
#define __leq    269    /* == */
#define __lne    270    /* != */
#define __lle    271    /* <= */
#define __lge    272    /* >= */
#define __shl    273    /* << */
#define __shru   274    /* >>> */
#define __shr    275    /* >> */
#define __inc    276    /* ++ */
#define __dec    277    /* -- */
#define __ellips 278    /* ... */
#define __dbldot 279    /* .. */
/* reserved words (statements) */
#define __assert  280
#define __break   281
#define __case    282
#define __char    283
#define __const   284
#define __continue 285
#define __default 286
#define __defined 287
#define __do      288
#define __else    289
#define __enum    290
#define __exit    291
#define __for     292
#define __forward 293
#define __goto    294
#define __if      295
#define __native  296
#define __new     297
#define __public  298
#define __return  299
#define __sizeof  300
#define __static  301
#define __switch  302
#define __while   303
/* compiler directives */
#define __assert_p 304  /* #assert */
#define __define  305
#define __else_p  306   /* #else */
#define __emit_p  307
#define __endif   308
#define __endinput 309
#define __endscrpt 310
#define __if_p    311   /* #if */
#define __include 312
#define __pragma  313
/* semicolon is a special case, because it can be optional */
#define __term   314    /* semicolon or newline */
/* other recognized tokens */
#define __number 315
#define __symbol 316
#define __label  317
#define __string 318
#define __expr   319    /* for assigment to "lastst" only */

/* (reversed) evaluation of staging buffer */
#define _startreorder 1
#define _endreorder   2
#define _exprstart    0xc0      /* top 2 bits set, rest is free */
#define _maxargs      64        /* relates to the bit pattern of _exprstart */

/* codes for ffabort() */
#define _exit           1       /* exit code in PRI */
#define _assertion      2       /* user abort */
#define _stackerror     3       /* stack/heap overflow */
#define _boundserror    4       /* array index out of bounds */
#define _memaccess      5       /* data access error */
#define _invinstr       6       /* invalid instruction */
#define _stackunderflow 7       /* stack underflow */
#define _heapunderflow  8       /* heap underflow */
#define _callbackerr    9       /* no, or invalid, callback */

/* Miscellanious  */
#define _no             0
#define _yes            1
#define _incseg         1       /* if parsing CODE */
#define _indseg         2       /* if parsing DATA */
#define _chkbounds      1       /* bit position in "debug" variable: check bounds */
#define _symbolic       2       /* bit position in "debug" variable: symbolic info */

#if !defined _MAX_PATH
  #define _MAX_PATH     255
#endif

/* interface functions */
#if defined __cplusplus
  extern "C" {
#endif
int sc_main(int argc, char **argv);
int sc_addconstant(char *name,cell value,int tag);
int sc_addtag(char *name);
#if defined __cplusplus
  }
#endif

/* function prototypes in SC1.C */
void dumplits(void);
void dumpzero(int count);
int gettag(char *name);
int constexpr(cell *val,int *tag);
constval *append_constval(constval *table,char *name,cell val);
constval *find_constval(constval *table,char *name);
void add_constant(char *name,cell val,int vclass,int tag);

/* function prototypes in SC2.C */
void pushstk(stkitem val);
stkitem popstk(void);
void plungefile(char *name);
void preprocess(void);
void lexinit(void);
int lex(cell *lexvalue,char **lexsym);
void lexpush(void);
int matchtoken(int token);
int tokeninfo(cell *val,char **str);
int needtoken(int token);
void stowlit(cell value);
int alphanum(char c);
int ishex(char c);
void delete_symbols(symbol *root,int level,int del_labels);
symbol *findglb(char *name);
symbol *findloc(char *name);
symbol *findconst(char *name);
symbol *finddepend(symbol *parent);
symbol *addsym(char *name,cell addr,int ident,int vclass,int tag,int usage);
symbol *addvariable(char *name,cell addr,int ident,int vclass,int tag,
                    int dim[],int numdim,int idxtag[]);
int getlabel(void);
char *itoh(ucell val);

/* function prototypes in SC3.C */
int hier14(value *lval1);       /* the highest expression level */
int expression(int *constant,cell *val,int *tag);

/* function prototypes in SC4.C */
void writetrailer(void);
void begcseg(void);
void begdseg(void);
cell nameincells(char *name);
void setfile(char *name,int fileno);
void setline(int line,int fileno);
void setlabel(int index);
void endexpr(void);
void startfunc(void);
void endfunc(void);
void defsymbol(char *name,int ident,int vclass,cell offset);
void symbolrange(int level,cell size);
void rvalue(value *lval);
void address(symbol *ptr);
void store(value *lval);
void memcopy(cell size);
void fillarray(symbol *sym,cell size,cell value);
void copyarray(symbol *sym,cell size);
void const1(cell val);
void const2(cell val);
void push1(void);
void pushval(cell val);
void pop2(void);
void ffswitch(int label);
void ffcase(cell value,char *labelname,int newtable);
void ffcall(symbol *sym,int numargs);
void ffret(void);
void ffabort(int reason);
void ffbounds(cell size);
void jumplabel(int number);
void defstorage(void);
void modstk(int delta);
void setstk(cell value);
void modheap(int delta);
void setheap_pri(void);
void setheap(cell value);
void cell2addr(void);
void cell2addr_alt(void);
void addr2cell(void);
void char2addr(void);
void charalign(void);
void addconst(cell value);

/*  Code generation functions for arithmetic operators.
 *
 *  Syntax: o[u|s|b]_name
 *          |   |   | +--- name of operator
 *          |   |   +----- underscore
 *          |   +--------- "u"nsigned operator, "s"igned operator or "b"oth
 *          +------------- "o"perator
 */
void os_mult(void); /* multiplication (signed) */
void os_div(void);  /* division (signed) */
void os_mod(void);  /* modulus (signed) */
void ob_add(void);  /* addition */
void ob_sub(void);  /* subtraction */
void ob_sal(void);  /* shift left (arithmetic) */
void os_sar(void);  /* shift right (arithmetic, signed) */
void ou_sar(void);  /* shift right (logical, unsigned) */
void ob_or(void);   /* bitwise or */
void ob_xor(void);  /* bitwise xor */
void ob_and(void);  /* bitwise and */
void os_le(void);   /* less or equal (signed) */
void os_ge(void);   /* greater or equal (signed) */
void os_lt(void);   /* less (signed) */
void os_gt(void);   /* greater (signed) */
void ob_eq(void);   /* equality */
void ob_ne(void);   /* inequality */

void lneg(void);
void neg(void);
void invert(void);
void inc(value *lval);
void dec(value *lval);
void jmp_ne0(int number);
void jmp_eq0(int number);
void outval(cell val, int newline);

/* function prototypes in SC5.C */
int error(int number,...);

/* function prototypes in SC6.C */
void assemble(FILE *fout,FILE *fin);

/* function prototypes in SC7.C */
void stgmark(char mark);
void stgwrite(char *st);
void stgout(int index);
void stgdel(int index,cell code_index);
int stgget(int *index,cell *code_index);
void stgset(int onoff);

/* external variables (defined in sc.c) */
extern symbol loctab;       /* local symbol table */
extern symbol glbtab;       /* global symbol table */
extern cell *litq;          /* the literal queue */
extern char pline[];        /* the line read from the input file */
extern char *lptr;          /* points to the current position in "pline" */
extern constval libname_tab;/* library table (#pragma library "..." syntax) */
extern char *inpfname;      /* name of the file currently read from */
extern char outfname[];     /* output file name */
extern char errfname[];     /* error file name */
extern char includepath[];  /* directory for system include files */

extern char ctrlchar;       /* the control character (or escape character) */
extern int litidx;          /* index to literal table */
extern int litmax;          /* current size of the literal table */
extern int stgidx;          /* index to the staging buffer */
extern int labnum;          /* number of (internal) labels */
extern int staging;         /* true if staging output */
extern cell declared;       /* number of local cells declared */
extern cell glb_declared;   /* number of global bytes declared */
extern cell code_idx;       /* number of bytes with generated code */
extern int ntv_funcid;      /* incremental number of native function */
extern int errflag;         /* 1 after first error in statement, -1 if no
                             * error on current line, but errors were found */
extern int errnum;          /* number of errors */
extern int warnnum;         /* number of warnings */
extern int verbose;         /* display extra information while compiling? */
extern int debug;           /* by default: full debug info */
extern int charbits;        /* number of bits for a character */
extern int packstr;         /* strings are packed by default? */
extern int needsemicolon;   /* semicolon required to terminate expressions? */
extern int curseg;          /* 1 if currently parsing CODE, 2 if parsing DATA */
extern cell stksize;        /* stack size */
extern int freading;        /* is there an input file ready for reading? */
extern int fline;           /* the line number in the current file */
extern int fnumber;         /* the file number in the file table (debugging) */
extern int fcurrent;        /* current file being processed (debugging) */
extern int intest;          /* true if inside a test */
extern int sideeffect;      /* true if an expression causes a side-effect */
extern int stmtindent;      /* current indent of the statement */
extern int indent_nowarn;   /* skip warning "217 loose indentation" */

extern FILE *inpf;          /* file read from */
extern FILE *outf;          /* file written to */

