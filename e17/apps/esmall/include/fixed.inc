/* Fixed point arithmetic
 *
 * (c) Compyright 1998-1999, ITB CompuPhase
 * This file is provided as is (no warranties).
 */
#if defined _fixed_included
  #endinput
#endif
#define _fixed_included

enum fround_method {
  fround_round,
  fround_floor,
  fround_ceil
}

native fixed:fixed(value);
native fixed:fixedstr(string[]);
native fixed:fmul(fixed:oper1, fixed:oper2);
native fixed:fdiv(fixed:dividend, fixed:divisor);
native fixed:ffract(fixed:value);
native       fround(fixed:value, fround_method:method=fround_round);
