/*  Copyright (C) 2006-2008 Davide Andreoli (see AUTHORS)
 *
 *  This file is part of Edje_editor.
 *  Edje_editor is free software: you can redistribute it and/or modify
 *  it under the terms of the GNU Lesser General Public License as published by
 *  the Free Software Foundation, either version 3 of the License, or
 *  (at your option) any later version.
 *
 *  Edje_editor is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU Lesser General Public License for more details.
 *
 *  You should have received a copy of the GNU Lesser General Public License
 *  along with Edje_editor.  If not, see <http://www.gnu.org/licenses/>.
 */

#include <string.h>
#include <errno.h>
#include <locale.h>
#include <Edje.h>
#include <Edje_Edit.h>
#include <Etk.h>
#include <Ecore_Str.h>
#include "config.h"
#include "main.h"

Evas_Object *EdjeTest_bg;
Evas_Object *EdjeTest_edje;

/***   Test MiniApp   ***/
void
_test_win_resize_cb(Ecore_Evas * ee)
{
   int w, h;

   evas_output_size_get(ecore_evas_get(ee), &w, &h);
   evas_object_resize(EdjeTest_bg, w , h );
   evas_object_resize(EdjeTest_edje, w , h );
}

void
test_edje_group(char *File,char *Group)
{
   Ecore_Evas  *ee;
   Evas        *evas;

   printf("Test EdjeFile: %s\nGroup: %s\nTheme: %s\n",File,Group,EdjeFile);

   /* ecore evas init */
   ecore_init();
   ecore_evas_init();
   ee = ecore_evas_software_x11_new(NULL, 0,  0, 0, 0, 0);
   ecore_evas_title_set(ee, "Edje Test Application");
   ecore_evas_callback_resize_set(ee, _test_win_resize_cb);
   edje_init();
   evas = ecore_evas_get(ee);

   /* Background */
   EdjeTest_bg = evas_object_image_add(evas);
   evas_object_image_file_set(EdjeTest_bg, EdjeFile, "images/1");//TODO Find a method to load by name and not by number
   evas_object_image_fill_set(EdjeTest_bg,0,0,240,240);
   evas_object_move(EdjeTest_bg, 0, 0);
   evas_object_resize(EdjeTest_bg, 300, 300);
   evas_object_show(EdjeTest_bg);

   /* Edje Group */
   EdjeTest_edje = edje_object_add(evas);
   edje_object_file_set(EdjeTest_edje,File, Group);
   evas_object_move(EdjeTest_edje, 0, 0);
   evas_object_resize(EdjeTest_edje, 300, 300);
   evas_object_show(EdjeTest_edje);

   /* Window Size */
   int minw,minh,maxw,maxh;
   edje_object_size_min_get(EdjeTest_edje, &minw, &minh);
   edje_object_size_max_get(EdjeTest_edje, &maxw, &maxh);
   if (minw <= 0) minw = 30;
   if (minh <= 0) minh = 30;
   ecore_evas_size_min_set(ee,minw,minh);
   ecore_evas_size_max_set(ee,maxw,maxh);
   ecore_evas_resize(ee, minw, minh);

   ecore_evas_show(ee);

   ecore_main_loop_begin();
}

/***   Debug Info   ***/
Evas_Bool
_debug_info_helper(const Evas_Hash *hash, const char *key, void *data, void *fdata)
{
   printf("Key: '%s' [%d]\n", key, (int)(long)data);
   return TRUE;
}

void
print_debug_info(int full)
{
   printf("\n\n ********************* D E B U G ***************************\n");
   printf(" ** open file name: %s\n",Cur.edj_file_name->string);
   printf(" ** temp file name: %s\n",Cur.edj_temp_name->string);
   printf(" ** edje_editor.edj: %s\n",EdjeFile);
   if (etk_string_length_get(Cur.group))
      printf(" ** Cur group: %s\n",Cur.group->string);
   else
      printf(" ** Cur group: (NULL)\n");
   if (etk_string_length_get(Cur.part))
      printf(" ** Cur part: %s\n",Cur.part->string);
   else
      printf(" ** Cur part: (NULL)\n");
   if (etk_string_length_get(Cur.state))
      printf(" ** Cur state: %s\n",Cur.state->string);
   else
      printf(" ** Cur state: (NULL)\n");
   if (etk_string_length_get(Cur.prog))
      printf(" ** Cur program: %s\n",Cur.prog->string);
   else
      printf(" ** Cur program: (NULL)\n");
   if (etk_string_length_get(Cur.tween))
      printf(" ** Cur tween: %s\n",Cur.tween->string);
   else
      printf(" ** Cur tween: (NULL)\n");
   printf(" *********************** Parts_Hash *****************************\n");
   evas_hash_foreach(Parts_Hash, _debug_info_helper, NULL);
   printf(" *********************** E N D *****************************\n\n");
}

/***   Implementation   ***/
void
print_usage(void)
{
   printf("\nUsage:\n");
   printf(" edje_editor [EDC | EDJ] [IMAGE_DIR] [FONT_DIR]\n");
   printf("\nExample:\n");
   printf(" edje_editor                #Open an empty file\n");
   printf(" edje_editor default.edj    #Open the given EDJ\n");
   printf(" edje_editor default.edc    #Open the given EDC\n");
   printf("\nTesting Edje file:\n");
   printf(" edje_editor -t file.edj \"group_name\"\n");
   printf("\nNote on open an EDC:\n");
   printf(" To open an EDC file you must pass the IMAGE_DIR and FONT_DIR\n");
   printf(" parameters. If not given the EDC directory is assumed.\n\n");
}

void
change_group(char *group)
{
   if (!group) return;

   if (Cur.group->string && !strcmp(group, Cur.group->string)) return;
   printf("Change to group: %s\n", group);

   toolbar_play_button_toggle(1);

   if (etk_string_length_get(Cur.group))
      edje_edit_save(edje_o);

   evas_object_hide(edje_o);
   consolle_clear();

   //evas_object_del(edje_o);
   //edje_o = NULL;
   //edje_o = edje_object_add(UI_evas);
   //edje_object_signal_callback_add(edje_o, "*", "*", signal_cb, NULL);

   if (!edje_object_file_set(edje_o, Cur.edj_temp_name->string, group))
   {
      dialog_alert_show("Error loading group");
      return;
   }
   evas_object_show(edje_o);
   Parts_Hash = NULL;         //TODO FREE

   Cur.group = etk_string_set(Cur.group, group);
   Cur.part = etk_string_clear(Cur.part);
   Cur.state = etk_string_clear(Cur.state);
   Cur.tween = etk_string_clear(Cur.tween);
   Cur.prog = etk_string_clear(Cur.prog);

   tree_populate();
   position_comboboxes_populate();
   program_source_combo_populate();
   program_signal_combo_populate();

   group_frame_update();
   part_frame_update();

   //update FakeWin title
   edje_object_part_text_set(EV_fakewin, "title", group);
}

void 
reload_edje(void)
{
   if (!etk_string_length_get(Cur.group)) return;
   if (!etk_string_length_get(Cur.edj_temp_name)) return;

   //Set a fake object to make sure edje is reloading
   //maybe we can fix this removing the check from _edje_object_file_set_internal
   edje_edit_save(edje_o);
   edje_object_file_set(edje_o, EdjeFile, "IMAGE.PNG");
   edje_object_file_set(edje_o, Cur.edj_temp_name->string, Cur.group->string);
}

int
load_edje(char *file)
{
   unsigned char new_file = 0;
   char *realp = NULL;
   char *old_temp;

   if (!file)
   {
      file = PACKAGE_DATA_DIR"/blank.edj";
      new_file = 1;
   }

   printf("** Load EDJ: '%s'\n",file);

   realp = ecore_file_realpath(file);
   if (!ecore_file_exists(realp))
   {
      dialog_alert_show("<b>ERROR:</b><br>File not exists.");
      return 0;
   }
   if (!ecore_file_can_read(realp))
   {
      dialog_alert_show("<b>ERROR</b>:<br>Can't read file.");
      return 0;
   }
   if (!ecore_str_has_suffix(realp, ".edj"))  //TODO: better check
   {
      dialog_alert_show("<b>ERROR</b>:<br>File is not an edje file.");
      return 0;
   }

   //Create temp file
   if (etk_string_length_get(Cur.edj_temp_name) > 0)
      old_temp = strdup(Cur.edj_temp_name->string);
   else
      old_temp = NULL;

   char tmpn[1024];
   int fd = 0;
   strcpy(tmpn, "/tmp/edje_editor_tmp.edj-XXXXXX");
   fd = mkstemp(tmpn);
   if (fd < 0)
   {
      printf("Can't create temp file '%s'\nError: %s\n", tmpn, strerror(errno));
      return 0;
   }
   Cur.edj_temp_name = etk_string_set(Cur.edj_temp_name, tmpn);


   if (!ecore_file_cp(realp, Cur.edj_temp_name->string))
   {
      dialog_alert_show("<b>ERROR</b>:<br>Can't copy to temp file.");
      return 0;
   }

   if (new_file)
   {
      Cur.edj_file_name = etk_string_clear(Cur.edj_file_name);
      ecore_evas_title_set(UI_ecore_MainWin, "Untitled");
   }
   else
   {
      Cur.edj_file_name = etk_string_set(Cur.edj_file_name, realp);
      ecore_evas_title_set(UI_ecore_MainWin, Cur.edj_file_name->string);
   }


   Cur.group  = etk_string_clear(Cur.group);
   Cur.part   = etk_string_clear(Cur.part);
   Cur.state  = etk_string_clear(Cur.state);
   Cur.prog   = etk_string_clear(Cur.prog);
   Cur.tween   = etk_string_clear(Cur.tween);
   Parts_Hash = NULL;

   tree_combobox_populate();
   text_font_combo_populate();
   gradient_spectra_combo_populate();


   //Delete old temp file
   if (old_temp && !ecore_file_unlink(old_temp))
   {
      dialog_alert_show("<b>ERROR</b>:<br>Can't remove temp file.");
      return 0;
   }
   if (old_temp) free(old_temp);
   return 1;
}

/* main */
int
main(int argc, char **argv)
{
   //Init Globals
   Cur.open_file_name = NULL;
   Cur.source_dir = NULL;
   Cur.main_source_file = NULL;
   Cur.fullscreen = 0;
   stack = NULL;

   //Setting Globals
   EdjeFile = PACKAGE_DATA_DIR"/edje_editor.edj";

   //--help
   if (argc > 1)
   {
      if ( (argc > 4) ||
           (0 == strcmp(argv[1],"-h")) ||
           (0 == strcmp(argv[1],"--help")) ||
           (0 == strcmp(argv[1],"--usage")) )
      {
         print_usage();
         return 1;
      }
   }

   //Test EDJ
   if (argc > 1 && (0 == strcmp(argv[1],"-t")))
   {
      if (argc < 4)
      {
         print_usage();
         return 1;
      }
      test_edje_group(argv[2],argv[3]);
      return 0;
   }

   //Init ETK
   if (!etk_init(argc, argv))
   {
      printf("Could not init etk!\n");
      return 1;
   }

   if (!ecore_init())
   {
     printf("ERROR: Cannot init Ecore!\n");
     return 1;
   }


   Cur.edj_file_name = etk_string_new("");
   Cur.edj_temp_name = etk_string_new("");
   Cur.group = etk_string_new("");
   Cur.part = etk_string_new("");
   Cur.state = etk_string_new("");
   Cur.prog = etk_string_new("");
   Cur.tween = etk_string_new("");
   Cur.spectra = etk_string_new("");


   setlocale(LC_NUMERIC,"C");
   //printf("LOCALE TEST:\n");
   //double val = 1.2;
   //sscanf("3.2","%lf", &val);
   //printf("%f\n", val);

   //Create the etk window and all the widgets
   window_main_create();

   //Open a file from command line
   if (argc == 2)
   {
      printf("Opening edje file: '%s'\n",argv[1]);
      load_edje(argv[1]);
   }
   else
   //Open blank.edj
   {
      load_edje(NULL);
   }

   //print_debug_info(FALSE);

   //Show main window
   ecore_evas_show(UI_ecore_MainWin);

   //Start main loop
   etk_main();

   //Remove temp file
   ecore_file_unlink(Cur.edj_temp_name->string);

   //Main loop is finished
   etk_shutdown();

   return 0;
}
