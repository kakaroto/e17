/*  Small debugger
 *
 *  Minimal debugger with a console interface.
 *
 *  Copyright (c) ITB CompuPhase, 1998, 1999
 *  This file may be freely used. No warranties of any kind.
 */
#include <assert.h>
#include <conio.h>
#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "amx.h"


#define MAXFILES        16
#define MAXBREAKS       16
#define NAMEMAX         20      /* length of variable/function names */
#define MAX_DIMS        2       /* number of array dimensions */
#define MAXWATCHES      4
#define SCREENLINES     24      /* number of lines that fit on a screen */
#define SCREENCOLUMNS   80
#define DEF_LIST        10      /* default length of code list */
#define CMDWINDOW       (SCREENLINES - MAXWATCHES - DEF_LIST - 2)

#if CMDWINDOW <= 1
  #error Command window too small (code list or watches window too big)
#endif

enum {
  BP_NONE,
  BP_CODE,
  BP_DATA,
  /* --- */
  BP_TYPES
};

enum {
  TERM_NONE,
  TERM_ANSI,
  /* --- */
  TERM_TYPES
};

typedef struct __symbol {
  char name[NAMEMAX];
  ucell addr;           /* line number for functions */
  int file;             /* file number that a function appears in */
  int local;            /* true if it is a local variable */
  int type;
  int calllevel;        /* nesting level (visibility) of the variable */
  int length[MAX_DIMS]; /* for arrays */
  int dims;             /* for arrays */
  struct __symbol *next;
} SYMBOL;

typedef struct {
  int type;             /* one of the BP_xxx types */
  cell addr;            /* line number, or previous value */
  int file;             /* file in which the breakpoint appears */
  int index;
  SYMBOL *sym;
} BREAKPOINT;

static char *filenames[MAXFILES];
static int curfileno=-1;
static char **cursource;
static int curline;
static int stopline;
static int autolist=1;
static BREAKPOINT breakpoints[MAXBREAKS];
static char watches[MAXWATCHES][NAMEMAX];
static SYMBOL functab;
static SYMBOL vartab;
static int terminal;
static int curtopline;  /* current line that is on top in the list */

static void term_clrscr(void)
{
  assert(terminal==TERM_ANSI);
  printf("\033[2J");
}

static void term_clreol(void)
{
  assert(terminal==TERM_ANSI);
  printf("\033[K");
}

static void term_csrset(int x,int y)
{
  assert(terminal==TERM_ANSI);
  printf("\033[%d;%dH",y,x);
}

static void term_csrget(int *x,int *y)
{
  int val,i;
  char str[10];

  assert(terminal==TERM_ANSI);
  printf("\033[6n");
  fflush(stdout);
  while (getch()!='\033')
    /* nothing */;
  val=getch();
  assert(val=='[');
  for (i=0; i<8 && (val=getch())!=';'; i++)
    str[i]=(char)val;
  str[i]='\0';
  *y=atoi(str);
  for (i=0; i<8 && (val=getch())!='R'; i++)
    str[i]=(char)val;
  str[i]='\0';
  *x=atoi(str);
  val=getch();
  assert(val=='\r');    /* ANSI driver adds CR to the end of the command */
}

static void term_csrsave(void)
{
  assert(terminal==TERM_ANSI);
  printf("\033[s");
}

static void term_csrrestore(void)
{
  assert(terminal==TERM_ANSI);
  printf("\033[u");
}

static void term_open(void)
{
  int i;

  assert(terminal==TERM_ANSI);
  term_clrscr();
  term_csrset(1,MAXWATCHES+1);
  for (i=0; i<SCREENCOLUMNS; i++)
    printf("-");
  term_csrset(1,MAXWATCHES+DEF_LIST+2);
  for (i=0; i<SCREENCOLUMNS; i++)
    printf("-");
}

static void term_close(void)
{
  assert(terminal==TERM_ANSI);
  term_clrscr();
}

static void term_restore(void)
{
  int x,y;

  if (terminal==TERM_NONE)
    return;
  term_csrget(&x,&y);
  if (y>=SCREENLINES)
    term_open();
}

static void source_free(char **source)
{
  int i;

  assert(source!=NULL);
  for (i=0; source[i]!=NULL; i++)
    free(source[i]);
  free(source);
}

static char **source_load(char *filename)
{
  char **source;
  FILE *fp;
  char line[256];
  int lines,i;

  /* open the file, number of characters */
  if ((fp=fopen(filename,"rt"))==NULL)
    return NULL;
  lines=0;
  while (fgets(line,255,fp)!=NULL)
    lines++;

  /* allocate memory, reload the file */
  if ((source=(char **)malloc((lines+1)*sizeof(char *)))==NULL) {
    fclose(fp);
    return NULL;
  } /* if */
  for (i=0; i<=lines; i++)      /* initialize, so that source_free() works */
    source[i]=NULL;
  rewind(fp);
  i=0;
  while (fgets(line,255,fp)!=NULL) {
    assert(i<lines);
    source[i]=strdup(line);
    if (source[i]==NULL) {
      fclose(fp);
      source_free(source);      /* free everything allocated so far */
      return NULL;
    } /* if */
    i++;
  } /* if */

  fclose(fp);
  return source;
}

static void source_list(int startline, int numlines)
{
  /* cursource and curline should already have been set */
  int result,lastline;

  if (terminal==TERM_ANSI) {
    term_csrsave();
    term_csrset(1,MAXWATCHES+2);
    numlines=DEF_LIST;  /* override user setting */
  } /* if */

  if (startline<0)
    startline=0;
  lastline=startline+numlines;
  curtopline=startline; /* save line that is currently displayed at the top */

  /* seek to line number from the start (to avoid displaying something
   * beyond the file)
   */
  for (result=0; cursource[result]!=NULL && result<startline; result++)
    /* nothing */;
  if (cursource[result]!=NULL) {
    assert(result==startline);
    while (cursource[startline]!=NULL && startline<lastline) {
      if (terminal==TERM_ANSI)
        term_clreol();
      if (startline==curline)
        printf("[%d]\t%s",startline+1,cursource[startline]);
      else
        printf(" %d \t%s",startline+1,cursource[startline]);
      startline++;
    } /* while */
  } /* if */

  if (terminal==TERM_ANSI) {
    while (startline<lastline) {
      term_clreol();
      printf("\n");
      startline++;
    } /* while */
    term_csrrestore();
  } /* if */
}

static int gettopline(int line,int topline)
{
  if (terminal==TERM_NONE)
    return topline;
  if (line<curtopline || line>=curtopline+DEF_LIST)
    return topline;
  return curtopline;
}

static char *skipwhitespace(char *str)
{
  while (*str==' ' || *str=='\t')
    str++;
  return str;
}

static char *skipvalue(char *str)
{
  while (isdigit(*str))
    str++;
  str=skipwhitespace(str);
  return str;
}

static char *skippath(char *str)
{
  char *p1,*p2;

  /* DOS/Windows pathnames */
  if ((p1=strrchr(str,'\\'))!=NULL)
    p1++;
  else
    p1=str;
  /* Unix pathnames */
  if ((p2=strrchr(str,'/'))!=NULL)
    p2++;
  else
    p2=str;
  return p1>p2 ? p1 : p2;
}

static SYMBOL *add_symbol(SYMBOL *table,char *name,int type,ucell addr,
                          int vclass,int level)
{
  SYMBOL *sym;

  if ((sym=(SYMBOL *)malloc(sizeof(SYMBOL)))==NULL)
    return NULL;
  memset(sym,0,sizeof(SYMBOL));
  assert(strlen(name)<NAMEMAX);
  strcpy(sym->name,name);
  sym->type=type;
  sym->addr=addr;
  sym->local=vclass;
  sym->calllevel=level;
  sym->length[0]=0;     /* indeterminate */
  sym->length[1]=0;     /* indeterminate */
  if (type==3 || type==4)
    sym->dims=1;        /* for now, assume single dimension */
  else
    sym->dims=0;        /* not an array */
  sym->next=table->next;
  table->next=sym;
  return sym;
}

static SYMBOL *find_symbol(SYMBOL *table,char *name,int level)
{
  SYMBOL *sym = table->next;

  while (sym!=NULL) {
    if (strcmp(name,sym->name)==0 && sym->calllevel==level)
      return sym;
    sym=sym->next;
  } /* while */
  return NULL;
}

static void delete_symbol(SYMBOL *table,ucell addr)
{
  SYMBOL *prev = table;
  SYMBOL *cur = prev->next;

  while (cur!=NULL) {
    if (cur->local==1 && cur->addr<addr) {
      prev->next=cur->next;
      free(cur);
      cur=prev->next;
    } else {
      prev=cur;
      cur=cur->next;
    } /* if */
  } /* while */
}

static void delete_allsymbols(SYMBOL *table)
{
  SYMBOL *sym=table->next, *next;

  while (sym!=NULL) {
    next=sym->next;
    free(sym);
    sym=next;
  } /* while */
}

static cell get_symbolvalue(AMX *amx,SYMBOL *sym,int index)
{
  cell *value;
  cell base;

  if (sym->type==2 || sym->type==4) {   /* a reference */
    amx_GetAddr(amx,sym->addr,&value);
    base=*value;
  } else {
    base=sym->addr;
  } /* if */
  amx_GetAddr(amx,base+index*sizeof(cell),&value);
  return *value;
}

static void watch_init(void)
{
  int i;

  for (i=0; i<MAXWATCHES && strlen(watches[i])>0; i++)
    watches[i][0]='\0';
}

static void watch_list(AMX *amx,int calllevel)
{
  int num,i,idx;
  SYMBOL *sym;
  char *indexptr;
  char name[NAMEMAX];

  if (terminal==TERM_ANSI) {
    term_csrsave();
    term_csrset(1,1);
  } /* if */

  for (i=num=0; i<MAXWATCHES; i++) {
    if (strlen(watches[i])>0) {
      strcpy(name,watches[i]);
      indexptr=strchr(name,'[');
      if (indexptr!=NULL) {
        idx=atoi(indexptr+1);
        *indexptr='\0';
      } else {
        idx=0;
      } /* if */
      printf("%d   %s\t",i+1,watches[i]);
      if ((sym=find_symbol(&vartab,name,calllevel))!=NULL) {
        if ((sym->type==3 || sym->type==4) && indexptr==NULL)
          printf("(missing index)");    // ??? try to print as string
        else if ((sym->type==3 || sym->type==4) && sym->length[0]>0 && idx>=sym->length[0])
          printf("(index out of range)");
        else if (sym->type!=3 && sym->type!=4 && indexptr!=NULL)
          printf("(invalid index)");
        else
          printf("%ld", get_symbolvalue(amx,sym,idx));
      } else {
        printf("(unknown symbol)");
      } /* if */
      if (terminal==TERM_ANSI)
        term_clreol();
      printf("\n");
      num++;
    } /* if */
  } /* for */

  if (terminal==TERM_ANSI) {
    if (num==0)
      printf("(no watches)");
    for ( ; num<MAXWATCHES; num++) {
      term_clreol();
      printf("\n");
    } /* for */
    term_csrrestore();
  } else {
    if (num>0)
      for (i=0; i<SCREENCOLUMNS; i++)
        printf("-");
  } /* if */
}

static int watch_set(int number, char *name)
{
  if (number<0 || number>MAXWATCHES)
    return 0;
  if (number==0) {
    /* find an empty spot */
    while (number<MAXWATCHES && strlen(watches[number])>0)
      number++;
    if (number>=MAXWATCHES)
      return 0;   /* watch table full */
  } else {
    number--;
  } /* if */
  /* add the watch */
  strcpy(watches[number],name);
  return 1;
}

static int watch_clear(int number)
{
  if (number>0 && number<=MAXWATCHES) {
    watches[number-1][0]='\0';
    return 1;
  } /* if */
  return 0;
}

static void break_init(void)
{
  int i;

  for (i=0; i<MAXBREAKS; i++) {
    breakpoints[i].type=BP_NONE;
    breakpoints[i].addr=0;
    breakpoints[i].sym=NULL;
    breakpoints[i].index=-1;
  } /* for */
}

static void break_clear(int index)
{
  if (index>=0 && index<=MAXBREAKS) {
    breakpoints[index].type=BP_NONE;
    breakpoints[index].addr=0;
    breakpoints[index].file=-1;
    breakpoints[index].sym=NULL;
    breakpoints[index].index=-1;
  } /* if */
}

static int break_set(AMX *amx,char *str,int calllevel)
{
  int index;
  SYMBOL *sym;

  /* find an empty spot */
  for (index=0; index<MAXBREAKS && breakpoints[index].type!=BP_NONE; index++)
    /* nothing */
  if (index>=MAXBREAKS)
    return -1;
  assert(breakpoints[index].sym==NULL);
  assert(breakpoints[index].addr==0);
  assert(breakpoints[index].index==-1);

  /* find type */
  str=skipwhitespace(str);
  if (isdigit(*str)) {
    breakpoints[index].type=BP_CODE;
    breakpoints[index].file=curfileno;
    breakpoints[index].addr=atol(str);
  } else if ((sym=find_symbol(&functab,str,-1))!=NULL) {
    breakpoints[index].type=BP_CODE;
    breakpoints[index].addr=sym->addr;
    breakpoints[index].file=sym->file;
    breakpoints[index].sym=sym;
  } else {
    char *idxptr=strchr(str,'[');
    int idx=-1;
    if (idxptr!=NULL) {
      idx=atoi(idxptr+1);
      *idxptr='\0';
    } /* if */
    if ((sym=find_symbol(&vartab,str,calllevel))!=NULL) {
      if (sym->type==3 || sym->type==4) {
        if (idxptr==NULL)
          return -1;            // missing index on array
        if (sym->length[0]>0 && idx>=sym->length[0])
          return -1;            // index out of range
      } /* if */
      if (sym->type!=3 && sym->type!=4 && idxptr!=NULL)
        return -1;
      breakpoints[index].type=BP_DATA;
      breakpoints[index].addr=get_symbolvalue(amx,sym,idx>=0 ? idx : 0);
      breakpoints[index].sym=sym;
      breakpoints[index].index=idx;
    } else {
      return -1;
    } /* if */
  } /* if */
  return index;
}

static void break_list(void)
{
  int index;

  for (index=0; index<MAXBREAKS; index++) {
    if (breakpoints[index].type==BP_NONE)
      continue;
    printf("%2d  ",index);
    if (breakpoints[index].type==BP_CODE) {
      int file = breakpoints[index].file;
      printf("line: %d",breakpoints[index].addr);
      if (breakpoints[index].sym!=NULL)
        printf("  func: %s",breakpoints[index].sym->name);
      else
        printf("  file: %s",skippath(filenames[file]));
      printf("\n");
    } else {
      /* must be variable */
      assert(breakpoints[index].sym!=NULL);
      printf("var: %s",breakpoints[index].sym->name);
      if (breakpoints[index].index>=0)
        printf("[%d]",breakpoints[index].index);
      printf("\n");
    } /* if */
  } /* for */
}

static int break_check(AMX *amx,int line,int file)
{
  int index;

  for (index=0; index<MAXBREAKS; index++) {
    if (breakpoints[index].type==BP_CODE && breakpoints[index].addr==line
        && breakpoints[index].file==file)
    {
      return index;
    } else if (breakpoints[index].type==BP_DATA) {
      int idx=breakpoints[index].index;
      SYMBOL *sym=breakpoints[index].sym;
      cell value;
      assert(sym!=NULL);
      value=get_symbolvalue(amx,sym,idx>=0 ? idx : 0);
      if (breakpoints[index].addr!=value) {
        breakpoints[index].addr=value;
        return index;
      } /* if */
    } /* if */
  } /* for */
  return -1;
}

enum {
  GO,
  GO_RET,
  NEXT,
  STEP,
};

static void listcommands(char *command)
{
  if (command==NULL)
    command="";
  if (stricmp(command,"break")==0) {
    printf("\tBREAK\t\tlist all breakpoints\n"
           "\tBREAK n\t\tset a breakpoint at line \"n\"\n"
           "\tBREAK func\tset a breakpoint at function with name \"func\"\n"
           "\tBREAK var\tset a breakpoint at variable \"var\"\n"
           "\tBREAK var[i]\tset a breakpoint at array element \"var[i]\"\n");
  } else if (stricmp(command,"cbreak")==0) {
    printf("\tCBREAK n\tremove breakpoint number \"n\"\n"
           "\tCBREAK *\tremove all breakpoints\n");
  } else if (stricmp(command,"cwatch")==0) {
    printf("\tCWATCH n\tremove watch number \"n\"\n"
           "\tCWATCH *\tremove all watches\n");
  } else if (stricmp(command,"disp")==0 || stricmp(command,"d")==0) {
    printf("\tDISP may be abbreviated to D\n\n"
           "\tDISP\t\tdisplay all variables that are currently in scope\n"
           "\tDISP var\tdisplay the value of variable \"var\"\n"
           "\tDISP var[i]\tdisplay the value of array element \"var[i]\"\n");
  } else if (stricmp(command,"file")==0) {
    printf("\tFILE\t\tlist all files that this program is composed off\n"
           "\tFILE name\tset the current file to \"name\"\n");
  } else if (stricmp(command,"g")==0 || stricmp(command,"go")==0) {
    printf("\tGO may be abbreviated to G\n\n"
           "\tGO\t\trun until the next breakpoint or program termination\n"
           "\tGO RET\t\trun until the end of the current function\n"
           "\tGO n\t\trun until line number \"n\"\n");
  } else if (stricmp(command,"l")==0 || stricmp(command,"list")==0) {
    printf("\tLIST may be abbreviated to L\n\n"
           "\tLIST\t\tdisplay 10 lines around the current line\n"
           "\tLIST n\t\tdisplay 10 lines, starting from line \"n\"\n"
           "\tLIST n m\tdisplay \"m\" lines, starting from line \"n\"\n"
           "\tLIST FUNCS\tdisplay all functions\n"
           "\tLIST ON\t\tautomatically list 10 lines after each step\n"
           "\tLIST OFF\tturn off automatic list\n");
  } else if (stricmp(command,"n")==0 || stricmp(command,"next")==0
             || stricmp(command,"quit")==0
             || stricmp(command,"s")==0 || stricmp(command,"step")==0)
  {
    printf("\tno additional information\n");
  } else if (stricmp(command,"term")==0) {
    printf("\tTERM ANSI\tuse VT100/ANSI terminal display\n"
           "\tTERM OFF\tno terminal support\n");
  } else if (stricmp(command,"watch")==0) {
    printf("\tWATCH var\tset a new watch at variable \"var\"\n"
           "\tWATCH n var\tchange watch \"n\" to variable \"var\"\n");
  } else {
    printf("\tBREAK\t\tset breakpoint at line number or variable name\n"
           /* "\tCALLS\t\tshow call stack\n" ??? not yet implemented */
           "\tCBREAK\t\tremove breakpoint\n"
           "\tCWATCH\t\tremove a \"watchpoint\"\n"
           "\tD(isp)\t\tdisplay the value of a variable, list variables\n"
           "\tFILE\t\tswitch to a file\n"
           "\tG(o)\t\trun program (until breakpoint)\n"
           "\tL(ist)\t\tdisplay source file and symbols\n"
           "\tN(ext)\t\tRun until next line, step over functions\n"
           "\tQUIT\t\texit debugger, terminate program\n"
           "\tS(tep)\t\tsingle step, step into functions\n"
           "\tTERM\t\tset terminal type\n"
           "\tWATCH\t\tset a \"watchpoint\" on a variable\n"
           "\n\tUse \"? <command name>\" to view more information on a command\n");
  } /* if */
}

static int docommand(AMX *amx,int calllevel)
{
static char lastcommand[10] = "";
  char line[100], command[32];
  int result,i;
  SYMBOL *sym;
  char *params;

  for ( ;; ) {
    printf("- ");
    gets(line);
    if (strlen(line)==0)
      strcpy(line,lastcommand);
    lastcommand[0]='\0';

    result=sscanf(line,"%8s",command);
    if (result<=0) {
      listcommands(NULL);
      continue;
    } /* if */
    params=strchr(line,' ');
    params=(params!=NULL) ? skipwhitespace(params) : "";

    if (stricmp(command,"?")==0) {
      result=sscanf(line,"%*s %30s",command);
      listcommands(result ? command : NULL);
    } else if (stricmp(command,"quit")==0) {
      exit(0);
    } else if (stricmp(command,"g")==0 || stricmp(command,"go")==0) {
      if (stricmp(params,"ret")==0)
        return GO_RET;
      stopline=atoi(params);
      return GO;
    } else if (stricmp(command,"s")==0 || stricmp(command,"step")==0) {
      strcpy(lastcommand,"s");
      return STEP;
    } else if (stricmp(command,"n")==0 || stricmp(command,"next")==0) {
      strcpy(lastcommand,"n");
      return NEXT;
    } else if (stricmp(command,"l")==0 || stricmp(command,"list")==0) {
      /* first check a few hard cases */
      if (stricmp(params,"funcs")==0) {
        for (sym=functab.next; sym!=NULL; sym=sym->next)
          printf("%s\t%s(%d)\n",sym->name,
                 skippath(filenames[sym->file]),(int)sym->addr);
      } else if (stricmp(params,"on")==0) {
        autolist=DEF_LIST;
        watch_list(amx,calllevel);
        source_list(curline-autolist/2,autolist);
      } else if (stricmp(params,"off")==0) {
        if (terminal==TERM_NONE)
          autolist=1;
        else
          printf("\tCommand not supported on terminals\n");
      } else {
        int lnum,numlines;
        lnum=curline-(DEF_LIST/2-1);    /* preset */
        numlines=DEF_LIST;
        sscanf(line,"%*s %d %d",&lnum,&numlines);
        lnum--;           /* if user filled in a line number, subtract 1 */
        term_restore();
        source_list(lnum,numlines);
      } /* if */
    } else if (stricmp(command,"break")==0) {
      if (*params=='\0') {
        break_list();
      } else {
        result=break_set(amx,params,calllevel);
        if (result<0)
          printf("Invalid breakpoint, or table full\n");
      } /* if */
    } else if (stricmp(command,"cbreak")==0) {
      if (*params=='*') {
        /* clear all breakpoints */
        for (i=0; i<MAXBREAKS; i++)
          break_clear(i);
      } else if (isdigit(*params)) {
        break_clear(atoi(params));
      } else {
        printf("\tInvalid command\n");
      } /* if */
    } else if (stricmp(command,"disp")==0 || stricmp(command,"d")==0) {
      if (*params=='\0') {
        /* display all */
        for (sym=vartab.next; sym!=NULL; sym=sym->next) {
          if (sym->calllevel==-1 || sym->calllevel==calllevel) {
            printf("%s\t%s\t%ld",sym->local>0 ? "loc" : "glb",sym->name,
                   get_symbolvalue(amx,sym,0));
            if (sym->type==3 || sym->type==4)
              printf(" [...]");   /* ??? an array (try to print as string) */
            printf("\n");
          } /* if */
        } /* for */
      } else {
        char *indexptr=strchr(params,'[');
        if (indexptr!=NULL) {
          i=atoi(indexptr+1);
          *indexptr='\0';
        } else {
          i=0;
        } /* if */
        if ((sym=find_symbol(&vartab,params,calllevel))!=NULL) {
          if (sym->dims>0 && indexptr==NULL)
            printf("\tArray must be indexed\n");  // ??? try to print as string
          else if (sym->dims==0 && indexptr!=NULL)
            printf("\tInvalid index, not an array\n");
          else if (sym->dims>0 && sym->length[0]>0 && i>=sym->length[0])
            printf("\tIndex out of range\n");
          else
            printf("%s\t%s\t%ld\n",sym->local>0 ? "loc" : "glb",sym->name,
                   get_symbolvalue(amx,sym,i));
        } else {
          printf("\tSymbol not found, or not a variable\n");
        } /* if */
      } /* if */
    } else if (stricmp(command,"file")==0) {
      if (*params=='\0') {
        for (i=0; i<MAXFILES && filenames[i]!=NULL; i++)
          printf("%5d\t%s\n",i,filenames[i]);
      } else {
        /* find the file */
        int file;
        for (file=0; file<MAXFILES; file++) {
          if (filenames[file]!=NULL
              && (stricmp(params,filenames[file])==0
                  || stricmp(params,skippath(filenames[file]))==0))
            break;
        } /* for */
        if (file<MAXFILES) {
          if (curfileno!=file) {
            curfileno=file;
            curline=0;
          } /* if */
          if (cursource!=NULL)
            source_free(cursource);
          assert(filenames[curfileno]!=NULL);
          cursource=source_load(filenames[curfileno]);
          if (cursource==NULL) {
            printf("\tSource file not found or insufficient memory\n");
            continue;
          } /* if */
        } else {
          printf("\tunknown file\n");
        } /* if */
      } /* if */
    } else if (stricmp(command,"term")==0) {
      int new_term = terminal;
      if (stricmp(params,"off")==0)
        new_term=TERM_NONE;
      else if (stricmp(params,"ansi")==0)
        new_term=TERM_ANSI;
      else
        printf("\tUnknown terminal type\n");
      if (terminal!=new_term) {
        curtopline=0;
        if (terminal!=TERM_NONE)
          term_close();
        terminal=new_term;
        if (terminal!=TERM_NONE) {
          autolist=DEF_LIST;
          term_open();
        } /* if */
        watch_list(amx,calllevel);
        source_list(gettopline(curline,curline-autolist/2),autolist);
      } /* if */
    } else if (stricmp(command,"watch")==0) {
      if (isdigit(*params)) {
        i=atoi(params);
        params=skipvalue(params);
      } else {
        i = 0;
      } /* if */
      result=watch_set(i,params);
      if (result>=0)
        watch_list(amx,calllevel);
      else
        printf("Invalid watch, or table full\n");
    } else if (stricmp(command,"cwatch")==0) {
      if (*params=='*') {
        /* clear all breakpoints */
        for (i=0; i<MAXBREAKS; i++)
          watch_clear(i);
      } else if (isdigit(*params)) {
        watch_clear(atoi(params));
      } else {
        printf("\tInvalid command\n");
      } /* if */
      watch_list(amx,calllevel);
    } else {
      printf("\tInvalid command, use \"?\" to view all commands\n");
    } /* if */
  } /* for */
}

static int debugproc(AMX *amx)
{
static int tracelevel;
static int calllevel;
static SYMBOL *curfunc;
static SYMBOL *curvar;
  int cmd,i,vclass,type,num;
  unsigned short flags;

  switch (amx->dbgcode) {
  case DBG_INIT:
    assert(amx->flags==AMX_FLAG_BROWSE);
    /* check whether we should run */
    amx_Flags(amx,&flags);
    if ((flags & AMX_FLAG_DEBUG)==0 || curfileno!=-1)
      return AMX_ERR_DEBUG;     /* the debugger cannot run */
    /* intialize the file table and other global variables */
    for (i=0; i<MAXFILES; i++)
      filenames[i]=NULL;
    cursource=NULL;
    curfileno=-1;
    stopline=0;
    break_init();
    watch_init();
    functab.next=NULL;
    vartab.next=NULL;
    /* initialize statics here */
    tracelevel=0;
    calllevel=0;
    curfunc=NULL;
    curvar=NULL;
    break;
  case DBG_FILE:        /* file number in curfile, filename in dbgname */
    assert(amx->flags==(AMX_FLAG_DEBUG | AMX_FLAG_BROWSE));
    if (amx->curfile>=MAXFILES) {
      printf("\nCritical error: too many source files\n");
      exit(1);
    } /* if */
    /* file should not already be set */
    num=(int)amx->curfile;
    assert(filenames[num]==NULL);
    filenames[num]=(char *)malloc(strlen(amx->dbgname)+1);
    if (filenames[num]!=NULL) {
      strcpy(filenames[num],amx->dbgname);
    } else {
      printf("\nCritical error: insufficient memory\n");
      exit(1);
    } /* if */
    break;
  case DBG_LINE:        /* line number in curline, file number in curfile */
    if ((amx->flags & AMX_FLAG_BROWSE)!=0) {
      /* check whether there is a function symbol that needs to be adjusted */
      if (curfunc!=NULL) {
        curfunc->addr=amx->curline;
        curfunc->file=(int)amx->curfile;
      } /* if */
      curfunc=NULL;
      break;            /* ??? could build a list with valid breakpoints */
    } /* if */
    curline=(int)amx->curline-1;
    /* check breakpoints */
    if ((int)amx->curline==stopline) {
      printf("STOPPED at line %d\n",(int)amx->curline);
      tracelevel=calllevel;
      stopline=0;
    } else if ((i=break_check(amx,(int)amx->curline,(int)amx->curfile))>=0) {
      printf("STOPPED at breakpoint %d\n",i);
      tracelevel=calllevel;
    } /* if */
    if (tracelevel<calllevel)
      break;
    assert(amx->curfile>=0 && amx->curfile<MAXFILES);
    if (curfileno!=(int)amx->curfile) {
      curfileno=(int)amx->curfile;
      if (cursource!=NULL)
        source_free(cursource);
      cursource=source_load(filenames[curfileno]);
      if (cursource==NULL) {
        printf("\nCritical error: source file not found or insufficient memory\n");
        exit(1);
      } /* if */
    } /* if */
    assert(cursource[curline]!=NULL);
    term_restore();
    watch_list(amx,calllevel);
    source_list(gettopline(curline,curline-autolist/2),autolist);
    cmd=docommand(amx,calllevel);
    switch (cmd) {
    case GO:
      tracelevel=-1;
      break;
    case GO_RET:
      tracelevel=calllevel-1;
      break;
    case NEXT:
      tracelevel= calllevel;    /* step OVER functions */
      break;
    case STEP:
      tracelevel= calllevel+1;  /* step INTO functions */
    } /* switch */
    break;
  case DBG_SYMBOL:      /* address in dbgaddr, class/type in dbgparam,
                         * symbol name in dbgname */
    vclass=(int)(amx->dbgparam>>8);
    type=(int)amx->dbgparam & 0xff;
    if (type==9) {
      /* function */
      assert(vclass==0);
      assert(amx->flags==(AMX_FLAG_DEBUG | AMX_FLAG_BROWSE));
      curfunc=add_symbol(&functab,amx->dbgname,type,0,vclass,-1);
    } else {
      /* must modify address relative to frame */
      if (vclass==1)
        amx->dbgaddr += amx->frm;
      assert((amx->flags & AMX_FLAG_DEBUG)!=0);
      if ((amx->flags & AMX_FLAG_BROWSE)!=0)
        calllevel=-1;
      curvar=add_symbol(&vartab,amx->dbgname,type,amx->dbgaddr,vclass,calllevel);
    } /* if */
    break;
  case DBG_SRANGE:
    /* check whether there is a symbol that needs to be adjusted */
    if (curvar!=NULL) {
      curvar->length[(int)amx->dbgaddr]=(int)amx->dbgparam;
      if (curvar->dims<(int)amx->dbgaddr+1)
        curvar->dims=(int)amx->dbgaddr+1;
      if (amx->dbgaddr==0)
        curvar=NULL;    /* last dimension handled */
    } /* if */
    break;
  case DBG_CLRSYM:      /* stk = stack address below which locals should be removed */
    assert((amx->flags & (AMX_FLAG_DEBUG | AMX_FLAG_BROWSE))==AMX_FLAG_DEBUG);
    delete_symbol(&vartab,amx->stk);
    break;
  case DBG_CALL:        /* function call */
    assert((amx->flags & (AMX_FLAG_DEBUG | AMX_FLAG_BROWSE))==AMX_FLAG_DEBUG);
    calllevel++;
    break;
  case DBG_RETURN:      /* function returns */
    assert((amx->flags & (AMX_FLAG_DEBUG | AMX_FLAG_BROWSE))==AMX_FLAG_DEBUG);
    calllevel--;
    if (tracelevel>calllevel)
      tracelevel=calllevel;
    break;
  case DBG_TERMINATE:   /* program ends */
    assert((amx->flags & (AMX_FLAG_DEBUG | AMX_FLAG_BROWSE))==AMX_FLAG_DEBUG);
    for (i=0; i<MAXFILES; i++)
      if (filenames[i]!=NULL)
        free(filenames[i]);
    if (cursource!=NULL)
      source_free(cursource);
    delete_allsymbols(&functab);
    delete_allsymbols(&vartab);
    curfileno=-1;
    /* ??? save breakpoints on exit */
    /* ??? save terminal type */
    break;
  } /* switch */
  return AMX_ERR_NONE;
}

static void *loadprogram(AMX *amx,char *filename)
{
  FILE *fp;
  AMX_HEADER hdr;
  void *program = NULL;

  if ((fp = fopen(filename,"rb")) != NULL) {
    fread(&hdr, sizeof hdr, 1, fp);
    if ((program = malloc((int)hdr.stp)) != NULL) {
      rewind(fp);
      fread(program, 1, (int)hdr.size, fp);
      fclose(fp);
      memset(amx, 0, sizeof *amx);
      amx_SetDebugHook(amx, debugproc);
      if (amx_Init(amx,program) == AMX_ERR_NONE)
        return program;
      free(program);
    } /* if */
  } /* if */
  return NULL;
}

int main(int argc,char *argv[])
{
extern AMX_NATIVE_INFO core_Natives[];
extern AMX_NATIVE_INFO console_Natives[];
extern void core_Init(void);
extern void core_Exit(void);

  AMX amx;
  cell ret;
  int err;
  void *program;
  unsigned short flags;

  printf("Small command line debugger\n\n");
  if (argc != 2 || (program = loadprogram(&amx,argv[1])) == NULL) {
    printf("Usage: SDBG <filename>\n\n"
           "The filename must include the extension\n");
    return 1;
  } /* if */
  amx_Flags(&amx,&flags);
  if ((flags & AMX_FLAG_DEBUG)==0) {
    printf("This program has no debug information\n");
    return 1;
  } /* if */

  core_Init();

  amx_Register(&amx, core_Natives, -1);
  err = amx_Register(&amx, console_Natives, -1);

  if (err == AMX_ERR_NONE)
    err = amx_Exec(&amx, &ret, AMX_EXEC_MAIN, 0);

  if (err != AMX_ERR_NONE)
    printf("Run time error %d on line %ld\n", err, amx.curline);
  else
    printf("Normal termination, return value %ld\n", (long)ret);

  free(program);
  core_Exit();

  return 0;
}

