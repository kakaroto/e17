#include "edje_parser_math.h"
#include <math.h>
#include <errno.h>
/* from edje_cc_parse.c...sort of */

/* simple expression parsing protos */
static const char *_alphai(const char *s,
                           int        *val,
                           Eina_Bool  *err);
static const char *_betai(const char *s,
                          int        *val,
                          Eina_Bool  *err);
static const char *_gammai(const char *s,
                           int        *val,
                           Eina_Bool  *err);
static const char *_deltai(const char *s,
                           int        *val,
                           Eina_Bool  *err);
static const char *_get_numi(const char *s,
                             int        *val,
                             Eina_Bool  *err);
static Eina_Bool _is_numi(char c);
static Eina_Bool _is_op1i(char c);
static Eina_Bool _is_op2i(char c);
static int       _calci(char op,
                        int  a,
                        int  b);

static const char *_alphaf(const char *s,
                           double     *val,
                           Eina_Bool  *err);
static const char *_betaf(const char *s,
                          double     *val,
                          Eina_Bool  *err);
static const char *_gammaf(const char *s,
                           double     *val,
                           Eina_Bool  *err);
static const char *_deltaf(const char *s,
                           double     *val,
                           Eina_Bool  *err);
static const char *_get_numf(const char *s,
                             double     *val,
                             Eina_Bool  *err);
static Eina_Bool _is_numf(char c);
static Eina_Bool _is_op1f(char c);
static Eina_Bool _is_op2f(char c);
static double    _calcf(char   op,
                        double a,
                        double b);

/* simple expression parsing stuff */

/*
 * alpha ::= beta + beta || beta
 * beta  ::= gamma + gamma || gamma
 * gamma ::= num || delta
 * delta ::= '(' alpha ')'
 *
 */

/* int set of function */

static const char *
_deltai(const char *s,
        int        *val,
        Eina_Bool  *err)
{
   if (*err || (!val)) return NULL;
   if (s[0] != '(')
     {
        *err = EINA_TRUE;
        return s;
     }
   s++;
   s = _alphai(s, val, err);
   s++;
   return s;
}

static const char *
_funci(const char *s,
       int        *val,
       Eina_Bool  *err)
{
   if (*err || (!val)) return NULL;
   if (!strncmp(s, "floor(", 6))
     {
        s = _deltai(s + 5, val, err);
        *val = *val;
     }
   else if (!strncmp(s, "ceil(", 5))
     {
        s = _deltai(s + 4, val, err);
        *val = *val;
     }
   else
     *err = EINA_TRUE;
   return s;
}

static const char *
_gammai(const char *s,
        int        *val,
        Eina_Bool  *err)
{
   if (*err || (!val)) return NULL;

   if (_is_numi(s[0]))
     s = _get_numi(s, val, err);
   else if (s[0] == '(')
     s = _deltai(s, val, err);
   else
     s = _funci(s, val, err);

   return s;
}

static const char *
_betai(const char *s,
       int        *val,
       Eina_Bool  *err)
{
   int a1, a2;
   char op;

   if (*err || (!val)) return NULL;
   s = _gammai(s, &a1, err);
   while (_is_op1i(s[0]))
     {
        op = s[0];
        s = _gammai(s + 1, &a2, err);
        a1 = _calci(op, a1, a2);
     }
   *val = a1;
   return s;
}

static const char *
_alphai(const char *s,
        int        *val,
        Eina_Bool  *err)
{
   int a1, a2;
   char op;

   if (*err || (!val)) return NULL;
   s = _betai(s, &a1, err);
   while (_is_op2i(s[0]))
     {
        op = s[0];
        s = _betai(s + 1, &a2, err);
        a1 = _calci(op, a1, a2);
     }
   (*val) = a1;
   return s;
}

const char *
_get_numi(const char *s,
          int        *val,
          Eina_Bool  *err)
{
   char *end;

   if (*err || (!val)) return s;

   errno = 0;
   *val = strtol(s, &end, 10);
   if (errno) *err = EINA_TRUE;
   return end;
}

static Eina_Bool
_is_numi(char c)
{
   if (((c >= '0') && (c <= '9')) || ('-' == c) || ('+' == c))
     return EINA_TRUE;
   else
     return EINA_FALSE;
}

static Eina_Bool
_is_op1i(char c)
{
   switch (c)
     {
      case '*':
      case '%':
      case '/':
        return EINA_TRUE;

      default: break;
     }
   return EINA_FALSE;
}

static Eina_Bool
_is_op2i(char c)
{
   switch (c)
     {
      case '+':
      case '-':
        return EINA_TRUE;

      default: break;
     }
   return EINA_FALSE;
}

static int
_calci(char op,
       int  a,
       int  b)
{
   switch(op)
     {
      case '+':
        a += b;
        break;

      case '-':
        a -= b;
        break;

      case '/':
        if (!b) ERR("Error: divide by zero");
        else a /= b;
        break;

      case '*':
        a *= b;
        break;

      case '%':
        if (!b) ERR("Error: modula by zero");
        else a = a % b;
        break;

      default:
        ERR("Error: unexpected character '%c'", op);
     }
   return a;
}

/* float set of functionns */

static const char *
_deltaf(const char *s,
        double     *val,
        Eina_Bool  *err)
{
   if (*err || (!val)) return NULL;
   if (s[0] != '(')
     *err = EINA_TRUE;
   else
     {
        s = _alphaf(s + 1, val, err);
        s++;
     }
   return s;
}

static const char *
_funcf(const char *s,
       double     *val,
       Eina_Bool  *err)
{
   if (!strncmp(s, "floor(", 6))
     {
        s = _deltaf(s + 5, val, err);
        *val = floor(*val);
     }
   else if (!strncmp(s, "ceil(", 5))
     {
        s = _deltaf(s + 4, val, err);
        *val = ceil(*val);
     }
   else
     ERR("Error: unexpected character at %s\n", s);

   return s;
}

static const char *
_gammaf(const char *s,
        double     *val,
        Eina_Bool  *err)
{
   if (*err || (!val)) return NULL;

   if (_is_numf(s[0]))
     s = _get_numf(s, val, err);
   else if (s[0] == '(')
     s = _deltaf(s, val, err);
   else
     s = _funcf(s, val, err);

   return s;
}

static const char *
_betaf(const char *s,
       double     *val,
       Eina_Bool  *err)
{
   double a1 = 0, a2 = 0;
   char op;

   if (*err || (!val)) return NULL;
   s = _gammaf(s, &a1, err);
   while (_is_op1f(s[0]))
     {
        op = s[0];
        s++;
        s = _gammaf(s, &a2, err);
        a1 = _calcf(op, a1, a2);
     }
   (*val) = a1;
   return s;
}

static const char *
_alphaf(const char *s,
        double     *val,
        Eina_Bool  *err)
{
   double a1 = 0, a2 = 0;
   char op;

   if (*err || (!val)) return NULL;
   s = _betaf(s, &a1, err);
   while (_is_op2f(s[0]))
     {
        op = s[0];
        s++;
        s = _betaf(s, &a2, err);
        a1 = _calcf(op, a1, a2);
     }
   (*val) = a1;
   return s;
}

static const char *
_get_numf(const char *s,
          double     *val,
          Eina_Bool  *err)
{
   char *end;

   if (*err || (!val)) return s;

   errno = 0;
   *val = strtof(s, &end);
   if (errno) *err = EINA_TRUE;
   return end;
}

static Eina_Bool
_is_numf(char c)
{
   if (((c >= '0') && (c <= '9'))
       || ('-' == c)
       || ('.' == c)
       || ('+' == c))
     return EINA_TRUE;
   return EINA_FALSE;
}

static Eina_Bool
_is_op1f(char c)
{
   switch(c)
     {
      case '*':
      case '%':
      case '/':
        return EINA_TRUE;

      default:
        break;
     }
   return EINA_FALSE;
}

static Eina_Bool
_is_op2f(char c)
{
   switch(c)
     {
      case '+':
      case '-':
        return EINA_TRUE;

      default:
        break;
     }
   return EINA_FALSE;
}

static double
_calcf(char   op,
       double a,
       double b)
{
   switch(op)
     {
      case '+':
        a += b;
        break;

      case '-':
        a -= b;
        break;

      case '/':
        if (b) a /= b;
        else ERR("Error: divide by zero");
        break;

      case '*':
        a *= b;
        break;

      case '%':
        if (b) a = (double)((int)a % (int)b);
        else ERR("Error: modula by zero");
        break;

      default:
        ERR("Error: unexpected character '%c'\n", op);
     }
   return a;
}

/* handle errno more easily */
Eina_Bool
edje_parser_strtol(const char *text,
                   int        *i)
{
   const char *end;
   Eina_Bool err = EINA_FALSE;

   end = _alphai(text, i, &err);
   if (end && end[0]) err = EINA_TRUE;

   return !err;
}

Eina_Bool
edje_parser_strtod(const char *text,
                   double     *d)
{
   const char *end;
   Eina_Bool err = EINA_FALSE;

   end = _alphaf(text, d, &err);
   if (end && end[0]) err = EINA_TRUE;

   return !err;
}

Eina_Bool
edje_parser_strtobool(const char *text,
                      int        *i)
{
   if (!edje_parser_strtol(text, i) || ((*i != 1) && (*i != 0)))
     return EINA_FALSE;
   return EINA_TRUE;
}

