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

/* Declaration for the spif_str_t classname variable. */
SPIF_DECL_CLASSNAME(str);
const size_t buff_inc = 4096;

spif_str_t
spif_str_new(void)
{
  spif_str_t self;

  self = SPIF_ALLOC(str);
  spif_str_init(self);
  return self;
}

spif_str_t
spif_str_new_from_ptr(spif_charptr_t old)
{
  spif_str_t self;

  self = SPIF_ALLOC(str);
  spif_str_init_from_ptr(self, old);
  return self;
}

spif_str_t
spif_str_new_from_buff(spif_charptr_t buff, size_t size)
{
  spif_str_t self;

  self = SPIF_ALLOC(str);
  spif_str_init_from_buff(self, buff, size);
  return self;
}

spif_str_t
spif_str_new_from_fp(FILE *fp)
{
  spif_str_t self;

  self = SPIF_ALLOC(str);
  spif_str_init_from_fp(self, fp);
  return self;
}

spif_str_t
spif_str_new_from_fd(int fd)
{
  spif_str_t self;

  self = SPIF_ALLOC(str);
  spif_str_init_from_fd(self, fd);
  return self;
}

spif_bool_t
spif_str_del(spif_str_t self)
{
  spif_str_done(self);
  SPIF_DEALLOC(self);
  return TRUE;
}

spif_bool_t
spif_str_init(spif_str_t self)
{
  spif_obj_init(SPIF_OBJ(self));
  spif_obj_set_classname(SPIF_OBJ(self), SPIF_CLASSNAME_TYPE(str));
  self->s = SPIF_NULL_TYPE(charptr);
  self->len = 0;
  self->mem = 0;
  return TRUE;
}

spif_bool_t
spif_str_init_from_ptr(spif_str_t self, spif_charptr_t old)
{
  self->len = strlen(old);
  self->mem = self->len + 1;
  self->s = SPIF_TYPE(charptr) MALLOC(self->mem);
  memcpy(self->s, old, self->mem);
  return TRUE;
}

spif_bool_t
spif_str_init_from_buff(spif_str_t self, spif_charptr_t buff, size_t size)
{
  self->mem = size;
  self->len = strnlen(buff, size);
  if (self->mem == self->len) {
    self->mem++;
  }
  self->s = SPIF_TYPE(charptr) MALLOC(self->mem);
  memcpy(self->s, buff, self->len);
  self->s[self->len] = 0;
  return TRUE;
}

spif_bool_t
spif_str_init_from_fp(spif_str_t self, FILE *fp)
{
  spif_charptr_t p, end = NULL;

  self->mem = buff_inc;
  self->len = 0;
  self->s = SPIF_TYPE(charptr) MALLOC(self->mem);

  for (p = self->s; fgets(p, buff_inc, fp); p += buff_inc) {
    if ((end = strchr(p, '\n')) == NULL) {
      self->mem += buff_inc;
      self->s = SPIF_TYPE(charptr) REALLOC(self->s, self->mem);
    } else {
      *end = 0;
      break;
    }
  }
  self->len = (size_t) ((end)
                        ? (end - self->s)
                        : ((int) strlen(self->s)));
  self->mem = self->len + 1;
  self->s = SPIF_TYPE(charptr) REALLOC(self->s, self->mem);
  return TRUE;
}

spif_bool_t
spif_str_init_from_fd(spif_str_t self, int fd)
{
  int n;
  spif_charptr_t p;

  self->mem = buff_inc;
  self->len = 0;
  self->s = SPIF_TYPE(charptr) MALLOC(self->mem);

  for (p = self->s; ((n = read(fd, p, buff_inc)) > 0) || (errno == EINTR); ) {
    self->mem += n;
    self->s = SPIF_TYPE(charptr) REALLOC(self->s, self->mem);
    p += n;
  }
  self->len = self->mem - buff_inc;
  self->mem = self->len + 1;
  self->s = SPIF_TYPE(charptr) REALLOC(self->s, self->mem);
  self->s[self->len] = 0;
  return TRUE;
}

spif_bool_t
spif_str_done(spif_str_t self)
{
  if (self->mem) {
    FREE(self->s);
    self->len = 0;
    self->mem = 0;
    self->s = SPIF_NULL_TYPE(charptr);
  }
  return TRUE;
}

spif_str_t
spif_str_dup(spif_str_t orig)
{
  spif_str_t self;

  self = SPIF_ALLOC(str);
  memcpy(self, orig, SPIF_SIZEOF_TYPE(str));
  self->s = STRDUP(SPIF_STR_STR(orig));
  self->len = orig->len;
  self->mem = orig->mem;
  return self;
}

int
spif_str_cmp(spif_str_t self, spif_str_t other)
{
  return (strcmp(SPIF_STR_STR(self), SPIF_STR_STR(other)));
}

int
spif_str_cmp_with_ptr(spif_str_t self, spif_charptr_t other)
{
  return (strcmp(SPIF_STR_STR(self), other));
}

int
spif_str_casecmp(spif_str_t self, spif_str_t other)
{
  return (strcasecmp(SPIF_STR_STR(self), SPIF_STR_STR(other)));
}

int
spif_str_casecmp_with_ptr(spif_str_t self, spif_charptr_t other)
{
  return (strcasecmp(SPIF_STR_STR(self), other));
}

int
spif_str_ncmp(spif_str_t self, spif_str_t other, size_t cnt)
{
  return (strncmp(SPIF_STR_STR(self), SPIF_STR_STR(other), cnt));
}

int
spif_str_ncmp_with_ptr(spif_str_t self, spif_charptr_t other, size_t cnt)
{
  return (strncmp(SPIF_STR_STR(self), other, cnt));
}

int
spif_str_ncasecmp(spif_str_t self, spif_str_t other, size_t cnt)
{
  return (strncasecmp(SPIF_STR_STR(self), SPIF_STR_STR(other), cnt));
}

int
spif_str_ncasecmp_with_ptr(spif_str_t self, spif_charptr_t other, size_t cnt)
{
  return (strncasecmp(SPIF_STR_STR(self), other, cnt));
}

size_t
spif_str_index(spif_str_t self, spif_char_t c)
{
  return (size_t) ((int) (index(SPIF_STR_STR(self), c)) - (int) (SPIF_STR_STR(self)));
}

size_t
spif_str_rindex(spif_str_t self, spif_char_t c)
{
  return (size_t) ((int) (rindex(SPIF_STR_STR(self), c)) - (int) (SPIF_STR_STR(self)));
}

size_t
spif_str_find(spif_str_t self, spif_str_t other)
{
  return (size_t) ((int) (strstr(SPIF_STR_STR(self), SPIF_STR_STR(other))) - (int) (SPIF_STR_STR(self)));
}

size_t
spif_str_find_from_ptr(spif_str_t self, spif_charptr_t other)
{
  return (size_t) ((int) (strstr(SPIF_STR_STR(self), other)) - (int) (SPIF_STR_STR(self)));
}

spif_str_t
spif_str_substr(spif_str_t self, spif_int32_t idx, spif_int32_t cnt)
{
  return spif_str_new_from_buff(SPIF_STR_STR(self) + ((idx < 0) ? (self->len) : (0)) + idx, cnt);
}

spif_charptr_t
spif_str_substr_to_ptr(spif_str_t self, spif_int32_t idx, spif_int32_t cnt)
{
  spif_charptr_t newstr;

  newstr = SPIF_TYPE(charptr) MALLOC(cnt + 1);
  memcpy(newstr, SPIF_STR_STR(self) + ((idx < 0) ? (self->len) : (0)) + idx, cnt);
  newstr[cnt] = 0;
  return newstr;
}

size_t
spif_str_to_num(spif_str_t self, int base)
{
  return (size_t) (strtoul(SPIF_STR_STR(self), (char **) NULL, base));
}

double
spif_str_to_float(spif_str_t self)
{
  return (double) (strtod(SPIF_STR_STR(self), (char **) NULL));
}

spif_bool_t
spif_str_append(spif_str_t self, spif_str_t other)
{
  self->mem += other->mem - 1;
  self->s = SPIF_TYPE(charptr) REALLOC(self->s, self->mem);
  memcpy(self->s + self->len, SPIF_STR_STR(other), other->len + 1);
  self->len += other->len;
  return TRUE;
}

spif_bool_t
spif_str_append_from_ptr(spif_str_t self, spif_charptr_t other)
{
  size_t len;

  len = strlen(other);
  self->mem += len;
  self->s = SPIF_TYPE(charptr) REALLOC(self->s, self->mem);
  memcpy(self->s + self->len, other, len + 1);
  self->len += len;
  return TRUE;
}

spif_bool_t
spif_str_trim(spif_str_t self)
{
  spif_charptr_t start, end;

  start = self->s;
  end = self->s + self->len - 1;
  for (; isspace((spif_uchar_t) (*start)) && (start < end); start++);
  for (; isspace((spif_uchar_t) (*end)) && (start < end); end--);
  if (start > end) {
    return spif_str_done(self);
  }
  *(++end) = 0;
  self->len = (size_t) (end - start);
  self->mem = self->len + 1;
  memmove(self->s, start, self->mem);
  self->s = SPIF_TYPE(charptr) REALLOC(self->s, self->mem);
  return TRUE;
}

spif_bool_t
spif_str_splice(spif_str_t self, size_t idx, size_t cnt, spif_str_t other)
{
  spif_charptr_t tmp, ptmp;
  size_t newsize;

  newsize = self->len + ((SPIF_OBJ_ISNULL(other)) ? (0) : (other->len)) - cnt + 1;
  ptmp = tmp = SPIF_TYPE(charptr) MALLOC(newsize);
  memcpy(tmp, self->s, idx);
  ptmp += idx;
  if (!SPIF_OBJ_ISNULL(other)) {
    memcpy(ptmp, other->s, other->len);
    ptmp += other->len;
  }
  memcpy(ptmp, self->s + idx + cnt, self->len - idx - cnt + 1);
  if (self->mem < newsize) {
    self->s = SPIF_TYPE(charptr) REALLOC(self->s, newsize);
    self->mem = newsize;
  }
  self->len = newsize - 1;
  memcpy(self->s, tmp, newsize);
  return TRUE;
}

spif_bool_t
spif_str_splice_from_ptr(spif_str_t self, size_t idx, size_t cnt, spif_charptr_t other)
{
  spif_charptr_t tmp, ptmp;
  size_t len, newsize;

  len = (other ? strlen(other) : 0);
  newsize = self->len + len - cnt + 1;
  ptmp = tmp = SPIF_TYPE(charptr) MALLOC(newsize);
  memcpy(tmp, self->s, idx);
  ptmp += idx;
  if (len) {
    memcpy(ptmp, other, len);
    ptmp += len;
  }
  memcpy(ptmp, self->s + idx + cnt, self->len - idx - cnt + 1);
  if (self->mem < newsize) {
    self->s = SPIF_TYPE(charptr) REALLOC(self->s, newsize);
    self->mem = newsize;
  }
  self->len = newsize - 1;
  memcpy(self->s, tmp, newsize);
  return TRUE;
}

spif_bool_t
spif_str_reverse(spif_str_t self)
{
  return ((strrev(self->s)) ? TRUE : FALSE);
}

size_t
spif_str_get_size(spif_str_t self)
{
  return self->mem;
}

spif_bool_t
spif_str_set_size(spif_str_t self, size_t size)
{
  if (size < self->mem) {
    return FALSE;
  } else if (size == self->mem) {
    return TRUE;
  }
  self->mem = size;
  if (self->s) {
    self->s = SPIF_TYPE(charptr) REALLOC(self->s, size);
  } else {
    self->s = SPIF_TYPE(charptr) MALLOC(size);
    *(self->s) = 0;
    self->len = 0;
  }
  return TRUE;
}

size_t
spif_str_get_len(spif_str_t self)
{
  return self->len;
}

spif_bool_t
spif_str_set_len(spif_str_t self, size_t len)
{
  /* Only a moron would use this function. */
  print_warning("spif_str_set_len() called.  Moron detected.\n");
#if DEBUG <= 4
  return FALSE;
#else
  self->len = len;
  return TRUE;
#endif
}

spif_bool_t
spif_str_show(spif_str_t self, spif_charptr_t name)
{
  printf("(spif_str_t) %s:  { \"%s\", len %lu, size %lu }\n", name, self->s,
         (unsigned long) self->len, (unsigned long) self->mem);
  return TRUE;
}
