/*

Copyright (C) 2000, 2001 Christian Kreibich <cK@whoop.org>.

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to
deal in the Software without restriction, including without limitation the
rights to use, copy, modify, merge, publish, distribute, sublicense, and/or
sell copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in
all copies of the Software and its documentation and acknowledgment shall be
given in the documentation and software packages that this Software was
used.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL
THE AUTHORS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER
IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN
CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.

*/
#if HAVE_CONFIG_H
# include <config.h>
#endif
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>

#include <efsd_macros.h>
#include <efsd_debug.h>
#include <efsd_misc.h>
#include <efsd_list.h>
#include <efsd_stack.h>


struct efsd_stack
{
  EfsdList         *stack;
};


EfsdStack *
efsd_stack_new(void)
{
  EfsdStack *s;

  D_ENTER;

  s = NEW(EfsdStack);
  s->stack = NULL;

  D_RETURN_(s);
}


void       
efsd_stack_free(EfsdStack *s, EfsdFunc free_func)
{
  D_ENTER;

  if (!s)
    D_RETURN;

  efsd_list_free(s->stack, free_func);
  FREE(s);
  
  D_RETURN;
}


void       
efsd_stack_push(EfsdStack *s, void *data)
{
  D_ENTER;

  if (!s || !data)
    D_RETURN;

  s->stack = efsd_list_prepend(s->stack, data);

  D_RETURN;
}


void      *
efsd_stack_pop(EfsdStack *s)
{
  void *data = NULL;

  D_ENTER;

  if (!s || !s->stack)
    D_RETURN_(NULL);

  data = efsd_list_data(s->stack);
  s->stack = efsd_list_remove(s->stack, s->stack, NULL);

  D_RETURN_(data);
}


void      *
efsd_stack_top(EfsdStack *s)
{
  D_ENTER;

  if (!s || !s->stack)
    D_RETURN_(NULL);

  D_RETURN_(efsd_list_data(s->stack));
}
