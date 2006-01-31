/* Mode: C; indent-tabs-mode: t; c-basic-offset: 4; tab-width: 4 -*- */

/*
 * main.c
 * Copyright (C) Christopher Michael 2005 <devilhorns@comcast.net>
 *
 * e17genmenu is free software copyrighted by Christopher Michael.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 * 3. Neither the name ``Christopher Michael'' nor the name of any other
 *    contributor may be used to endorse or promote products derived
 *    from this software without specific prior written permission.
 *
 * e17genmenu IS PROVIDED BY Christopher Michael ``AS IS'' AND ANY EXPRESS
 * OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
 * WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED.  IN NO EVENT SHALL Christopher Michael OR ANY OTHER CONTRIBUTORS
 * BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
 * CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
 * SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR
 * BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY,
 * WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR
 * OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF
 * ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
*/

#include "global.h"
#include "config.h"
#include "menus.h"
#include "sort.h"
#include "E_Menu.h"

/* Function Prototypes */
void _e17genmenu_backup(void);
void _e17genmenu_help(void);
void _e17genmenu_parseargs(int argc, char **argv);
void _e17genmenu_init(void);
void _e17genmenu_shutdown(void);

/* Functions */
void _e17genmenu_backup()
{
#ifdef DEBUG
   fprintf(stderr, "Backing up Existing Eaps...\n");
#endif
   backup_eaps();
}

void _e17genmenu_help()
{
   printf("e17genmenu - Generate Enlightenment DR17 Menus\n");
   printf("Usage: e17genmenu <-options>\n");
   printf("\n");
   printf(" -b | --backup\tBackup Existing Eaps First\n");
   printf(" -s=<size> | --icon-size=<size>\tUse <size> icons. (48x48)\n");
   printf(" -t=<theme> | --icon-theme=<theme>\tUse <theme> for icons\n");
   printf(" -l | --lossy\tUse Lossy Compression For Eaps\n");
   printf(" -d=<dir> | --desktop-dir=<dir>\tCreate eaps for .desktop files in <dir>\n");
   printf(" -o | --overwrite\tOverwrite Eaps\n");
   printf(" -m | --mapping\tGenerate Mapping File\n");
   printf(" -h | --help\t\tShow this help screen\n");

   /* Stop E Stuff */
   _e17genmenu_shutdown();

   exit(0);
}

void _e17genmenu_parseargs(int argc, char **argv)
{
   int i;

   for (i = 1; i < argc; i++)
     {
	if (argv[i])
	  {
	     if ((strstr(argv[i], "--help")) || (strstr(argv[i], "-h")))
	       _e17genmenu_help();
	     if ((strstr(argv[i], "--backup")) || (strstr(argv[i], "-b")))
	       _e17genmenu_backup();
	  }
     }
}

void _e17genmenu_init()
{
   char path[MAX_PATH];

    /* Setup Ecore */
   if (!ecore_init())
     {
	fprintf(stderr, "ERROR: Unable to init ecore, exiting\n");
	exit(-1);
     }

    /* Setup Ecore_File */
   if (!ecore_file_init())
     {
	fprintf(stderr, "ERROR: Unable to init ecore_file, exiting\n");
	ecore_shutdown();
	exit(-1);
     }

   /* Setup Eet */
   if (!eet_init())
     {
	fprintf(stderr, "ERROR: Unable To Init Eet !\n");
	ecore_file_shutdown();
	ecore_shutdown();
	exit(-1);
     }

    /* Check That Dir All Exists */
   snprintf(path, sizeof(path), "%s"EAPPDIR, get_home());
   if (!ecore_file_exists(path))
     {
	fprintf(stderr, "ERROR: %s doesn't exist. Where are the eapps?\n", path);
	eet_shutdown();
	ecore_file_shutdown();
	ecore_shutdown();
	exit(-1);
     }
}

void _e17genmenu_shutdown()
{
#ifdef DEBUG
   fprintf(stderr, "Finished\n");
#endif
   eet_shutdown();
   ecore_file_shutdown();
   ecore_shutdown();
   return;
}

int main(int argc, char **argv)
{
   char path[MAX_PATH];

   char *tmp;
   int ret;
   
   //tmp = e_gen_menu_category_get("System");
   //fprintf(stderr, "Tmp: %s\n", tmp);
   ret = e_menu_generate("/usr/share/applications");
   fprintf(stderr, "Ret: %d\n", ret);
   exit(0);
      
   
   /* Init E Stuff */
   _e17genmenu_init();

    /* Parse Arguments */
   _e17genmenu_parseargs(argc, argv);

    /* Set App Args */
   ecore_app_args_set(argc, (const char **)argv);

   /* Start Making Menus */
   make_menus();

   /* Sort Menus */
   sort_favorites();
   sort_menus();

   /* Update E Cache */
#ifdef DEBUG
   fprintf(stderr, "Regenerating Eapp Cache...\n");
#endif
   snprintf(path, sizeof(path), "enlightenment_eapp_cache_gen %s"EAPPDIR" -r", get_home());
   system(path);

   /* Shutdown */
   _e17genmenu_shutdown();

   return(0);
}
