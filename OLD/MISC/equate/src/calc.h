#ifndef _EQUATE_CALC_H
#define _EQUATE_CALC_H

/* Functions the gui calls                           */
void            equate_clear(void);
int             equate_append(char *str);
double          equate_eval(void);

/* Fonctions type.                                   */
typedef double  (*func_t) (double);

/* Data type for links in the chain of symbols.      */
struct symrec {
   char           *name;        /* name of symbol                     */
   int             type;        /* type of symbol: either VAR or FNCT */
   union {
      double          var;      /* value of a VAR   */
      func_t          fnctptr;  /* value of a FNCT  */
   } value;
   struct symrec  *next;        /* link field              */
};

typedef struct symrec symrec;

/* The symbol table: a chain of `struct symrec'.     */
extern symrec  *sym_table;

symrec         *putsym(const char *, int);
symrec         *getsym(const char *);

#endif
