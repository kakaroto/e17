/*
 * Copyright (C) 1997-2000, Michael Jennings
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

#ifndef HAVE_MEMMEM
/* Find first occurance of bytestring needle of size needlelen in memory region
   haystack of size haystacklen */
void *
memmem(const void *haystack, register size_t haystacklen, const void *needle, register size_t needlelen)
{
  register char *hs = (char *) haystack;
  register char *n = (char *) needle;
  register unsigned long i;
  register size_t len = haystacklen - needlelen;

  for (i = 0; i < len; i++) {
    if (!memcmp(hs + i, n, needlelen)) {
      return (hs + i);
    }
  }
  return (NULL);
}
#endif

#ifndef HAVE_USLEEP
void
usleep(unsigned long usec)
{
  struct timeval delay;

  delay.tv_sec = 0;
  delay.tv_usec = usec;
  select(0, NULL, NULL, NULL, &delay);

}

#endif

/***** Not needed ******
#ifndef HAVE_NANOSLEEP
__inline__ void
nanosleep(unsigned long nsec) {
    usleep(nsec / 1000);
}
#endif
************************/

/* Return the leftmost cnt characters of str */
char *
left_str(const char *str, unsigned long cnt)
{
  char *tmpstr;

  REQUIRE_RVAL(str != NULL, NULL);
  REQUIRE_RVAL(cnt <= strlen(str), NULL);
  REQUIRE_RVAL(cnt > 0, NULL);

  tmpstr = (char *) MALLOC(cnt + 1);
  strncpy(tmpstr, str, cnt);
  tmpstr[cnt] = 0;
  return (tmpstr);
}

/* Return cnt characters from str, starting at position index (from 0) */
char *
mid_str(const char *str, unsigned long index, unsigned long cnt)
{
  char *tmpstr;
  const char *pstr = str;
  size_t len;

  REQUIRE_RVAL(str != NULL, NULL);
  len = strlen(str);
  REQUIRE_RVAL(index < len, NULL);
  REQUIRE_RVAL(cnt <= len, NULL);
  REQUIRE_RVAL(cnt > 0, NULL);

  tmpstr = (char *) MALLOC(cnt + 1);
  pstr += index;
  strncpy(tmpstr, pstr, cnt);
  tmpstr[cnt] = 0;
  return (tmpstr);
}

/* Return the rightmost characters of str */
char *
right_str(const char *str, unsigned long cnt)
{
  char *tmpstr;
  const char *pstr = str;

  REQUIRE_RVAL(str != NULL, NULL);
  REQUIRE_RVAL(cnt <= strlen(str), NULL);
  REQUIRE_RVAL(cnt > 0, NULL);

  tmpstr = (char *) MALLOC(cnt + 1);
  pstr += strlen(str);
  pstr -= cnt;
  strcpy(tmpstr, pstr);
  return (tmpstr);
}

/* Returns TRUE if str matches regular expression pattern, FALSE otherwise */
#if defined(HAVE_REGEX_H)
unsigned char
regexp_match(register const char *str, register const char *pattern)
{
  static regex_t *rexp = NULL;
  register int result;
  char errbuf[256];

  if (!rexp) {
    rexp = (regex_t *) MALLOC(sizeof(regex_t));
  }

  REQUIRE_RVAL(str != NULL, FALSE);

  if (pattern) {
    if ((result = regcomp(rexp, pattern, REG_EXTENDED)) != 0) {
      regerror(result, rexp, errbuf, 256);
      print_error("Unable to compile regexp %s -- %s.\n", pattern, errbuf);
      return (FALSE);
    }
  }

  if (((result = regexec(rexp, str, (size_t) 0, (regmatch_t *) NULL, 0))
       != 0) && (result != REG_NOMATCH)) {
    regerror(result, rexp, errbuf, 256);
    print_error("Error testing input string %s -- %s.\n", str, errbuf);
    return (FALSE);
  }
  return (!result);
}

unsigned char
regexp_match_r(register const char *str, register const char *pattern, register regex_t **rexp)
{
  register int result;
  char errbuf[256];

  ASSERT_RVAL(rexp != NULL, FALSE);
  if (*rexp == NULL) {
    *rexp = (regex_t *) MALLOC(sizeof(regex_t));
  }

  if (pattern) {
    if ((result = regcomp(*rexp, pattern, REG_EXTENDED)) != 0) {
      regerror(result, *rexp, errbuf, 256);
      print_error("Unable to compile regexp %s -- %s.\n", pattern, errbuf);
      FREE(*rexp);
      return (FALSE);
    }
  }

  if (((result = regexec(*rexp, str, (size_t) 0, (regmatch_t *) NULL, 0))
       != 0) && (result != REG_NOMATCH)) {
    regerror(result, *rexp, errbuf, 256);
    print_error("Error testing input string %s -- %s.\n", str, errbuf);
    return (FALSE);
  }
  return (!result);
}
#endif

#define IS_DELIM(c)  ((delim != NULL) ? (strchr(delim, (c)) != NULL) : (isspace(c)))
#define IS_QUOTE(c)  (quote && quote == (c))

char **
split(const char *delim, const char *str)
{
  char **slist;
  register const char *pstr;
  register char *pdest;
  char quote = 0;
  unsigned short cnt = 0;
  unsigned long len;

  REQUIRE_RVAL(str != NULL, (char **) NULL);

  if ((slist = (char **) MALLOC(sizeof(char *))) == NULL) {
    print_error("split():  Unable to allocate memory -- %s\n", strerror(errno));
    return ((char **) NULL);
  }

  /* Before we do anything, skip leading "whitespace." */
  for (pstr = str; *pstr && IS_DELIM(*pstr); pstr++);

  /* The outermost for loop is where we traverse the string.  Each new
     word brings us back to the top where we resize our string list. */
  for (; *pstr; cnt++) {
    /* First, resize the list to two bigger than our count.  Why two?
       One for the string we're about to do, and one for a trailing NULL. */
    if ((slist = (char **) REALLOC(slist, sizeof(char *) * (cnt + 2))) == NULL) {
      print_error("split():  Unable to allocate memory -- %s\n", strerror(errno));
      return ((char **) NULL);
    }

    /* The string we're about to create can't possibly be larger than the remainder
       of the string we have yet to parse, so allocate that much space to start. */
    len = strlen(pstr) + 1;
    if ((slist[cnt] = (char *) MALLOC(len)) == NULL) {
      print_error("split():  Unable to allocate memory -- %s.\n", strerror(errno));
      return ((char **) NULL);
    }
    pdest = slist[cnt];

    /* This for loop is where we process each character. */
    for (; *pstr && (quote || !IS_DELIM(*pstr));) {
      if (*pstr == '\"' || *pstr == '\'') {
        /* It's a quote character, so set or reset the quote variable. */
        if (quote) {
          if (quote == *pstr) {
            quote = 0;
          } else {
            /* It's a single quote inside double quotes, or vice versa.  Leave it alone. */
            *pdest++ = *pstr++;
          }
        } else {
          quote = *pstr;
        }
        pstr++;
      } else {
        /* Handle any backslashes that are escaping delimiters or quotes. */
        if ((*pstr == '\\') && (IS_DELIM(*(pstr + 1)) || IS_QUOTE(*(pstr + 1)))) {
          /* Incrementing pstr here moves us past the backslash so that the line
             below will copy the next character to the new token, no questions asked. */
          pstr++;
        }
        *pdest++ = *pstr++;
      }
    }
    /* Add the trailing \0 to terminate the new string. */
    *pdest = 0;

    /* Reallocate the new string to be just the right size. */
    len = strlen(slist[cnt]) + 1;
    slist[cnt] = (char *) REALLOC(slist[cnt], len);

    /* Move past any trailing "whitespace." */
    for (; *pstr && IS_DELIM(*pstr); pstr++);
  }
  if (cnt == 0) {
    return NULL;
  } else {
    /* The last element of slist[] should be NULL. */
    slist[cnt] = 0;
    return slist;
  }
}

char **
split_regexp(const char *regexp, const char *str)
{

}

char *
join(const char *sep, char **slist)
{
  register unsigned long i;
  size_t len, slen;
  char *new_str;

  if (sep == NULL) {
    sep = "";
  }
  slen = strlen(sep);
  for (i = len = 0; slist[i]; i++) {
    len += strlen(slist[i]);
  }
  len += slen * (i - 1);
  new_str = (char *) MALLOC(len);
  strcpy(new_str, slist[0]);
  for (i = 1; slist[i]; i++) {
    if (slen) {
      strcat(new_str, sep);
    }
    strcat(new_str, slist[i]);
  }
  return new_str;
}

/* Return malloc'd pointer to index-th word in str.  "..." counts as 1 word. */
#undef IS_DELIM
#define IS_DELIM(c)  (delim ? ((c) == delim) : isspace(c))

char *
get_word(unsigned long index, const char *str)
{
  char *tmpstr;
  char delim = 0;
  register unsigned long i, j, k;

  k = strlen(str) + 1;
  if ((tmpstr = (char *) MALLOC(k)) == NULL) {
    print_error("get_word(%lu, %s):  Unable to allocate memory -- %s.\n",
	    index, str, strerror(errno));
    return ((char *) NULL);
  }
  *tmpstr = 0;
  for (i = 0, j = 0; j < index && str[i]; j++) {
    for (; isspace(str[i]); i++);
    switch (str[i]) {
      case '\"':
	delim = '\"';
	i++;
	break;
      case '\'':
	delim = '\'';
	i++;
	break;
      default:
	delim = 0;
    }
    for (k = 0; str[i] && !IS_DELIM(str[i]);) {
      if (str[i] == '\\') {
	if (str[i + 1] == '\'' || str[i + 1] == '\"') {
	  i++;
	}
      }
      tmpstr[k++] = str[i++];
    }
    switch (str[i]) {
      case '\"':
      case '\'':
	i++;
	break;
    }
    tmpstr[k] = 0;
  }

  if (j != index) {
    FREE(tmpstr);
    D_STRINGS(("get_word(%lu, %s) returning NULL.\n", index, str));
    return ((char *) NULL);
  } else {
    tmpstr = (char *) REALLOC(tmpstr, strlen(tmpstr) + 1);
    D_STRINGS(("get_word(%lu, %s) returning \"%s\".\n", index, str, tmpstr));
    return (tmpstr);
  }
}

/* Return pointer into str to index-th word in str.  "..." counts as 1 word. */
char *
get_pword(unsigned long index, const char *str)
{
  register const char *tmpstr = str;
  register unsigned long j;

  if (!str)
    return ((char *) NULL);
  for (; isspace(*tmpstr) && *tmpstr; tmpstr++);
  for (j = 1; j < index && *tmpstr; j++) {
    for (; !isspace(*tmpstr) && *tmpstr; tmpstr++);
    for (; isspace(*tmpstr) && *tmpstr; tmpstr++);
  }

  if (*tmpstr == '\"' || *tmpstr == '\'') {
    tmpstr++;
  }
  if (*tmpstr == '\0') {
    D_STRINGS(("get_pword(%lu, %s) returning NULL.\n", index, str));
    return ((char *) NULL);
  } else {
    D_STRINGS(("get_pword(%lu, %s) returning \"%s\"\n", index, str, tmpstr));
    return (char *) tmpstr;
  }
}

/* Returns the number of words in str, for use with get_word() and get_pword().  "..." counts as 1 word. */
unsigned long
num_words(const char *str)
{
  register unsigned long cnt = 0;
  char delim = 0;
  register unsigned long i;

  for (i = 0; str[i] && IS_DELIM(str[i]); i++);
  for (; str[i]; cnt++) {
    switch (str[i]) {
      case '\"':
	delim = '\"';
	i++;
	break;
      case '\'':
	delim = '\'';
	i++;
	break;
      default:
	delim = 0;
    }
    for (; str[i] && !IS_DELIM(str[i]); i++);
    switch (str[i]) {
      case '\"':
      case '\'':
	i++;
	break;
    }
    for (; str[i] && isspace(str[i]); i++);
  }

  D_STRINGS(("num_words() returning %lu\n", cnt));
  return (cnt);
}

/* chomp() removes leading and trailing whitespace from a string */
char *
chomp(char *s)
{

  register char *front, *back;

  ASSERT_RVAL(s != NULL, NULL);
  for (front = s; *front && isspace(*front); front++);
  for (back = s + strlen(s) - 1; *back && isspace(*back) && back > front; back--);

  *(++back) = 0;
  if (front != s) {
    memmove(s, front, back - front + 1);
  }
  return (s);
}

char *
strip_whitespace(register char *str)
{
  register unsigned long i, j;

  ASSERT_RVAL(str != NULL, NULL);
  if ((j = strlen(str))) {
    for (i = j - 1; isspace(*(str + i)); i--);
    str[j = i + 1] = 0;
    for (i = 0; isspace(*(str + i)); i++);
    j -= i;
    memmove(str, str + i, j + 1);
  }
  return (str);
}

char *
downcase_str(char *str)
{
  register char *tmp;

  for (tmp = str; *tmp; tmp++) {
    *tmp = tolower(*tmp);
  }
  D_STRINGS(("downcase_str() returning %s\n", str));
  return (str);
}

char *
upcase_str(char *str)
{
  register char *tmp;

  for (tmp = str; *tmp; tmp++) {
    *tmp = toupper(*tmp);
  }
  D_STRINGS(("upcase_str() returning %s\n", str));
  return (str);
}

#ifndef HAVE_STRCASESTR
char *
strcasestr(const char *haystack, register const char *needle)
{
  register const char *t;
  register size_t len = strlen(needle);

  for (t = haystack; t && *t; t++) {
    if (!strncasecmp(t, needle, len)) {
      return ((char *) t);
    }
  }
  return (NULL);
}
#endif

#ifndef HAVE_STRCASECHR
char *
strcasechr(const char *haystack, register const char needle)
{
  register const char *t;

  for (t = haystack; t && *t; t++) {
    if (tolower(*t) == tolower(needle)) {
      return ((char *) t);
    }
  }
  return (NULL);
}
#endif

#ifndef HAVE_STRCASEPBRK
char *
strcasepbrk(const char *haystack, register const char *needle)
{
  register const char *t;

  for (t = haystack; t && *t; t++) {
    if (strcasechr(needle, *t)) {
      return ((char *) t);
    }
  }
  return (NULL);
}
#endif

#ifndef HAVE_STRREV
char *
strrev(register char *str)
{
  register int i, j;

  i = strlen(str);
  for (j = 0, i--; i > j; i--, j++) {
    SWAP(str[j], str[i]);
  }
  return (str);

}
#endif

#if !(HAVE_STRSEP)
char *
strsep(char **str, register char *sep)
{

  register char *s = *str;
  char *sptr;

  D_STRINGS(("strsep(%s, %s) called.\n", *str, sep));
  sptr = s;
  for (; *s && !strchr(sep, *s); s++);
  if (!*s) {
    if (s != sptr) {
      *str = s;
      D_STRINGS(("Reached end of string with token \"%s\" in buffer\n", sptr));
      return (sptr);
    } else {
      D_STRINGS(("Reached end of string\n"));
      return ((char *) NULL);
    }
  }
  *s = 0;
  *str = s + 1;
  D_STRINGS(("Got token \"%s\", *str == \"%s\"\n", sptr, *str));
  return (sptr);
}
#endif

char *
garbage_collect(char *buff, size_t len)
{

  register char *tbuff = buff, *pbuff = buff;
  register unsigned long i, j;

  D_STRINGS(("Garbage collecting on %lu bytes at %10.8p\n", len, buff));
  for (i = 0, j = 0; j < len; j++)
    if (pbuff[j])
      tbuff[i++] = pbuff[j];
  tbuff[i++] = '\0';
  D_STRINGS(("Garbage collecting gives: \n%s\n", buff));
  return ((char *) REALLOC(buff, sizeof(char) * i));
}

char *
file_garbage_collect(char *buff, size_t len)
{

  register char *tbuff = buff, *pbuff = buff;
  char *tmp1, *tmp2;
  register unsigned long j;

  D_STRINGS(("File garbage collecting on %lu bytes at %10.8p\n", len, buff));
  for (j = 0; j < len;) {
    switch (pbuff[j]) {
      case '#':
	for (; !strchr("\r\n", pbuff[j]) && j < len; j++)
	  pbuff[j] = '\0';	/* First null out the line up to the CR and/or LF */
	for (; strchr("\r\n", pbuff[j]) && j < len; j++)
	  pbuff[j] = '\0';	/* Then null out the CR and/or LF */
	break;
      case '\r':
      case '\n':
      case '\f':
      case ' ':
      case '\t':
      case '\v':
	for (; isspace(pbuff[j]) && j < len; j++)
	  pbuff[j] = '\0';	/* Null out the whitespace */
	break;
      default:
	/* Find the end of this line and the occurence of the
	   next mid-line comment. */
	tmp1 = strpbrk(pbuff + j, "\r\n");
	tmp2 = strstr(pbuff + j, " #");

	/* If either is null, take the non-null one.  Otherwise,
	   take the lesser of the two. */
	if (!tmp1 || !tmp2) {
	  tbuff = ((tmp1) ? (tmp1) : (tmp2));
	} else {
	  tbuff = ((tmp1 < tmp2) ? (tmp1) : (tmp2));
	}

	/* Now let j catch up so that pbuff+j = tbuff; i.e., let
	   pbuff[j] refer to the same character that tbuff does */
	j += tbuff - (pbuff + j);

	/* Finally, change whatever is at pbuff[j] to a newline.
	   This will accomplish several things at once:
	   o It will change a \r to a \n if that's what's there
	   o If it's a \n, it'll stay the same.  No biggie.
	   o If it's a space, it will end the line there and the
	   next line will begin with a comment, which is handled
	   above. */
	if (j < len)
	  pbuff[j++] = '\n';

    }
  }

  /* Change all occurances of a backslash followed by a newline to nulls
     and null out all whitespace up to the next non-whitespace character.
     This handles support for breaking a string across multiple lines. */
  for (j = 0; j < len; j++) {
    if (pbuff[j] == '\\' && pbuff[j + 1] == '\n') {
      pbuff[j++] = '\0';
      for (; isspace(pbuff[j]) && j < len; j++)
	pbuff[j] = '\0';	/* Null out the whitespace */
    }
  }

  /* And the final step, garbage collect the buffer to condense all
     those nulls we just put in. */
  return (garbage_collect(buff, len));
}

char *
condense_whitespace(char *s)
{

  register unsigned char gotspc = 0;
  register char *pbuff = s, *pbuff2 = s;

  D_STRINGS(("condense_whitespace(%s) called.\n", s));
  for (; *pbuff2; pbuff2++) {
    if (isspace(*pbuff2)) {
      if (!gotspc) {
	*pbuff = ' ';
	gotspc = 1;
	pbuff++;
      }
    } else {
      *pbuff = *pbuff2;
      gotspc = 0;
      pbuff++;
    }
  }
  if ((pbuff >= s) && (isspace(*(pbuff - 1))))
    pbuff--;
  *pbuff = 0;
  D_STRINGS(("condense_whitespace() returning \"%s\".\n", s));
  return (REALLOC(s, strlen(s) + 1));
}

char *
safe_str(register char *str, unsigned short len)
{
  register unsigned short i;

  for (i = 0; i < len; i++) {
    if (iscntrl(str[i])) {
      str[i] = '.';
    }
  }

  return (str);
}

void
hex_dump(void *buff, register size_t count)
{

  register unsigned long j, k, l;
  register unsigned char *ptr;
  unsigned char buffr[9];

  print_error(" Address |  Size  | Offset  | 00 01 02 03 04 05 06 07 |  ASCII  \n");
  print_error("---------+--------+---------+-------------------------+---------\n");
  for (ptr = (unsigned char *) buff, j = 0; j < count; j += 8) {
    print_error(" %8p | %06lu | %07x | ", buff, (unsigned long) count, (unsigned int) j);
    l = ((count - j < 8) ? (count - j) : (8));
    memset(buffr, 0, 9);
    memcpy(buffr, ptr + j, l);
    for (k = 0; k < l; k++) {
      print_error("%02x ", buffr[k]);
    }
    for (; k < 8; k++) {
      print_error("   ");
    }
    print_error("| %-8s\n", safe_str((char *) buffr, l));
  }
}
