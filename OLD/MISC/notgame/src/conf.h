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

#ifndef _CONF_H_
#define _CONF_H_

/************ Macros and Definitions ************/
#define CONFIG_BUFF 20480
#if defined(PATH_MAX) && (PATH_MAX < 255)
#  undef PATH_MAX
#endif
#ifndef PATH_MAX
#  define PATH_MAX 255
#endif

#define CONF_BEGIN_CHAR  1
#define CONF_END_CHAR    2

# ifndef min
#  define min(a,b)	(((a) < (b)) ? (a) : (b))
# endif
# ifndef max
#  define max(a,b)	(((a) > (b)) ? (a) : (b))
# endif
# ifndef MIN
#  define MIN(a,b)	(((a) < (b)) ? (a) : (b))
# endif
# ifndef MAX
#  define MAX(a,b)	(((a) > (b)) ? (a) : (b))
# endif
# define LOWER_BOUND(current, other)    (((current) < (other)) ? ((current) = (other)) : (current))
# define AT_LEAST(current, other)       LOWER_BOUND(current, other)
# define MAX_IT(current, other)         LOWER_BOUND(current, other)
# define UPPER_BOUND(current, other)    (((current) > (other)) ? ((current) = (other)) : (current))
# define AT_MOST(current, other)        UPPER_BOUND(current, other)
# define MIN_IT(current, other)         UPPER_BOUND(current, other)
# define SWAP_IT(one, two, tmp)         do {(tmp) = (one); (one) = (two); (two) = (tmp);} while (0)

/* The context table */
#define ctx_name_to_id(the_id, n, i) do { \
                                       for ((i)=0; (i) <= ctx_idx; (i)++) { \
                                         if (!strcasecmp((n), context[(i)].name)) { \
		                           (the_id) = (i); \
					   break; \
					 } \
			               } \
                                       if ((i) > ctx_idx) (the_id) = 0; \
                                     } while (0)
#define ctx_id_to_name(id)         (context[(id)].name)
#define ctx_id_to_func(id)         (context[(id)].handler)

/* The context state stack.  This keeps track of the current context and each previous one. */
#define ctx_push(ctx)              conf_register_context_state(ctx)
#define ctx_pop()                  (ctx_state_idx--)
#define ctx_peek()                 (ctx_state[ctx_state_idx])
#define ctx_peek_id()              (ctx_state[ctx_state_idx].ctx_id)
#define ctx_peek_state()           (ctx_state[ctx_state_idx].state)
#define ctx_peek_last_id()         (ctx_state[(ctx_state_idx?ctx_state_idx-1:0)].ctx_id)
#define ctx_peek_last_state()      (ctx_state[(ctx_state_idx?ctx_state_idx-1:0)].state)
#define ctx_poke_state(q)          ((ctx_state[ctx_state_idx].state) = (q))
#define ctx_get_depth()            (ctx_state_idx)

/* The file state stack */
#define FILE_SKIP_TO_END           (0x01)
#define FILE_PREPROC               (0x02)
#define file_push(f, p, o, l, fl)  conf_register_fstate(f, p, o, l, fl)
#define file_pop()                 (fstate_idx--)
#define file_peek()                (fstate[fstate_idx])
#define file_peek_fp()             (fstate[fstate_idx].fp)
#define file_peek_path()           (fstate[fstate_idx].path)
#define file_peek_outfile()        (fstate[fstate_idx].outfile)
#define file_peek_line()           (fstate[fstate_idx].line)
#define file_peek_skip()           (fstate[fstate_idx].flags & FILE_SKIP_TO_END)
#define file_peek_preproc()        (fstate[fstate_idx].flags & FILE_PREPROC)

#define file_poke_fp(f)            ((fstate[fstate_idx].fp) = (f))
#define file_poke_path(p)          ((fstate[fstate_idx].path) = (p))
#define file_poke_outfile(o)       ((fstate[fstate_idx].outfile) = (o))
#define file_poke_line(l)          ((fstate[fstate_idx].line) = (l))
#define file_poke_skip(s)          do {if (s) {fstate[fstate_idx].flags |= FILE_SKIP_TO_END;} else {fstate[fstate_idx].flags &= ~(FILE_SKIP_TO_END);} } while (0)
#define file_poke_preproc(s)       do {if (s) {fstate[fstate_idx].flags |= FILE_PREPROC;} else {fstate[fstate_idx].flags &= ~(FILE_PREPROC);} } while (0)
#define file_poke(f, p, o, l, fl)  do {file_poke_fp(f); file_poke_path(p); file_poke_outfile(o); file_poke_line(l); fstate[fstate_idx].flags = (fl);} while (0)

#define file_inc_line()            (fstate[fstate_idx].line++)

/************ Structures ************/
/* Contexts */
typedef void * (*ctx_handler_t)(char *, void *);
typedef struct context_struct {
  char *name;
  ctx_handler_t handler;
} ctx_t;
typedef struct ctx_state_struct {
  unsigned char ctx_id;
  void *state;
} ctx_state_t;

/* Parser states */
typedef struct file_state_struct {
  FILE *fp;
  char *path, *outfile;
  unsigned long line;
  unsigned char flags;
} fstate_t;

/* Built-in functions */
typedef char * (*eterm_func_ptr_t) (char *);
typedef struct eterm_func_struct {
  char *name;
  eterm_func_ptr_t ptr;
} eterm_func_t;

/************ Variables ************/
extern fstate_t *fstate;
extern unsigned char fstate_idx;

/************ Function Prototypes ************/
extern void conf_init_subsystem(void);
extern unsigned char conf_register_context(char *name, ctx_handler_t handler);
extern unsigned char conf_register_fstate(FILE *fp, char *path, char *outfile, unsigned long line, unsigned char flags);
extern unsigned char conf_register_builtin(char *name, eterm_func_ptr_t ptr);
extern unsigned char conf_register_context_state(unsigned char ctx_id);
extern char *chomp(char *s);
extern char *shell_expand(char *s);
extern char *conf_find_file(const char *file, const char *pathlist);
extern FILE *open_config_file(char *name);
extern void conf_parse(char *conf_name, const char *path);

#endif	/* _CONF_H_ */
