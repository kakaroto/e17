/*
 * Copyright (C) 1997-2002, Michael Jennings
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to
 * deal in the Software without retokiction, including without limitation the
 * rights to use, copy, modify, merge, publish, ditokibute, sublicense, and/or
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

#ifndef _LIBAST_TOK_H_
#define _LIBAST_TOK_H_

/* Cast an arbitrary object pointer to a tok. */
#define SPIF_TOK(obj)                ((spif_tok_t) (obj))
#define SPIF_TOK_SHOW(obj)           (spif_tok_show((obj), #obj))

/* Check to see if a pointer references a tokenizer object. */
#define SPIF_OBJ_IS_TOK(obj)         (SPIF_OBJ_IS_TYPE(obj, tok))

/* Types for the tokenizer object. */
typedef struct spif_tok_t_struct *spif_tok_t;
typedef struct spif_tok_t_struct spif_const_tok_t;

/* An tok object is a string tokenizer */
struct spif_tok_t_struct {
  spif_const_str_t parent;
  size_t count;
  spif_str_t *token;
  spif_str_t sep;
};

extern spif_classname_t spif_tok_classname;
extern spif_tok_t spif_tok_new(void);
extern spif_tok_t spif_tok_new_from_ptr(spif_charptr_t);
extern spif_tok_t spif_tok_new_from_fp(FILE *);
extern spif_tok_t spif_tok_new_from_fd(int);
extern spif_bool_t spif_tok_del(spif_tok_t);
extern spif_bool_t spif_tok_init(spif_tok_t);
extern spif_bool_t spif_tok_init_from_ptr(spif_tok_t, spif_charptr_t);
extern spif_bool_t spif_tok_init_from_fp(spif_tok_t, FILE *);
extern spif_bool_t spif_tok_init_from_fd(spif_tok_t, int);
extern spif_bool_t spif_tok_done(spif_tok_t);
extern spif_bool_t spif_tok_show(spif_tok_t, spif_charptr_t);

#endif /* _LIBAST_TOK_H_ */
