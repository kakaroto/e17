/*  Small compiler - File input, preprocessing and lexical analysis functions
 *
 *  Copyright (c) ITB CompuPhase, 1997-1999
 *  This file may be freely used. No warranties of any kind.
 */
#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include "sc.h"

static void lexclr(void);
static int match(char *st,int end);
static cell litchar(char **lptr);
static int alpha(char c);


static int icomment;    /* currently in multiline comment? */

/*  pushstk & popstk
 *
 *  Uses a LIFO stack to store information. The stack is used by doinclude(),
 *  doswitch() (to hold the state of "swactive") and some other routines.
 *
 *  Porting note: I made the bold assumption that an integer will not be
 *  larger than a pointer (it may be smaller). That is, the stack element
 *  is typedef'ed as a pointer type, but I also store integers on it. See
 *  SC.H for "stkitem"
 *
 *  Global references: stack,stkidx (private to pushstk() and popstk())
 */
static stkitem stack[_stkmax];
static int stkidx;
void pushstk(stkitem val)
{
  if (stkidx>=_stkmax)
    error(102,"parser stack");  /* stack overflow (recursive include?) */
  stack[stkidx]=val;
  stkidx+=1;
}

stkitem popstk(void)
{
  if (stkidx==0)
    return (stkitem) -1;        /* stack is empty */
  stkidx-=1;
  return stack[stkidx];
}

void plungefile(char *name)
{
  FILE *fp;
  char *ext=NULL;

  fp=fopen(name,"rt");
  if (fp==NULL) {
    ext=strchr(name,'\0');      /* save position */
    /* try to append an extension */
    strcat(name,".inc");
    fp=fopen(name,"rt");
    *ext='\0';                  /* restore */
  } /* if */
  if (fp==NULL) {
    ext=strchr(name,'\0');
    /* try to append an extension */
    strcat(name,".sma");
    fp=fopen(name,"rt");
    *ext='\0';
  } /* if */
  if (fp==NULL)
    error(100,name);        /* cannot read from ... (fatal error) */
  pushstk((stkitem)inpf);
  pushstk((stkitem)inpfname);   /* pointer to current file name */
  pushstk((stkitem)icomment);
  pushstk((stkitem)fcurrent);
  pushstk((stkitem)fline);
  inpfname=(char *)malloc(strlen(name)+1);
  strcpy(inpfname,name);    /* set name of include file */
  inpf=fp;                  /* set input file pointer to include file */
  fnumber++;
  fline=0;                  /* set current line number to 0 */
  fcurrent=fnumber;
  icomment=_no;
  setfile(inpfname,fcurrent);
}

static void check_empty(char *lptr)
{
  /* verifies that the string contains only whitespace */
  while (*lptr<=' ' && *lptr!='\0')
    lptr++;
  if (*lptr!='\0')
    error(38);          /* extra characters on line */
}

/*  doinclude
 *
 *  Gets the name of an include file, pushes the old file on the stack and
 *  sets some options. This routine doesn't use lex(), since lex() doesn't
 *  recognize file names (and directories).
 *
 *  Global references: inpf     (altered)
 *                     inpfname (altered)
 *                     fline    (altered)
 *                     lptr     (altered)
 */
static void doinclude(void)
{
  char name[_MAX_PATH],c;
  int i;

  while (*lptr<=' ' && *lptr!=0)
    lptr+=1;
  if (*lptr=='<' || *lptr=='\"'){
    if (*lptr=='"') {
      c='"';
      name[0]='\0';
    } else {
      c='>';
      strcpy(name,includepath);
    } /* if */
    lptr+=1;
  } else {
    error(37);  /* invalid string */
    return;
  } /* if */
  i=strlen(name);
  while (*lptr!=c && *lptr!='\0'){      /* find the end of the string */
    name[i]=*lptr;
    i+=1;
    lptr+=1;
  } /* while */
  if (*lptr!=c) {       /* verify correct string termination */
    error(37);  /* invalid string */
    return;
  } /* if */
  /* verify that the rest of the line is whitespace */
  check_empty(lptr+1);
  name[i]='\0';
  plungefile(name);
}

/*  readline
 *
 *  Reads in a new line from the input file pointed to by "inpf". If this
 *  file is done, readline() attempts to pop off the previous file from
 *  the stack. If that fails too, it sets "freading" to 0.
 *
 *  Global references: inpf,fline,inpfname,freading,icomment (altered)
 */
static void readline(char *line)
{
  int i,num,cont;
  char *ptr;

  num=_linemax;
  cont=_no;
  do {
    if (inpf==NULL || feof(inpf)) {
      if (cont)
        error(49);      /* invalid line continuation */
      if (inpf!=NULL)
        fclose(inpf);
      i=(int)(long)popstk();
      if (i==-1) {        /* All's done; popstk() returns "stack is empty" */
        freading=_no;
        *line='\0';
        return;
      } /* if */
      fline=i;
      fcurrent=(int)(long)popstk();
      icomment=(int)(long)popstk();
      free(inpfname);     /* return memory allocated for the include file name */
      inpfname=(char *)popstk();
      inpf=(FILE *)popstk();
    } /* if */

    if (fgets(line,num,inpf)==NULL) {
      *line='\0';     /* delete line */
      cont=_no;
    } else {
      /* check whether to erase leading spaces */
      if (cont) {
        char *ptr=line;
        while (*ptr==' ' || *ptr=='\t')
          ptr++;
        if (ptr!=line)
          memmove(line,ptr,strlen(ptr)+1);
      } /* if */
      cont=_no;
      /* check if the next line must be concatenated to this line */
      if ((ptr=strchr(line,'\n'))!=NULL && ptr>line) {
        assert(*(ptr+1)=='\0'); /* '\n' should be last in the string */
        while (*ptr=='\n' || *ptr==' ' || *ptr=='\t')
          ptr--;        /* skip trailing whitespace */
        if (*ptr=='\\') {
          cont=_yes;
          /* set '\r' at the position of '\\' to make it possible to check
           * for a line continuation in a single line comment (error 49)
           */
          *ptr++='\r';
          *ptr='\0';    /* erase '\n' (and any trailing whitespace) */
        } /* if */
      } /* if */
      num-=strlen(line);
      line+=strlen(line);
    } /* if */
    fline+=1;
  } while (num>=0 && cont);
}

/*  stripcom
 *
 *  Replaces all comments from the line by space characters. It updates
 *  a global variable ("icomment") for multiline comments.
 *
 *  This routine also supports the C++ extension for single line comments.
 *  These comments are started with "//" and end at the end of the line.
 *
 *  Global references: icomment  (private to "stripcom")
 */
static void stripcom(char *line)
{
  char c;

  while (*line){
    if (icomment){
      if (*line=='*' && *(line+1)=='/') {
        icomment=_no;   /* comment has ended */
        *line=' ';      /* replace '*' and '/' characters by spaces */
        *(line+1)=' ';
        line+=2;
      } else {
        if (*line=='/' && *(line+1)=='*')
          error(216);   /* nested comment */
        *line=' ';      /* replace comments by spaces */
        line+=1;
      } /* if */
    } else {
      if (*line=='/' && *(line+1)=='*'){
        icomment=_yes;  /* start comment */
        *line=' ';      /* replace '/' and '*' characters by spaces */
        *(line+1)=' ';
        line+=2;
      } else if (*line=='/' && *(line+1)=='/'){  /* comment to end of line */
        if (strchr(line,'\r')!=NULL)
          error(49);    /* invalid line continuation */
        *line++='\n';   /* put "newline" at first slash */
        *line='\0';     /* put "zero-terminator" at second slash */
      } else {
        if (*line=='\"' || *line=='\''){        /* leave literals unaltered */
          c=*line;      /* ending quote, single or double */
          line+=1;
          while ((*line!=c || *(line-1)=='\\') && *line!='\0')
            line+=1;
          line+=1;      /* skip final quote */
        } else {
          #if defined(NO_CASE_SENSITIVE)
            *line=(char)tolower(*line);
          #endif
          line+=1;
        } /* endif */
      } /* endif */
    } /* endif */
  } /* endwhile */
}

/*  btoi
 *
 *  Attempts to interpret a nummeric symbol as a boolean value. On success
 *  it returns the number of characters processed (so the line pointer can be
 *  adjusted) and the value is stored in "val". Otherwise it returns 0 and
 *  "val" is garbage.
 *
 *  A boolean value must start with "0b"
 */
static int btoi(cell *val,char *curptr)
{
  char *ptr;

  *val=0;
  ptr=curptr;
  if (*ptr=='0' && *(ptr+1)=='b') {
    ptr+=2;
    while (*ptr=='0' || *ptr=='1'){
      *val=(*val<<1) | (*ptr-'0');
      ptr+=1;
    } /* while */
  } else {
    return 0;
  } /* if */
  if (alphanum(*ptr))   /* number must be delimited by non-alphanumeric char */
    return 0;
  else
    return (int)(ptr-curptr);
}

/*  dtoi
 *
 *  Attempts to interpret a nummeric symbol as a decimal value. On success
 *  it returns the number of characters processed and the value is stored in
 *  "val". Otherwise it returns 0 and "val" is garbage.
 */
static int dtoi(cell *val,char *curptr)
{
  char *ptr;

  *val=0;
  ptr=curptr;
  if (!isdigit(*ptr))   /* should start with digit */
    return 0;
  while (isdigit(*ptr)){
    *val=(*val*10)+(*ptr-'0');
    ptr+=1;
  } /* endwhile */
  if (alphanum(*ptr))  /* number must be delimited by non-alphanumerical */
    return 0;
  return (int)(ptr-curptr);
}

/*  htoi
 *
 *  Attempts to interpret a nummeric symbol as a hexadecimal value. On
 *  success it returns the number of characters processed and the value is
 *  stored in "val". Otherwise it return 0 and "val" is garbage.
 *
 *  A hexadecimal value must be specified conforming the C syntax. For
 *  example: 0x7fa
 */
static int htoi(cell *val,char *curptr)
{
  char *ptr;

  *val=0;
  ptr=curptr;
  if (!isdigit(*ptr))   /* should start with digit */
    return 0;
  if (*ptr=='0' && *(ptr+1)=='x'){      /* C style hexadecimal notation */
    ptr+=2;
    while (ishex(*ptr)){
      *val= *val<<4;
      if (isdigit(*ptr))
        *val+= (*ptr-'0');
      else
        *val+= (tolower(*ptr)-'a'+10);
      ptr+=1;
    } /* endwhile */
  } else {
    return 0;
  } /* endif */
  if (alphanum(*ptr))
    return 0;
  else
    return (int)(ptr-curptr);
}

/*  number
 *
 *  Reads in a number (binary, decimal or hexadecimal). It returns the number
 *  of characters processed or 0 if the symbol couldn't be interpreted as a
 *  number (in this case the argument "val" remains unchanged). This routine
 *  relies on the 'early dropout' implementation of the logical or (||)
 *  operator.
 *
 *  Note: the routine doesn't check for a sign (+ or -). The - is checked
 *        for at "hier2()" (in fact, it is viewed as an operator, not as a
 *        sign) and the + is invalid (as in standard C).
 */
static int number(cell *val,char *curptr)
{
  int i;
  cell value;

  if ((i=btoi(&value,curptr))!=0      /* binary? */
      || (i=dtoi(&value,curptr))!=0   /* decimal? */
      || (i=htoi(&value,curptr))!=0)  /* hexadecimal? */
  {
    *val=value;
    return i;
  } else {
    return 0;                      /* else not a number */
  } /* endif */
}

static int preproc_expr(cell *val)
{
  int result;

  /* append a special symbol to the string, so the expression
   * analyzer won't try to read a next line when it encounters
   * an end-of-line
   */
  strcat(pline,"\x7f");         /* the "DEL" code */
  result=constexpr(val,NULL);   /* get value (or 0 on error) */
  *lptr='\0';                   /* erase the token */
  lexclr();                     /* clear any "pushed" tokens */
  return result;
}

/*  command
 *
 *  Recognizes the compiler directives. The function returns:
 *     0  if the line must be processed;
 *     !0 if the line must be skipped, because:
 *          - false "#if.." code
 *          - line contains recognized compiler directive
 *          - line is empty
 *
 *  Global variables: iflevel, skiplevel  (private to command()).
 *                    lptr      (altered)
 */
static int iflevel;
static int skiplevel;
static int command(void)
{
  int tok,ret;
  cell val;
  char *str;

  while (*lptr<=' ' && *lptr!='\0')
    lptr+=1;
  if (*lptr=='\0')
    return 1;                   /* empty line */
  if (*lptr!='#')
    return skiplevel;           /* it is not a compiler directive */
  /* compiler directive found */
  indent_nowarn=1;              /* allow loose indentation" */
  lexclr();                     /* clear any "pushed" tokens */
  tok=lex(&val,&str);
  ret=1;                        /* preset 'ret' to 1 (most common case) */
  switch (tok) {
  case __if_pX:                  /* conditional compilation */
    iflevel+=1;
    if (skiplevel)
      break;                    /* break out of switch */
    preproc_expr(&val);         /* get value (or 0 on error) */
    if (!val)
      skiplevel=iflevel;
    check_empty(lptr);
    break;
  case __else_pX:
    if (iflevel==0 && skiplevel==0) {
      error(26);                /* no matching #if */
      errflag=0;
    } else if (skiplevel==iflevel) {
      skiplevel=0;
    } else if (skiplevel==0) {
      skiplevel=iflevel;
    } /* if */
    check_empty(lptr);
    break;
  case __endifX:
    if (iflevel==0 && skiplevel==0){
      error(26);
      errflag=0;
    } else {
      if (skiplevel==iflevel)
        skiplevel=0;
      iflevel-=1;
    } /* if */
    check_empty(lptr);
    break;
  case __includeX:               /* #include directive */
    if (skiplevel==0)
      doinclude();
    break;
#if !defined NO_DEFINE
  case __defineX:
      if (skiplevel==0) {
        if (lex(&val,&str)==__symbolX) {
          char name[_namemax+1];
          strcpy(name,str);
          while (*lptr<=' ' && *lptr!='\0')
            lptr++;
          if (*lptr!='\0')
            preproc_expr(&val); /* get constant expression */
          else
            val=0;
          add_constant(name,val,_global,0);
        } else {
          error(20);            /* invalid symbol name */
        } /* if */
        check_empty(lptr);
      } /* if */
    break;
#endif
  case __assert_pX:
    if (skiplevel==0 && (debug & _chkbounds)!=0) {
      preproc_expr(&val);       /* get constant expression (or 0 on error) */
      if (!val)
        error(7);               /* assertion failed */
      check_empty(lptr);
    } /* if */
    break;
  case __pragmaX:
    if (skiplevel==0) {
      if (lex(&val,&str)==__symbolX) {
        if (strcmp(str,"ctrlchar")==0) {
          if (lex(&val,&str)!=__numberX)
            error(27);          /* invalid character constant */
          ctrlchar=(char)val;
        } else if (strcmp(str,"dynamic")==0) {
          preproc_expr(&stksize);
        } else if (strcmp(str,"library")==0) {
          while (*lptr<=' ' && *lptr!='\0')
            lptr++;
          if (*lptr!='"') {
            error(37);          /* invalid string */
          } else {
            char name[_namemax+1];
            int len=0;
            lptr++;             /* skip " */
            while (*lptr!='"' && *lptr!='\0' && len<_namemax)
              name[len++]=*lptr++;
            name[len]='\0';
            if (*lptr!='"')
              error(37);        /* invalid string */
            else
              lptr++;
            /* add the name if it does not yet exist in the table */
            if (find_constval(&libname_tab,name)==NULL)
              append_constval(&libname_tab,name,0);
          } /* if */
        } else if (strcmp(str,"semicolon")==0) {
          cell val;
          preproc_expr(&val);
          needsemicolon=(int)val;
        } else if (strcmp(str,"pack")==0) {
          cell val;
          preproc_expr(&val);   /* default = packed/unpacked */
          packstr=(int)val;
        } else {
          error(207);           /* unknown #pragma */
        } /* if */
      } else {
        error(207);             /* unknown #pragma */
      } /* if */
      check_empty(lptr);
    } /* if */
    break;
  case __endinputX:
  case __endscrptX:
    if (skiplevel==0) {
      check_empty(lptr);
      fclose(inpf);
      inpf=NULL;
    } /* if */
    break;
  case __emit_pX: {
    /* write opcode to output file */
    char name[30];
    int i;
    while (*lptr<=' ' && *lptr!='\0')
      lptr++;
    for (i=0; i<40 && (isalpha(*lptr) || *lptr=='.'); i++,lptr++)
      name[i]=tolower(*lptr);
    name[i]='\0';
    stgwrite("\t");
    stgwrite(name);
    stgwrite(" ");
    code_idx+=opcodes(1);
    /* write parameter (if any) */
    while (*lptr<=' ' && *lptr!='\0')
      lptr++;
    if (*lptr!='\0') {
      symbol *sym;
      tok=lex(&val,&str);
      switch (tok) {
      case __numberX:
        outval(val,_yes);
        code_idx+=opargs(1);
        break;
      case __symbolX:
        sym=findloc(str);
        if (sym==NULL)
          sym=findglb(str);
        if (sym==NULL) {
          error(17,str);    /* undefined symbol */
        } else {
          outval(sym->addr,_yes);
          sym->usage|=_read;
          code_idx+=opargs(1);
        } /* if */
        break;
      default:
        error(1,__symbolX,tok);
        break;
      } /* switch */
      check_empty(lptr);
    } /* if */
    break;
  } /* case */
  default:
    error(31);          /* unknown compiler directive */
    ret=skiplevel;      /* line must be processed (if skiplevel==0) */
  } /* switch */
  return ret;
}

/*  preprocess
 *
 *  Reads a line by "readline" into "pline" and performs basic preprocessing:
 *  deleting comments, skipping lines with false "#if.." code and recognizing
 *  other compiler directives. There is an indirect recursion: lex() calls
 *  preprocess() if a new line must be read, preprocess() calls command(),
 *  which at his turn calls lex() to identify the token.
 *
 *  Global references: lptr     (altered)
 *                     pline    (altered)
 *                     freading (referred to only)
 */
void preprocess(void)
{
  if (!freading)
    return;
  do {
    readline(pline);
    stripcom(pline);
    lptr=pline; /* set "line pointer" to beginning of the parsing buffer */
  } while (command() && freading); /* enddo */
}

static char *unpackedstring(char *lptr)
{
  while (*lptr!='\"' && *lptr!='\0')
    stowlit(litchar(&lptr));    /* litchar() alters "lptr" */
  stowlit(0);                   /* terminate string */
  return lptr;
}

static char *packedstring(char *lptr)
{
  int i;
  ucell val,c;

  i=sizeof(ucell)-(charbits/8); /* start at most significant byte */
  val=0;
  while (*lptr!='\"' && *lptr!='\0') {
    c=litchar(&lptr);           /* litchar() alters "lptr" */
    if (c>=(ucell)(1 << charbits))
      error(43);                /* character constant exceeds range */
    val |= (c << 8*i);
    if (i==0) {
      stowlit(val);
      val=0;
    } /* if */
    i=(i+sizeof(ucell)-(charbits/8)) % sizeof(ucell);
  } /* if */
  /* save last code; make sure there is at least one terminating zero character */
  if (i!=(int)(sizeof(ucell)-(charbits/8)))
    stowlit(val);       /* at least one zero character in "val" */
  else
    stowlit(0);         /* add full cell of zeros */
  return lptr;
}

/*  lex(lexvalue,lexsym)        Lexical Analysis
 *
 *  lex() first deletes leading white space, then checks for multi-character
 *  operators, keywords (including most compiler directives), numbers,
 *  labels, symbols and literals (literal characters are converted to a number
 *  and are returned as such). If every check fails, the line must contain
 *  a single-character operator. So, lex() returns this character. In the other
 *  case (something did match), lex() returns the number of the token. All
 *  these tokens have been assigned numbers above 255.
 *
 *  Some tokens have "attributes":
 *     __numberX         the value of the number is return in "lexvalue".
 *     __symbolX         the first _namemax characters of the symbol are
 *                      stored in a buffer, a pointer to this buffer is
 *                      returned in "lexsym".
 *     __labelX          the first _namemax characters of the label are
 *                      stored in a buffer, a pointer to this buffer is
 *                      returned in "lexsym".
 *     __stringX         the string is stored in the literal pool, the index
 *                      in the literal pool to this string is stored in
 *                      "lexvalue".
 *
 *  lex() stores all information (the token found and possibly its attribute)
 *  in global variables. This allows a token to be examined twice. If "_pushed"
 *  is true, this information is returned.
 *
 *  Global references: lptr          (altered)
 *                     errflag       (altered)
 *                     fline         (referred to only)
 *                     litidx        (referred to only)
 *                     _lextok, _lexval, _lexstr
 *                     _pushed
 */

static int _pushed;
static int _lextok;
static cell _lexval;
static char _lexstr[_linemax+1];
static int _lexnewline;

void lexinit(void)
{
  stkidx=0;     /* index for pushstk() and popstk() */
  iflevel=0;    /* preprocessor: nesting of "#if" */
  skiplevel=0;  /* preprocessor: skipping lines or compiling lines */
  icomment=_no; /* currently not in a multiline comment */
  _pushed=_no;  /* no token pushed back into lex */
  _lexnewline=_no;
}

static char *tokens[] = {
         "*=", "/=", "%=", "+=", "-=", "<<=", ">>>=", ">>=", "&=", "^=", "|=",
         "||", "&&", "==", "!=", "<=", ">=", "<<", ">>>", ">>", "++", "--",
         "...", "..",
         "assert", "break", "case", "char", "const", "continue", "default",
         "defined", "do", "else", "enum", "exit", "for", "forward", "goto",
         "if", "native", "new", "public", "return", "sizeof", "static",
         "switch", "while",
         "#assert", "#define", "#else", "#emit", "#endif", "#endinput",
         "#endscript", "#if", "#include", "#pragma",
         ";", "-constant-", "-identifier-", "-label-", "-string-"
       };

int lex(cell *lexvalue,char **lexsym)
{
  int i,toolong,newline;
  char **tokptr;

  if (_pushed) {
    _pushed=_no;        /* reset "_pushed" flag */
    *lexvalue=_lexval;
    *lexsym=_lexstr;
    return _lextok;
  } /* if */

  _lextok=0;            /* preset all values */
  _lexval=0;
  _lexstr[0]='\0';
  *lexvalue=_lexval;
  *lexsym=_lexstr;
  _lexnewline=_no;
  if (!freading)
    return 0;

  newline= (lptr==pline);       /* does lptr point to start of line buffer */
  while (*lptr<=' ') {  /* delete leading white space */
    if (*lptr=='\0') {
      _lexnewline=_yes;
      preprocess();     /* preprocess resets "lptr" */
      if (!freading)
        return 0;
      newline=_yes;
    } else {
      lptr+=1;
    } /* if */
  } /* while */
  if (newline)
    stmtindent=(int)(lptr-pline);

  i=_firsttok;
  tokptr=tokens;
  while (i<=_midtok) {  /* match multi-character operators */
    if (match(*tokptr,_no)) {
      _lextok=i;
      return _lextok;
    } /* if */
    i+=1;
    tokptr+=1;
  } /* while */
  while (i<=_lasttok) {  /* match reserved words and compiler directives */
    if (match(*tokptr,_yes)) {
      _lextok=i;
      return _lextok;
    } /* if */
    i+=1;
    tokptr+=1;
  } /* while */

  if ((i=number(&_lexval,lptr))!=0){   /* number */
    _lextok=__numberX;
    *lexvalue=_lexval;
    lptr+=i;
  } else if (alpha(*lptr)){     /* symbol or label */
    /*  Note: only _namemax characters are significant. The compiler
     *        generates a warning if a symbol exceeds this length.
     */
    _lextok=__symbolX;
    i=0;
    toolong=0;
    while (alphanum(*lptr)){
      _lexstr[i]=*lptr;
      lptr+=1;
      if (i<_namemax)
        i+=1;
      else
        toolong=1;
    } /* while */
    _lexstr[i]='\0';
    if (toolong)
      error(200,_namemax);  /* symbol too long, truncated to _namemax chars */
    if (_lexstr[0]=='@' && _lexstr[1]=='\0') {
      _lextok='@';      /* '@' all alone is not a symbol, it is an operator */
    } else if (_lexstr[0]=='_' && _lexstr[1]=='\0') {
      _lextok='_';      /* '_' by itself is not a symbol, it is a placeholder */
    } else if (*lptr==':') {
      _lextok=__labelX;  /* it wasn't a normal symbol, it was a label/tagname */
      lptr+=1;          /* skip colon */
    } /* if */
  } else if (*lptr=='\"'){      /* unpacked string literal */
    _lextok=__stringX;
    *lexvalue=_lexval=litidx;
    lptr+=1;            /* skip double quote */
    lptr=packstr ? packedstring(lptr) : unpackedstring(lptr);
    if (*lptr=='\"')
      lptr+=1;          /* skip final quote */
    else
      error(37);        /* invalid (non-terminated) string */
  } else if (*lptr=='!' && *(lptr+1)=='\"') {      /* packed string literal */
    _lextok=__stringX;
    *lexvalue=_lexval=litidx;
    lptr+=2;            /* skip exclamation point and double quote */
    lptr=packstr ? unpackedstring(lptr) : packedstring(lptr);
    if (*lptr=='\"')
      lptr+=1;          /* skip final quote */
    else
      error(37);        /* invalid (non-terminated) string */
  } else if (*lptr=='\''){      /* character literal */
    lptr+=1;            /* skip quote */
    _lextok=__numberX;
    *lexvalue=_lexval=litchar(&lptr);
    if (*lptr=='\'')
      lptr+=1;          /* skip final quote */
    else
      error(1,"\'",lptr);       /* token ' expected */
  } else if (*lptr==';') {      /* semicolumn resets "error" flag */
    _lextok=';';
    lptr+=1;
    errflag=0;          /* reset error flag (clear the "panic mode")*/
  } else {
    _lextok=*lptr;      /* if every match fails, return the character */
    lptr+=1;            /* increase the "lptr" pointer */
  } /* if */
  return _lextok;
}

/*  lexpush
 *
 *  Pushes a token back, so the next call to lex() will return the token
 *  last examined, instead of a new token.
 *
 *  Only one token can be pushed back.
 *
 *  In fact, lex() already stores the information it finds into global
 *  variables, so all that is to be done is set a flag that informs lex()
 *  to read and return the information from these variables, rather than
 *  to read in a new token from the input file.
 */
void lexpush(void)
{
  assert(_pushed==_no);
  _pushed=_yes;
}

/*  lexclr
 *
 *  Sets the variable "_pushed" to 0 to make sure lex() will read in a new
 *  symbol (a not continue with some old one). This is required upon return
 *  from Assembler mode.
 */
static void lexclr(void)
{
  _pushed=_no;
}

/*  matchtoken
 *
 *  This routine is useful if only a simple check is needed. If the token
 *  differs from the one expected, it is pushed back.
 */
int matchtoken(int token)
{
  cell val;
  char *str;
  int tok;

  tok=lex(&val,&str);
  if (tok==token || token==__termX && tok==';') {
    return 1;
  } else if (!needsemicolon && token==__termX && _lexnewline) {
    lexpush();  /* push "tok" back, we use the "hidden" newline token */
    return 1;
  } else {
    lexpush();
    return 0;
  } /* if */
}

/*  tokeninfo
 *
 *  Returns additional information of a token after using "matchtoken()".
 *  It does no harm using this routine after a call to "lex()", but the
 *  information might no be correct if "lexpush()" has been called.
 *
 *  The token itself is the return value. Normally, this one is already known.
 */
int tokeninfo(cell *val,char **str)
{
  *val=_lexval;
  *str=_lexstr;
  return _lextok;
}

/*  needtoken
 *
 *  This routine checks for a required token and gives an error message if
 *  it isn't there (and returns 0 in that case).
 *
 *  Global references: _lextok;
 */
int needtoken(int token)
{
  char s1[20],s2[20];

  if (matchtoken(token)) {
    return 1;
  } else {
    /* token already pushed back */
    assert(_pushed);
    if (token<256)
      sprintf(s1,"%c",(char)token);        /* single character token */
    else
      strcpy(s1,tokens[token-_firsttok]);  /* multi-character symbol */
    if (_lextok<256)
      sprintf(s2,"%c",(char)_lextok);
    else
      strcpy(s2,tokens[_lextok-_firsttok]);
    error(1,s1,s2);     /* expected ..., but found ... */
    return 0;
  } /* if */
}

/*  match
 *
 *  Compares a series of characters from the input file with the characters
 *  in "st" (that contains a token). If the token on the input file matches
 *  "st", the input file pointer "lptr" is adjusted to point to the next
 *  token, otherwise "lptr" remains unaltered.
 *
 *  If the parameter "end: is true, match() requires that the first character
 *  behind the recognized token is non-alphanumeric.
 *
 *  Global references: lptr   (altered)
 */
static int match(char *st,int end)
{
  int k;
  char *ptr;

  k=0;
  ptr=lptr;
  while (st[k]) {
    if (st[k]!=*ptr)
      return 0;
    k+=1;
    ptr+=1;
  } /* while */
  if (end) {            /* symbol must terminate with non-alphanumeric char */
    if (alphanum(*ptr))
      return 0;
  } /* if */
  lptr=ptr;     /* match found, skip symbol */
  return 1;
}

/*  stowlit
 *
 *  Stores a value into the literal queue. The literal queue is used for
 *  literal strings used in functions and for initializing array variables.
 *
 *  Global references: litidx  (altered)
 *                     litq    (altered)
 */
void stowlit(cell value)
{
  if (litidx>=litmax) {
    cell *p;

    litmax+=_def_litmax;
    p=(long *)realloc(litq,litmax*sizeof(cell));
    if (p==NULL)
      error(102,"literal table");   /* literal table overflow (fatal error) */
    litq=p;
  } /* if */
  assert(litidx<litmax);
  litq[litidx++]=value;
}

/*  litchar
 *
 *  Return current literal character and increase the pointer to point
 *  just behind this literal character.
 *
 *  Note: standard "escape sequences" are suported, but the backslash is
 *        replaced by the caret (^); the syntax '^ddd' is supported, but ddd
 *        must be decimal!
 */
static cell litchar(char **lptr)
{
  cell c;
  char *cptr;

  cptr=*lptr;
  if (*cptr!=ctrlchar) {        /* the caret identifies a control character */
    c=*cptr;
    cptr+=1;
  } else {
    cptr+=1;
    if (*cptr==ctrlchar) {
      c=*cptr;          /* ^^ == ^ (the caret itself) */
      cptr+=1;
    } else {
      switch (*cptr) {
      case 'a':         /* ^a == audible alarm */
        c=7;
        cptr+=1;
        break;
      case 'b':         /* ^b == backspace */
        c=8;
        cptr+=1;
        break;
      case 'e':         /* ^e == escape */
        c=27;
        cptr+=1;
        break;
      case 'f':         /* ^f == form feed */
        c=12;
        cptr+=1;
        break;
      case 'n':         /* ^n == NewLine character */
        c=10;
        cptr+=1;
        break;
      case 'r':         /* ^r == carriage return */
        c=13;
        cptr+=1;
        break;
      case 't':         /* ^t == horizontal TAB */
        c=9;
        cptr+=1;
        break;
      case 'v':         /* ^v == vertical TAB */
        c=11;
        cptr+=1;
        break;
      case '\'':        /* ^' == ' (single quote) */
      case '\"':        /* ^" == " (single quote) */
        c=*cptr;
        cptr+=1;
        break;
      default:
        if (isdigit(*cptr)) {   /* ^ddd */
          c=0;
          while (*cptr>='0' && *cptr<='9')  /* decimal! */
            c=c*10 + *cptr++ - '0';
          if (*cptr==';')
            cptr++;     /* swallow a trailing ';' */
        } else {
          error(27);    /* invalid character constant */
        } /* if */
      } /* switch */
    } /* if */
  } /* if */
  *lptr=cptr;
  return c;
}

/*  alpha
 *
 *  Test if character "c" is alphabetic ("a".."z"), an underscore ("_")
 *  or an "at" sign ("@"). The "@" is an extension to standard C.
 */
static int alpha(char c)
{
  return (isalpha(c) || c=='_' || c=='@');
}

/*  alphanum
 *
 *  Test if character "c" is alphanumeric ("a".."z", "0".."9", "_" or "@")
 */
int alphanum(char c)
{
  return (alpha(c) || isdigit(c));
}

/*  ishex
 *
 *  Test if character "c" is a hexadecimal digit ("0".."9" or "a".."f").
 */
int ishex(char c)
{
  return (c>='0' && c<='9') || (c>='a' && c<='f') || (c>='A' && c<='F');
}

/* The local variable table must be searched backwards, so that the deepest
 * nesting of local variables is searched first. The simplest way to do
 * this is to insert all new items at the head of the list.
 * In the global list, the symbols are kept in sorted order, so that the
 * public functions are written in sorted order.
 */
static symbol *add_symbol(symbol *root,symbol *entry,int sort)
{
  symbol *newsym;

  if (sort)
    while (root->next!=NULL && strcmp(entry->name,root->next->name)>0)
      root=root->next;

  if ((newsym=(symbol *)malloc(sizeof(symbol)))==NULL) {
    error(103);
    return NULL;
  } /* if */
  memcpy(newsym,entry,sizeof(symbol));
  newsym->next=root->next;
  root->next=newsym;
  return newsym;
}

void delete_symbols(symbol *root,int level,int delete_labels)
{
  symbol *sym;
  arginfo *arg;

  /* erase only the symbols with a deeper nesting level than the
   * specified nesting level */
  while (root->next!=NULL) {
    sym=root->next;
    if (sym->compound<level)
      break;
    if (delete_labels || sym->ident!=_label) {
      root->next=sym->next;
      if (sym->ident==_functn) {
        /* run through the argument list; "default array" arguments
         * must be freed explicitly */
        for (arg=sym->dim.arglist; arg->ident!=0; arg++)
          if (arg->ident==_refarray && arg->hasdefault)
            free(arg->defvalue.array.data);
        free(sym->dim.arglist);
      } /* if */
      free(sym);
    } else {
      root=sym;         /* skip the symbol */
    } /* if */
  } /* if */
}

static symbol *find_symbol(symbol *root,char *name)
{
  symbol *ptr=root->next;
  while (ptr!=NULL) {
    if (strcmp(name,ptr->name)==0 && ptr->parent==NULL)
      return ptr;
    ptr=ptr->next;
  } /* while */
  return NULL;
}

static symbol *find_symbol_child(symbol *root,symbol *sym)
{
  symbol *ptr=root->next;
  while (ptr!=NULL) {
    if (ptr->parent==sym)
      return ptr;
    ptr=ptr->next;
  } /* while */
  return NULL;
}


/*  findglb
 *
 *  Returns a pointer to the global symbol (if found) or NULL (if not found)
 */
symbol *findglb(char *name)
{
  return find_symbol(&glbtab,name);
}

/*  findloc
 *
 *  Returns a pointer to the local symbol (if found) or NULL (if not found).
 *  See add_symbol() how the deepest nesting level is searched first.
 */
symbol *findloc(char *name)
{
  return find_symbol(&loctab,name);
}

symbol *findconst(char *name)
{
  symbol *sym;

  sym=find_symbol(&loctab,name);            /* try local symbols first */
  if (sym==NULL || sym->ident!=_constexpr)  /* not found, or not a constant */
    sym=find_symbol(&glbtab,name);
  if (sym==NULL || sym->ident!=_constexpr)
    return NULL;
  assert(sym->parent==NULL);    /* constants have no hierarchy */
  return sym;
}

symbol *finddepend(symbol *parent)
{
  symbol *sym;

  sym=find_symbol_child(&loctab,parent);    /* try local symbols first */
  if (sym==NULL)                            /* not found */
    sym=find_symbol_child(&glbtab,parent);
  return sym;
}

/*  addsym
 *
 *  Adds a symbol to the symbol table (either global or local variables,
 *  or global and local constants).
 */
symbol *addsym(char *name,cell addr,int ident,int vclass,int tag,int usage)
{
  symbol entry;

  /* global variables/constants/functions may only be defined once */
  assert(!(ident==_functn || ident==_constexpr) || vclass!=_global || findglb(name)==NULL);
  /* labels may only be defined once */
  assert(ident!=_label || findloc(name)==NULL);

  /* first fill in the entry */
  strcpy(entry.name,name);
  entry.addr=addr;
  entry.vclass=(char)vclass;
  entry.ident=(char)ident;
  entry.tag=tag;
  entry.usage=(char)usage;
  entry.compound=0;     /* may be overridden later */
  entry.parent=NULL;

  /* then insert it in the list */
  if (vclass==_global)
    return add_symbol(&glbtab,&entry,_yes);
  else
    return add_symbol(&loctab,&entry,_no);
}

symbol *addvariable(char *name,cell addr,int ident,int vclass,int tag,
                    int dim[],int numdim,int idxtag[])
{
  symbol *sym,*parent,*top;
  int level;

  /* global variables may only be defined once */
  assert(vclass!=_global || findglb(name)==NULL);

  if (ident==_array || ident==_refarray) {
    parent=NULL;
    for (level=0; level<numdim; level++) {
      top=addsym(name,addr,ident,vclass,tag,_define);
      top->dim.array.length=dim[level];
      top->dim.array.level=numdim-level-1;
      top->x.idxtag=idxtag[level];
      top->parent=parent;
      parent=top;
      if (level==0)
        sym=top;
    } /* for */
  } else {
    sym=addsym(name,addr,ident,vclass,tag,_define);
  } /* if */
  return sym;
}

/*  getlabel
 *
 *  Return next available internal label number.
 */
int getlabel(void)
{
  return labnum++;
}

/*  itoh
 *
 *  Converts a number to a hexadecimal string and returns a pointer to that
 *  string.
 */
char *itoh(ucell val)
{
static char itohstr[15];        /* hex number is 10 characters long at most */
  char *ptr;
  int i,nibble[8];              /* a 32-bit hexadecimal cell has 8 nibbles */
  int max;

  #if defined(BIT16)
    max=4;
  #else
    max=8;
  #endif
  ptr=itohstr;
  for (i=0; i<max; i+=1){
    nibble[i]=(int)(val & 0x0f);        /* nibble 0 is lowest nibble */
    val>>=4;
  } /* endfor */
  i=max-1;
  while (nibble[i]==0 && i>0)   /* search for highest non-zero nibble */
    i-=1;
  while (i>=0){
    if (nibble[i]>=10)
      *ptr++=(char)('a'+(nibble[i]-10));
    else
      *ptr++=(char)('0'+nibble[i]);
    i-=1;
  } /* while */
  *ptr='\0';            /* and a zero-terminator */
  return itohstr;
}

