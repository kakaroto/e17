/*
 * Copyright 2011 Mike Blumenkrantz <mike@zentific.com>
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU Lesser General Public License as published
 * by the Free Software Foundation, either version 2.1 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

#include <Eina.h>
#include <Ecore.h>
#include <Ecore_Getopt.h>
#include <Ecore_File.h>
#include "edje_parser_lib.h"
#ifndef __UNUSED__
# define __UNUSED__ __attribute__((unused))
#endif
int edje_parser_log_dom = 0;

static Edje *edje;
static Eina_Strbuf *edje_str;
static Eina_Strbuf *inc, *defs;
static Eina_List *cmds;

extern void edje_parser_Trace(FILE *,
                              char *);
static const Ecore_Getopt opts = {
   "Edje_Parser",
   "Edje_Parser [options] file.edc",
   "1.0alpha",
   "(C) 2011 Mike Blumenkrantz",
   "LGPL",
   "Parse an edc file\n",
   1,
   {
      ECORE_GETOPT_APPEND('I', NULL, "Directories from which to include files\n (can be specified repeatedly)", ECORE_GETOPT_TYPE_STR),
      ECORE_GETOPT_APPEND('D', NULL, "Define a macro/value\n (can be specified repeatedly)", ECORE_GETOPT_TYPE_STR),
      ECORE_GETOPT_STORE_TRUE('d', "debug", "Print debugging output"),
      ECORE_GETOPT_VERSION('V', "version"),
      ECORE_GETOPT_COPYRIGHT('R', "copyright"),
      ECORE_GETOPT_LICENSE('L', "license"),
      ECORE_GETOPT_HELP('h', "help"),
      ECORE_GETOPT_SENTINEL
   }
};

static Eina_Bool
compile_data_cb(void *data            __UNUSED__,
                int type              __UNUSED__,
                Ecore_Exe_Event_Data *ev)
{
   if (!edje_str) edje_str = eina_strbuf_new();
   eina_strbuf_append_length(edje_str, (char *)ev->data, ev->size);
   return ECORE_CALLBACK_RENEW;
}

static Eina_Bool
compile_end_cb(void *data           __UNUSED__,
               int type             __UNUSED__,
               Ecore_Exe_Event_Del *ev)
{
   if (ev->exit_signal) /* failure */
     {
        if (cmds && cmds->data)
          {
             free(cmds->data);
             cmds = eina_list_remove_list(cmds, cmds);
          }
        if (cmds)
          ecore_exe_pipe_run(cmds->data, ECORE_EXE_PIPE_READ, NULL);  /* neeext */
        else /* no more cmds to try, failure all around */
          {
             ERR("Could not preprocess file!");
             ecore_main_loop_quit();
          }
     }
   else
     ecore_main_loop_quit();  /* success! */
   return ECORE_CALLBACK_RENEW;
}

/* from edje_cc_parse.c...sort of */
void
compile_setup(const char *file_in)
{
   Eina_Strbuf *buf;

   buf = eina_strbuf_new();
   /*
    * Run the input through the C pre-processor.
    */

   /*
    * On OpenSolaris, the default cpp is located in different places.
    * Alan Coppersmith told me to do what xorg does: using /usr/ccs/lib/cpp
    *
    * Also, that preprocessor is not managing C++ comments, so pass the
    * sun cc preprocessor just after.
    */
   if (ecore_file_exists("/usr/ccs/lib/cpp"))
     {
        eina_strbuf_append_printf(buf, "/usr/ccs/lib/cpp -P -C %s %s %s", eina_strbuf_string_get(inc), eina_strbuf_string_get(defs), file_in);
        cmds = eina_list_append(cmds, eina_strbuf_string_steal(buf));
        eina_strbuf_append_printf(buf, "cc -E -P -C %s %s %s", eina_strbuf_string_get(inc), eina_strbuf_string_get(defs), file_in);
        cmds = eina_list_append(cmds, eina_strbuf_string_steal(buf));
     }

   /* Trying gcc and other syntax */
   eina_strbuf_append_printf(buf, "%s -E -P -C -std=c99 %s %s - < %s", getenv("CC") ? getenv("CC") : "cc",
                             inc ? eina_strbuf_string_get(inc) : "", defs ? eina_strbuf_string_get(defs) : "", file_in);
   cmds = eina_list_append(cmds, eina_strbuf_string_steal(buf));
   /* Trying suncc syntax */
   eina_strbuf_append_printf(buf, "%s -E -P -C -xc99 %s %s - < %s", getenv("CC") ? getenv("CC") : "cc",
                             inc ? eina_strbuf_string_get(inc) : "", defs ? eina_strbuf_string_get(defs) : "", file_in);
   cmds = eina_list_append(cmds, eina_strbuf_string_steal(buf));

   ecore_exe_pipe_run(cmds->data, ECORE_EXE_PIPE_READ, NULL); /* try first cmd */
   eina_strbuf_free(buf);
}

int
main(int   argc,
     char *argv[])
{
   Eina_Bool err;
   Eina_Bool debug = EINA_FALSE;
   Eina_Bool exit_option = EINA_FALSE;
   Eina_List *includes = NULL;
   Eina_List *defines = NULL;
   char *edje_file;
   int args;

   Ecore_Getopt_Value values[] =
   {
      ECORE_GETOPT_VALUE_LIST(includes),
      ECORE_GETOPT_VALUE_LIST(defines),
      ECORE_GETOPT_VALUE_BOOL(debug),
      ECORE_GETOPT_VALUE_BOOL(exit_option),
      ECORE_GETOPT_VALUE_BOOL(exit_option),
      ECORE_GETOPT_VALUE_BOOL(exit_option),
      ECORE_GETOPT_VALUE_BOOL(exit_option)
   };

   eina_init();
   ecore_init();
   ecore_app_args_set(argc, (const char **)argv);
   edje_parser_log_dom = eina_log_domain_register("edje_parser", EINA_COLOR_YELLOW);

   args = ecore_getopt_parse(&opts, values, argc, argv);
   if (args < 0) return 1;
   if (exit_option) return 0;

   edje_file = argv[args];

   if (!edje_file)
     {
        ERR("You must specify the .edc file.");
        return 1;
     }
   if (includes)
     {
        Eina_List *l;
        const char *i;
        inc = eina_strbuf_new();
        EINA_LIST_FOREACH(includes, l, i)
          eina_strbuf_append_printf(inc, "-I%s%s", i, l->next ? " " : "");
     }
   if (defines)
     {
        Eina_List *l;
        const char *i;
        defs = eina_strbuf_new();
        EINA_LIST_FOREACH(defines, l, i)
          eina_strbuf_append_printf(defs, "-D%s%s", i, l->next ? " " : "");
     }
   if (debug)
     {
        edje_parser_Trace(stdout, "Edje_Parser: ");
        eina_log_domain_level_set("edje_parser", EINA_LOG_LEVEL_DBG);
     }
   ecore_event_handler_add(ECORE_EXE_EVENT_DATA, (Ecore_Event_Handler_Cb)compile_data_cb, NULL);
   ecore_event_handler_add(ECORE_EXE_EVENT_DEL, (Ecore_Event_Handler_Cb)compile_end_cb, NULL);
   compile_setup(edje_file);
   ecore_main_loop_begin();
   err = EINA_FALSE;
   edje = edje_parse_string(eina_strbuf_string_get(edje_str), &err);
   if ((!edje) || err)
     {
        printf("Error parsing file!\n");
        exit(1);
     }

   if (debug) printf("edje-parser: Done!!\n");
/* cleanup for reference
   eina_strbuf_free(edje_str);
   eina_strbuf_free(inc);
   eina_strbuf_free(defs);
   {
      char *s;
      EINA_LIST_FREE(cmds, s)
        free(s);
   }
 */
   return 0;
}

