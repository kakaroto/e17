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

/* Declaration for the spif_tok_t classname variable. */
SPIF_DECL_CLASSNAME(tok);

spif_tok_t
spif_tok_new(void)
{
  spif_tok_t self;

  self = SPIF_ALLOC(tok);
  spif_tok_init(self);
  return self;
}

spif_tok_t
spif_tok_new_from_ptr(spif_charptr_t old)
{
  spif_tok_t self;

  self = SPIF_ALLOC(tok);
  spif_tok_init_from_ptr(self, old);
  return self;
}

spif_tok_t
spif_tok_new_from_fp(FILE *fp)
{
  spif_tok_t self;

  self = SPIF_ALLOC(tok);
  spif_tok_init_from_fp(self, fp);
  return self;
}

spif_tok_t
spif_tok_new_from_fd(int fd)
{
  spif_tok_t self;

  self = SPIF_ALLOC(tok);
  spif_tok_init_from_fd(self, fd);
  return self;
}

spif_bool_t
spif_tok_del(spif_tok_t self)
{
  spif_tok_done(self);
  SPIF_DEALLOC(self);
  return TRUE;
}

spif_bool_t
spif_tok_init(spif_tok_t self)
{
  spif_str_init(SPIF_STR(self));
  spif_obj_set_classname(SPIF_OBJ(self), SPIF_CLASSNAME_TYPE(tok));
  self->count = 0;
  self->token = ((spif_str_t *) (NULL));
  self->sep = SPIF_NULL_TYPE(str);
  return TRUE;
}

spif_bool_t
spif_tok_init_from_ptr(spif_tok_t self, spif_charptr_t old)
{
  spif_str_init_from_ptr(SPIF_STR(self), old);
  spif_obj_set_classname(SPIF_OBJ(self), SPIF_CLASSNAME_TYPE(tok));
  self->count = 0;
  self->token = ((spif_str_t *) (NULL));
  self->sep = SPIF_NULL_TYPE(str);
  return TRUE;
}

spif_bool_t
spif_tok_init_from_fp(spif_tok_t self, FILE *fp)
{
  spif_str_init_from_fp(SPIF_STR(self), fp);
  spif_obj_set_classname(SPIF_OBJ(self), SPIF_CLASSNAME_TYPE(tok));
  self->count = 0;
  self->token = ((spif_str_t *) (NULL));
  self->sep = SPIF_NULL_TYPE(str);
  return TRUE;
}

spif_bool_t
spif_tok_init_from_fd(spif_tok_t self, int fd)
{
  spif_str_init_from_fd(SPIF_STR(self), fd);
  spif_obj_set_classname(SPIF_OBJ(self), SPIF_CLASSNAME_TYPE(tok));
  self->count = 0;
  self->token = ((spif_str_t *) (NULL));
  self->sep = SPIF_NULL_TYPE(str);
  return TRUE;
}

spif_bool_t
spif_tok_done(spif_tok_t self)
{
  if (self->token) {
    size_t i;

    for (i = 0; i < self->count; i++) {
      spif_str_done(SPIF_STR(self->token[i]));
    }
    FREE(self->token);
    self->token = ((spif_str_t *) (NULL));
    self->count = 0;
  }
  if (!SPIF_OBJ_ISNULL(self->sep)) {
    spif_str_done(SPIF_STR(self->sep));
    self->sep = SPIF_NULL_TYPE(str);
  }
  spif_str_done(SPIF_STR(self));
  spif_str_init(SPIF_STR(self));
  return TRUE;
}

spif_bool_t
spif_tok_show(spif_tok_t self, spif_charptr_t name)
{
  return TRUE;
}
