/*  Small compiler
 *
 *  Function and variable definition and declaration, statement parser.
 *
 *  Copyright (c) ITB CompuPhase, 1997-1999
 *  This file may be freely used. No warranties of any kind.
 */
#include <assert.h>
#include <ctype.h>
#include <sys/io.h>
#include <limits.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#if defined __BORLANDC__ || defined __WATCOMC__
  #include <dos.h>
  static unsigned total_drives; /* dummy variable */
  #define dos_setdrive(i)      _dos_setdrive(i,&total_drives)
#endif
#if defined __BORLANDC__
  #include <dir.h>              /* for chdir() */
#elif defined __WATCOMC__
  #include <direct.h>           /* for chdir() */
#endif
#if defined __WIN32__ || defined _WIN32
  #include <windows.h>
#endif

#include "sc.h"
#define VERSION_STR "1.1"

static void initglobals(void);
static void setopt(int argc,char **argv,char *iname,char *oname,char *bname,
                   char *ename,char *pname,char *incpath);
static void setconfig(char *root);
static void about(void);
static void setconstants(void);
static void parse(void);
static void dumplits(void);
static void dumpzero(int count);
static void declglb(void);
static int declloc(int isstatic);
static void decl_const(int table);
static void decl_enum(int table);
static cell needsub(int *tag);
static void initials(int ident,int tag,cell *size,int dim[],int numdim);
static cell initvector(int ident,int tag,cell size,int fillzero);
static cell init(int ident,int *tag);
static symbol *fetchfunc(char *name,int tag);
static void funcstub(int native);
static void newfunc(int fpublic);
static int declargs(symbol *sym);
static void doarg(char *name,int ident,int offset,int tag,arginfo *arg);
static int testsymbols(symbol *root,int level,int testlabs,int testconst);
static void delete_consttable(constval *table);
static void statement(int *lastindent);
static void compound(void);
static void doexpr(int comma,int chkeffect,int allowarray,int *tag);
static void doassert(void);
static void doexit(void);
static void test(int label,int parens,int invert);
static void doif(void);
static void dowhile(void);
static void dodo(void);
static void dofor(void);
static void doswitch(void);
static void dogoto(void);
static void dolabel(void);
static symbol *fetchlab(char *name);
static void doreturn(void);
static void dobreak(void);
static void docont(void);
static void addwhile(int *ptr);
static void delwhile(void);
static int *readwhile(void);

constval tagname_tab  = { "", 0, NULL};  /* tagname table */
static symbol *curfunc= NULL;   /* pointer to current function */
static int lastst     = 0;      /* last executed statement type */
static int listing    = _no;    /* create .ASM file? */
static int ncmp       = 0;      /* number of active (open) compound statements */
static int rettype    = 0;      /* the type that a "return" expression should have */
static int skipinput  = 0;      /* number of lines to skip from the first input file */
static int wq[_wqtabsz];        /* "while queue", internal stack for nested loops */
static int *wqptr;              /* pointer to next entry */


#if !defined NO_MAIN
int main(int argc, char **argv)
{
  glbtab.next=NULL;     /* clear global variables/constants table */
  loctab.next=NULL;     /*   "   local      "    /    "       "   */
  tagname_tab.next=NULL;
  libname_tab.next=NULL;

  return sc_main(argc,argv);
}
#endif

/*  "main" of the compiler
 */
#if defined __cplusplus
  extern "C"
#endif
int sc_main(int argc, char **argv)
{
  int entry;
  char incfname[_MAX_PATH];
  char binfname[_MAX_PATH];     /* binary file name */
  FILE *binf;

  litmax=_def_litmax;

  /* set global variables to their initial value */
  initglobals();
  lexinit();

  /* allocate all memory */
  inpfname=(char *)malloc(_MAX_PATH);
  litq=(long *)malloc(litmax*sizeof(cell));
  if (litq==NULL)
    error(103);         /* insufficient memory */

  setopt(argc,argv,inpfname,outfname,binfname,errfname,incfname,includepath);
  if (strlen(errfname)!=0)
    unlink(errfname);   /* delete file on startup */
  else
    puts("Small compiler " VERSION_STR "\t\tCopyright (c) 1997-1999, ITB CompuPhase\n");
  setconfig(argv[0]);
  inpf=fopen(inpfname,"rt");
  if (inpf==NULL)
    error(100,inpfname);
  freading=_yes;
  outf=fopen(outfname,"wt");    /* first write to assembler file (may be temporary) */
  if (outf==NULL)
    error(101,outfname);
  /* immediately open the binary file, for other programs to check */
  if (!listing) {
    binf=fopen(binfname,"wb");
    if (binf==NULL)
      error(101,binfname);
  } /* if */
  setconstants();               /* set predefined constants and tagnames */
  append_constval(&libname_tab,"",0);   /* set "unnamed" libary index */
  setfile(inpfname,fnumber);
  while (skipinput>0) {         /* skip lines in the input file */
    if (fgets(pline,_linemax,inpf)!=NULL)
      fline++;                  /* keep line number up to date */
    skipinput--;
  } /* while */
  if (strlen(incfname)>0)
    plungefile(incfname);       /* parse implicit include file */
  preprocess();         /* fetch first line */
  parse();              /* process all input */
  entry=testsymbols(&glbtab,0,_yes,_no);/* test for unused or undefined
                                         * functions and variables */
  if (!entry)
    error(13);          /* no entry point (no public functions) */
  /* inpf is already closed when readline() attempts to pop of a file */
  writetrailer();       /* write remaining stuff */
  fclose(outf);
  /* write the binary file (the file is already open) */
  if (!listing && errnum==0) {
    inpf=fopen(outfname,"rt");
    assemble(binf,inpf);
    fclose(inpf);
    fclose(binf);
  } /* if */

  assert(loctab.next==NULL);    /* local symbols should already have been deleted */
  free(inpfname);
  free(litq);
  delete_symbols(&glbtab,0,_yes);
  delete_consttable(&tagname_tab);
  delete_consttable(&libname_tab);
  if (!listing)
    unlink(outfname);
  if (errnum!=0){
    if (strlen(errfname)==0)
      printf("\n%d Error%s.\n",errnum,(errnum>1) ? "s" : "");
    unlink(binfname);
    return 2;
  } else if (warnnum!=0){
    if (strlen(errfname)==0)
      printf("\n%d Warning%s.\n",warnnum,(warnnum>1) ? "s" : "");
    return 1;
  } else {
    if (strlen(errfname)==0)
      puts("Done.");
    return 0;
  } /* if */
}

#if defined __cplusplus
  extern "C"
#endif
int sc_addconstant(char *name,cell value,int tag)
{
  errflag=1;            /* make sure error engine is silenced */
  add_constant(name,value,_global,tag);
  return 1;
}

#if defined __cplusplus
  extern "C"
#endif
int sc_addtag(char *name)
{
  cell val;
  constval *ptr;
  int next;

  if (name==NULL) {
    /* no tagname was given, check for one */
    if (lex(&val,&name)!=__labelX) {
      lexpush();
      return 0;         /* untagged */
    } /* if */
  } /* if */

  next=0;
  ptr=tagname_tab.next;
  while (ptr!=NULL) {
    assert(next==(int)ptr->value);
    if (strcmp(name,ptr->name)==0)
      return next;      /* tagname is known, return its sequence number */
    ptr=ptr->next;
    next++;
  } /* while */

  /* tagname currently unknown, add it */
  append_constval(&tagname_tab,name,next);
  return next;
}

static void initglobals(void)
{
  curfunc=NULL;         /* pointer to current function */
  lastst=0;             /* last executed statement type */
  listing=_no;          /* create .ASM file? */
  ncmp=0;               /* number of active (open) compound statements */
  rettype=0;            /* the type that a "return" expression should have */
  skipinput=0;          /* number of lines to skip from the first input file */
  ctrlchar=CTRL_CHAR;   /* the control character (or escape character)*/
  litidx=0;             /* index to literal table */
  litmax=_def_litmax;   /* current size of the literal table */
  stgidx=0;             /* index to the staging buffer */
  labnum=0;             /* number of (internal) labels */
  staging=0;            /* true if staging output */
  declared=0;           /* number of local cells declared */
  glb_declared=0;       /* number of global bytes declared */
  code_idx=0;           /* number of bytes with generated code */
  ntv_funcid=0;         /* incremental number of native function */
  errflag=0;
  errnum=0;             /* number of errors */
  warnnum=0;            /* number of warnings */
  verbose=_no;          /* display extra information while compiling? */
  debug=_chkbounds;     /* by default: bounds checking+assertions */
  charbits=8;           /* a "char" is 8 bits */
  packstr=_no;          /* strings are packed by default? */
  needsemicolon=_yes;   /* semicolon required to terminate expressions? */
  curseg=0;             /* 1 if currently parsing CODE, 2 if parsing DATA */
  stksize=2048;         /* default stack size */
  freading=_no;         /* Is there an input file ready for reading? */
  fline=0;              /* the line number in the current file */
  fnumber=0;            /* the file number in the file table (debugging) */
  fcurrent=0;           /* current file being processed (debugging) */
  intest=0;             /* true if inside a test */
  sideeffect=0;         /* true if an expression causes a side-effect */
  stmtindent=0;         /* current indent of the statement */
  indent_nowarn=0;      /* skip warning "217 loose indentation" */

  outfname[0]='\0';     /* output file name */
  errfname[0]='\0';     /* error file name */
  includepath[0]='\0';  /* directory for system include files */
  inpf=NULL;            /* file read from */
  outf=NULL;            /* file written to */

  wqptr=wq;             /* initialize while queue pointer */
}

/* Parsing command line options is indirectly recursive: parseoptions()
 * calls parserespf() to handle options in a a response file and
 * parserespf() calls parseoptions() at its turn after having created
 * an "option list" from the contents of the file.
 */
static void parserespf(char *filename,char *iname,char *oname,
                       char *bname,char *ename,char *pname,char *incpath);

static void parseoptions(int argc,char **argv,char *iname,char *oname,
                         char *bname,char *ename,char *pname,char *incpath)
{
  char str[_MAX_PATH],*ptr;
  int arg,i;

  for (arg=1; arg<argc; arg++){
    if (argv[arg][0]=='/' || argv[arg][0]=='-') {
      ptr=&argv[arg][1];
      switch (*ptr) {
      case 'a':
        listing=_yes;           /* skip last pass of making binary file */
        break;
      case 'c':
        i=atoi(ptr+1);
        if (i==8 || i==16)
          charbits=i;           /* character size is 8 or 16 bits */
        else
          about();
        break;
#if defined dos_setdrive
      case 'D':                 /* set active directory */
        ptr++;
        if (ptr[1]==':')
          dos_setdrive(toupper(*ptr)-'A');   /* set active drive */
        chdir(ptr);
        break;
#endif
      case 'd':
        switch (ptr[1]) {
        case '0':
          debug=0;
          break;
        case '1':
          debug=_chkbounds;     /* assertions and bounds checking */
          break;
        case '2':
          debug=_chkbounds | _symbolic; /* also symbolic info */
          break;
        default:
          about();
        } /* switch */
        break;
      case 'e':
        strcpy(ename,ptr+1);    /* set name of error file */
        break;
      case 'i':
        strcpy(incpath,ptr+1);  /* set name of include directory */
        i=strlen(incpath);
        if (i>0 && incpath[i-1]!='\\')
          strcat(incpath,"\\");
        break;
      case 'o':
        strcpy(bname,ptr+1);    /* set name of binary output file */
        break;
      case 'P':
        packstr=_yes;
        break;
      case 'p':
        strcpy(pname,ptr+1);    /* set name of implicit include file */
        break;
      case 's':
        skipinput=atoi(ptr+1);
        break;
      case 'v':
        verbose=_yes;           /* output function names to the screen */
        break;
      case '\\':                /* use \ instead of ^ for control characters */
        ctrlchar='\\';
        break;
      case ';':
        needsemicolon=_no;
        break;
      default:                  /* wrong option */
        about();
      } /* switch */
    } else if (argv[arg][0]=='@') {
      parserespf(&argv[arg][1],iname,oname,bname,ename,pname,incpath);
    } else if ((ptr=strchr(argv[arg],'='))!=NULL) {
      i=(int)(ptr-argv[arg]);
      if (i>_namemax)
        i=_namemax;
      strncpy(str,argv[arg],i);
      str[i]='\0';              /* str holds symbol name */
      i=atoi(ptr+1);
      add_constant(str,i,_global,0);
    } else if (strlen(iname)>0) {
      about();
    } else {
      strcpy(str,argv[arg]);
      strlwr(str);
      strcpy(iname,str);
      if ((ptr=strchr(str,'.'))==NULL)
        strcat(iname,".sma");
      else
        *ptr=0;   /* set zero terminator at the position of the period */
      /* The output name is the input name with the extension .ASM. The
       * binary file has the extension .AMX. */
      if ((ptr=strrchr(str,'\\'))!=NULL)
        ptr++;          /* strip path */
      else
        ptr=str;
      strcpy(oname,ptr);
      strcat(oname,".asm");
      if (strlen(bname)==0) {
        strcpy(bname,ptr);
        strcat(bname,".amx");
      } /* if */
    } /* if */
  } /* for */
}

static void parserespf(char *filename,char *iname,char *oname,
                       char *bname,char *ename,char *pname,char *incpath)
{
#define MAX_OPTIONS     100
  FILE *fp;
  char *string, *ptr, **argv;
  int argc;
  long size;

  if ((fp=fopen(filename,"rt"))==NULL)
    error(100,filename);        /* error reading input file */
  /* load the complete file into memory */
  fseek(fp,0L,SEEK_END);
  size=ftell(fp);
  fseek(fp,0L,SEEK_SET);
  assert(size<INT_MAX);
  if ((string=(char *)malloc((int)size+1))==NULL)
    error(103);                 /* insufficient memory */
  /* fill with zeros; in MS-DOS, fread() may collapse CR/LF pairs to
   * a single '\n', so the string size may be smaller than the file
   * size. */
  memset(string,0,(int)size+1);
  fread(string,1,(int)size,fp);
  fclose(fp);
  /* allocate table for option pointers */
  if ((argv=(char **)malloc(MAX_OPTIONS*sizeof(char*)))==NULL)
    error(103);                 /* insufficient memory */
  /* fill the options table */
  ptr=strtok(string," \t\r\n");
  for (argc=1; argc<MAX_OPTIONS && ptr!=NULL; argc++) {
    /* note: the routine skips argv[0], for compatibility with main() */
    argv[argc]=ptr;
    ptr=strtok(NULL," \t\r\n");
  } /* if */
  if (ptr!=NULL)
    error(102,"option table");  /* table overflow */
  /* parse the option table */
  parseoptions(argc,argv,iname,oname,bname,ename,pname,incpath);
  /* free allocated memory */
  free(argv);
  free(string);
}

static void setopt(int argc,char **argv,char *iname,char *oname,char *bname,
                   char *ename,char *pname,char *incpath)
{
  *iname='\0';
  *bname='\0';
  *ename='\0';
  *pname='\0';

  #if 0 /* needed to test with BoundsChecker for DOS (it does not pass
         * through arguments) */
    strcpy(iname,"test.sma");
    strcpy(oname,"test.asm");
    strcpy(bname,"test.amx");
  #endif

  parseoptions(argc,argv,iname,oname,bname,ename,pname,incpath);
  if (strlen(iname)==0)
    about();
}

static void setconfig(char *root)
{
  char *ptr;

  if (strlen(includepath)==0) {
    #if defined __WIN32__ || defined _WIN32
      GetModuleFileName(NULL,includepath,_MAX_PATH);
    #else if defined __MSDOS__
      strcpy(includepath,root); /* filename with options */
      /* strip the options */
      if ((ptr=strpbrk(includepath," \t/"))!=NULL)
        *ptr='\0';
    #endif
    /* terminate just behind last \ or : */
    if ((ptr=strrchr(includepath,'\\'))!=NULL || (ptr=strchr(includepath,':'))!=NULL)
      *(ptr+1)='\0';
    else
      /* there was no terminating \ or : so the filename probably does not
       * contain the path */
      includepath[0]='\0';
    strcat(includepath,"include\\");
  } /* if */
}

static void about(void)
{
  if (strlen(errfname)==0) {
    puts("Usage:   sc <filename> [options]\n");
    puts("Options:");
    puts("         -a       output assembler code");
    puts("         -c8      [default] a character is 8-bits (ASCII/ISO Latin-1)");
    puts("         -c16     a character is 16-bits (Unicode)");
#if defined dos_setdrive
    puts("         -Dpath   active directory path");
#endif
    puts("         -d0      no symbolic information, no run-time checks");
    puts("         -d1      [default] run-time checks, no symbolic information");
    puts("         -d2      full debug information and dynamic checking");
    puts("         -e<name> set name of error file (quiet compile)");
    puts("         -i<name> path for include files");
    puts("         -o<name> set name of binary output file");
    puts("         -P       strings are \"packed\" by default");
    puts("         -p<name> set name of \"prefix\" file");
    puts("         -s<num>  skip lines from the input file");
    puts("         -v       verbose, detailed messages");
    puts("         -\\       use '\\' instead of '^' for control characters");
    puts("         -;       semicolon is optional");
    puts("         sym=val  define constant \"sym\" with value \"val\"");
    puts("         sym=     define constant \"sym\" with value 0");
  } /* if */
  exit(0);
}

static void setconstants(void)
{
  append_constval(&tagname_tab,"",0);   /* "untagged" */
  append_constval(&tagname_tab,"bool",1);

  add_constant("true",1,_global,1);     /* boolean flags */
  add_constant("false",0,_global,1);
  #if defined(BIT16)
    add_constant("cellbits",16,_global,0);
    add_constant("cellmax",SHORT_MAX,_global,0);
    add_constant("cellmin",SHORT_MIN,_global,0);
  #else
    add_constant("cellbits",32,_global,0);
    add_constant("cellmax",LONG_MAX,_global,0);
    add_constant("cellmin",LONG_MIN,_global,0);
  #endif
  add_constant("charbits",charbits,_global,0);
  add_constant("charmin",0,_global,0);
  add_constant("charmax",(charbits==16) ? 0xffff : 0xff,_global,0);

  add_constant("language",1,_global,0);
  add_constant("compiler",1,_global,0);
}

/*  parse       - process all input text
 *
 *  At this level, only static declarations and function definitions are legal.
 */
static void parse(void)
{
  int tok;
  cell val;
  char *str;

  while (freading){
    /* first try whether a declaration possibly is native or public */
    tok=lex(&val,&str);  /* read in (new) token */
    switch (tok) {
    case 0:
      /* ignore zero's */
      break;
    case __newX:
    case __staticX:
      declglb();
      break;
    case __constX:
      decl_const(_global);
      break;
    case __enumX:
      decl_enum(_global);
      break;
    case __publicX:
      newfunc(_yes);
      break;
    case __labelX:
    case __symbolX:
      lexpush();
      newfunc(_no);
      break;
    case __nativeX:
      funcstub(_yes);           /* create a dummy function */
      break;
    case __forwardX:
      funcstub(_no);
      break;
    case '}':
      error(54);                /* unmatched closing brace */
      break;
    case '{':
      error(55);                /* start of function body without function header */
      break;
    default:
      if (freading)
        error(10);              /* illegal function or declaration */
    } /* switch */
  } /* while */
}

/*  dumplits
 *
 *  Dump the literal pool (strings etc.)
 *
 *  Global references: litidx (referred to only)
 */
static void dumplits(void)
{
  int j,k;

  k=0;
  while (k<litidx){
    /* should be in the data segment */
    assert(curseg==2);
    defstorage();
    j=16;       /* 16 values per line */
    while (j && k<litidx){
      outval(litq[k], _no);
      stgwrite(" ");
      k++;
      j--;
      if (j==0 || k>=litidx)
        stgwrite("\n");         /* force a newline after 10 dumps */
      /* Note: stgwrite() buffers a line until it is complete. It recognizes
       * the end of line as a sequence of "\n\0", so something like "\n\t"
       * so  should not be passed to stgwrite().
       */
    } /* while */
  } /* while */
}

/*  dumpzero
 *
 *  Dump zero's for default initial values
 */
static void dumpzero(int count)
{
  int i;

  if (count<=0)
    return;
  assert(curseg==2);
  defstorage();
  i=0;
  while (count-- > 0) {
    outval(0, _no);
    i=(i+1) % 16;
    stgwrite((i==0 || count==0) ? "\n" : " ");
  } /* while */
}

/*  declglb     - declare global symbols
 *
 *  Declare a static (global) variable. Global variables are stored in
 *  the DATA segment.
 *
 *  global references: glb_declared     (altered)
 */
static void declglb(void)
{
  int ident,tag;
  int idxtag[_dimen_max];
  char name[_namemax+1];
  cell val,size;
  char *str;
  int dim[_dimen_max];
  int numdim,level;

  do {
    size=1;             /* single size (no array) */
    numdim=0;           /* no dimensions */
    ident=_variable;
    tag=sc_addtag(NULL);
    if (lex(&val,&str)!=__symbolX)       /* read in (new) token */
      error(20,str);                    /* invalid symbol name */
    strcpy(name,str);                   /* save symbol name */
    if (findglb(name) || findconst(name))
      error(21,name);                   /* symbol already defined */
    while (matchtoken('[')) {
      ident=_array;
      if (numdim == _dimen_max) {
        error(53);              /* exceeding maximum number of dimensions */
        return;
      } /* if */
      if (numdim>0 && dim[numdim-1]==0)
        error(52);              /* only last dimension may be variable length */
      size=needsub(&idxtag[numdim]);    /* get size; size==0 for "var[]" */
      #if INT_MAX < LONG_MAX
        if (size > INT_MAX)
          error(105);           /* overflow, exceeding capacity */
      #endif
      dim[numdim++]=(int)size;
    } /* while */
    defsymbol(name,ident,_global,sizeof(cell)*glb_declared);
    begdseg();          /* real (initialized) data in data segment */
    litidx=0;           /* global initial data is dumped, so restart at zero */
    initials(ident,tag,&size,dim,numdim);/* stores values in the literal queue */
    assert(size>=litidx);
    if (numdim==1)
      dim[0]=(int)size;
    dumplits();         /* dump the literal queue */
    dumpzero((int)size-litidx);
    addvariable(name,sizeof(cell)*glb_declared,ident,_global,tag,
                dim,numdim,idxtag);
    if (ident==_array)
      for (level=0; level<numdim; level++)
        symbolrange(level,dim[level]);
    glb_declared+=(int)size;    /* add total number of cells */
  } while (matchtoken(',')); /* enddo */   /* more? */
  needtoken(__termX);    /* if not comma, must be semicolumn */
}

/*  declloc     - declare local symbols
 *
 *  Declare local (automatic) variables. Since these variables are relative
 *  to the STACK, there is no switch to the DATA segment. These variables
 *  cannot be initialized either.
 *
 *  global references: declared   (altered)
 *                     funcstatus (referred to only)
 */
static int declloc(int isstatic)
{
  int ident,tag;
  int idxtag[_dimen_max];
  char name[_namemax+1];
  symbol *sym;
  cell val,size;
  char *str;
  value lval;
  int cur_lit;
  int dim[_dimen_max];
  int numdim,level;

  do {
    ident=_variable;
    size=1;
    numdim=0;           /* no dimensions */
    tag=sc_addtag(NULL);
    if (lex(&val,&str)!=__symbolX)       /* read in (new) token */
      error(20,str);                    /* invalid symbol name */
    strcpy(name,str);                   /* save symbol name */
    /* Note: block locals may be named identical to locals at higher
     * compound blocks (as with standard C); so we must check (and add)
     * the "nesting level" of local variables to verify the
     * multi-definition of symbols.
     */
    if ((sym=findloc(name))!=NULL && sym->compound==ncmp)
      error(21,name);           /* symbol already defined */
    /* Although valid, a local variable that is equal to a global
     * variable or to a local variable at a lower level might
     * indicate a bug.
     */
    if ((sym=findloc(name))!=NULL && sym->compound!=ncmp || findglb(name)!=NULL)
      error(219,name);          /* variable shadows another symbol */
    while (matchtoken('[')){
      ident=_array;
      if (numdim == _dimen_max) {
        error(53);              /* exceeding maximum number of dimensions */
        return ident;
      } /* if */
      if (numdim>0 && dim[numdim-1]==0)
        error(52);              /* only last dimension may be variable length */
      size=needsub(&idxtag[numdim]);    /* get size; size==0 for "var[]" */
      #if INT_MAX < LONG_MAX
        if (size > INT_MAX)
          error(105);           /* overflow, exceeding capacity */
      #endif
      dim[numdim++]=(int)size;
    } /* while */
    if (ident==_array || isstatic) {
      cur_lit=litidx;           /* save current index in the literal table */
      initials(ident,tag,&size,dim,numdim);
      if (size==0)
        return ident;           /* error message already given */
    } /* if */
    /* reserve memory (on the stack) for the variable */
    if (isstatic) {
      /* write zeros for uninitialized fields */
      while (litidx<cur_lit+size)
        stowlit(0);
      sym=addvariable(name,(cur_lit+glb_declared)*sizeof(cell),ident,_static,
                      tag,dim,numdim,idxtag);
      defsymbol(name,ident,_static,(cur_lit+glb_declared)*sizeof(cell));
    } else {
      declared+=(int)size;      /* variables are put on stack, adjust "declared" */
      sym=addvariable(name,-declared*sizeof(cell),ident,_local,tag,
                      dim,numdim,idxtag);
      defsymbol(name,ident,_local,-declared*sizeof(cell));
      modstk(-(int)size*sizeof(cell));
    } /* if */
    /* now that we have reserved memory for the variable, we can proceed
     * to initialize it */
    assert(sym!=NULL);          /* we declared it, it must be there */
    sym->compound=ncmp;         /* for multiple declaration/shadowing check */
    if (ident==_array)
      for (level=0; level<numdim; level++)
        symbolrange(level,dim[level]);
    if (!isstatic) {            /* static variables already initialized */
      if (ident==_variable) {
        /* simple variable, also supports initialization */
        int ctag = tag;         /* set to "tag" by default */
        int explicit_init=_no;  /* is the variable explicitly initialized? */
        if (matchtoken('=')) {
          doexpr(_no,_no,_no,&ctag);
          explicit_init=_yes;
        } else {
          const1(0);            /* uninitialized variable, set to zero */
        } /* if */
        /* now try to save the value (still in PRI) in the variable */
        lval.sym=sym;
        lval.ident=_variable;
        lval.constval=0;
        store(&lval);
        if (tag!=0 && tag!=ctag)
          error(213);           /* tagname mismatch */
        /* if the variable was not explicitly initialized, reset the
         * "_written" flag that store() set */
        if (!explicit_init)
          sym->usage &= ~_written;
      } else {
        /* an array */
        assert(cur_lit>=0 && cur_lit<=litidx && litidx<=litmax);
        /* if the array is not completely filled, set all values to zero first */
        assert(size>0 && size>=sym->dim.array.length);
        assert(numdim>1 || size==sym->dim.array.length);
        if (litidx-cur_lit < size)
          fillarray(sym,size*sizeof(cell),0);
        if (cur_lit<litidx) {
          /* check whether the complete array is set to a single value; if
           * it is, more compact code can be generated */
          cell first=litq[cur_lit];
          int i;
          for (i=cur_lit; i<litidx && litq[i]==first; i++)
            /* nothing */;
          if (i==litidx) {
            /* all values are the same */
            fillarray(sym,(litidx-cur_lit)*sizeof(cell),first);
            litidx=cur_lit;     /* reset literal table */
          } else {
            /* copy the literals to the array */
            const1((cur_lit+glb_declared)*sizeof(cell));
            copyarray(sym,(litidx-cur_lit)*sizeof(cell));
          } /* if */
        } /* if */
      } /* if */
    } /* if */
  } while (matchtoken(',')); /* enddo */   /* more? */
  needtoken(__termX);    /* if not comma, must be semicolumn */
  return ident;
}

static cell calc_arraysize(int dim[],int numdim,int cur)
{
  if (cur==numdim)
    return 0;
  return dim[cur]+(dim[cur]*calc_arraysize(dim,numdim,cur+1));
}

/*  initials
 *
 *  Initialize global objects and local arrays.
 *    size==array cells (count), if 0 on input, the routine counts the number of elements
 *    tag==required tagname id (not the returned tag)
 *
 *  Global references: litidx (altered)
 */
static void initials(int ident,int tag,cell *size,int dim[],int numdim)
{
  int ctag;
  int curlit=litidx;
  int d;

  if (!matchtoken('=')) {
    if (ident==_array && dim[numdim-1]==0) {
      /* declared as "myvar[];" which is senseless (note: this *does* make
       * sense in the case of a _refarray, which is a function parameter)
       */
      error(9);         /* array has zero length -> invalid size */
    } /* if */
    if (numdim>1) {
      /* initialize the indirection tables */
      #if _dimen_max>2
        #error Array algorithms for more than 2 dimensions are not implemented
      #endif
      assert(numdim==2);
      *size=calc_arraysize(dim,numdim,0);
      for (d=0; d<dim[0]; d++)
        stowlit((dim[0]+d*(dim[1]-1)) * sizeof(cell));
    } /* if */
    return;
  } /* if */

  if (ident==_variable) {
    assert(*size==1);
    init(ident,&ctag);
    if (tag!=0 && tag!=ctag)
      error(213);       /* tagname mismatch */
  } else {
    assert(numdim>0);
    if (numdim==1) {
      *size=initvector(ident,tag,dim[0],_no);
    } else {
      cell offs,dsize;
      /* The simple algorithm below only works for arrays with one or two
       * dimensions. This should be some recursive algorithm.
       */
      #if _dimen_max>2
        #error Array algorithms for more than 2 dimensions are not implemented
      #endif
      assert(numdim==2);
      if (dim[numdim-1]!=0)
        *size=calc_arraysize(dim,numdim,0); /* set size to (known) full size */
      /* dump indirection tables */
      for (d=0; d<dim[0]; d++)
        stowlit(0);
      /* now dump individual vectors */
      needtoken('{');
      offs=dim[0];
      for (d=0; d<dim[0]; d++) {
        litq[curlit+d]=offs*sizeof(cell);
        dsize=initvector(ident,tag,dim[1],_yes);
        offs+=dsize-1;
        if (d+1<dim[0])
          needtoken(',');
      } /* for */
      needtoken('}');
    } /* if */
  } /* if */

  if (*size==0)
    *size=litidx-curlit;                /* number of elements defined */
}

/*  initvector
 *  Initialize a single dimensional array
 */
static cell initvector(int ident,int tag,cell size,int fillzero)
{
  cell prev1=0,prev2=0;
  int ctag;
  int ellips=_no;
  int curlit=litidx;

  assert(ident==_array || ident==_refarray);
  if (matchtoken('{')) {
    do {
      if ((ellips=matchtoken(__ellips))!=0)
        break;
      prev2=prev1;
      prev1=init(ident,&ctag);
      if (tag!=0 && tag!=ctag)
        error(213);           /* tagname mismatch */
    } while (matchtoken(',')); /* do */
    needtoken('}');
  } else {
    init(ident,&ctag);
    if (tag!=0 && tag!=ctag)
      error(213);     /* tagname mismatch */
  } /* if */
  /* fill up the literal queue with a series */
  if (ellips) {
    cell step=((litidx-curlit)==1) ? (cell)0 : prev1-prev2;
    if (size==0 || (litidx-curlit)==0)
      error(41);        /* invalid ellipsis, array size unknown */
    else if ((litidx-curlit)==(int)size)
      error(18);        /* initialisation data exceeds declared size */
    while ((litidx-curlit)<(int)size) {
      prev1+=step;
      stowlit(prev1);
    } /* while */
  } /* if */
  if (fillzero && size>0) {
    while ((litidx-curlit)<(int)size)
      stowlit(0);
  } /* if */
  if (size==0) {
    size=litidx-curlit;                 /* number of elements defined */
  } else if (litidx-curlit>(int)size) { /* e.g. "myvar[3]={1,2,3,4};" */
    error(18);                  /* initialisation data exceeds declared size */
    litidx=(int)size+curlit;    /* avoid overflow in memory moves */
  } /* if */
  return size;
}

/*  init
 *
 *  Evaluate one initializer.
 */
static cell init(int ident,int *tag)
{
  cell i = 0;

  if (matchtoken(__stringX)){
    /* lex() automatically stores strings in the literal table (and
     * increases "litidx") */
    if (ident==_variable)
      error(6);         /* must be assigned to an array */
    *tag=0;
  } else if (constexpr(&i,tag)){
    stowlit(i);         /* store expression result in literal table */
  } /* if */
  return i;
}

/*  needsub
 *
 *  Get required array size
 */
static cell needsub(int *tag)
{
  cell val;

  *tag=0;
  if (matchtoken(']'))  /* we've already seen "[" */
    return 0;           /* null size (like "char msg[]") */
  constexpr(&val,tag);  /* get value (must be constant expression) */
  if (val<0) {
    error(9);           /* negative array size is invalid; assumed zero */
    val=0;
  } /* if */
  needtoken(']');
  return val;           /* return array size */
}

/*  decl_const  - declare a single constant
 *
 */
static void decl_const(int vclass)
{
  char constname[_namemax+1];
  cell val;
  char *str;
  int tag;

  tag=sc_addtag(NULL);
  if (lex(&val,&str)!=__symbolX)         /* read in (new) token */
    error(20,str);                      /* invalid symbol name */
  strcpy(constname,str);                /* save symbol name */
  needtoken('=');
  constexpr(&val,NULL);                 /* get value */
  needtoken(__termX);
  /* add_constant() checks for duplicate definitions */
  add_constant(constname,val,vclass,tag);
}

/*  decl_enum   - declare enumerated constants
 *
 */
static void decl_enum(int vclass)
{
  char enumname[_namemax+1],constname[_namemax+1];
  cell val,value,size;
  char *str;
  int tag;

  if (lex(&val,&str)==__symbolX) {       /* read in (new) token */
    strcpy(enumname,str);               /* save enum name (last constant) */
    tag=sc_addtag(enumname);
  } else {
    lexpush();                          /* analyze again */
    enumname[0]='\0';
    tag=0;
  } /* if */
  needtoken('{');
  /* go through all constants */
  value=0;                              /* default starting value */
  do {
    if (matchtoken('}')) {              /* quick exit if '}' follows ',' */
      lexpush();
      break;
    } /* if */
    if (lex(&val,&str)!=__symbolX)       /* read in (new) token */
      error(20,str);                    /* invalid symbol name */
    strcpy(constname,str);              /* save symbol name */
    size=1;                             /* default increment of 'val' */
    if (matchtoken('='))
      constexpr(&value,NULL);           /* get value */
    else if (matchtoken(':'))
      constexpr(&size,NULL);            /* get size */
    /* add_constant() checks whether a variable (global or local) or
     * a constant with the same name already exists */
    add_constant(constname,value,vclass,tag);
    value+=size;
  } while (matchtoken(','));
  needtoken('}');       /* terminates the constant list */
  matchtoken(';');      /* eat an optional ; */
  /* set the enum name to the last value plus one */
  if (strlen(enumname)>0)
    add_constant(enumname,value,vclass,tag);
}

/*
 *  Finds a function in the global symbol table or creates a new entry.
 *  It does some basic processing and error checking.
 */
static symbol *fetchfunc(char *name,int tag)
{
  symbol *sym;
  cell offset;

  offset=code_idx;
  if ((debug & _symbolic)!=0) {
    offset+=opcodes(1)+opargs(3)+nameincells(name);
    /* ^^^ The address for the symbol is the code address. But the "symbol"
     *     instruction itself generates code. Therefore the offset is
     *     pre-adjusted to the value it will have after the symbol instruction.
     */
  } /* if */
  if ((sym=findglb(name))!=0) {         /* already in symbol table? */
    if (sym->ident!=_functn)
      error(21,name);                       /* yes, but not as function */
    else if ((sym->usage & _define)!=0)
      error(21,name);                       /* yes, and it's already defined */
    else if ((sym->usage & _native)!=0)
      error(21,name);                       /* yes, and it is an native */
    assert(sym->vclass==_global);
    if ((sym->usage & _define)==0) {
      /* as long as the function stays undefined, update the address */
      sym->addr=offset;
    } /* if */
  } else {
    /* don't set the "_define" flag; it may be a prototype */
    sym=addsym(name,offset,_functn,_global,tag,0);
    /* assume no arguments */
    sym->dim.arglist=(arginfo*)malloc(1*sizeof(arginfo));
    sym->dim.arglist[0].ident=0;
  } /* if */
  return sym;
}

/* This routine adds symbolic information for each argument.
 */
static void define_args(void)
{
  symbol *sym;

  /* At this point, no local variables have been declared. All
   * local symbols are function arguments.
   */
  sym=loctab.next;
  while (sym!=NULL) {
    assert(sym->ident!=_label);
    assert(sym->vclass==_local);
    defsymbol(sym->name,sym->ident,_local,sym->addr);
    if (sym->ident==_refarray) {
      symbol *sub=sym;
      while (sub!=NULL) {
        symbolrange(sub->dim.array.level,sub->dim.array.length);
        sub=finddepend(sub);
      } /* while */
    } /* if */
    sym=sym->next;
  } /* while */
}

static void funcstub(int native)
{
  int tok,tag;
  char *str;
  cell val;
  char symbolname[_namemax+1];
  symbol *sym;

  lastst=0;
  litidx=0;                     /* clear the literal pool */
  assert(loctab.next==NULL);    /* local symbol table should be empty */

  tag=sc_addtag(NULL);
  tok=lex(&val,&str);
  if (native) {
    if (tok==__publicX || tok==__symbolX && *str==PUBLIC_CHAR)
      error(42);                /* invalid combination of class specifiers */
  } else {
    if (tok==__publicX)
      tok=lex(&val,&str);
  } /* if */
  if (tok!=__symbolX)
    error(10);                  /* illegal function or declaration */
  strcpy(symbolname,str);
  needtoken('(');               /* only functions may be native/forward */

  sym=fetchfunc(symbolname,tag);/* get a pointer to the function entry */
  sym->usage|=_define;
  if (native)
    sym->usage|=_native | _retvalue;

  declargs(sym);
  #if 0
    /* Read in a module name and find it in the "library names" list.
     * This feature has not (yet) been implemented. */
    sym->x.libidx=lib;          /* save library index */
  #endif
  /* "declargs()" found the ")". A native declaration must be a prototype,
   * so the next token must be a semicolon */
  needtoken(__termX);
  litidx=0;                     /* clear the literal pool */
  delete_symbols(&loctab,0,_yes);/* clear local variables queue */
}

/*  newfunc    - begin a function
 *
 *  This routine is called from "parse" and tries to make a function
 *  out of the following text
 *
 *  Global references: funcstatus,lastst,litidx
 *                     rettype  (altered)
 *                     curfunc  (altered)
 *                     declared (altered)
 *                     glb_declared (altered)
 */
static void newfunc(int fpublic)
{
  symbol *sym;
  int argcnt,tok,tag,funcline;
  char symbolname[_namemax+1];
  char *str;
  cell val;

  lastst=0;             /* no statement yet */
  litidx=0;             /* clear the literal pool */
  assert(loctab.next==NULL);    /* local symbol table should be empty */

  tag=sc_addtag(NULL);
  tok=lex(&val,&str);
  if (tok==__nativeX)
    error(42);                  /* invalid combination of class specifiers */
  else if (tok!=__symbolX && freading)
    error(10);          /* illegal function or declaration */
  funcline=fline;       /* save line at which the function is defined */
  strcpy(symbolname,str);
  if (symbolname[0]==PUBLIC_CHAR)
    fpublic=_yes;       /* implicitly public function */
  /* check whether this is a function or a variable declaration */
  if (!matchtoken('(')) {
    error(10);          /* illegal function or declaration */
    return;
  } /* if */
  sym=fetchfunc(symbolname,tag);    /* get a pointer to the function entry */
  if (fpublic)
    sym->usage|=_public;
  /* so it is a function, proceed */
  argcnt=declargs(sym);
  if (strcmp(symbolname,"main")==0) {
    if (argcnt>0)
      error(5);         /* "main()" function may not have any arguments */
    sym->usage|=_refer; /* "main()" is the program's entry point: always used */
  } /* if */
  /* "declargs()" found the ")"; if a ";" appears after this, it was a
   * prototype */
  if (matchtoken(';')) {
    if (!needsemicolon)
      error(218);       /* old style prototypes used with optional semicolumns */
    delete_symbols(&loctab,0,_yes);  /* prototype is done; forget everything */
    return;
  } /* if */
  /* so it is not a prototype, proceed */
  begcseg();
  sym->usage|=_define;  /* set the definition flag */
  if (fpublic) {
    sym->usage|=_refer; /* public functions are always "used" */
    /* ??? public_sym(symbolname); <- needed when we get a separate linker */
  } /* if */
  defsymbol(symbolname,_functn,_global,
            code_idx+opcodes(1)+opargs(3)+nameincells(symbolname));
         /* ^^^ The address for the symbol is the code address. But the
          * "symbol" instruction itself generates code. Therefore the
          * offset is pre-adjusted to the value it will have after the
          * symbol instruction.
          */
  startfunc();          /* creates stack frame */
  setline(funcline,fcurrent);
  declared=0;           /* number of local cells */
  rettype=(sym->usage & _retvalue);      /* set "return type" variable */
  curfunc=sym;
  define_args();        /* add the symbolic info for the function arguments */
  statement(NULL);
  if ((rettype & _retvalue)!=0)
    sym->usage|=_retvalue;
  if ((lastst!=__returnX) && (lastst!=__gotoX)){
    const1(0);
    ffret();
    if ((sym->usage & _retvalue)!=0)
      error(209);               /* function should return a value */
  } /* if */
  endfunc();
  if (litidx) {                 /* if there are literals defined */
    glb_declared+=litidx;
    begdseg();                  /* flip to DATA segment */
    dumplits();                 /* dump literal strings */
  } /* if */
  testsymbols(&loctab,0,_yes,_yes); /* test for unused arguments and labels */
  delete_symbols(&loctab,0,_yes);/* clear local variables queue */
  assert(loctab.next==NULL);
  curfunc=NULL;
}

static int argcompare(arginfo *a1,arginfo *a2)
{
  int result,level;

  result= strcmp(a1->name,a2->name);
  if (result)
    result= a1->ident==a2->ident;
  if (result)
    result= a1->tag==a2->tag;
  if (result)
    result= a1->hasdefault==a2->hasdefault;
  if (result && a1->hasdefault) {
    if (a1->ident==_refarray) {
      result= a1->defvalue.array.size==a2->defvalue.array.size;
      /* also check the dimensions of both arrays */
      result= a1->numdim==a2->numdim;
      for (level=0; result && level<a1->numdim; level++)
        result= a1->dim[level]==a2->dim[level];
      /* ??? should also check contents of the default array */
    } else {
      result= a1->defvalue.val==a2->defvalue.val;
    } /* if */
  } /* if */
  return result;
}

/*  declargs()
 *
 *  This routine adds an entry in the local symbol table for each argument
 *  found in the argument list. It returns the number of arguments.
 */
static int declargs(symbol *sym)
{
  char *st;
  int argcnt,tok,tag;
  cell val;
  arginfo arg;
  char name[_namemax+1];
  int ident;

  /* the '(' parantheses has already been parsed */
  argcnt=0;                     /* zero aruments up to now */
  ident=_variable;
  tag=0;
  if (!matchtoken(')')){
    do {                        /* there are arguments; process them */
      /* any legal name increases argument count (and stack offset) */
      tok=lex(&val,&st);
      switch (tok) {
      case 0:
        /* nothing */
        break;
      case '&':
        if (ident!=_variable)
          error(1,"-identifier-","&");
        ident=_reference;
        break;
      case __labelX:
        tag=sc_addtag(st);
        break;
      case __symbolX:
        if (argcnt>=_maxargs)
          error(45);            /* too many function arguments */
        strcpy(name,st);        /* save symbol name */
        /* Stack layout:
         *   base + 0*sizeof(cell)  == previous "base"
         *   base + 1*sizeof(cell)  == function return address
         *   base + 2*sizeof(cell)  == number of arguments
         *   base + 3*sizeof(cell)  == first argument of the function
         * So the offset of each argument is "(argcnt+3) * sizeof(cell)".
         */
        doarg(name,ident,(argcnt+3)*sizeof(cell),tag,&arg);
        if ((sym->usage & _prototyped)==0) {
          /* redimension the argument list, add the entry _varargs */
          sym->dim.arglist=(arginfo*)realloc(sym->dim.arglist,(argcnt+2)*sizeof(arginfo));
          sym->dim.arglist[argcnt]=arg;
          sym->dim.arglist[argcnt+1].ident=0;   /* keep the list terminated */
        } else {
          /* check the argument with the earlier definition */
          if (!argcompare(&sym->dim.arglist[argcnt],&arg))
            error(25);    /* function definition does not match prototype */
          /* may need to free default array argument */
          if (arg.ident==_refarray && arg.hasdefault)
            free(arg.defvalue.array.data);
        } /* if */
        argcnt++;
        ident=_variable;
        tag=0;
        break;
      case __ellips:
        if (ident!=_variable)
          error(10);    /* illegal function or declaration */
        if ((sym->usage & _prototyped)==0) {
          /* redimension the argument list, add the entry _varargs */
          sym->dim.arglist=(arginfo*)realloc(sym->dim.arglist,(argcnt+2)*sizeof(arginfo));
          sym->dim.arglist[argcnt].ident=_varargs;
          sym->dim.arglist[argcnt].tag=tag;
          sym->dim.arglist[argcnt+1].ident=0;   /* keep the list terminated */
        } /* if */
        break;
      default:
        error(10);      /* illegal function or declaration */
      } /* switch */
    } while (tok=='&' || tok==__labelX
             || tok!=__ellips && matchtoken(',')); /* more? */
    /* if the next token is not ",", it should be ")" */
    needtoken(')');
  } /* endif */
  sym->usage|=_prototyped;
  return argcnt;
}

/*  doarg       - declare one argument type
 *
 *  this routine is called from "declargs()" and adds an entry in the local
 *  symbol table for one argument.
 */
static void doarg(char *name,int ident,int offset,int tag,arginfo *arg)
{
  symbol *argsym;
  cell size;
  int idxtag[_dimen_max];

  strcpy(arg->name,name);
  arg->hasdefault=_no;  /* preset (most common case) */
  arg->defvalue.val=0;  /* clear */
  arg->numdim=0;
  if (matchtoken('[')) {
    if (ident==_reference)
      error(10);        /* illegal declaration ("&name[]" is unsupported) */
    do {
      if (arg->numdim == _dimen_max) {
        error(53);              /* exceeding maximum number of dimensions */
        return;
      } /* if */
      if (arg->numdim>0 && arg->dim[arg->numdim-1]==0)
        error(52);              /* only last dimension may be variable length */
      size=needsub(&idxtag[arg->numdim]);/* may be zero here, it is a pointer anyway */
      #if INT_MAX < LONG_MAX
        if (size > INT_MAX)
          error(105);           /* overflow, exceeding capacity */
      #endif
      arg->dim[arg->numdim]=(int)size;
      arg->numdim+=1;
    } while (matchtoken('['));
    ident=_refarray;            /* "reference to array" (is a pointer) */
    if (matchtoken('=')) {
      lexpush();                /* initials() needs it again */
      assert(litidx==0);        /* at the start of a function, this is reset */
      initials(ident,tag,&size,arg->dim,arg->numdim);
      assert(size>=litidx);
      /* allocate memory to hold the initial values */
      arg->defvalue.array.data=(long *)malloc(litidx*sizeof(cell));
      if (arg->defvalue.array.data!=NULL) {
        memcpy(arg->defvalue.array.data,litq,litidx*sizeof(cell));
        arg->hasdefault=_yes;   /* argument has default value */
        arg->defvalue.array.size=litidx;
      } /* if */
      litidx=0;                 /* reset */
    } /* if */
  } else {
    if (matchtoken('=')) {
      assert(ident==_variable || ident==_reference);
      arg->hasdefault=_yes;     /* argument has a default value */
      constexpr(&arg->defvalue.val,NULL);
    } /* if */
  } /* if */
  arg->ident=(char)ident;
  arg->tag=tag;
  argsym=findloc(name);
  if (argsym!=NULL) {
    error(21,name);             /* symbol already defined */
  } else {
    if (findglb(name)!=NULL)
      error(219,name);          /* variable shadows another symbol */
    /* add details of type and address */
    argsym=addvariable(name,offset,ident,_local,tag,
                       arg->dim,arg->numdim,idxtag);
    argsym->compound=0;
    if (ident==_reference)
      argsym->usage|=_read;     /* because references are passed back */
  } /* if */
}

/*  testsymbols - test for unused local variables
 *
 *  "Public" functions are excluded from the check, since these
 *  may be exported to other object modules.
 *  Labels are excluded from the check if the argument 'testlabs'
 *  is 0. Thus, labels are not tested until the end of the function.
 *  Constants are also excluded.
 *
 *  When the nesting level drops below "level", the check stops.
 *
 *  The function returns whether there is an "entry" point for the file.
 *  This flag will only be 1 when browsing the global symbol table.
 */
static int testsymbols(symbol *root,int level,int testlabs,int testconst)
{
  int entry=_no;

  symbol *sym=root->next;
  while (sym!=NULL && sym->compound>=level) {
    switch (sym->ident) {
    case _label:
      if (testlabs) {
        if ((sym->usage & _define)==0)
          error(19,sym->name);            /* not a label: ... */
        else if ((sym->usage & _refer)==0)
          error(203,sym->name);           /* symbol isn't used: ... */
      } /* if */
      break;
    case _functn:
      if ((sym->usage & (_define | _refer | _native))==_define)
        error(203,sym->name);   /* symbol isn't used ... (and not native) */
      else if ((sym->usage & (_define | _refer))==_refer)
        error(4,sym->name);     /* function not defined */
      if ((sym->usage & _public)!=0 || strcmp(sym->name,"main")==0)
        entry=_yes;             /* there is an entry point */
      break;
    case _constexpr:
      if (testconst && (sym->usage & _read)==0)
        error(203,sym->name);   /* symbol isn't used: ... */
      break;
    default:
      /* a variable */
      if (sym->parent!=NULL)
        break;                  /* hierarchical data type */
      if ((sym->usage & (_written | _read))==0)
        error(203,sym->name);   /* symbol isn't used ... */
      else if ((sym->usage & _read)==0)
        error(204,sym->name);   /* value assigned to symbol is never used */
    } /* if */
    sym=sym->next;
  } /* while */

  return entry;
}

static constval *insert_constval(constval *prev,constval *next,char *name,cell val)
{
  constval *eq;

  if ((eq=(constval*)malloc(sizeof(constval)))==NULL)
    error(103);       /* insufficient memory (fatal error) */
  memset(eq,0,sizeof(constval));
  strcpy(eq->name,name);
  eq->value=val;
  eq->next=next;
  prev->next=eq;
  return eq;
}

constval *append_constval(constval *table,char *name,cell val)
{
  constval *eq,*prev;

  /* find the end of the constant table */
  for (prev=table, eq=table->next; eq!=NULL; prev=eq, eq=eq->next)
    /* nothing */;
  return insert_constval(prev,NULL,name,val);
}

constval *find_constval(constval *table,char *name)
{
  constval *ptr = table->next;

  while (ptr!=NULL) {
    if (strcmp(name,ptr->name)==0)
      return ptr;
    ptr=ptr->next;
  } /* while */
  return NULL;
}

#if 0
static int delete_constval(constval *table,char *name)
{
  constval *prev = table;
  constval *cur = prev->next;

  while (cur!=NULL) {
    if (strcmp(name,cur->name)==0) {
      prev->next=cur->next;
      free(cur);
      return _yes;
    } /* if */
    prev=cur;
    cur=cur->next;
  } /* while */
  return _no;
}
#endif

static void delete_consttable(constval *table)
{
  constval *eq=table->next, *next;

  while (eq!=NULL) {
    next=eq->next;
    free(eq);
    eq=next;
  } /* while */
}

/*  add_constant
 *
 *  Adds a symbol to the #define symbol table.
 */
void add_constant(char *name,cell val,int vclass,int tag)
{
  symbol *sym;

  /* Test whether a global or local symbol with the same name exists. Since
   * constants are stored in the symbols table, this also finds previously
   * defind constants. */
  sym=findglb(name);
  if (!sym)
    sym=findloc(name);
  if (sym) {
    /* silently ignore redefinitions of constants with the same value */
    if (sym->ident==_constexpr) {
      if (sym->addr!=val)
        error(201,name);/* redefinition of constant (different value) */
    } else {
      error(21,name);   /* symbol already defined */
    } /* if */
    return;
  } /* if */

  /* constant doesn't exist yet, an entry must be created */
  addsym(name,val,_constexpr,vclass,tag,_define);
}

/*  statement           - The Statement Parser
 *
 *  This routine is called whenever the parser needs to know what statement
 *  it encounters (i.e. whenever program syntax requires a statement).
 *
 *  Global references: declared, ncmp
 */
static void statement(int *lastindent)
{
  int tok;
  cell val;
  char *st;

  if (!freading) {
    error(36);                  /* empty statement */
    return;
  } /* if */

  tok=lex(&val,&st);
  if (tok!='{')
    setline(fline,fcurrent);
  /* lex() has set stmtindent */
  if (lastindent!=NULL) {
    if (*lastindent>=0 && *lastindent!=stmtindent && !indent_nowarn)
      error(217);               /* loose indentation */
    *lastindent=stmtindent;
    indent_nowarn=0;            /* if warning was blocked, re-enable it */
  } /* if */
  switch (tok) {
  case 0:
    /* nothing */
    break;
  case __newX:
    declloc(_no);
    lastst=__newX;
    break;
  case __staticX:
    declloc(_yes);
    lastst=__newX;
    break;
  case '{':
    if (!matchtoken('}'))       /* {} is the empty statement */
      compound();
    /* "last statement" does not change */
    break;
  case ';':
    error(36);                  /* empty statement */
    break;
  case __ifX:
    doif();
    lastst=__ifX;
    break;
  case __whileX:
    dowhile();
    lastst=__whileX;
    break;
  case __doX:
    dodo();
    lastst=__doX;
    break;
  case __forX:
    dofor();
    lastst=__forX;
    break;
  case __switchX:
    doswitch();
    lastst=__switchX;
    break;
  case __caseX:
  case __defaultX:
    error(14);     /* not in switch */
    break;
  case __gotoX:
    dogoto();
    lastst=__gotoX;
    break;
  case __labelX:
    dolabel();
    lastst=__labelX;
    break;
  case __returnX:
    doreturn();
    lastst=__returnX;
    break;
  case __breakX:
    dobreak();
    lastst=__breakX;
    break;
  case __continueX:
    docont();
    lastst=__continueX;
    break;
  case __exitX:
    doexit();
    break;
  case __assertX:
    doassert();
    break;
  case __constX:
    decl_const(_local);
    break;
  case __enumX:
    decl_enum(_local);
    break;
  default:          /* non-empty expression */
    lexpush();      /* analyze token later */
    doexpr(_yes,_yes,_yes,NULL);
    needtoken(__termX);
    lastst=__exprX;
  } /* switch */
  return;
}

static void compound(void)
{
  cell save_decl;
  int indent=-1;

  save_decl=declared;
  ncmp+=1;              /* increase compound statement level */
  while (matchtoken('}')==0){  /* repeat until compound statement is closed */
    if (!freading){
      needtoken('}');   /* gives error: "expected token }" */
      break;
    } else {
      statement(&indent);       /* do a statement */
    } /* if */
  } /* while */
  if ((lastst!=__returnX) && (lastst!=__gotoX))
    modstk((int)(declared-save_decl)*sizeof(cell));  /* delete local variable space */
  testsymbols(&loctab,ncmp,_no,_yes);   /* look for unused block locals */
  declared=save_decl;
  delete_symbols(&loctab,ncmp,_no);     /* erase local symbols, but retain
                                         * block local labels (within the
                                         * function) */
  ncmp-=1;              /* decrease compound statement level */
}

/*  doexpr
 *
 *  Global references: stgidx   (referred to only)
 */
static void doexpr(int comma,int chkeffect,int allowarray,int *tag)
{
  int constant,index,ident;
  cell val;

  stgset(_yes);         /* start stage-buffering */
  do {
    index=stgidx;
    sideeffect=_no;
    ident=expression(&constant,&val,tag);
    endexpr();
    if (!allowarray && (ident==_array || ident==_refarray))
      error(33,"<unknown>");    /* array must be indexed */
    if (chkeffect && !sideeffect)
      error(215);               /* expression has no effect */
    stgout(index);
  } while (comma && matchtoken(',')); /* more? */
  stgset(_no);          /* stop stage-buffering */
  // for ( ;; ) loop notes:
  // comma, chkeffect & allowarray are all three _yes
  // tag == NULL
}

/*  constexpr
 *
 *  Global references: stgidx   (referred to only)
 */
int constexpr(cell *val,int *tag)
{
  int constant,index;
  cell cidx;

  stgset(_yes);         /* start stage-buffering */
  stgget(&index,&cidx); /* mark position in code generator */
  expression(&constant,val,tag);
  stgdel(index,cidx);   /* scratch generated code */
  stgset(_no);          /* stop stage-buffering */
  if (constant==0)
    error(8);           /* must be constant expression */
  return constant;
}

/*  test
 *
 *  In the case a "simple assignment" operator ("=") is used within a test,
 *  the warning "possibly unintended assignment" is displayed. This routine
 *  sets the global variable "intest" to true, it is restored upon termination.
 *  In the case the assignment was intended, use parantheses around the
 *  expression to avoid the warning; primary() sets "intest" to 0.
 *
 *  Global references: stgidx   (referred to only)
 *                     intest   (altered, but restored upon termination)
 */
static void test(int label,int parens,int invert)
{
  int index,tok;
  cell cidx;
  value lval;

  stgset(_yes);         /* start staging */
  pushstk((stkitem)intest);
  intest=1;
  if (parens)
    needtoken('(');
  do {
    stgget(&index,&cidx);       /* mark position (of last expression) in
                                 * code generator */
    if (hier14(&lval))
      rvalue(&lval);
    tok=matchtoken(',');
    if (tok)                    /* if there is another expression... */
      stgout(index);            /* ...write first one */
  } while (tok); /* enddo */
  if (parens)
    needtoken(')');
  if (lval.ident==_array || lval.ident==_refarray)
    error(33,lval.sym->name);   /* array must be indexed */
  if (lval.ident==_constexpr) { /* constant expression */
    stgdel(index,cidx);
    if (lval.constval) {        /* code always executed */
      error(206);               /* redundant test: always non-zero */
      return;
    } /* if */
    error(205);                 /* redundant code: never executed */
    jumplabel(label);
    return;
  } /* if */
  if (invert)
    jmp_ne0(label);             /* jump to label if true (different from 0) */
  else
    jmp_eq0(label);             /* jump to label if false (equal to 0) */
  intest=(int)(long)popstk();   /* double typecast to avoid warning with Microsoft C */
  stgout(index);
  stgset(_no);          /* stop staging */
  // for ( ;; ) loop notes:
  // parens & invert are both _no
}

static void doif(void)
{
  int flab1,flab2;
  int ifindent;

  ifindent=stmtindent;  /* save the indent of the "if" instruction */
  flab1=getlabel();     /* get label number for false branch */
  test(flab1,_yes,_no); /* get expression and branch to flab1 if false */
  statement(NULL);      /* if true, do a statement */
  if (matchtoken(__elseX)==0){  /* if...else ? */
    setlabel(flab1);    /* no, simple if..., print false label */
  } else {
    /* to avoid the "dangling else" error, we want a warning if the "else"
     * has a lower indent than the matching "if" */
    if (stmtindent<ifindent)
      error(217);       /* loose indentation */
    flab2=getlabel();
    if ((lastst!=__returnX) && (lastst!=__gotoX))
      jumplabel(flab2);
    setlabel(flab1);    /* print false label */
    statement(NULL);    /* do "else" clause */
    setlabel(flab2);    /* print true label */
  } /* endif */
}

static void dowhile(void)
{
  int wq[_wqsiz];         /* allocate local queue */

  addwhile(wq);           /* add entry to queue for "break" */
  setlabel(wq[_wqloop]);  /* loop label */
  test(wq[_wqexit],_yes,_no);   /* branch to wq[_wqexit] if false */
  statement(NULL);        /* if so, do a statement */
  jumplabel(wq[_wqloop]); /* and loop to "while" start */
  setlabel(wq[_wqexit]);  /* exit label */
  delwhile();             /* delete queue entry */
}

/*
 *  Note that "continue" will in this case not jump to the top of the loop, but
 *  to the end: just before the TRUE-or-FALSE testing code.
 */
static void dodo(void)
{
  int wq[_wqsiz],top;

  addwhile(wq);           /* see "dowhile" for more info */
  top=getlabel();         /* make a label first */
  setlabel(top);          /* loop label */
  statement(NULL);
  needtoken(__whileX);
  setlabel(wq[_wqloop]);  /* "continue" always jumps to WQLOOP. */
  test(wq[_wqexit],_yes,_no);
  jumplabel(top);
  setlabel(wq[_wqexit]);
  delwhile();
  needtoken(__termX);
}

static void dofor(void)
{
  int wq[_wqsiz],flab1,flab2;
  cell save_decl;
  int save_ncmp;
  int *ptr;

  save_decl=declared;
  save_ncmp=ncmp;

  addwhile(wq);
  flab1=getlabel();
  flab2=getlabel();
  needtoken('(');
  if (matchtoken(';')==0) {
    /* new variable declarations are allowed here */
    if (matchtoken(__newX)) {
      /* The variable in expr1 of the for loop is at a
       * 'compound statement' level of it own.
       */
      ncmp++;
      declloc(_no); /* declare local variable */
    } else {
      doexpr(_yes,_yes,_yes,NULL);  /* expression 1 */
      needtoken(';');
    } /* if */
  } /* if */
  /* Adjust the "declared" field in the "while queue", in case that
   * local variables were declared in the first expression of the
   * "for" loop. A "break" must delete these, but a "continue" must not.
   */
  ptr=readwhile();
  assert(ptr!=NULL);
  ptr[_wqcont]=(int)declared;
  setlabel(flab1);
  if (matchtoken(';')==0) {
    test(wq[_wqexit],_no,_no);  /* expression 2 (jump to wq[_wqexit] if false) */
    needtoken(';');
  } /* if */
  jumplabel(flab2);             /* skip expression 3 for now */
  setlabel(wq[_wqloop]);        /* "continue" goes to this label: expr3 */
  if (matchtoken(')')==0) {
    doexpr(_yes,_yes,_yes,NULL);/* expression 3 */
    needtoken(')');
  } /* if */
  jumplabel(flab1);
  setlabel(flab2);
  statement(NULL);
  jumplabel(wq[_wqloop]);
  setlabel(wq[_wqexit]);
  delwhile();

  assert(ncmp>=save_ncmp);
  if (ncmp>save_ncmp) {
    /* Clean up the space and the symbol table for the local
     * variable in "expr1".
     */
    modstk((int)(declared-save_decl)*sizeof(cell));
    declared=save_decl;
    delete_symbols(&loctab,ncmp,_yes);
    ncmp=save_ncmp;     /* reset 'compound statement' nesting level */
  } /* if */
}

/* The switch statement is incompatible with its C sibling:
 * 1. the cases are not drop through
 * 2. only one instruction may appear below each case, use a compound
 *    instruction to execute multiple instructions
 * 3. the "case" keyword accepts a comma separated list of values to
 *    match, it also accepts a range using the syntax "1 .. 4"
 *
 * SWITCH param
 *   PRI = expression result
 *   param = table offset (code segment)
 *
 */
static void doswitch(void)
{
  int lbl_table,lbl_exit,lbl_case;
  int tok,swdefault,casecount;
  cell val;
  char *str;
  constval caselist = { "", 0, NULL};   /* case list starts empty */
  constval *cse,*csp;
  char labelname[_namemax+1];

  needtoken('(');
  doexpr(_yes,_no,_no,NULL);    /* evaluate switch expression */
  needtoken(')');
  /* generate the code for the switch statement, the label is the address
   * of the case table (to be generated later).
   */
  lbl_table=getlabel();
  ffswitch(lbl_table);

  needtoken('{');
  lbl_exit=getlabel();          /* get label number for jumping out of switch */
  swdefault=_no;
  casecount=0;
  do {
    tok=lex(&val,&str);         /* read in (new) token */
    switch (tok) {
    case __caseX:
      if (swdefault!=_no)
        error(15);      /* "default" case must be last in switch statement */
      lbl_case=getlabel();
      for ( ;; ) {
        casecount++;
        constexpr(&val,NULL);
        /* Search the insertion point (the table is kept in sorted order, so
         * that advanced abstract machines can sift the case table with a
         * binary search). Check for duplicate case values at the same time.
         */
        for (csp=&caselist, cse=caselist.next;
             cse!=NULL && cse->value<val;
             csp=cse, cse=cse->next)
          /* nothing */;
        if (cse!=NULL && cse->value==val)
          error(40,val);        /* duplicate "case" label */
        /* Since the label is stored as a string in the "constval", the
         * size of an identifier must be at least 8, as there are 8
         * hexadecimal digits in a 32-bit number.
         */
        #if _namemax < 8
          #error Length of identifier (_namemax) too small.
        #endif
        assert(csp!=NULL);
        assert(csp->next==cse);
        insert_constval(csp,cse,itoh(lbl_case),val);
        if (matchtoken(__dbldot)) {
          cell end;
          constexpr(&end,NULL);
          if (end<=val)
            error(50);          /* invalid range */
          while (++val<=end) {
            casecount++;
            /* find the new insertion point */
            for (csp=&caselist, cse=caselist.next;
                 cse!=NULL && cse->value<val;
                 csp=cse, cse=cse->next)
              /* nothing */;
            if (cse!=NULL && cse->value==val)
              error(40,val);        /* duplicate "case" label */
            assert(csp!=NULL);
            assert(csp->next==cse);
            insert_constval(csp,cse,itoh(lbl_case),val);
          } /* if */
        } /* if */
        if (matchtoken(':'))
          break;
        needtoken(','); /* if not ':', must be ',' */
      } /* for */
      setlabel(lbl_case);
      statement(NULL);
      jumplabel(lbl_exit);
      break;
    case __defaultX:
      if (swdefault!=_no)
        error(16);      /* multiple defaults in switch */
      lbl_case=getlabel();
      setlabel(lbl_case);
      needtoken(':');
      swdefault=_yes;
      statement(NULL);
      /* Jump to lbl_exit, even thouh this is the last clause in the
       * switch, because the jump table is generated between the last
       * clause of the switch and the exit label.
       */
      jumplabel(lbl_exit);
      break;
    case '}':
      /* nothing, but avoid dropping into "default" */
      break;
    default:
      lexpush();        /* let needtoken() do the error handling */
      needtoken('}');
      tok='}';          /* break out of the loop after an error */
    } /* switch */
  } while (tok!='}');

  #if !defined NDEBUG
    /* verify that the case table is sorted */
    for (cse=caselist.next; cse!=NULL && cse->next!=NULL; cse=cse->next)
      assert(cse->value < cse->next->value);
  #endif
  /* generate the table here, before lbl_exit (general jump target) */
  setlabel(lbl_table);
  assert(swdefault==_no|| swdefault==_yes);
  if (swdefault==_no) {
    /* store lbl_exit as the "none-matched" label in the switch table */
    strcpy(labelname,itoh(lbl_exit));
  } else {
    /* lbl_case holds the label of the "default" clause */
    strcpy(labelname,itoh(lbl_case));
  } /* if */
  ffcase(casecount,labelname,_yes);
  /* generate the rest of the table */
  for (cse=caselist.next; cse!=NULL; cse=cse->next)
    ffcase(cse->value,cse->name,_no);

  setlabel(lbl_exit);
  delete_consttable(&caselist); /* clear list of case labels */
  lastst=__switchX;
}

static void doassert(void)
{
  int flab1,index;
  cell cidx;
  value lval;

  if ((debug & _chkbounds)!=0) {
    flab1=getlabel();           /* get label number for "OK" branch */
    test(flab1,_no,_yes);       /* get expression and branch to flab1 if true */
    setline(fline,fcurrent);    /* make sure we abort on the correct line number */
    ffabort(_assertion);
    setlabel(flab1);
  } else {
    stgset(_yes);               /* start staging */
    stgget(&index,&cidx);       /* mark position in code generator */
    do {
      if (hier14(&lval))
        rvalue(&lval);
      stgdel(index,cidx);       /* just scrap the code */
    } while (matchtoken(',')); /* do */
    stgset(_no);                /* stop staging */
  } /* if */
  needtoken(__termX);
}

static void dogoto(void)
{
  char *st;
  cell val;
  symbol *sym;

  if (lex(&val,&st)==__symbolX) {
    sym=fetchlab(st);
    jumplabel((int)sym->addr);
    sym->usage|=_refer; /* set "_refer" bit */
  } else {
    error(20,st);       /* illegal symbol name */
  } /* if */
  needtoken(__termX);
}

static void dolabel(void)
{
  char *st;
  cell val;
  symbol *sym;

  tokeninfo(&val,&st);  /* retrieve label name again */
  sym=fetchlab(st);
  setlabel((int)sym->addr);
  /* since one can jump around variable declarations or out of compound
   * blocks, the stack must be manually adjusted
   */
  setstk(-declared*sizeof(cell));
  sym->usage|=_define;  /* label is now defined */
}

/*  fetchlab
 *
 *  Finds a label from the (local) symbol table or adds one to it.
 *  Labels are local in scope.
 *
 *  Note: The "_usage" bit is set to zero. The routines that call "fetchlab()"
 *        must set this bit accordingly.
 */
static symbol *fetchlab(char *name)
{
  symbol *sym;

  sym=findloc(name);            /* labels are local in scope */
  if (sym){
    if (sym->ident!=_label)
      error(19,sym->name);       /* not a label: ... */
  } else {
    sym=addsym(name,getlabel(),_label,_local,0,0);
    sym->x.declared=(int)declared;
    sym->compound=ncmp;
  } /* if */
  return sym;
}

/*  doreturn
 *
 *  Global references: rettype  (altered)
 */
static void doreturn(void)
{
  int tag;
  if (matchtoken(__termX)==0){
    if ((rettype & _retnone)!=0)
      error(208);                       /* mix "return;" and "return value;" */
    doexpr(_yes,_no,_no,&tag);
    needtoken(__termX);
    rettype|=_retvalue;                 /* function returns a value */
    /* check tagname with function tagname */
    assert(curfunc!=NULL);
    if (curfunc->tag!=0 && curfunc->tag!=tag)
      error(213);                       /* tagname mismatch */
  } else {
    /* this return statement contains no expression */
    const1(0);
    if ((rettype & _retvalue)!=0)
      error(209);                       /* function should return a value */
    rettype|=_retnone;                  /* function does not return anything */
  } /* if */
  modstk((int)declared*sizeof(cell));   /* end of function, remove *all*
                                         * local variables */
  ffret();
}

static void dobreak(void)
{
  int *ptr;

  ptr=readwhile();      /* readwhile() gives an error if not in loop */
  needtoken(__termX);
  if (ptr==NULL)
    return;
  modstk(((int)declared-ptr[_wqbr])*sizeof(cell));
  jumplabel(ptr[_wqexit]);
}

static void docont(void)
{
  int *ptr;

  ptr=readwhile();      /* readwhile() gives an error if not in loop */
  needtoken(__termX);
  if (ptr==NULL)
    return;
  modstk(((int)declared-ptr[_wqcont])*sizeof(cell));
  jumplabel(ptr[_wqloop]);
}

static void doexit(void)
{
  if (matchtoken(__termX)==0){
    doexpr(_yes,_no,_no,NULL);
    needtoken(__termX);
  } /* if */
  ffabort(_exit);
}

void addwhile(int *ptr)
{
  int k;

  ptr[_wqbr]=(int)declared;     /* stack pointer (for "break") */
  ptr[_wqcont]=(int)declared;   /* for "continue", possibly adjusted later */
  ptr[_wqloop]=getlabel();
  ptr[_wqexit]=getlabel();
  if (wqptr>=(wq+_wqtabsz-_wqsiz))
    error(102,"loop table");    /* loop table overflow (too many active loops)*/
  k=0;
  while (k<_wqsiz){     /* copy "ptr" to while queue table */
    *wqptr=*ptr;
    wqptr+=1;
    ptr+=1;
    k+=1;
  } /* while */
}

void delwhile(void)
{
  if (wqptr>wq)
    wqptr-=_wqsiz;
}

int *readwhile(void)
{
  if (wqptr<=wq){
    error(24);         /* out of context */
    return NULL;
  } else {
    return (wqptr-_wqsiz);
  } /* if */
}

