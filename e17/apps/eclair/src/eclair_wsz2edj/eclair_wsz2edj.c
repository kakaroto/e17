#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <dirent.h>
#include <sys/types.h>
#include <sys/stat.h>
#include "../../config.h"

#define EXTRACTION_DIR "wsz2edj_temp"

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
//Return NULL if not found
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
   snprintf(command, len, "montage \"%s\" -gravity NorthWest -transparent \"#d32b2b\" -crop %dx%d+%d+%d -geometry %dx%d -background none \"%s\"",
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

//Convert the skin!
int main(int argc, char *argv[])
{
   struct stat st;
   char *root_dir;
   char *image_file;
   char *command;
   int extract_result;

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
   extract_result &= extract_image_part(EXTRACTION_DIR"/images/volume.png", 0, 0, 68, 13, EXTRACTION_DIR"/images/volume_bar.png");
   extract_result &= extract_image_part(EXTRACTION_DIR"/images/volume.png", 15, 422, 14, 11, EXTRACTION_DIR"/images/volume_bar_drag.png");
   extract_result &= extract_image_part(EXTRACTION_DIR"/images/volume.png", 0, 422, 14, 11, EXTRACTION_DIR"/images/volume_bar_drag_down.png");
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
   printf("Decompressing edc template\n");
   if (system("tar -xzf \"" PACKAGE_DATA_DIR "/wsz2edj/wsz2edj_edc.tar.gz\" -C \"" EXTRACTION_DIR "\"") != 0)
   {
      printf("Error: Unable to decompress the edc template \"" PACKAGE_DATA_DIR "/wsz2edj/wsz2edj_edc.tar.gz\"\n");
      free(root_dir);
      return 1;
   }
   printf("Edc template decompressed\n\n");

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
   printf("You can now launch eclair with this command:\neclair -t %s\n", argv[2]);

   free(command);
   free(root_dir);

   return 0;
}
