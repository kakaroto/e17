/* ewd_value.h

Copyright (C) 2001 Christopher Rosendahl    <smugg@fatelabs.com>
                   Nathan Ingersoll         <ningerso@d.umn.edu>

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

#ifndef _EWD_VALUE_H
#define _EWD_VALUE_H

#define PRIME_TABLE_MAX 21
#define PRIME_MIN 17
#define PRIME_MAX 16777213

extern const unsigned int ewd_prime_table[];

typedef void (*Ewd_For_Each) (void *value);
#define EWD_FOR_EACH(function) ((Ewd_For_Each)function)

typedef void (*Ewd_Free_Cb) (void *data);
#define EWD_FREE_CB(func) ((Ewd_Free_Cb)func)

typedef unsigned int (*Ewd_Hash_Cb) (void *key);
#define EWD_HASH_CB(function) ((Ewd_Hash_Cb)function)

typedef int (*Ewd_Compare_Cb) (void *data1, void *data2);
#define EWD_COMPARE_CB(function) ((Ewd_Compare_Cb)function)

int ewd_direct_compare(void *key1, void *key2);
int ewd_str_compare(void *key1, void *key2);

unsigned int ewd_direct_hash(void *key);
unsigned int ewd_str_hash(void *key);

#endif				/* _EWD_VALUE_H */
