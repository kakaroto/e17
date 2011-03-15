#define _GNU_SOURCE

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <getopt.h>
#include <sys/types.h>
#include <sys/param.h>
#include <unistd.h>
#include <pwd.h>
#include <grp.h>
#include <time.h>
#include <libgen.h>

#include <Elixir.h>

#include "config.h"
#include "elixir_private.h"

extern char **environ;

extern FILE *tracker;

static void*
elixir_env(void* handle, const char** name, const char** value, void* context)
{
   static char*         prev_name = NULL;
   static char*         prev_value = NULL;

   char**               start;

   (void) context ;

   start = (!handle) ? environ : handle;

   while (*start && strncmp(*start, "ELX_", 4) != 0)
     ++start;

   if (*start)
     {
        const char*     tmpv = strchr(*start, '=');

        if (!tmpv)
          return NULL;

        free(prev_name);
        free(prev_value);

        *name = prev_name = strndup(*start + 4, tmpv - *start - 4);
        *value = prev_value = strdup(tmpv + 1);

        return start + 1;
     }
   return NULL;
}

static void
elixir_help(void)
{
   fprintf(stderr, "Elixir %s - Javascript standalone bindings.\n", VERSION);
   fprintf(stderr, " -h, --help                 : Show this usage message.\n");
   fprintf(stderr, " -m, --monitor              : Restart file execution as soon as file change (not working).\n");
   fprintf(stderr, " -c, --chroot path          : Chroot the executable into a real Linux chroot.\n");
   fprintf(stderr, " -t, --virtual-chroot path  : Virtually wrap all file access inside an directory.\n");
   fprintf(stderr, " -l, --log-file file        : All error message are send to a file.\n");
   fprintf(stderr, " -u, --user user            : Change to user identity.\n");
   fprintf(stderr, " -g, --group group          : Change to group identity.\n");
   fprintf(stderr, " -d, --chdir path           : Change current dir to path.\n");
   fprintf(stderr, " -p, --policy 0,1,2         : 0 -> virtual chroot data only, 1 -> no javascript in virtual chroot, 2 -> virtual chroot all.\n");
   fprintf(stderr, " -o, --out file             : output everything to file.\n");
   fprintf(stderr, " -r, --random               : Randomly allocate some stuff on the stack.\n");
   fprintf(stderr, " -s, --security authorized  : List of authorized security module separated by ','.\n");
   fprintf(stderr, " -e, --external authorized  : List of authorized file format separated by ','.\n");
   fprintf(stderr, " -i, --pid file             : Put elixir pid in file.\n");
   fprintf(stderr, " -a, --tracker file         : When executing a script put it's name in the stack.\n");
   fprintf(stderr, "\n");
}

static const struct option      lopt[] = {
  { "chroot", 1, 0, 0 },
  { "virtual-chroot", 1, 0, 0 },
  { "log-file", 1, 0, 0 },
  { "monitor", 0, 0, 0 },
  { "help", 0, 0, 0 },
  { "user", 1, 0, 0 },
  { "chdir", 1, 0, 0 },
  { "policy", 1, 0, 0 },
  { "random", 0, 0, 0 },
  { "security", 1, 0, 0 },
  { "group", 1, 0, 0 },
  { "pid", 1, 0, 0 },
  { "tracker", 1, 0, 0 },
  { 0, 0, 0, 0 }
};

static const char *loaders[] = {
  "eet",
  "text",
  "edje",
  NULL
};

static const char *security[] = {
  "let",
  NULL
};

int
main(int argc, char **argv)
{
   FILE                 *fl = NULL;
   FILE                 *fl_out = NULL;
   struct passwd        *pw = NULL;
   struct group         *grp = NULL;
   char                 *chroot = NULL;
   char                 *directory;
   const char           *s;
   Elixir_Runtime       *er;
   Elixir_Script        *es;
   JSObject             *env;
   JSObject             *elx;
   void			*stack = NULL;
   char                **array = NULL;
   jsval                 jsret;
   int                   size;
   int                   i;
   int32_t               ret = 0;

   srandom(time(0));

#ifdef DEBUG
   elixir_debug_print_switch(ELIXIR_DEBUG_STDERR, NULL);
#else
   elixir_debug_print_switch(ELIXIR_DEBUG_NONE, NULL);
#endif
   s = getenv("ELIXIR_DEBUG");
   if (s)
     {
        if (!strcasecmp(s, "stderr"))
          elixir_debug_print_switch(ELIXIR_DEBUG_STDERR, NULL);
        else if (!strcasecmp(s, "stdout"))
          elixir_debug_print_switch(ELIXIR_DEBUG_STDOUT, NULL);
        else if (!strcasecmp(s, "console"))
          elixir_debug_print_switch(ELIXIR_DEBUG_CONSOLE, NULL);
        else if (!strcasecmp(s, "syslog"))
          elixir_debug_print_switch(ELIXIR_DEBUG_SYSLOG, NULL);
        else if ((!strcasecmp(s, "none")) || (!strcasecmp(s, "null")))
          elixir_debug_print_switch(ELIXIR_DEBUG_NONE, NULL);
     }

   elixir_id_init();
   elixir_class_init();
   elixir_file_init();
   elixir_modules_init();
   er = elixir_init();
   if (!er)
     return 128;

   elixir_loader_init(3, loaders);

   elixir_security_init(1, security);

   if (argc <= 1)
     return 129;

   while (1)
     {
        int             option_index = 0;
        int             c;

        c = getopt_long(argc, argv, "mc:t:l:hu:d:p:o:rs:e:g:i:a:", lopt, &option_index);
        if (c == -1)
          break ;

        /* Long options. */
        if (c == 0)
          {
             switch (option_index)
               {
                case 0: c = 'c'; break;
                case 1: c = 't'; break;
                case 2: c = 'l'; break;
                case 3: c = 'm'; break;
                case 4: c = 'h'; break;
                case 5: c = 'u'; break;
                case 6: c = 'd'; break;
                case 7: c = 'p'; break;
                case 8: c = 'o'; break;
		case 9: c = 'r'; break;
		case 10: c = 's'; break;
		case 11: c = 'e'; break;
		case 12: c = 'g'; break;
		case 13: c = 'i'; break;
		case 14: c = 'a'; break;
               }
          }

        switch (c)
          {
           case 0:
	   case 'e':
	   case 's': /* Authorized security modules. */
	      array = malloc(sizeof (char*));
	      array[0] = optarg;
	      for (i = 0, size = 1; optarg[i] != '\0'; ++i)
		{
		   if (optarg[i] == ',')
		     {
			array[size++] = optarg + i + 1;
			optarg[i] = '\0';

			array = realloc(array, sizeof (char*) * size);
		     }
		}
	      if (c == 's') elixir_security_init(size, (const char **)array);
	      else elixir_loader_init(size, (const char **)array);
	      free(array);
	      break;
           case 'm': /* Activate monitoring. */
              break;
           case 'c': /* Call chroot. */
              chroot = strdupa(optarg);
              break;
           case 't': /* Call virtual chroot. */
              elixir_file_virtual_chroot_set(optarg);
              break;
           case 'l': /* Log file. */
              if (fl)
                fclose(fl);
              fl = fopen(optarg, "a");
              if (!fl)
                {
                   fprintf(stderr, "Unable to open `%s'.\n\n", optarg);
                   elixir_help();
                   exit(0);
                }

              elixir_debug_print_switch(ELIXIR_DEBUG_FILE, fl);
              break;
           case 'u':
              pw = getpwnam(optarg);
              if (!pw)
                {
                   fprintf(stderr, "Unable to get user `%s' information.\n\n", optarg);
                   elixir_help();
                   exit(0);
                }
              break;
	   case 'g':
	      grp = getgrnam(optarg);
	      if (!grp)
		{
		   fprintf(stderr, "Unable to get group `%s' information.\n\n", optarg);
		   elixir_help();
		   exit(0);
		}
	      break;
           case 'd':
              if (chdir(optarg))
	        {
		   fprintf(stderr, "Unable to chdir to %s.\n\n", optarg);
		   elixir_help();
		   exit(0);
		};
              break;
           case 'p':
              if (strlen(optarg) != 1)
                {
                   fprintf(stderr, "`%s' is not a valid policy.\n\n", optarg);
                   elixir_help();
                   exit(0);
                }

              switch (*optarg)
                {
                 case '0': elixir_file_virtual_chroot_directive_set(ELIXIR_VCHROOT_DATA_ONLY); break;
                 case '1': elixir_file_virtual_chroot_directive_set(ELIXIR_VCHROOT_NOEXEC); break;
                 case '2': elixir_file_virtual_chroot_directive_set(ELIXIR_VCHROOT_ALL); break;
                 default:
                    fprintf(stderr, "`%s' is not a valid policy.\n\n", optarg);
                    elixir_help();
                    exit(0);
                }

              break;
           case 'o':
              if (fl_out)
                fclose(fl_out);
              fl_out = fopen(optarg, "w");
              break;
	   case 'r':
	      stack = alloca(random() & 0xFF);
	      break;
	   case 'i':
	     {
		FILE *fid;

		fid = fopen(optarg, "w");
		if (fid)
		  {
		     pid_t pid;

		     pid = getpid();
		     fprintf(fid, "%i\n", pid);
		     fclose(fid);
		  }

		break;
	     }
	   case 'a':
	      tracker = fopen(optarg, "w");
	      break;
           case 'h': /* Help. */
           case '?':
           default:
              elixir_help();
              exit(0);
              break;
          }
     }

   if (optind >= argc)
     return 129;

   if (chroot)
     {
        if (!elixir_file_chroot_set(chroot))
          return 133;
     }

   if (pw)
     {
        gid_t   newgid;
        uid_t   newuid;
        gid_t   oldgid;
        uid_t   olduid;

        oldgid = getegid();
        olduid = geteuid();

        newgid = grp ? grp->gr_gid : pw->pw_gid;
        newuid = pw->pw_uid;

        if (!olduid) setgroups(1, &newgid);

#if !defined(linux)
        setegid(newgid);
        if (setgid(newgid) == -1) abort();
#else
        if (setregid(newgid, newgid) == -1) abort();
#endif

#if !defined(linux)
        seteuid(newuid);
        if (setuid(newuid) == -1) abort();
#else
        if (setreuid(newuid, newuid) == -1) abort();
#endif
     }
   else
     {
	if (grp)
	  {
#if !defined(linux)
	     setegid(grp->gr_gid);
	     if (setgid(grp->gr_gid) == -1) abort();
#else
	     if (setregid(grp->gr_gid, grp->gr_gid) == -1) abort();
#endif
	  }
     }

   directory = dirname(strdupa(argv[optind]));
   elixir_debug_print("chdir to `%s`.", directory);
   chdir(directory);

   es = elixir_script_file(er, argc - optind, (const char**) argv + optind);
   elixir_script_set_out(es, fl_out ? fl_out : stdout);

   elixir_lock_cx(er->cx);

   elx = elixir_object_get_object(er->cx, er->root, "elx");
   if (elx)
     if (!elixir_object_register(er->cx, &elx, NULL))
       return 130;

   env = elixir_object_create(er, elx, "env", elixir_env, NULL);
   if (!env)
     return 131;

   if (!elixir_script_run(es, &jsret))
     return 132;

   /* Useless I believe. */
   if (JSVAL_IS_BOOLEAN(jsret)
       || JSVAL_IS_INT(jsret)
       || JSVAL_IS_DOUBLE(jsret)
       || JSVAL_IS_STRING(jsret))
     if (JS_ValueToInt32(es->er->cx, jsret, &ret) != JS_TRUE)
       ret = 0;

   if (elx)
     elixir_object_unregister(er->cx, &elx);

   elixir_unlock_cx(er->cx);

   elixir_script_free(es);
   elixir_shutdown(er);
   elixir_file_shutdown();
   elixir_modules_shutdown();
   elixir_class_shutdown();
   elixir_id_shutdown();

   if (stack) return 1;

   return 0;
}
