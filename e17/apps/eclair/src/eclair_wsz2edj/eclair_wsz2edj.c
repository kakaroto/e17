#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <dirent.h>
#include <sys/types.h>
#include <sys/stat.h>
#include "../../config.h"

#define EXTRACTION_DIR "wsz2edj_temp"
#define MAX_LINE_SIZE 512

//Build the dir. Return 0 if failed
int make_dir(const char *dir)
{
   struct stat st;

   if (!dir)
      return 0;

   if (stat(dir, &st) >= 0)
   {
      if (!S_ISDIR(st.st_mode))
      {
         printf("Error: Unable to make dir \"%s\": a file with the same name already exists\n", dir);
         return 0;
      }
   }
   else if (mkdir(dir, 0755) != 0)
   {
      printf("Error: Unable to make dir \"%s\"\n", dir);
      return 0;
   }
   return 1;
}

//Decompress the skin file. Return 0 if failed
int decompress_wsz(const char *filename, const char *destination_dir)
{
   char *command;

   if (!filename || !destination_dir)
      return 0;

   command = (char *)malloc(strlen("unzip -q -d \"\"") + strlen(destination_dir) + strlen(" -o \"\"") + strlen(filename) + 1);
   sprintf(command, "unzip -q -d \"%s\" -o \"%s\"", destination_dir, filename);
   if (system(command) != 0)
   {
      printf("Error: Unable to decompress source skin \"%s\"\n", filename);
      free(command);
      return 0;
   }
   free(command);

   return 1;
}

//Search a file recursively, ignoring case
//Return NULL if not found. Returned string has to be freed
char *get_case_filename(const char *filename, const char *root_dir)
{
   struct dirent **namelist;
   int i, n;
   char *result = NULL, *current_file;

   if (!filename || !root_dir)
      return NULL;
   
   n = scandir(root_dir, &namelist, NULL, alphasort);
   if (n < 0)
      return NULL;
   else
   {
      for (i = 0; i < n; i++)
      {
         if (strcmp(namelist[i]->d_name, ".") != 0 && strcmp(namelist[i]->d_name, "..") != 0)
         {
            current_file = malloc(strlen(root_dir) + strlen(namelist[i]->d_name) + 2);
            sprintf(current_file, "%s/%s", root_dir, namelist[i]->d_name);
   
            if (!result && strcasecmp(filename, namelist[i]->d_name) == 0)
               result = current_file;
            else
            {
               if (!result)
                  result = get_case_filename(filename, current_file);
               free(current_file);
            }
         }
         free(namelist[i]);
      }
      free(namelist);
   }
   return result;
}

//Extract an image part from source_file to dest_file
int extract_image_part(const char *source_file, int x, int y, int w, int h, const char *dest_file)
{
   char *command;
   int len;
   
   if (!source_file || !dest_file)
      return 0;

   len = strlen("montage \"") + strlen(source_file) + strlen("\" -gravity NorthWest -transparent \"#d32b2b\" -crop ") +
      strlen(" -geometry ") + strlen(" -background none \"\"") + strlen(dest_file) + 50;
   command = malloc(len);
   // -transparent \"#d32b2b\"
   snprintf(command, len, "montage \"%s\" -gravity NorthWest -crop %dx%d+%d+%d -geometry %dx%d -background none \"%s\"",
      source_file, w, h, x, y, w, h, dest_file);
   if (system(command) != 0)
   {
      printf("Error: Unable to extract image part \"%s\" from \"%s\"\n", dest_file, source_file);
      free(command);
      return 0;
   }
   free(command);

   return 1;
}

//Read the file containing the skin infos (pledit.txt) and write the interesting data to edje/defines.inc
int read_skin_infos(const char *text_file)
{
   FILE *pledit_file, *header_file;
   char line[MAX_LINE_SIZE];
   char *value;
   int r, g, b;
   int selected_bg_color[4] = { 255, 255, 255, 0 };
   int normal_bg_color[4] = { 128, 128, 128, 255 };
   int normal_color[4] = { 0, 0, 0, 255 };
   int current_color[4] = { 255, 255, 255, 0 };

   if (!text_file)
      return 0;

   if (!(pledit_file = fopen(text_file, "rt")))
   {
      printf("Error: unable to open file \"%s\" for reading\n", text_file);
      return 1;
   }
   if (!(header_file = fopen(EXTRACTION_DIR "/edje/header.inc", "wt")))
   {
      printf("Error: unable to open file \"" EXTRACTION_DIR "/edje/defines.inc\" for reading\n");
      fclose(pledit_file);
      return 1;
   }

   while (fgets(line, MAX_LINE_SIZE, pledit_file))
   {
      if (strncasecmp(line, "SelectedBG", 10) == 0 && (value = rindex(line, '#')) && sscanf(value, "#%2x%2x%2x", &r, &g, &b) == 3)
      {
         selected_bg_color[0] = r;
         selected_bg_color[1] = g;
         selected_bg_color[2] = b;
         selected_bg_color[3] = 255;
      }
      else if (strncasecmp(line, "NormalBG", 8) == 0 && (value = rindex(line, '#')) && sscanf(value, "#%2x%2x%2x", &r, &g, &b) == 3)
      {
         normal_bg_color[0] = r;
         normal_bg_color[1] = g;
         normal_bg_color[2] = b;
         normal_bg_color[3] = 255;
      }
      else if (strncasecmp(line, "Normal", 6) == 0 && (value = rindex(line, '#')) && sscanf(value, "#%2x%2x%2x", &r, &g, &b) == 3)
      {
         normal_color[0] = r;
         normal_color[1] = g;
         normal_color[2] = b;
         normal_color[3] = 255;
      }
      else if (strncasecmp(line, "Current", 7) == 0 && (value = rindex(line, '#')) && sscanf(value, "#%2x%2x%2x", &r, &g, &b) == 3)
      {
         current_color[0] = r;
         current_color[1] = g;
         current_color[2] = b;
         current_color[3] = 255;
      }
   }

   fprintf(header_file, "#define SELECTED_BG_COLOR %d %d %d %d\n", selected_bg_color[0], selected_bg_color[1], selected_bg_color[2], selected_bg_color[3]);
   fprintf(header_file, "#define PLAYLIST_BG_COLOR %d %d %d %d\n", normal_bg_color[0], normal_bg_color[1], normal_bg_color[2], normal_bg_color[3]);
   fprintf(header_file, "#define ENTRY_COLOR %d %d %d %d\n", normal_color[0], normal_color[1], normal_color[2], normal_color[3]);
   fprintf(header_file, "#define CURRENT_ENTRY_COLOR %d %d %d %d\n\n", current_color[0], current_color[1], current_color[2], current_color[3]);

   fprintf(header_file, "data {\n");
   fprintf(header_file, "   item: \"normal_entry_fg_color\" \"%d %d %d %d\";\n", normal_color[0], normal_color[1], normal_color[2], normal_color[3]);
   fprintf(header_file, "   item: \"selected_entry_bg_color\" \"%d %d %d %d\";\n", selected_bg_color[0], selected_bg_color[1], selected_bg_color[2], selected_bg_color[3]);
   fprintf(header_file, "   item: \"current_entry_fg_color\" \"%d %d %d %d\";\n", current_color[0], current_color[1], current_color[2], current_color[3]);
   fprintf(header_file, "}\n");

   fclose(pledit_file);
   fclose(header_file);

   return 1;
}

//Convert the skin!
int main(int argc, char *argv[])
{
   struct stat st;
   char *root_dir;
   char *image_file;
   char *text_file;
   char *command;
   char *extracted_file;
   int extract_result;
   int i;

   if (argc != 3)
   {
      printf("Usage: eclair_wsz2edj source_skin.wsz destination_skin.edj\n");
      printf("or: eclair_wsz2edj source_skin_dir destination_skin.edj\n");
      return 1;
   }

   if (stat(argv[1], &st) < 0)
   {
      printf("Error: source file doesn't exist\n");
      return 1;
   }
   if (!make_dir(EXTRACTION_DIR))
      return 1;  
   if (!S_ISDIR(st.st_mode))
   {
      printf("Decompressing the skin...\n");
      if (!make_dir(EXTRACTION_DIR"/skin_dir"))
         return 1; 
      decompress_wsz(argv[1], EXTRACTION_DIR"/skin_dir");
      printf("Skin decompressed\n\n");
      root_dir = strdup(EXTRACTION_DIR"/skin_dir");
   }
   else
      root_dir = strdup(argv[1]);

   extract_result = 1;
   printf("Extracting image parts...\n");
   if (!make_dir(EXTRACTION_DIR"/images"))
      return 1; 
   //main.bmp
   if (!(image_file = get_case_filename("main.bmp", root_dir)))
   {
      printf("Error: File \"%s\" is not a valid winamp skin: can\'t find file main.bmp\n", argv[1]);
      free(root_dir);
      return 1;
   }
   extract_result &= extract_image_part(image_file, 0, 0, 275, 116, EXTRACTION_DIR"/images/body.png");
   extract_result &= extract_image_part(EXTRACTION_DIR"/images/body.png", 71, 26, 5, 13, EXTRACTION_DIR"/images/num_separator.png");
   free(image_file);

   //cbuttons.bmp
   if (!(image_file = get_case_filename("cbuttons.bmp", root_dir)))
   {
      printf("Error: File \"%s\" is not a valid winamp skin: can\'t find file cbuttons.bmp\n", argv[1]);
      free(root_dir);
      return 1;
   }
   extract_result &= extract_image_part(image_file, 0, 0, 136, 36, EXTRACTION_DIR"/images/cbuttons.png");
   extract_result &= extract_image_part(EXTRACTION_DIR"/images/cbuttons.png", 0, 0, 23, 18, EXTRACTION_DIR"/images/prev.png");
   extract_result &= extract_image_part(EXTRACTION_DIR"/images/cbuttons.png", 0, 18, 23, 18, EXTRACTION_DIR"/images/prev_down.png");
   extract_result &= extract_image_part(EXTRACTION_DIR"/images/cbuttons.png", 23, 0, 23, 18, EXTRACTION_DIR"/images/play.png");
   extract_result &= extract_image_part(EXTRACTION_DIR"/images/cbuttons.png", 23, 18, 23, 18, EXTRACTION_DIR"/images/play_down.png");
   extract_result &= extract_image_part(EXTRACTION_DIR"/images/cbuttons.png", 46, 0, 23, 18, EXTRACTION_DIR"/images/pause.png");
   extract_result &= extract_image_part(EXTRACTION_DIR"/images/cbuttons.png", 46, 18, 23, 18, EXTRACTION_DIR"/images/pause_down.png");
   extract_result &= extract_image_part(EXTRACTION_DIR"/images/cbuttons.png", 69, 0, 23, 18, EXTRACTION_DIR"/images/stop.png");
   extract_result &= extract_image_part(EXTRACTION_DIR"/images/cbuttons.png", 69, 18, 23, 18, EXTRACTION_DIR"/images/stop_down.png");
   extract_result &= extract_image_part(EXTRACTION_DIR"/images/cbuttons.png", 92, 0, 22, 18, EXTRACTION_DIR"/images/next.png");
   extract_result &= extract_image_part(EXTRACTION_DIR"/images/cbuttons.png", 92, 18, 22, 18, EXTRACTION_DIR"/images/next_down.png");
   extract_result &= extract_image_part(EXTRACTION_DIR"/images/cbuttons.png", 114, 0, 22, 16, EXTRACTION_DIR"/images/open.png");
   extract_result &= extract_image_part(EXTRACTION_DIR"/images/cbuttons.png", 114, 16, 22, 16, EXTRACTION_DIR"/images/open_down.png");
   
   free(image_file);

   //shufrep.bmp
   if (!(image_file = get_case_filename("shufrep.bmp", root_dir)))
   {
      printf("Error: File \"%s\" is not a valid winamp skin: can\'t find file shufrep.bmp\n", argv[1]);
      free(root_dir);
      return 1;
   }
   extract_result &= extract_image_part(image_file, 0, 0, 92, 85, EXTRACTION_DIR"/images/shufrep.png");
   extract_result &= extract_image_part(EXTRACTION_DIR"/images/shufrep.png", 0, 0, 28, 15, EXTRACTION_DIR"/images/repeat_off.png");
   extract_result &= extract_image_part(EXTRACTION_DIR"/images/shufrep.png", 0, 15, 28, 15, EXTRACTION_DIR"/images/repeat_off_down.png");
   extract_result &= extract_image_part(EXTRACTION_DIR"/images/shufrep.png", 0, 30, 28, 15, EXTRACTION_DIR"/images/repeat_on.png");
   extract_result &= extract_image_part(EXTRACTION_DIR"/images/shufrep.png", 0, 45, 28, 15, EXTRACTION_DIR"/images/repeat_on_down.png");
   extract_result &= extract_image_part(EXTRACTION_DIR"/images/shufrep.png", 28, 0, 47, 15, EXTRACTION_DIR"/images/shuffle_off.png");
   extract_result &= extract_image_part(EXTRACTION_DIR"/images/shufrep.png", 28, 15, 47, 15, EXTRACTION_DIR"/images/shuffle_off_down.png");
   extract_result &= extract_image_part(EXTRACTION_DIR"/images/shufrep.png", 28, 30, 47, 15, EXTRACTION_DIR"/images/shuffle_on.png");
   extract_result &= extract_image_part(EXTRACTION_DIR"/images/shufrep.png", 28, 45, 47, 15, EXTRACTION_DIR"/images/shuffle_on_down.png");
   extract_result &= extract_image_part(EXTRACTION_DIR"/images/shufrep.png", 0, 61, 23, 12, EXTRACTION_DIR"/images/equalizer_off.png");
   extract_result &= extract_image_part(EXTRACTION_DIR"/images/shufrep.png", 46, 61, 23, 12, EXTRACTION_DIR"/images/equalizer_off_down.png");
   extract_result &= extract_image_part(EXTRACTION_DIR"/images/shufrep.png", 0, 73, 23, 12, EXTRACTION_DIR"/images/equalizer_on.png");
   extract_result &= extract_image_part(EXTRACTION_DIR"/images/shufrep.png", 46, 73, 23, 12, EXTRACTION_DIR"/images/equalizer_on_down.png");
   extract_result &= extract_image_part(EXTRACTION_DIR"/images/shufrep.png", 23, 61, 23, 12, EXTRACTION_DIR"/images/playlist_off.png");
   extract_result &= extract_image_part(EXTRACTION_DIR"/images/shufrep.png", 69, 61, 23, 12, EXTRACTION_DIR"/images/playlist_off_down.png");
   extract_result &= extract_image_part(EXTRACTION_DIR"/images/shufrep.png", 23, 73, 23, 12, EXTRACTION_DIR"/images/playlist_on.png");
   extract_result &= extract_image_part(EXTRACTION_DIR"/images/shufrep.png", 69, 73, 23, 12, EXTRACTION_DIR"/images/playlist_on_down.png");
   free(image_file);

   //posbar.bmp
   if (!(image_file = get_case_filename("posbar.bmp", root_dir)))
   {
      printf("Error: File \"%s\" is not a valid winamp skin: can\'t find file posbar.bmp\n", argv[1]);
      free(root_dir);
      return 1;
   }
   extract_result &= extract_image_part(image_file, 0, 0, 307, 10, EXTRACTION_DIR"/images/posbar.png");
   extract_result &= extract_image_part(EXTRACTION_DIR"/images/posbar.png", 0, 0, 248, 10, EXTRACTION_DIR"/images/progress_bar.png");
   extract_result &= extract_image_part(EXTRACTION_DIR"/images/posbar.png", 248, 0, 29, 10, EXTRACTION_DIR"/images/progress_bar_drag.png");
   extract_result &= extract_image_part(EXTRACTION_DIR"/images/posbar.png", 278, 0, 29, 10, EXTRACTION_DIR"/images/progress_bar_drag_down.png");
   free(image_file);

   //titlebar.bmp
   if (!(image_file = get_case_filename("titlebar.bmp", root_dir)))
   {
      printf("Error: File \"%s\" is not a valid winamp skin: can\'t find file titlebar.bmp\n", argv[1]);
      free(root_dir);
      return 1;
   }
   extract_result &= extract_image_part(image_file, 0, 0, 344, 87, EXTRACTION_DIR"/images/titlebar.png");
   extract_result &= extract_image_part(EXTRACTION_DIR"/images/titlebar.png", 27, 0, 275, 14, EXTRACTION_DIR"/images/body_titlebar.png");
   extract_result &= extract_image_part(EXTRACTION_DIR"/images/titlebar.png", 9, 0, 9, 9, EXTRACTION_DIR"/images/minimize.png");
   extract_result &= extract_image_part(EXTRACTION_DIR"/images/titlebar.png", 9, 9, 9, 9, EXTRACTION_DIR"/images/minimize_down.png");
   extract_result &= extract_image_part(EXTRACTION_DIR"/images/titlebar.png", 18, 0, 9, 9, EXTRACTION_DIR"/images/close.png");
   extract_result &= extract_image_part(EXTRACTION_DIR"/images/titlebar.png", 18, 9, 9, 9, EXTRACTION_DIR"/images/close_down.png");
   free(image_file);

   //volume.bmp
   if (!(image_file = get_case_filename("volume.bmp", root_dir)))
   {
      printf("Error: File \"%s\" is not a valid winamp skin: can\'t find file volume.bmp\n", argv[1]);
      free(root_dir);
      return 1;
   }
   extract_result &= extract_image_part(image_file, 0, 0, 68, 433, EXTRACTION_DIR"/images/volume.png");
   extracted_file = malloc(strlen(EXTRACTION_DIR"/images/volume_bar_.png") + 3);
   for (i = 0; i < 28; i++)
   {
      sprintf(extracted_file, EXTRACTION_DIR"/images/volume_bar_%d.png", i);
      extract_result &= extract_image_part(image_file, 0, i * 15, 68, 13, extracted_file);
   }
   free(extracted_file);
   extract_result &= extract_image_part(EXTRACTION_DIR"/images/volume.png", 15, 422, 14, 11, EXTRACTION_DIR"/images/volume_bar_drag.png");
   extract_result &= extract_image_part(EXTRACTION_DIR"/images/volume.png", 0, 422, 14, 11, EXTRACTION_DIR"/images/volume_bar_drag_down.png");
   free(image_file);

   //pledit.bmp
   if (!(image_file = get_case_filename("pledit.bmp", root_dir)))
   {
      printf("Error: File \"%s\" is not a valid winamp skin: can\'t find file pledit.bmp\n", argv[1]);
      free(root_dir);
      return 1;
   }
   extract_result &= extract_image_part(image_file, 0, 0, 280, 186, EXTRACTION_DIR"/images/pledit.png");
   extract_result &= extract_image_part(EXTRACTION_DIR"/images/pledit.png", 0, 0, 25, 20, EXTRACTION_DIR"/images/playlist_top_left.png");
   extract_result &= extract_image_part(EXTRACTION_DIR"/images/pledit.png", 26, 0, 100, 20, EXTRACTION_DIR"/images/playlist_title.png");
   extract_result &= extract_image_part(EXTRACTION_DIR"/images/pledit.png", 127, 0, 25, 20, EXTRACTION_DIR"/images/playlist_top.png");
   extract_result &= extract_image_part(EXTRACTION_DIR"/images/pledit.png", 153, 0, 25, 20, EXTRACTION_DIR"/images/playlist_top_right.png");
   extract_result &= extract_image_part(EXTRACTION_DIR"/images/pledit.png", 0, 72, 125, 38, EXTRACTION_DIR"/images/playlist_bottom_left.png");
   extract_result &= extract_image_part(EXTRACTION_DIR"/images/pledit.png", 179, 0, 25, 38, EXTRACTION_DIR"/images/playlist_bottom.png");
   extract_result &= extract_image_part(EXTRACTION_DIR"/images/pledit.png", 126, 72, 150, 38, EXTRACTION_DIR"/images/playlist_bottom_right.png");
   extract_result &= extract_image_part(EXTRACTION_DIR"/images/pledit.png", 0, 42, 25, 29, EXTRACTION_DIR"/images/playlist_left.png");
   extract_result &= extract_image_part(EXTRACTION_DIR"/images/pledit.png", 26, 42, 25, 29, EXTRACTION_DIR"/images/playlist_right.png");
   extract_result &= extract_image_part(EXTRACTION_DIR"/images/pledit.png", 52, 53, 8, 18, EXTRACTION_DIR"/images/playlist_scrollbar_drag.png");
   extract_result &= extract_image_part(EXTRACTION_DIR"/images/pledit.png", 61, 53, 8, 18, EXTRACTION_DIR"/images/playlist_scrollbar_drag_down.png");
   extract_result &= extract_image_part(EXTRACTION_DIR"/images/pledit.png", 52, 42, 9, 9, EXTRACTION_DIR"/images/playlist_close_down.png");
   extract_result &= extract_image_part(EXTRACTION_DIR"/images/pledit.png", 62, 42, 9, 9, EXTRACTION_DIR"/images/playlist_shade_down.png");

   extract_result &= extract_image_part(EXTRACTION_DIR"/images/pledit.png", 48, 111, 3, 54, EXTRACTION_DIR"/images/playlist_add_border.png");
   extract_result &= extract_image_part(EXTRACTION_DIR"/images/pledit.png", 0, 111, 22, 18, EXTRACTION_DIR"/images/playlist_add_url_off.png");
   extract_result &= extract_image_part(EXTRACTION_DIR"/images/pledit.png", 23, 111, 22, 18, EXTRACTION_DIR"/images/playlist_add_url_on.png");
   extract_result &= extract_image_part(EXTRACTION_DIR"/images/pledit.png", 0, 130, 22, 18, EXTRACTION_DIR"/images/playlist_add_dir_off.png");
   extract_result &= extract_image_part(EXTRACTION_DIR"/images/pledit.png", 23, 130, 22, 18, EXTRACTION_DIR"/images/playlist_add_dir_on.png");
   extract_result &= extract_image_part(EXTRACTION_DIR"/images/pledit.png", 0, 149, 22, 18, EXTRACTION_DIR"/images/playlist_add_file_off.png");
   extract_result &= extract_image_part(EXTRACTION_DIR"/images/pledit.png", 23, 149, 22, 18, EXTRACTION_DIR"/images/playlist_add_file_on.png");
   extract_result &= extract_image_part(EXTRACTION_DIR"/images/pledit.png", 100, 111, 3, 72, EXTRACTION_DIR"/images/playlist_remove_border.png");
   extract_result &= extract_image_part(EXTRACTION_DIR"/images/pledit.png", 54, 111, 22, 18, EXTRACTION_DIR"/images/playlist_remove_all_off.png");
   extract_result &= extract_image_part(EXTRACTION_DIR"/images/pledit.png", 77, 111, 22, 18, EXTRACTION_DIR"/images/playlist_remove_all_on.png");
   extract_result &= extract_image_part(EXTRACTION_DIR"/images/pledit.png", 54, 130, 22, 18, EXTRACTION_DIR"/images/playlist_remove_unselected_off.png");
   extract_result &= extract_image_part(EXTRACTION_DIR"/images/pledit.png", 77, 130, 22, 18, EXTRACTION_DIR"/images/playlist_remove_unselected_on.png");
   extract_result &= extract_image_part(EXTRACTION_DIR"/images/pledit.png", 54, 149, 22, 18, EXTRACTION_DIR"/images/playlist_remove_selected_off.png");
   extract_result &= extract_image_part(EXTRACTION_DIR"/images/pledit.png", 77, 149, 22, 18, EXTRACTION_DIR"/images/playlist_remove_selected_on.png");
   extract_result &= extract_image_part(EXTRACTION_DIR"/images/pledit.png", 54, 168, 22, 18, EXTRACTION_DIR"/images/playlist_remove_misc_off.png");
   extract_result &= extract_image_part(EXTRACTION_DIR"/images/pledit.png", 77, 168, 22, 18, EXTRACTION_DIR"/images/playlist_remove_misc_on.png");
   extract_result &= extract_image_part(EXTRACTION_DIR"/images/pledit.png", 150, 111, 3, 54, EXTRACTION_DIR"/images/playlist_select_border.png");
   extract_result &= extract_image_part(EXTRACTION_DIR"/images/pledit.png", 104, 111, 22, 18, EXTRACTION_DIR"/images/playlist_select_inverse_off.png");
   extract_result &= extract_image_part(EXTRACTION_DIR"/images/pledit.png", 127, 111, 22, 18, EXTRACTION_DIR"/images/playlist_select_inverse_on.png");
   extract_result &= extract_image_part(EXTRACTION_DIR"/images/pledit.png", 104, 130, 22, 18, EXTRACTION_DIR"/images/playlist_select_none_off.png");
   extract_result &= extract_image_part(EXTRACTION_DIR"/images/pledit.png", 127, 130, 22, 18, EXTRACTION_DIR"/images/playlist_select_none_on.png");
   extract_result &= extract_image_part(EXTRACTION_DIR"/images/pledit.png", 104, 149, 22, 18, EXTRACTION_DIR"/images/playlist_select_all_off.png");
   extract_result &= extract_image_part(EXTRACTION_DIR"/images/pledit.png", 127, 149, 22, 18, EXTRACTION_DIR"/images/playlist_select_all_on.png");
   extract_result &= extract_image_part(EXTRACTION_DIR"/images/pledit.png", 200, 111, 3, 54, EXTRACTION_DIR"/images/playlist_misc_border.png");
   extract_result &= extract_image_part(EXTRACTION_DIR"/images/pledit.png", 154, 111, 22, 18, EXTRACTION_DIR"/images/playlist_misc_sort_off.png");
   extract_result &= extract_image_part(EXTRACTION_DIR"/images/pledit.png", 177, 111, 22, 18, EXTRACTION_DIR"/images/playlist_misc_sort_on.png");
   extract_result &= extract_image_part(EXTRACTION_DIR"/images/pledit.png", 154, 130, 22, 18, EXTRACTION_DIR"/images/playlist_misc_info_off.png");
   extract_result &= extract_image_part(EXTRACTION_DIR"/images/pledit.png", 177, 130, 22, 18, EXTRACTION_DIR"/images/playlist_misc_info_on.png");
   extract_result &= extract_image_part(EXTRACTION_DIR"/images/pledit.png", 154, 149, 22, 18, EXTRACTION_DIR"/images/playlist_misc_opts_off.png");
   extract_result &= extract_image_part(EXTRACTION_DIR"/images/pledit.png", 177, 149, 22, 18, EXTRACTION_DIR"/images/playlist_misc_opts_on.png");
   extract_result &= extract_image_part(EXTRACTION_DIR"/images/pledit.png", 250, 111, 3, 54, EXTRACTION_DIR"/images/playlist_playlist_border.png");
   extract_result &= extract_image_part(EXTRACTION_DIR"/images/pledit.png", 204, 111, 22, 18, EXTRACTION_DIR"/images/playlist_playlist_new_off.png");
   extract_result &= extract_image_part(EXTRACTION_DIR"/images/pledit.png", 227, 111, 22, 18, EXTRACTION_DIR"/images/playlist_playlist_new_on.png");
   extract_result &= extract_image_part(EXTRACTION_DIR"/images/pledit.png", 204, 130, 22, 18, EXTRACTION_DIR"/images/playlist_playlist_save_off.png");
   extract_result &= extract_image_part(EXTRACTION_DIR"/images/pledit.png", 227, 130, 22, 18, EXTRACTION_DIR"/images/playlist_playlist_save_on.png");
   extract_result &= extract_image_part(EXTRACTION_DIR"/images/pledit.png", 204, 149, 22, 18, EXTRACTION_DIR"/images/playlist_playlist_load_off.png");
   extract_result &= extract_image_part(EXTRACTION_DIR"/images/pledit.png", 227, 149, 22, 18, EXTRACTION_DIR"/images/playlist_playlist_load_on.png");
   

   free(image_file);

   //nums_ex.bmp or numbers.bmp
   if ((image_file = get_case_filename("nums_ex.bmp", root_dir)))
   {
      extract_result &= extract_image_part(image_file, 0, 0, 108, 13, EXTRACTION_DIR"/images/nums_ex.png");
      extract_result &= extract_image_part(EXTRACTION_DIR"/images/nums_ex.png", 0, 0, 9, 13, EXTRACTION_DIR"/images/num_0.png");
      extract_result &= extract_image_part(EXTRACTION_DIR"/images/nums_ex.png", 9, 0, 9, 13, EXTRACTION_DIR"/images/num_1.png");
      extract_result &= extract_image_part(EXTRACTION_DIR"/images/nums_ex.png", 18, 0, 9, 13, EXTRACTION_DIR"/images/num_2.png");
      extract_result &= extract_image_part(EXTRACTION_DIR"/images/nums_ex.png", 27, 0, 9, 13, EXTRACTION_DIR"/images/num_3.png");
      extract_result &= extract_image_part(EXTRACTION_DIR"/images/nums_ex.png", 36, 0, 9, 13, EXTRACTION_DIR"/images/num_4.png");
      extract_result &= extract_image_part(EXTRACTION_DIR"/images/nums_ex.png", 45, 0, 9, 13, EXTRACTION_DIR"/images/num_5.png");
      extract_result &= extract_image_part(EXTRACTION_DIR"/images/nums_ex.png", 54, 0, 9, 13, EXTRACTION_DIR"/images/num_6.png");
      extract_result &= extract_image_part(EXTRACTION_DIR"/images/nums_ex.png", 63, 0, 9, 13, EXTRACTION_DIR"/images/num_7.png");
      extract_result &= extract_image_part(EXTRACTION_DIR"/images/nums_ex.png", 72, 0, 9, 13, EXTRACTION_DIR"/images/num_8.png");
      extract_result &= extract_image_part(EXTRACTION_DIR"/images/nums_ex.png", 81, 0, 9, 13, EXTRACTION_DIR"/images/num_9.png");
      extract_result &= extract_image_part(EXTRACTION_DIR"/images/nums_ex.png", 90, 0, 9, 13, EXTRACTION_DIR"/images/num_none.png");
      extract_result &= extract_image_part(EXTRACTION_DIR"/images/nums_ex.png", 99, 0, 9, 13, EXTRACTION_DIR"/images/num_minus.png");
      free(image_file);
   }
   else if ((image_file = get_case_filename("numbers.bmp", root_dir)))
   {
      extract_result &= extract_image_part(image_file, 0, 0, 99, 13, EXTRACTION_DIR"/images/numbers.png");
      extract_result &= extract_image_part(EXTRACTION_DIR"/images/numbers.png", 0, 0, 9, 13, EXTRACTION_DIR"/images/num_0.png");
      extract_result &= extract_image_part(EXTRACTION_DIR"/images/numbers.png", 9, 0, 9, 13, EXTRACTION_DIR"/images/num_1.png");
      extract_result &= extract_image_part(EXTRACTION_DIR"/images/numbers.png", 18, 0, 9, 13, EXTRACTION_DIR"/images/num_2.png");
      extract_result &= extract_image_part(EXTRACTION_DIR"/images/numbers.png", 27, 0, 9, 13, EXTRACTION_DIR"/images/num_3.png");
      extract_result &= extract_image_part(EXTRACTION_DIR"/images/numbers.png", 36, 0, 9, 13, EXTRACTION_DIR"/images/num_4.png");
      extract_result &= extract_image_part(EXTRACTION_DIR"/images/numbers.png", 45, 0, 9, 13, EXTRACTION_DIR"/images/num_5.png");
      extract_result &= extract_image_part(EXTRACTION_DIR"/images/numbers.png", 54, 0, 9, 13, EXTRACTION_DIR"/images/num_6.png");
      extract_result &= extract_image_part(EXTRACTION_DIR"/images/numbers.png", 63, 0, 9, 13, EXTRACTION_DIR"/images/num_7.png");
      extract_result &= extract_image_part(EXTRACTION_DIR"/images/numbers.png", 72, 0, 9, 13, EXTRACTION_DIR"/images/num_8.png");
      extract_result &= extract_image_part(EXTRACTION_DIR"/images/numbers.png", 81, 0, 9, 13, EXTRACTION_DIR"/images/num_9.png");
      extract_result &= extract_image_part(EXTRACTION_DIR"/images/numbers.png", 90, 0, 9, 13, EXTRACTION_DIR"/images/num_none.png");
      extract_result &= extract_image_part(EXTRACTION_DIR"/images/numbers.png", 20, 6, 5, 1, EXTRACTION_DIR"/images/num_minus.png");
      free(image_file);
   }
   else
   {
      printf("Error: File \"%s\" is not a valid winamp skin: can\'t find file num_ex.bmp or numbers.bmp\n", argv[1]);
      free(root_dir);
      return 1;
   }

   //titlebar.bmp
   if (!(image_file = get_case_filename("eqmain.bmp", root_dir)))
   {
      printf("Error: File \"%s\" is not a valid winamp skin: can\'t find file eqmain.bmp\n", argv[1]);
      free(root_dir);
      return 1;
   }
   extract_result &= extract_image_part(image_file, 0, 0, 275, 293, EXTRACTION_DIR"/images/eqmain.png");
   extract_result &= extract_image_part(EXTRACTION_DIR"/images/eqmain.png", 0, 0, 275, 116, EXTRACTION_DIR"/images/equalizer_main.png");
   extract_result &= extract_image_part(EXTRACTION_DIR"/images/eqmain.png", 0, 164, 11, 11, EXTRACTION_DIR"/images/equalizer_drag.png");
   extract_result &= extract_image_part(EXTRACTION_DIR"/images/eqmain.png", 0, 176, 11, 11, EXTRACTION_DIR"/images/equalizer_drag_down.png");
   extracted_file = malloc(strlen(EXTRACTION_DIR"/images/equalizer_bar_.png") + 3);
   for (i = 0; i < 28; i++)
   {
      sprintf(extracted_file, EXTRACTION_DIR"/images/equalizer_bar_%d.png", i);
      extract_result &= extract_image_part(EXTRACTION_DIR"/images/eqmain.png", 13 + (i % 14) * 15, 164 + (i / 14) * 65, 14, 63, extracted_file);
   }
   free(extracted_file);
   free(image_file);

   if (!extract_result)
   {
      printf("Error: an error happened while extracting the image parts\n");
      printf("Maybe it\'s because you do not have \"montage\" from ImageMagick\n");
      free(root_dir);
      return 1;
   }
   printf("Image parts extracted\n\n");


   //Decompress the edc template
   printf("Decompressing edc template...\n");
   if (system("tar -xzf \"" PACKAGE_DATA_DIR "/wsz2edj/wsz2edj_edc.tar.gz\" -C \"" EXTRACTION_DIR "\"") != 0)
   {
      printf("Error: Unable to decompress the edc template \"" PACKAGE_DATA_DIR "/wsz2edj/wsz2edj_edc.tar.gz\"\n");
      free(root_dir);
      return 1;
   }
   printf("Edc template decompressed\n\n");

   //Read skin infos
   printf("Reading skin infos...\n");
   if (!(text_file = get_case_filename("pledit.txt", root_dir)))
   {
      printf("Error: File \"%s\" is not a valid winamp skin: can\'t find file pledit.txt\n", argv[1]);
      free(root_dir);
      return 1;
   }
   if (!read_skin_infos(text_file));
   free(text_file);
   printf("Skin infos read\n\n");

   //Compile the edje theme
   command = malloc(strlen("edje_cc -id \"" EXTRACTION_DIR "/images\" -fd \"" EXTRACTION_DIR "/edje/fonts\" \"" EXTRACTION_DIR "/edje/main.edc\" \"\"") +
      strlen(argv[2]) + 1);
   sprintf(command, "edje_cc -id \"" EXTRACTION_DIR "/images\" -fd \"" EXTRACTION_DIR "/edje/fonts\" \"" EXTRACTION_DIR "/edje/main.edc\" \"%s\"", argv[2]);
   if (system(command) != 0)
   {
      printf("Error: Unable to compile the edje theme\n");
      free(command);
      return 1;
   }

   printf("Conversion has been a success\n");
   printf("The \"wsz2edj_temp\" is not needed anymore, you can remove it\n");
   printf("You can now launch eclair with this command:\neclair -t %s\n", argv[2]);

   free(command);
   free(root_dir);

   return 0;
}
