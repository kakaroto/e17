/*
 * Copyright (C) 1997-2002, Michael Jennings
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to
 * deal in the Software without restriction, including without limitation the
 * rights to use, copy, modify, merge, publish, distribute, sublicense, and/or
 * sell copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in
 * all copies of the Software, its documentation and marketing & publicity
 * materials, and acknowledgment shall be given in the documentation, materials
 * and software packages that this Software was used.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL
 * THE AUTHORS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER
 * IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN
 * CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
 */

static const char cvs_ident[] = "$Id$";

#ifdef HAVE_CONFIG_H
# include <config.h>
#endif

#include <libast_internal.h>

/*spif_classname_t spif_obj_classname = "spif_obj_t";*/
SPIF_DECL_CLASSNAME(obj);

spif_nullobj_t
spif_nullobj_new(void)
{
  /* DO NOT USE */
  return ((spif_nullobj_t) (NULL));
}

spif_bool_t
spif_nullobj_del(spif_nullobj_t self)
{
  /* DO NOT USE */
  USE_VAR(self);
  return TRUE;
}

spif_bool_t
spif_nullobj_init(spif_nullobj_t self)
{
  /* DO NOT USE */
  USE_VAR(self);
  return TRUE;
}

spif_bool_t
spif_nullobj_done(spif_nullobj_t self)
{
  /* DO NOT USE */
  USE_VAR(self);
  return TRUE;
}


spif_obj_t
spif_obj_new(void)
{
  spif_obj_t self;

  self = SPIF_ALLOC(obj);
  spif_obj_init(self);
  return self;
}

spif_bool_t
spif_obj_del(spif_obj_t self)
{
  spif_obj_done(self);
  SPIF_DEALLOC(self);
  return TRUE;
}

spif_bool_t
spif_obj_init(spif_obj_t self)
{
  spif_obj_set_classname(self, SPIF_CLASSNAME_TYPE(obj));
  return TRUE;
}

spif_bool_t
spif_obj_done(spif_obj_t self)
{
  USE_VAR(self);
  return TRUE;
}

spif_classname_t
spif_obj_get_classname(spif_obj_t self)
{
  return ((self) ? (self->classname) : ((spif_classname_t) SPIF_NULLSTR_TYPE(obj)));
}

spif_bool_t
spif_obj_set_classname(spif_obj_t self, spif_classname_t newname)
{
  if (SPIF_OBJ_ISNULL(self)) {
    return FALSE;
  }
  SPIF_OBJ_CLASSNAME(self) = newname;
  return TRUE;
}

spif_bool_t
spif_obj_show(spif_obj_t self, spif_charptr_t name)
{
  printf("%s:  (spif_obj_t) { \"%s\" }\n", name, self->classname);
  return TRUE;
}
