/* The Not Game
 *
 * Original concept and Java implementation by Rob Coffey.  Concept
 * and name used with permission.
 *
 * The Not Game for Gtk+, Copyright 1999, Michael Jennings
 *
 * This program is free software and is distributed under the terms of
 * the Artistic License.  Please see the file "Artistic" supplied with
 * this program for license terms.
 */

static const char cvs_ident[] = "$Id$";

#include "config.h"

#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include <string.h>
#include <fcntl.h>
#include <sys/time.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <ctype.h>
#include <unistd.h>
#include <errno.h>

#include "debug.h"
#include "conf.h"
#include "notgame.h"
#include "strings.h"

/*
 * This module is intended to be a drop-in configuration subsystem.  It requires
 * very little from the external program.  It requires PACKAGE and VERSION to have
 * valid string values at compile-time, and it requires calling conf_init_subsystem
 * before you use any other part of it.  That's pretty much it.  After that, all
 * you have to do is register a parser function for each context you want, and then
 * call conf_parse() with the config file name and an optional search path.  The
 * module does the rest.  Do note that, if used in a setuid program, you are
 * responsible for replacing system() with a secure version.  See Eterm for a good
 * solid sample system() replacement as well as example context parsers.
 */

fstate_t *fstate;
unsigned char fstate_idx;

static char *builtin_random(char *);
static char *builtin_version(char *);
static char *builtin_appname(char *);
static void *parse_null(char *, void *);

static ctx_t *context;
static ctx_state_t *ctx_state;
static eterm_func_t *builtins;
static unsigned char ctx_cnt, ctx_idx, ctx_state_idx, ctx_state_cnt, fstate_cnt, builtin_cnt, builtin_idx;

/* This function must be called before any other conf_*() function.
   Otherwise you will be bitten by dragons.  That's life. */
void
conf_init_subsystem(void) {

  /* Initialize the context list and establish a catch-all "null" context */
  ctx_cnt = 20;
  ctx_idx = 0;
  context = (ctx_t *) malloc(sizeof(ctx_t) * ctx_cnt);
  MEMSET(context, 0, sizeof(ctx_t) * ctx_cnt);
  context[0].name = "null";
  context[0].handler = parse_null;

  /* Initialize the context state stack and set the current context to "null" */
  ctx_state_cnt = 20;
  ctx_state_idx = 0;
  ctx_state = (ctx_state_t *) malloc(sizeof(ctx_state_t) * ctx_state_cnt);
  MEMSET(ctx_state, 0, sizeof(ctx_state_t) * ctx_state_cnt);

  /* Initialize the file state stack */
  fstate_cnt = 10;
  fstate_idx = 0;
  fstate = (fstate_t *) malloc(sizeof(fstate_t) * fstate_cnt);
  MEMSET(fstate, 0, sizeof(fstate_t) * fstate_cnt);

  /* Initialize the builtin function table */
  builtin_cnt = 10;
  builtin_idx = 0;
  builtins = (eterm_func_t *) malloc(sizeof(eterm_func_t) * builtin_cnt);
  MEMSET(builtins, 0, sizeof(eterm_func_t) * builtin_cnt);

  /* Register the omni-present builtin functions */
  conf_register_builtin("appname", builtin_appname);
  conf_register_builtin("version", builtin_version);
  conf_register_builtin("random", builtin_random);
}

/* Register a new config file context */
unsigned char
conf_register_context(char *name, ctx_handler_t handler) {

  if (++ctx_idx == ctx_cnt) {
    ctx_cnt *= 2;
    context = (ctx_t *) realloc(context, sizeof(ctx_t) * ctx_cnt);
  }
  context[ctx_idx].name = strdup(name);
  context[ctx_idx].handler = handler;
  D(("conf_register_context():  Added context \"%s\" with ID %d and handler 0x%08x\n",
     context[ctx_idx].name, ctx_idx, context[ctx_idx].handler));
  return (ctx_idx);
}

/* Register a new file state structure */
unsigned char
conf_register_fstate(FILE *fp, char *path, char *outfile, unsigned long line, unsigned char flags) {

  if (++fstate_idx == fstate_cnt) {
    fstate_cnt *= 2;
    fstate = (fstate_t *) realloc(fstate, sizeof(fstate_t) * fstate_cnt);
  }
  fstate[fstate_idx].fp = fp;
  fstate[fstate_idx].path = path;
  fstate[fstate_idx].outfile = outfile;
  fstate[fstate_idx].line = line;
  fstate[fstate_idx].flags = flags;
  return (fstate_idx);
}

/* Register a new builtin function */
unsigned char
conf_register_builtin(char *name, eterm_func_ptr_t ptr) {

  builtins[builtin_idx].name = strdup(name);
  builtins[builtin_idx].ptr = ptr;
  if (++builtin_idx == builtin_cnt) {
    builtin_cnt *= 2;
    builtins = (eterm_func_t *) realloc(builtins, sizeof(eterm_func_t) * builtin_cnt);
  }
  return (builtin_idx - 1);
}

/* Register a new config file context */
unsigned char
conf_register_context_state(unsigned char ctx_id) {

  if (++ctx_state_idx == ctx_state_cnt) {
    ctx_state_cnt *= 2;
    ctx_state = (ctx_state_t *) realloc(ctx_state, sizeof(ctx_state_t) * ctx_state_cnt);
  }
  ctx_state[ctx_state_idx].ctx_id = ctx_id;
  ctx_state[ctx_state_idx].state = NULL;
  return (ctx_state_idx);
}

/************ Builtin functions ************/
static char *
builtin_random(char *param) {

  unsigned long n, index;
  static unsigned int rseed = 0;

  if (rseed == 0) {
    rseed = (unsigned int) (getpid() * time(NULL) % ((unsigned int) -1));
    srand(rseed);
  }
  n = NumWords(param);
  index = (int) (n * ((float) rand()) / (RAND_MAX + 1.0)) + 1;

  return (Word(index, param));
}
static char *
builtin_version(char *param) {return (strdup(VERSION));}
static char *
builtin_appname(char *param) {return (strdup(PACKAGE "-" VERSION));}

/************ Parser ************/
/* chomp() removes leading and trailing whitespace/quotes from a string */
char *
chomp(char *s)
{

  register char *front, *back;

  for (front = s; *front && isspace(*front); front++);
  for (back = s + strlen(s) - 1; *back && isspace(*back) && back > front; back--);

  *(++back) = 0;
  if (front != s) {
    memmove(s, front, back - front + 1);
  }
  return (s);
}

/* shell_expand() takes care of shell variable expansion,
   quote conventions, calling of built-in functions, etc. */
char *
shell_expand(char *s)
{

  register char *tmp;
  register char *pbuff = s, *tmp1;
  register unsigned long j, k, l = 0;
  char new[CONFIG_BUFF];
  unsigned char eval_escape = 1, eval_var = 1, eval_exec = 1, eval_func = 1, in_single = 0, in_double = 0;
  unsigned long fsize, cnt1 = 0, cnt2 = 0;
  const unsigned long max = CONFIG_BUFF - 1;
  char *Command, *Output, *EnvVar, *OutFile;
  FILE *fp;

  ASSERT_RVAL(s != NULL, (char *) NULL);

#if 0
  new = (char *) malloc(CONFIG_BUFF);
#endif

  for (j = 0; *pbuff && j < max; pbuff++, j++) {
    switch (*pbuff) {
      case '~':
	D(("Tilde detected.\n"));
	if (eval_var) {
	  strncpy(new + j, getenv("HOME"), max - j);
	  cnt1 = strlen(getenv("HOME")) - 1;
          cnt2 = max - j - 1;
          j += MIN(cnt1, cnt2);
	} else {
	  new[j] = *pbuff;
	}
	break;
      case '\\':
	D(("Escape sequence detected.\n"));
	if (eval_escape || (in_single && *(pbuff + 1) == '\'')) {
	  switch (tolower(*(++pbuff))) {
	    case 'n':
	      new[j] = '\n';
	      break;
	    case 'r':
	      new[j] = '\r';
	      break;
	    case 't':
	      new[j] = '\t';
	      break;
	    case 'b':
	      j -= 2;
	      break;
	    case 'f':
	      new[j] = '\f';
	      break;
	    case 'a':
	      new[j] = '\a';
	      break;
	    case 'v':
	      new[j] = '\v';
	      break;
	    case 'e':
	      new[j] = '\033';
	      break;
	    default:
	      new[j] = *pbuff;
	      break;
	  }
	} else {
	  new[j++] = *(pbuff++);
	  new[j] = *pbuff;
	}
	break;
      case '%':
	D(("%% detected.\n"));
	for (k = 0, pbuff++; builtins[k].name != NULL; k++) {
	  D(("Checking for function %%%s, pbuff == \"%s\"\n", builtins[k].name, pbuff));
	  l = strlen(builtins[k].name);
	  if (!strncasecmp(builtins[k].name, pbuff, l) &&
	      ((pbuff[l] == '(') || (pbuff[l] == ' ' && pbuff[l + 1] == ')'))) {
	    break;
	  }
	}
	if (builtins[k].name == NULL) {
	  new[j] = *pbuff;
	} else {
	  D(("Call to built-in function %s detected.\n", builtins[k].name));
	  Command = (char *) malloc(CONFIG_BUFF);
	  pbuff += l;
	  if (*pbuff != '(')
	    pbuff++;
	  for (tmp1 = Command, pbuff++, l = 1; l && *pbuff; pbuff++, tmp1++) {
	    switch (*pbuff) {
	      case '(':
		l++;
		*tmp1 = *pbuff;
		break;
	      case ')':
		l--;
	      default:
		*tmp1 = *pbuff;
		break;
	    }
	  }
	  *(--tmp1) = 0;
	  if (l) {
	    print_error("parse error in file %s, line %lu:  Mismatched parentheses",
			file_peek_path(), file_peek_line());
	    return ((char *) NULL);
	  }
	  Command = shell_expand(Command);
	  Output = (builtins[k].ptr) (Command);
	  free(Command);
	  if (Output && *Output) {
	    l = strlen(Output) - 1;
	    strncpy(new + j, Output, max - j);
            cnt2 = max - j - 1;
	    j += MIN(l, cnt2);
	    free(Output);
	  } else {
	    j--;
	  }
	  pbuff--;
	}
	break;
      case '`':
	D(("Backquotes detected.  Evaluating expression.\n"));
	if (eval_exec) {
	  Command = (char *) malloc(CONFIG_BUFF);
	  l = 0;
	  for (pbuff++; *pbuff && *pbuff != '`' && l < max; pbuff++, l++) {
	    switch (*pbuff) {
	      case '$':
		D(("Environment variable detected.  Evaluating.\n"));
		EnvVar = (char *) malloc(128);
		switch (*(++pbuff)) {
		  case '{':
		    for (pbuff++, k = 0; *pbuff != '}' && k < 127; k++, pbuff++)
		      EnvVar[k] = *pbuff;
		    break;
		  case '(':
		    for (pbuff++, k = 0; *pbuff != ')' && k < 127; k++, pbuff++)
		      EnvVar[k] = *pbuff;
		    break;
		  default:
		    for (k = 0; (isalnum(*pbuff) || *pbuff == '_') && k < 127; k++, pbuff++)
		      EnvVar[k] = *pbuff;
		    break;
		}
		EnvVar[k] = 0;
		if ((tmp = getenv(EnvVar))) {
		  strncpy(Command + l, tmp, max - l);
                  cnt1 = strlen(tmp) - 1;
                  cnt2 = max - l - 1;
                  l += MIN(cnt1, cnt2);
		}
		pbuff--;
		break;
	      default:
		Command[l] = *pbuff;
	    }
	  }
          ASSERT(l < CONFIG_BUFF);
	  Command[l] = 0;
	  OutFile = tmpnam(NULL);
	  if (l + strlen(OutFile) + 8 > CONFIG_BUFF) {
	    print_error("Parse error in file %s, line %lu:  Cannot execute command, line too long",
			file_peek_path(), file_peek_line());
	    return ((char *) NULL);
	  }
	  strcat(Command, " >");
	  strcat(Command, OutFile);
	  system(Command);
	  if ((fp = fopen(OutFile, "rb")) != NULL) {
	    fseek(fp, 0, SEEK_END);
	    fsize = ftell(fp);
	    rewind(fp);
	    if (fsize) {
	      Output = (char *) malloc(fsize + 1);
	      fread(Output, fsize, 1, fp);
	      Output[fsize] = 0;
	      fclose(fp);
	      remove(OutFile);
	      Output = CondenseWhitespace(Output);
	      strncpy(new + j, Output, max - j);
              cnt1 = strlen(Output) - 1;
              cnt2 = max - j - 1;
	      j += MIN(cnt1, cnt2);
	      free(Output);
	    } else {
	      print_warning("Command at line %lu of file %s returned no output.", file_peek_line(), file_peek_path());
	    }
	  } else {
	    print_warning("Output file %s could not be created.  (line %lu of file %s)", NONULL(OutFile),
			  file_peek_line(), file_peek_path());
	  }
	  free(Command);
	} else {
	  new[j] = *pbuff;
	}
	break;
      case '$':
	D(("Environment variable detected.  Evaluating.\n"));
	if (eval_var) {
	  EnvVar = (char *) malloc(128);
	  switch (*(++pbuff)) {
	    case '{':
	      for (pbuff++, k = 0; *pbuff != '}' && k < 127; k++, pbuff++)
		EnvVar[k] = *pbuff;
	      break;
	    case '(':
	      for (pbuff++, k = 0; *pbuff != ')' && k < 127; k++, pbuff++)
		EnvVar[k] = *pbuff;
	      break;
	    default:
	      for (k = 0; (isalnum(*pbuff) || *pbuff == '_') && k < 127; k++, pbuff++)
		EnvVar[k] = *pbuff;
	      break;
	  }
	  EnvVar[k] = 0;
	  if ((tmp = getenv(EnvVar))) {
	    strncpy(new, tmp, max - j);
            cnt1 = strlen(tmp) - 1;
            cnt2 = max - j - 1;
	    j += MIN(cnt1, cnt2);
	  }
	  pbuff--;
	} else {
	  new[j] = *pbuff;
	}
	break;
      case '\"':
	D(("Double quotes detected.\n"));
	if (!in_single) {
	  if (in_double) {
	    in_double = 0;
	  } else {
	    in_double = 1;
	  }
	}
	new[j] = *pbuff;
	break;

      case '\'':
	D(("Single quotes detected.\n"));
	if (in_single) {
	  eval_var = 1;
	  eval_exec = 1;
	  eval_func = 1;
	  eval_escape = 1;
	  in_single = 0;
	} else {
	  eval_var = 0;
	  eval_exec = 0;
	  eval_func = 0;
	  eval_escape = 0;
	  in_single = 1;
	}
	new[j] = *pbuff;
	break;

      default:
	new[j] = *pbuff;
    }
  }
  ASSERT(j < CONFIG_BUFF);
  new[j] = 0;

  D(("shell_expand(%s) returning \"%s\"\n", s, new));
  D((" strlen(s) == %lu, strlen(new) == %lu, j == %lu\n", strlen(s), strlen(new), j));

  strcpy(s, new);
#if 0
  free(new);
#endif
  return (s);
}

char *
conf_find_file(const char *file, const char *pathlist) {

  static char name[PATH_MAX];
  const char *path;
  char *p;
  short maxpathlen;
  unsigned short len;
  struct stat fst;

  REQUIRE_RVAL(file != NULL, NULL);
  REQUIRE_RVAL(pathlist != NULL, NULL);

  getcwd(name, PATH_MAX);
  D(("conf_find_file(\"%s\", \"%s\") called from directory \"%s\".\n", file, pathlist, name));

  strcpy(name, file);
  len = strlen(name);
  D(("conf_find_file():  Checking for file \"%s\"\n", name));
  if ((!access(name, R_OK)) && (!stat(name, &fst)) && (!S_ISDIR(fst.st_mode))) {
    return ((char *) name);
  }

  /* maxpathlen is the longest possible path we can stuff into name[].  The - 2 saves room for
     an additional / and the trailing null. */
  if ((maxpathlen = sizeof(name) - len - 2) <= 0) {
    return ((char *) NULL);
  }

  for (path = pathlist; path != NULL && *path != '\0'; path = p) {
    short n;

    /* Calculate the length of the next directory in the path */
    if ((p = strchr(path, ':')) != NULL) {
      n = p++ - path;
    } else {
      n = strlen(path);
    }

    /* Don't try if it's too long */
    if (n > 0 && n <= maxpathlen) {
      /* Compose the /path/file combo */
      strncpy(name, path, n);
      if (name[n - 1] != '/') {
	name[n++] = '/';
      }
      name[n] = '\0';
      strcat(name, file);

      D(("conf_find_file():  Checking for file \"%s\"\n", name));
      if ((!access(name, R_OK)) && (!stat(name, &fst)) && (!S_ISDIR(fst.st_mode))) {
        return ((char *) name);
      }
    }
  }
  D(("conf_find_file():  File \"%s\" not found in path.\n", file));
  return ((char *) NULL);
}

FILE *
open_config_file(char *name)
{

  FILE *fp;
  int ver;
  char buff[256], *begin_ptr, *end_ptr;

  ASSERT(name != NULL);

  fp = fopen(name, "rt");
  if (fp != NULL) {
    fgets(buff, 256, fp);
    if (BEG_STRCASECMP(buff, "<" PACKAGE "-")) {
      fclose(fp);
      fp = NULL;
    } else {
      begin_ptr = strchr(buff, '-') + 1;
      if ((end_ptr = strchr(buff, '>')) != NULL) {
	*end_ptr = 0;
      }
      if ((ver = BEG_STRCASECMP(begin_ptr, VERSION)) > 0) {
	print_warning("Config file is designed for a newer version of " PACKAGE);
      }
    }
  }
  return (fp);
}

void
conf_parse(char *conf_name, const char *path) {

  FILE *fp;
  char *name, *outfile, *p;
  char buff[CONFIG_BUFF], orig_dir[PATH_MAX];
  register unsigned long i = 0;
  unsigned char id = 0;

  REQUIRE(conf_name != NULL);

  *orig_dir = 0;
  if (path) {
    if ((name = conf_find_file(conf_name, path)) != NULL) {
      if ((p = strrchr(name, '/')) != NULL) {
        getcwd(orig_dir, PATH_MAX);
        *p = 0;
        chdir(name);
        *p = '/';
      }
    } else {
      print_error("Unable to find/open config file %s in path \"%s\".  Continuing with defaults.", conf_name, path);
      return;
    }
  }
  if ((fp = open_config_file(conf_name)) == NULL) {
    print_error("Unable to find/open config file %s.  Continuing with defaults.", conf_name);
    return;
  }
  file_push(fp, conf_name, NULL, 1, 0);  /* Line count starts at 1 because open_config_file() parses the first line */

  for (; fstate_idx > 0;) {
    for (; fgets(buff, CONFIG_BUFF, file_peek_fp());) {
      file_inc_line();
      if (!strchr(buff, '\n')) {
	print_error("Parse error in file %s, line %lu:  line too long", file_peek_path(), file_peek_line());
	for (; fgets(buff, CONFIG_BUFF, file_peek_fp()) && !strrchr(buff, '\n'););
	continue;
      }
      if (!(*buff) || *buff == '\n') {
	continue;
      }
      chomp(buff);
      switch (*buff) {
	case '#':
	case '<':
	  break;
	case '%':
	  D(("read_config():  Parsing line #%lu of file %s\n", file_peek_line(), file_peek_path()));
	  if (!BEG_STRCASECMP(PWord(1, buff + 1), "include ")) {
            char *path;
            FILE *fp;

	    shell_expand(buff);
	    path = Word(2, buff + 1);
	    if ((fp = open_config_file(path)) == NULL) {
	      print_error("Error in file %s, line %lu:  Unable to locate %%included config file %s (%s), continuing", file_peek_path(), file_peek_line(),
                          path, strerror(errno));
	    } else {
              file_push(fp, path, NULL, 0, 0);
            }
	  } else if (!BEG_STRCASECMP(PWord(1, buff + 1), "preproc ")) {
	    char cmd[PATH_MAX];
	    FILE *fp;

	    if (file_peek_preproc()) {
	      continue;
	    }
	    outfile = tmpnam(NULL);
	    snprintf(cmd, PATH_MAX, "%s < %s > %s", PWord(2, buff), file_peek_path(), outfile);
	    system(cmd);
	    fp = fopen(outfile, "rt");
	    if (fp != NULL) {
	      fclose(file_peek_fp());
	      file_poke_fp(fp);
	      file_poke_preproc(1);
	      file_poke_outfile(outfile);
	    }
	  } else {
	    print_error("Parse error in file %s, line %lu:  Undefined macro \"%s\"", file_peek_path(), file_peek_line(), buff);
	  }
	  break;
	case 'b':
	  D(("read_config():  Parsing line #%lu of file %s\n", file_peek_line(), file_peek_path()));
	  if (file_peek_skip()) {
	    continue;
          }
	  if (!BEG_STRCASECMP(buff, "begin ")) {
	    name = PWord(2, buff);
	    ctx_name_to_id(id, name, i);
            ctx_push(id);
            *buff = CONF_BEGIN_CHAR;
            ctx_poke_state((*ctx_id_to_func(id))(buff, NULL));
            break;
          }
          /* Intentional pass-through */
	case 'e':
	  D(("read_config():  Parsing line #%lu of file %s\n", file_peek_line(), file_peek_path()));
	  if (!BEG_STRCASECMP(buff, "end ") || !strcasecmp(buff, "end")) {
	    if (ctx_get_depth()) {
              *buff = CONF_END_CHAR;
              (*ctx_id_to_func(id))(buff, ctx_peek_state());
              ctx_poke_state(NULL);
	      ctx_pop();
	      file_poke_skip(0);
	    }
	    break;
	  }
          /* Intentional pass-through */
	default:
	  D(("read_config():  Parsing line #%lu of file %s\n", file_peek_line(), file_peek_path()));
	  if (file_peek_skip()) {
	    continue;
          }
	  id = ctx_peek_id();
          shell_expand(buff);
          ctx_poke_state((*ctx_id_to_func(id))(buff, ctx_peek_state()));
      }
    }
    fclose(file_peek_fp());
    if (file_peek_preproc()) {
      remove(file_peek_outfile());
    }
    file_pop();
  }
  if (*orig_dir) {
    chdir(orig_dir);
  }
}

static void *
parse_null(char *buff, void *state) {

  if (*buff == CONF_BEGIN_CHAR) {
    return (NULL);
  } else if (*buff == CONF_END_CHAR) {
    return (NULL);
  } else {
    print_error("Parse error in file %s, line %lu:  Not allowed in \"null\" context:  \"%s\"", file_peek_path(), file_peek_line(), buff);
    return (state);
  }
}

