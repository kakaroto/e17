/*  Small compiler - Recursive descend expresion parser
 *
 *  Copyright (c) ITB CompuPhase, 1997-1999
 *  This file may be freely used. No warranties of any kind.
 */
#include <assert.h>
#include <stdio.h>
#include <stdlib.h>		/* for _MAX_PATH */
#include <string.h>
#include "sc.h"
static int skim (int *opstr, void (*testfunc) (), int dropval, int endval,
		 int (*hier) (), value * lval);
static void dropout (int lvalue, void (*testfunc) (int val), int exit1,
		     value * lval);
static int plnge (int *opstr, int opoff, int (*hier) (value * lval),
		  value * lval);
static int plnge1 (int (*hier) (value * lval), value * lval);
static void plnge2 (void (*oper1) (void),
		    int (*hier) (value * lval), value * lval1, value * lval2);
static cell calc (cell left, void (*oper) (), cell right);
static int hier13 (value * lval);
static int hier12 (value * lval);
static int hier11 (value * lval);
static int hier10 (value * lval);
static int hier9 (value * lval);
static int hier8 (value * lval);
static int hier7 (value * lval);
static int hier6 (value * lval);
static int hier5 (value * lval);
static int hier4 (value * lval);
static int hier3 (value * lval);
static int hier2 (value * lval);
static int hier1 (value * lval1);
static int primary (value * lval);
static void callfunction (symbol * sym);
static int dbltest (void (*oper) (), value * lval1, value * lval2);
static int commutative (void (*oper) ());
static int constant (value * lval);
/* Function addresses of binary operators for signed operations */
static void (*op1[17]) () =
{
  os_mult, os_div, os_mod,	/* hier3, index 0 */
    ob_add, ob_sub,		/* hier4, index 3 */
    ob_sal, os_sar, ou_sar,	/* hier5, index 5 */
    os_le, os_ge, os_lt, os_gt,	/* hier6, index 8 */
    ob_eq, ob_ne,		/* hier7, index 12 */
    ob_and,			/* hier8, index 14 */
    ob_xor,			/* hier9, index 15 */
    ob_or			/* hier10, index 16 */
};
/*
 *  Searches for a binary operator a list of operators. The list is stored in
 *  the array "list". The last entry in the list should be set to 0.
 *
 *  The index of an operator in "list" (if found) is returned in "opidx". If
 *  no operator is found, nextop() returns 0.
 */
static int
nextop (int *opidx, int *list)
{
  *opidx = 0;
  while (*list)
    {
      if (matchtoken (*list))
	{
	  return 1;		/* found! */
	}
      else
	{
	  list += 1;
	  *opidx += 1;
	}			/* if */
    }				/* while */
  return 0;			/* entire list scanned, nothing found */
}
/*
 *  The 8086 family of processors has no direct support for logical (boolean)
 *  operations. These have to be done via comparing and jumping. Since we are
 *  already jumping through the code, we might as well implement an "early
 *  drop-out" evaluation (also called "short-circuit"). This conforms to
 *  standard C:
 *
 *  expr1 || expr2           expr2 will only be evaluated if expr1 is false.
 *  expr1 && expr2           expr2 will only be evaluated if expr1 is true.
 *
 *  expr1 || expr2 && expr3  expr2 will only be evaluated if expr1 is false
 *                           and expr3 will only be evaluated if expr1 is
 *                           false and expr2 is true.
 *
 *  Code generation for the last example proceeds thus:
 *
 *      evaluate expr1
 *      operator || found
 *      jump to "l1" if result of expr1 not equal to 0
 *      evaluate expr2
 *      ->  operator && found; skip to higher level in hierarchy diagram
 *          jump to "l2" if result of expr2 equal to 0
 *          evaluate expr3
 *          jump to "l2" if result of expr3 equal to 0
 *          set expression result to 1 (true)
 *          jump to "l3"
 *      l2: set expression result to 0 (false)
 *      l3:
 *      <-  drop back to previous hierarchy level
 *      jump to "l1" if result of expr2 && expr3 not equal to 0
 *      set expression result to 0 (false)
 *      jump to "l4"
 *  l1: set expression result to 1 (true)
 *  l4:
 *
 */
/*
 *  Skim over terms adjoining || and && operators
 */
static int
skim (int *opstr, void (*testfunc) (), int dropval, int endval,
      int (*hier) (), value * lval)
{
  int lvalue, hits, droplab, endlab, opidx;
  hits = 0;
  for (;;)
    {
      lvalue = plnge1 (hier, lval);	/* evaluate left expression */
      if (nextop (&opidx, opstr))
	{
	  if (hits == 0)
	    {
	      hits = 1;
	      droplab = getlabel ();
	    }			/* endif */
	  dropout (lvalue, testfunc, droplab, lval);
	}
      else if (hits)
	{			/* no (more) identical operators */
	  dropout (lvalue, testfunc, droplab, lval);	/* found at least one operator! */
	  const1 (endval);
	  jumplabel (endlab = getlabel ());
	  setlabel (droplab);
	  const1 (dropval);
	  setlabel (endlab);
	  lval->sym = NULL;
	  lval->ident = 0;
	  lval->constval = 0;
	  lval->tag = 0;
	  return 0;
	}
      else
	{
	  return lvalue;	/* none of the operators in "opstr" were found */
	}			/* endif */
    }				/* endwhile */
}
/*
 *  Reads into the primary register the variable pointed to by lval if
 *  plunging through the hierarchy levels detected an lvalue. Otherwise
 *  if a constant was detected, it is loaded. If there is no constant and
 *  no lvalue, the primary register must already contain the expression
 *  result.
 *
 *  After that, the compare routines "jmp_ne0" or "jmp_eq0" are called, which
 *  compare the primary register against 0, and jump to the "early drop-out"
 *  label "exit1" if the condition is true.
 */
static void
dropout (int lvalue, void (*testfunc) (int val), int exit1, value * lval)
{
  if (lvalue)
    rvalue (lval);
  else if (lval->ident == _constexpr)
    const1 (lval->constval);
  (*testfunc) (exit1);
}
static void
checkfunction (value * lval)
{
  symbol *sym = lval->sym;
  if (sym == NULL || lval->ident != _functn)
    return;			/* no known symbol, or not a function result */
  assert ((sym->usage & (_define | _refer)) != 0);	/* at least one should be set */
  if ((sym->usage & _define) != 0)
    {
      /* function is defined, can now check the return value */
      if ((sym->usage & _retvalue) == 0)
	error (209);		/* function should return a value */
    }
  else
    {
      /* function not yet defined, set */
      sym->usage |= _retvalue;	/* make sure that a future implementation of
				   * the function uses "return <value>;" */
    }				/* if */
}
/*
 *  Plunge to a lower level
 */
static int
plnge (int *opstr, int opoff, int (*hier) (value * lval), value * lval)
{
  int lvalue, opidx;
  value lval2;
  lvalue = plnge1 (hier, lval);
  if (nextop (&opidx, opstr) == 0)
    return lvalue;		/* no operator in "opstr" found */
  if (lvalue)
    rvalue (lval);
  do
    {
      opidx += opoff;
      plnge2 (op1[opidx], hier, lval, &lval2);
    }
  while (nextop (&opidx, opstr));	/* enddo */
  return 0;			/* result of expression is not an lvalue */
}
/*  plnge1
 *
 *  Unary plunge to lower level
 *  Called by: skim(), plnge(), plnge2 and hier14()
 *
 *  Global references: stgidx (reffered to only)
 */
static int
plnge1 (int (*hier) (value * lval), value * lval)
{
  int lvalue, index;
  cell cidx;
  stgget (&index, &cidx);	/* mark position in code generator */
  lvalue = (*hier) (lval);
  if (lval->ident == _constexpr)
    stgdel (index, cidx);	/* load constant later */
  return lvalue;
}
/*  plnge2
 *
 *  Binary plunge to lower level
 *  Called by: plnge(), hier14() and hier1()
 *
 *  Global references: stgidx (reffered to only)
 */
static void
plnge2 (void (*oper1) (void),
	int (*hier) (value * lval), value * lval1, value * lval2)
{
  int index;
  cell cidx;
  stgget (&index, &cidx);	/* mark position in code generator */
  if (lval1->ident == _constexpr)
    {				/* constant on left side; it is not yet loaded */
      if (plnge1 (hier, lval2))
	rvalue (lval2);
      const2 (lval1->constval << dbltest (oper1, lval2, lval1));
      /* ^ doubling of constants operating on integer addresses */
      /*   is restricted to "add" and "subtract" operators */
    }
  else
    {				/* non-constant on left side */
      push1 ();
      if (plnge1 (hier, lval2))
	rvalue (lval2);
      if (lval2->ident == _constexpr)
	{			/* constant on rigth side */
	  if (commutative (oper1))
	    {			/* test for commutative operators */
	      pop2 ();		/* undo previous "push1()" before "stgdel()" */
	      stgdel (index, cidx);	/* order to adjust global variable "csp" */
	      const2 (lval2->constval << dbltest (oper1, lval1, lval2));
	    }
	  else
	    {
	      const1 (lval2->constval << dbltest (oper1, lval1, lval2));
	      pop2 ();		/* pop result of left side into secundary register */
	    }			/* if */
	}
      else
	{			/* non-constants on both sides */
	  pop2 ();
	  if (dbltest (oper1, lval1, lval2))
	    cell2addr ();	/* double primary register */
	  if (dbltest (oper1, lval2, lval1))
	    cell2addr_alt ();	/* double secundary register */
	}			/* if */
    }				/* if */
  if (oper1)
    {
      /* If used in an expression, a function should return a value.
       * If the function has been defined, we can check this. If the
       * function was not defined, we can set this requirement (so that
       * a future function definition can check this bit.
       */
      checkfunction (lval1);
      checkfunction (lval2);
      if (lval1->ident == _array || lval1->ident == _refarray)
	error (33, lval1->sym->name);	/* array must be indexed */
      else if (lval2->ident == _array || lval2->ident == _refarray)
	error (33, lval2->sym->name);	/* array must be indexed */
      /* ??? ^^^ should do same kind of error checking with functions */
      if (lval1->tag != lval2->tag)
	error (213);
      if (lval1->ident == _constexpr && lval2->ident == _constexpr)
	{
	  /* only constant expression if both constant */
	  stgdel (index, cidx);	/* scratch generated code and calculate */
	  lval1->constval = calc (lval1->constval, oper1, lval2->constval);
	}
      else
	{
	  if (intest && (oper1 == ob_and || oper1 == ob_or))
	    error (212);	/* possibly unintended bitwise operation */
	  (*oper1) ();		/* do the (signed) operation */
	  lval1->ident = _expression;
	}			/* endif */
    }				/* if */
}
static cell
truemodulus (cell a, cell b)
{
  return (a % b + b) % b;
}
cell
calc (cell left, void (*oper) (), cell right)
{
  if (oper == ob_or)
    return (left | right);
  else if (oper == ob_xor)
    return (left ^ right);
  else if (oper == ob_and)
    return (left & right);
  else if (oper == ob_eq)
    return (left == right);
  else if (oper == ob_ne)
    return (left != right);
  else if (oper == os_le)
    return (left <= right);
  else if (oper == os_ge)
    return (left >= right);
  else if (oper == os_lt)
    return (left < right);
  else if (oper == os_gt)
    return (left > right);
  else if (oper == os_sar)
    return ((ucell) left >> (int) right);
  else if (oper == ob_sal)
    return ((ucell) left << (int) right);
  else if (oper == ob_add)
    return (left + right);
  else if (oper == ob_sub)
    return (left - right);
  else if (oper == os_mult)
    return (left * right);
  else if (oper == os_div)
    return (left - truemodulus (left, right)) / right;
  else if (oper == os_mod)
    return truemodulus (left, right);
  else
    error (29);			/* invalid expression, assumed 0 (this should never occur) */
  return 0;
}
int
expression (int *constant, cell * val, int *tag)
{
  value lval;
  if (hier14 (&lval))
    rvalue (&lval);
  if (lval.ident == _constexpr)
    {				/* constant expression */
      *constant = _yes;
      *val = lval.constval;
    }
  else
    {
      *constant = 0;
    }				/* if */
  if (tag != NULL)
    *tag = lval.tag;
  return lval.ident;
}
static cell
arraysize (symbol * sym)
{
  cell length;
  assert (sym != NULL);
  assert (sym->ident == _array || sym->ident == _refarray);
  length = sym->dim.array.length;
  if (sym->dim.array.level > 0)
    length += length * arraysize (finddepend (sym));
  return length;
}
/*  hier14
 *
 *  Lowest hierarchy level (except for the , operator).
 *
 *  Global references: intest   (reffered to only)
 */
int
hier14 (value * lval1)
{
  int lvalue;
  value lval2, lval3;
  void (*oper1) (void);
  int tok, level;
  cell val;
  char *st;
  lvalue = plnge1 (hier13, lval1);
  if (lval1->ident == _constexpr)	/* load constant here */
    const1 (lval1->constval);
  tok = lex (&val, &st);
  switch (tok)
    {
    case __aor:
      oper1 = ob_or;
      break;
    case __axor:
      oper1 = ob_xor;
      break;
    case __aand:
      oper1 = ob_and;
      break;
    case __aadd:
      oper1 = ob_add;
      break;
    case __asub:
      oper1 = ob_sub;
      break;
    case __amult:
      oper1 = os_mult;
      break;
    case __adiv:
      oper1 = os_div;
      break;
    case __amod:
      oper1 = os_mod;
      break;
    case __ashru:
      oper1 = ou_sar;
      break;
    case __ashr:
      oper1 = os_sar;
      break;
    case __ashl:
      oper1 = ob_sal;
      break;
    case '=':			/* simple assignment */
      oper1 = NULL;
      if (intest)
	error (211);		/* possibly unintended assignment */
      break;
    default:
      lexpush ();
      return lvalue;
    }				/* switch */
  /* if we get here, it was an assignment; first check a few special cases
   * and then the general */
  if (lval1->ident == _arraychar)
    {
      /* special case, assignment to packed character in a cell is permitted */
      lvalue = _yes;
    }
  else if (lval1->ident == _array || lval1->ident == _refarray)
    {
      /* array assignment is permitted too (with restrictions) */
      if (oper1)
	return error (23);	/* array assignment must be simple assigment */
      assert (lval1->sym != NULL);
      if (lval1->sym->dim.array.length == 0)
	return error (46, lval1->sym->name);	/* unknown array size */
      lvalue = _yes;
    }				/* if */
  if (lvalue == 0)		/* operand on left side of assignment must be lvalue */
    return error (22);		/* must be lvalue */
  lval3 = *lval1;		/* save symbol to enable storage of expresion result */
  if (lval1->ident == _arraycell || lval1->ident == _arraychar
      || lval1->ident == _array || lval1->ident == _refarray)
    {
      /* if indirect fetch: save PRI (cell address) */
      if (oper1)
	{
	  push1 ();
	  rvalue (lval1);
	}			/* if */
      plnge2 (oper1, hier14, lval1, &lval2);
      if (oper1)
	pop2 ();
    }
  else
    {
      if (oper1)
	{
	  rvalue (lval1);
	  plnge2 (oper1, hier14, lval1, &lval2);
	}
      else
	{
	  /* if direct fetch and simple assignment: no "push"
	   * and "pop" needed -> call hier14() directly, */
	  if (hier14 (&lval2))
	    rvalue (&lval2);	/* instead of plnge2(). */
	  checkfunction (&lval2);
	}			/* if */
    }				/* if */
  if (lval3.tag != 0 && lval3.tag != lval2.tag)
    error (213);		/* tagname mismatch */
  if (lval3.ident == _array || lval3.ident == _refarray)
    {
      /* left operand is an array, right operand should be an array variable
       * of the same size and the same dimension, an array literal (of the
       * same size) or a literal string.
       */
      int exactmatch = _yes;
      if (lval2.ident != _array && lval2.ident != _refarray)
	error (33, lval3.sym->name);	/* array must be indexed */
      if (lval2.sym != NULL)
	{
	  val = lval2.sym->dim.array.length;	/* array variable */
	  level = lval2.sym->dim.array.level;
	}
      else
	{
	  val = lval2.constval;	/* literal array */
	  level = 0;
	  /* If val is negative, it means that lval2 is a
	   * literal string. The string array size may be
	   * smaller than the destination array.
	   */
	  if (val < 0)
	    {
	      val = -val;
	      exactmatch = _no;
	    }			/* if */
	}			/* if */
      if (lval3.sym->dim.array.level != level)
	return error (48);	/* array dimensions must match */
      else if (lval3.sym->dim.array.length < val
	       || exactmatch && lval3.sym->dim.array.length > val)
	return error (47);	/* array sizes must match */
      if (level > 0)
	{
	  /* check the sizes of all sublevels too */
	  symbol *sym1 = lval3.sym;
	  symbol *sym2 = lval2.sym;
	  int i;
	  assert (sym1 != NULL && sym2 != NULL);
	  /* ^^^ sym2 must be valid, because only variables can be
	   *     multi-dimensional (there are no multi-dimensional arrays),
	   *     sym1 must be valid because it must be an lvalue
	   */
	  assert (exactmatch);
	  for (i = 0; i < level; i++)
	    {
	      sym1 = finddepend (sym1);
	      sym2 = finddepend (sym2);
	      assert (sym1 != NULL && sym2 != NULL);
	      /* ^^^ both arrays have the same dimensions (this was checked
	       *     earlier) so the dependend should always be found
	       */
	      if (sym1->dim.array.length != sym2->dim.array.length)
		error (47);	/* array sizes must match */
	    }			/* for */
	  // get the total size in cells of the multi-dimensional array
	  val = arraysize (lval3.sym);
	}			/* if */
    }
  else
    {
      /* left operand is not an array, right operand should then not be either */
      if (lval2.ident == _array || lval2.ident == _refarray)
	error (6);		/* must be assigned to an array */
    }				/* if */
  if (lval3.ident == _array || lval3.ident == _refarray)
    memcopy (val * sizeof (cell));
  else
    store (&lval3);		/* now, store the expression result */
  sideeffect = _yes;
  return 0;			/* expression result is never an lvalue */
}
static int
hier13 (value * lval)
{
  int lvalue, flab1, flab2;
  lvalue = plnge1 (hier12, lval);
  if (matchtoken ('?'))
    {
      flab1 = getlabel ();
      flab2 = getlabel ();
      if (lvalue)
	{
	  rvalue (lval);
	}
      else if (lval->ident == _constexpr)
	{
	  const1 (lval->constval);
	  error (lval->constval ? 206 : 205);	/* redundant test */
	}			/* if */
      jmp_eq0 (flab1);		/* go to second expression if primary register==0 */
      if (hier14 (lval))
	rvalue (lval);
      jumplabel (flab2);
      setlabel (flab1);
      needtoken (':');
      if (hier14 (lval))
	rvalue (lval);
      setlabel (flab2);
      return 0;			/* conditional expression is no lvalue */
    }
  else
    {
      return lvalue;
    }				/* endif */
}
/* the order of the operators in these lists is important and must cohere */
/* with the order of the operators in the array "op1" */
static int list3[] = { '*', '/', '%', 0 };
static int list4[] = { '+', '-', 0 };
static int list5[] = { __shl, __shr, __shru, 0 };
static int list6[] = { __lle, __lge, '<', '>', 0 };
static int list7[] = { __leq, __lne, 0 };
static int list8[] = { '&', 0 };
static int list9[] = { '^', 0 };
static int list10[] = { '|', 0 };
static int list11[] = { __land, 0 };
static int list12[] = { __lor, 0 };
static int
hier12 (value * lval)
{
  return skim (list12, jmp_ne0, 1, 0, hier11, lval);
}
static int
hier11 (value * lval)
{
  return skim (list11, jmp_eq0, 0, 1, hier10, lval);
}
static int
hier10 (value * lval)
{
  return plnge (list10, 16, hier9, lval);
}				/* ^ this variable is the starting index in the op1[]
				 *   array of the operators of this hierarchy level */
static int
hier9 (value * lval)
{
  return plnge (list9, 15, hier8, lval);
}
static int
hier8 (value * lval)
{
  return plnge (list8, 14, hier7, lval);
}
static int
hier7 (value * lval)
{
  return plnge (list7, 12, hier6, lval);
}
static int
hier6 (value * lval)
{
  return plnge (list6, 8, hier5, lval);
}
static int
hier5 (value * lval)
{
  return plnge (list5, 5, hier4, lval);
}
static int
hier4 (value * lval)
{
  return plnge (list4, 3, hier3, lval);
}
static int
hier3 (value * lval)
{
  return plnge (list3, 0, hier2, lval);
}
static int
hier2 (value * lval)
{
  int lvalue, tok;
  int tag, paranthese;
  cell val;
  char *st;
  symbol *sym;
  tok = lex (&val, &st);
  switch (tok)
    {
    case __inc:		/*  ++lval  */
      if (hier2 (lval) == 0)
	return error (22);	/* must be lvalue */
      inc (lval);		/* increase variable first */
      rvalue (lval);		/* and read the result into ax */
      sideeffect = _yes;
      return 0;			/* result is no longer lvalue */
    case __dec:		/*  --lval  */
      if (hier2 (lval) == 0)
	return error (22);	/* must be lvalue */
      dec (lval);		/* decrease variable first */
      rvalue (lval);		/* and read the result into ax */
      sideeffect = _yes;
      return 0;			/* result is no longer lvalue */
    case '~':			/*  ~ (one's complement)  */
      if (hier2 (lval))
	rvalue (lval);
      invert ();		/* bitwise NOT */
      lval->constval = ~lval->constval;
      return 0;
    case '!':			/*  ! (logical negate)  */
      if (hier2 (lval))
	rvalue (lval);
      lneg ();			/* 0 -> 1,  !0 -> 0 */
      lval->constval = !lval->constval;
      return 0;
    case '-':			/*  unary - (two's complement)  */
      if (hier2 (lval))
	rvalue (lval);
      neg ();			/* arithmic negation */
      lval->constval = -lval->constval;
      return 0;
    case __labelX:		/* tagname override */
      tag = sc_addtag (st);
      if (hier2 (lval))
	rvalue (lval);
      lval->tag = tag;
      return 0;
    case __definedX:
      paranthese = matchtoken ('(');
      tok = lex (&val, &st);
      if (tok != __symbolX)
	return error (20, st);	/* illegal symbol name */
      lval->ident = _constexpr;
      lval->constval = findconst (st) != NULL || findloc (st) != NULL
	|| findglb (st) != NULL;
      const1 (lval->constval);
      if (paranthese)
	needtoken (')');
      return 0;
    case __sizeofX:
      paranthese = matchtoken ('(');
      tok = lex (&val, &st);
      if (tok != __symbolX)
	return error (20, st);	/* illegal symbol name */
      sym = findloc (st);
      if (sym == NULL)
	sym = findglb (st);
      if (sym == NULL)
	return error (17, st);	/* undefined symbol */
      if (sym->ident == _constexpr)
	error (39);		/* constant symbol has no size */
      lval->ident = _constexpr;
      if (sym->ident == _array || sym->ident == _refarray)
	lval->constval = arraysize (sym);
      else
	lval->constval = 1;
      const1 (lval->constval);
      if (paranthese)
	needtoken (')');
      return 0;
    default:
      lexpush ();
      lvalue = hier1 (lval);
      tok = lex (&val, &st);
      switch (tok)
	{
	case __inc:		/*  lval++  */
	  if (lvalue == 0)
	    return error (22);	/* must be lvalue */
	  rvalue (lval);	/* read current value into ax */
	  inc (lval);		/* increase variable afterwards */
	  sideeffect = _yes;
	  return 0;		/* result is no longer lvalue */
	case __dec:		/*  lval--  */
	  if (lvalue == 0)
	    return error (22);	/* must be lvalue */
	  rvalue (lval);	/* read current value into ax */
	  dec (lval);		/* decrease variable afterwards */
	  sideeffect = _yes;
	  return 0;
	case __charX:		/*  char (compute required # of cells */
	  if (lval->ident == _constexpr)
	    {
	      lval->constval *= charbits / 8;	/* from char to bytes */
	      lval->constval =
		(lval->constval + sizeof (cell) - 1) / sizeof (cell);
	    }
	  else
	    {
	      if (lvalue)
		rvalue (lval);	/* fetch value if not already in PRI */
	      char2addr ();	/* from characters to bytes */
	      addconst (sizeof (cell) - 1);	/* make sure the value is rounded up */
	      addr2cell ();	/* truncate to number of cells */
	    }			/* if */
	  return 0;
	default:
	  lexpush ();
	  return lvalue;
	}			/* switch */
    }				/* switch */
}
/*  hier1
 *
 *  The highest hierarchy level: it looks for pointer and array indices
 *  and function calls.
 *  Generates code to fetch a pointer value if it is indexed and code to
 *  add to the pointer value or the array address (the address is already
 *  read at primary()). It also generates code to fetch a function address
 *  if that hasn't already been done at primary() (check lval[4]) and calls
 *  callfunction() to call the function.
 *
 *  Global references: stgidx  (reffered to only)
 */
static int
hier1 (value * lval1)
{
  int lvalue, index, tok, close;
  cell val, cidx;
  value lval2;
  char *st;
  symbol *sym;
  lvalue = primary (lval1);
restart:
  sym = lval1->sym;
  if (matchtoken ('[') || matchtoken ('{') || matchtoken ('('))
    {
      tok = tokeninfo (&val, &st);
      if (tok == '[' || tok == '{')
	{			/* subscript */
	  close = (tok == '[') ? ']' : '}';
	  if (sym == NULL)
	    {			/* sym==NULL if lval is a constant or a literal */
	      error (28);	/* cannot subscript */
	      needtoken (close);
	      return 0;
	    }
	  else if (sym->ident != _array && sym->ident != _refarray)
	    {
	      error (28);	/* cannot subscript, variable is not an array */
	      needtoken (close);
	      return 0;
	    }
	  else if (sym->dim.array.level > 0 && close != ']')
	    {
	      error (51);	/* invalid subscript, must use [ ] */
	      needtoken (close);
	      return 0;
	    }			/* if */
	  stgget (&index, &cidx);	/* mark position in code generator */
	  push1 ();		/* save base address of the array */
	  if (hier14 (&lval2))	/* create expression for the array index */
	    rvalue (&lval2);
	  if (lval2.ident == _array || lval2.ident == _refarray)
	    error (33, lval2.sym->name);	/* array must be indexed */
	  needtoken (close);
	  if (sym->x.idxtag != 0 && sym->x.idxtag != lval2.tag)
	    error (213);
	  if (lval2.ident == _constexpr)
	    {			/* constant expression */
	      stgdel (index, cidx);	/* scratch generated code */
	      if (close == ']')
		{
		  /* normal array index */
		  if (sym->dim.array.length != 0
		      && sym->dim.array.length <= lval2.constval)
		    error (32, sym->name);	/* array index out of bounds */
		  if (lval2.constval != 0)
		    {
		      /* don't add offsets for zero subscripts */
#if defined(BIT16)
		      const2 (lval2.constval << 1);
#else /*  */
		      const2 (lval2.constval << 2);
#endif /*  */
		      ob_add ();
		    }		/* if */
		}
	      else
		{
		  /* character index */
		  if (sym->dim.array.length != 0
		      && sym->dim.array.length * (32 / charbits) <=
		      lval2.constval)
		    error (32, sym->name);	/* array index out of bounds */
		  if (lval2.constval != 0)
		    {
		      /* don't add offsets for zero subscripts */
		      if (charbits == 16)
			const2 (lval2.constval << 1);	/* 16-bit character */
		      else
			const2 (lval2.constval);	/* 8-bit character */
		      ob_add ();
		    }		/* if */
		  charalign ();	/* align character index into array */
		}		/* if */
	    }
	  else
	    {
	      if (close == ']')
		{
		  if (sym->dim.array.length != 0)
		    ffbounds (sym->dim.array.length);	/* run time check for array bounds */
		  cell2addr ();	/* normal array index */
		}
	      else
		{
		  if (sym->dim.array.length != 0)
		    ffbounds (sym->dim.array.length * (32 / charbits));
		  char2addr ();	/* character array index */
		}		/* if */
	      pop2 ();
	      ob_add ();	/* base address was popped into secundary register */
	      if (close != ']')
		charalign ();	/* align character index into array */
	    }			/* if */
	  /* the indexed item may be another array (multi-dimensional arrays) */
	  assert (lval1->sym == sym && sym != NULL);	/* should still be set */
	  if (sym->dim.array.level > 0)
	    {
	      assert (close == ']');	/* checked earlier */
	      /* read the offset to the subarray and add it to the current address */
	      lval1->ident = _arraycell;
	      push1 ();		/* the optimizer makes this to a MOVE.alt */
	      rvalue (lval1);
	      pop2 ();
	      ob_add ();
	      /* adjust the "value" structure and find the referenced array */
	      lval1->ident = _refarray;
	      lval1->sym = finddepend (sym);
	      assert (lval1->sym != NULL);
	      assert (lval1->sym->dim.array.level ==
		      sym->dim.array.level - 1);
	      /* try to parse subsequent array indices */
	      lvalue = _no;	/* for now, a _refarray is no lvalue */
	      goto restart;
	    }			/* if */
	  assert (sym->dim.array.level == 0);
	  /* set type to fetch... INDIRECTLY */
	  lval1->ident = (close == ']') ? _arraycell : _arraychar;
	  lval1->tag = sym->tag;
	  /* a cell in an array is an lvalue, a character in an array is not
	   * always a *valid* lvalue */
	  return 1;
	}
      else
	{			/* tok=='(' -> function(...) */
	  if (sym == NULL
	      || (sym->ident != _functn && sym->ident != _reffunc))
	    {
	      return error (12);	/* invalid function call */
	    }
	  else
	    {
	      callfunction (sym);
	    }			/* if */
	  lval1->ident = _expression;
	  lval1->constval = 0;
	  lval1->tag = sym->tag;
	  return 0;		/* result of function call is no lvalue */
	}			/* if */
    }				/* if */
  if (sym != NULL && sym->ident == _functn)
    {
      address (sym);
      lval1->sym = NULL;
      lval1->ident = _reffunc;
      return 0;
    }				/* if */
  return lvalue;
}
/*  primary
 *
 *  Returns 1 if the operand is an lvalue (everything except arrays, functions
 *  constants and -of course- errors).
 *  Generates code to fetch the address of arrays. Code for constants is
 *  already generated by constant().
 *  This routine first clears the entire lval array (all fields are set to 0).
 *
 *  Global references: intest  (may be altered, but restored upon termination)
 */
static int
primary (value * lval)
{
  char *st;
  int lvalue, tok;
  cell val;
  symbol *sym;
  if (matchtoken ('('))
    {				/* sub-expression - (expression,...) */
      pushstk ((stkitem) intest);
      intest = 0;		/* no longer in "test" expression */
      do
	lvalue = hier14 (lval);
      while (matchtoken (','));
      needtoken (')');
      intest = (int) (long) popstk ();
      return lvalue;
    }				/* endif */
  lval->sym = NULL;		/* clear lval */
  lval->ident = 0;
  lval->constval = 0;
  lval->tag = 0;
  tok = lex (&val, &st);
  if (tok == __symbolX && !findconst (st))
    {
      /* first look for a local variable */
      if ((sym = findloc (st)) != 0)
	{
	  if (sym->ident == _label)
	    {
	      error (29);	/* expression error, assumed 0 */
	      const1 (0);	/* load 0 */
	      return 0;		/* return 0 for labels (expression error) */
	    }			/* if */
	  lval->sym = sym;
	  lval->ident = sym->ident;
	  lval->tag = sym->tag;
	  if (sym->ident == _array || sym->ident == _refarray)
	    {
	      address (sym);	/* get starting address in primary register */
	      return 0;		/* return 0 for array (not lvalue) */
	    }
	  else
	    {
	      return 1;		/* return 1 if lvalue (not label or array) */
	    }			/* if */
	}			/* if */
      /* now try a global variable */
      if ((sym = findglb (st)) != 0)
	{
	  if (sym->ident != _functn)
	    {
	      lval->sym = sym;
	      lval->tag = sym->tag;
	      if (sym->ident == _array)
		{
		  lval->ident = _arraycell;
		  address (sym);	/* get starting address in primary register */
		  return 0;	/* return 0 for array (not lvalue) */
		}
	      else
		{
		  lval->ident = sym->ident;
		  return 1;	/* return 1 if lvalue (not function or array) */
		}		/* if */
	    }			/* if */
	}
      else
	{
	  return error (17, st);	/* undefined symbol */
	}			/* endif */
      lval->sym = sym;
      lval->ident = sym->ident;
      lval->tag = sym->tag;
      return 0;			/* return 0 for function (not an lvalue) */
    }				/* if */
  lexpush ();			/* push the token, it is analyzed by constant() */
  if (constant (lval) == 0)
    {
      error (29);		/* expression error, assumed 0 */
      const1 (0);		/* load 0 */
    }				/* if */
  return 0;			/* return 0 for constants (or errors) */
}
static void
setdefarray (cell * string, cell size)
{
  const1 ((litidx + glb_declared) * sizeof (cell));
  while (size--)
    stowlit (*string++);
}
static int
findnamedarg (arginfo * arg, char *name)
{
  int i;
  for (i = 0; arg[i].ident != 0 && arg[i].ident != _varargs; i++)
    if (strcmp (arg[i].name, name) == 0)
      return i;
  return -1;
}
/*  callfunction
 *
 *  Generates code to call a function. This routine handles default arguments
 *  and positional as well as named parameters.
 */
static void
callfunction (symbol * sym)
{
  int close, lvalue;
  int argpos;			/* index in the output stream (argpos==nargs if positional parameters) */
  int argidx = 0;		/* index in "arginfo" list */
  int nargs = 0;		/* number of arguments */
  int heapalloc = 0;
  int namedparams = _no;
  value lval;
  arginfo *arg;
  char arglist[_maxargs];
  cell lexval;
  char *lexstr;
  assert (sym != NULL);
  arg = sym->dim.arglist;
  assert (arg != NULL);
  stgmark (_startreorder);
  for (argpos = 0; argpos < _maxargs; argpos++)
    arglist[argpos] = _no;
  if (!matchtoken (')'))
    {
      do
	{
	  if (matchtoken ('.'))
	    {
	      if (!namedparams && nargs > 0)
		error (44);	/* mixing named and positional parameters */
	      namedparams = _yes;
	      needtoken (__symbolX);
	      tokeninfo (&lexval, &lexstr);
	      argpos = findnamedarg (arg, lexstr);
	      if (argpos < 0)
		{
		  error (17, lexstr);	/* undefined symbol */
		  break;	/* exit loop, argpos is invalid */
		}		/* if */
	      needtoken ('=');
	      argidx = argpos;
	    }
	  else
	    {
	      if (namedparams)
		error (44);	/* mixing named and positional parameters */
	      argpos = nargs;
	    }			/* if */
	  stgmark (_exprstart + argpos);	/* mark beginning of new expression in stage */
	  arglist[argpos] = _yes;	/* flag argument as "present" */
	  if (matchtoken ('_'))
	    {
	      if (arg[argidx].ident == 0 || arg[argidx].ident == _varargs)
		error (202);	/* argument count mismatch */
	      else if (!arg[argidx].hasdefault)
		error (34, nargs + 1);	/* argument has no default value */
	      if (arg[argidx].ident == _refarray)
		{
		  setdefarray (arg[argidx].defvalue.array.data,
			       arg[argidx].defvalue.array.size);
		}
	      else if (arg[argidx].ident == _reference)
		{
		  setheap (arg[argidx].defvalue.val);
		  /* address of the value on the heap in PRI */
		  heapalloc++;
		}
	      else
		{
		  const1 (arg[argidx].defvalue.val);
		}		/* if */
	      if (arg[argidx].ident != 0 && arg[argidx].ident != _varargs)
		argidx++;
	    }
	  else
	    {
	      lvalue = hier14 (&lval);
	      switch (arg[argidx].ident)
		{
		case 0:
		  error (202);	/* argument count mismatch */
		  break;
		case _varargs:
		  /* always pass by reference */
		  if (lval.ident == _variable)
		    {
		      assert (lval.sym != NULL);
		      address (lval.sym);
		    }
		  else if (lval.ident == _constexpr
			   || lval.ident == _expression
			   || lval.ident == _arraychar)
		    {
		      /* fetch value if needed */
		      if (lval.ident == _arraychar)
			rvalue (&lval);
		      /* allocate a cell on the heap and store the
		       * value (already in PRI) there */
		      setheap_pri ();	/* address of the value on the heap in PRI */
		      heapalloc++;
		    }		/* if */
		  /* otherwise, the address is already in PRI */
		  if (lval.sym != NULL)
		    lval.sym->usage |= _written;
		  if (arg[argidx].tag != 0 && arg[argidx].tag != lval.tag)
		    error (213);
		  break;
		case _variable:
		  if (lval.ident == _label || lval.ident == _functn
		      || lval.ident == _reffunc || lval.ident == _array
		      || lval.ident == _refarray)
		    error (35, argidx);	/* argument type mismatch */
		  if (lvalue)
		    rvalue (&lval);	/* get value (direct or indirect) */
		  /* otherwise, the expression result is already in PRI */
		  if (arg[argidx].tag != 0 && arg[argidx].tag != lval.tag)
		    error (213);
		  argidx++;	/* argument done */
		  break;
		case _reference:
		  if (!lvalue	/* && lval.ident!=_array && lval.ident!=_refarray ??? */
		      || lval.ident == _arraychar)
		    error (35, argidx);	/* argument type mismatch */
#if 0				/* ??? */
		  /* Sorry, I do not remember the reasoning behind the feature to
		   * pass an array to a function that takes a reference argument,
		   * so I disabled it.
		   */
		  if (lval.ident == _array || lval1.ident == _refarray)
		    {
		      /* an single dimensioned array cell can be passed to a function
		       * expecting a reference to a cell */
		      assert (lval.sym != NULL);
		      if (lval.sym->dim.array.level != 1)
			error (35, argidx);	/* argument type mismatch */
		    }		/* if */
#endif /*  */
		  if (lval.ident == _variable)
		    {
		      assert (lval.sym != NULL);
		      address (lval.sym);
		    }		/* if */
		  /* otherwise, the address is already in PRI */
		  if (arg[argidx].tag != 0 && arg[argidx].tag != lval.tag)
		    error (213);
		  argidx++;	/* argument done */
		  if (lval.sym != NULL)
		    lval.sym->usage |= _written;
		  break;
		case _refarray:
		  if (lval.ident != _array && lval.ident != _refarray
		      && lval.ident != _arraycell)
		    error (35, argidx);	/* argument type mismatch */
		  /* in the case that lval.ident == _arraycell, verify that the
		   * dimension of the array in arg[argidx] is 1
		   */
		  if (lval.ident == _arraycell && arg[argidx].numdim != 1)
		    error (35, argidx);	/* argument type mismatch */
		  /* In the case that lval.ident == _array or _refarray, verify that
		   * the dimensions match with those in arg[argidx]. A literal
		   * array is always a single dimensions.
		   */
		  if (lval.ident == _array || lval.ident == _refarray)
		    {
		      if (lval.sym == NULL)
			{
			  if (arg[argidx].numdim != 1)
			    error (48);	/* array dimensions must match */
			  else if (arg[argidx].dim[0] != 0)
			    error (47);	/* array sizes must match */
			}
		      else
			{
			  symbol *sym = lval.sym;
			  int level = 0;
			  assert (sym != NULL);
			  if (sym->dim.array.level + 1 != arg[argidx].numdim)
			    error (48);	/* array dimensions must match */
			  /* the lengths for all dimensions must match, except for the
			   * last dimension
			   */
			  while (sym->dim.array.level > 0)
			    {
			      assert (level < _dimen_max);
			      if (sym->dim.array.length !=
				  arg[argidx].dim[level])
				error (47);	/* array sizes must match */
			      sym = finddepend (sym);
			      assert (sym != NULL);
			      level++;
			    }	/* if */
			  /* the last dimension is optionally checked */
			  assert (level < _dimen_max);
			  assert (sym != NULL);
			  if (arg[argidx].dim[level] != 0
			      && sym->dim.array.length !=
			      arg[argidx].dim[level])
			    error (47);	/* array sizes must match */
			}	/* if */
		    }		/* if */
		  /* address already in PRI */
		  if (arg[argidx].tag != 0 && arg[argidx].tag != lval.tag)
		    error (213);
		  argidx++;	/* argument done */
		  break;
		}		/* switch */
	    }			/* switch */
	  push1 ();		/* store the function argument on the stack */
	  nargs++;
	  close = matchtoken (')');
	  if (!close)		/* if not paranthese... */
	    needtoken (',');	/* ...should be comma */
	}
      while (!close);		/* do */
    }				/* if */
  /* check remaining function arguments (they may have default values) */
  for (argidx = 0; arg[argidx].ident != 0 && arg[argidx].ident != _varargs;
       argidx++)
    {
      if (arglist[argidx])
	continue;		/* already seen this argument */
      stgmark (_exprstart + argidx);	/* mark beginning of new expression in stage */
      if (!arg[argidx].hasdefault)
	error (202, argidx);	/* argument count mismatch */
      if (arg[argidx].ident == _refarray)
	{
	  setdefarray (arg[argidx].defvalue.array.data,
		       arg[argidx].defvalue.array.size);
	}
      else if (arg[argidx].ident == _reference)
	{
	  setheap (arg[argidx].defvalue.val);
	  heapalloc++;
	}
      else
	{
	  const1 (arg[argidx].defvalue.val);
	}			/* if */
      push1 ();			/* store the function argument on the stack */
      nargs++;
    }				/* while */
  stgmark (_endreorder);	/* mark end of reversed evaluation */
  pushval ((cell) nargs * sizeof (cell));
  ffcall (sym, nargs);
  sym->usage |= _refer;
  modheap (-heapalloc * sizeof (cell));
  sideeffect = _yes;		/* assume functions carry out a side-effect */
}
/*  dbltest
 *
 *  Returns a non-zero value if lval1 an array and lval2 is not an array and
 *  the operation is addition or subtraction.
 *
 *  Returns the "shift" count (1 for 16-bit, 2 for 32-bit) to allign a cell
 *  to an array offset.
 */
static int
dbltest (void (*oper) (), value * lval1, value * lval2)
{
  if ((oper != ob_add) && (oper != ob_sub))
    return 0;
  if (lval1->ident != _array)
    return 0;
  if (lval2->ident == _array)
    return 0;
  return sizeof (cell) / 2;	/* 1 for 16-bit, 2 for 32-bit */
}
/*  commutative
 *
 *  Test whether an operator is commutative, i.e. x oper y == y oper x.
 *  Commutative operators are: +  (addition)
 *                             *  (multiplication)
 *                             == (equality)
 *                             != (inequality)
 *                             &  (bitwise and)
 *                             ^  (bitwise xor)
 *                             |  (bitwise or)
 *
 *  If in an expression, code for the left operand has been generated and
 *  the right operand is a constant and the operator is commutative, the
 *  precautionary "push" of the primary register is scrapped and the constant
 *  is read into the secundary register immediately.
 */
static int
commutative (void (*oper) ())
{
  return oper == ob_add || oper == os_mult
    || oper == ob_eq || oper == ob_ne
    || oper == ob_and || oper == ob_xor || oper == ob_or;
}
/*  constant
 *
 *  Generates code to fetch a number, a literal character (which is returned
 *  by lex() as a number as well) or a literal string (lex() stores the
 *  strings in the literal queue). If the operand was a number, it is stored
 *  in lval->constval.
 *
 *  The function returns 1 if the token was a constant or a string, 0
 *  otherwise.
 */
static int
constant (value * lval)
{
  int tok, index, constant;
  cell val, item, cidx;
  char *st;
  symbol *sym;
  tok = lex (&val, &st);
  if (tok == __symbolX && (sym = findconst (st)) != 0)
    {
      lval->constval = sym->addr;
      const1 (lval->constval);
      lval->ident = _constexpr;
      lval->tag = sym->tag;
      sym->usage |= _read;
    }
  else if (tok == __numberX)
    {
      lval->constval = val;
      const1 (lval->constval);
      lval->ident = _constexpr;
    }
  else if (tok == __stringX)
    {
      /* lex() stores starting index of string in the literal table in 'val' */
      const1 ((val + glb_declared) * sizeof (cell));
      lval->ident = _array;	/* pretend this is a global array */
      lval->constval = val - litidx;	/* constval == the negative value of the
					   * size of the literal array; using a negative
					   * value distinguishes between literal arrays
					   * and literal values */
    }
  else if (tok == '{')
    {
      int tag, lasttag = -1;
      val = litidx;
      do
	{
	  /* cannot call constexpr() here, because "staging" is already turned
	   * on at this point */
	  assert (staging);
	  stgget (&index, &cidx);	/* mark position in code generator */
	  expression (&constant, &item, &tag);
	  stgdel (index, cidx);	/* scratch generated code */
	  if (constant == 0)
	    error (8);		/* must be constant expression */
	  if (lasttag < 0)
	    lasttag = tag;
	  else if (lasttag != tag)
	    error (213);	/* tagname mismatch */
	  stowlit (item);	/* store expression result in literal table */
	}
      while (matchtoken (','));
      needtoken ('}');
      const1 ((val + glb_declared) * sizeof (cell));
      lval->ident = _array;	/* pretend this is a global array */
      lval->constval = litidx - val;	/* constval == the size of the literal array */
    }
  else
    {
      return 0;			/* no, it cannot be interpreted as a constant */
    }				/* if */
  return 1;			/* yes, it was a constant value */
}
