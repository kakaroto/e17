#include "global.h"
#include "config.h"
#include "icons.h"
#include "eaps.h"

/* Create a .directory.eap for this dir */
void
create_dir_eap(char *path, char *cat)
{
   char path2[MAX_PATH];
   char *icon;

   snprintf(path2, sizeof(path2), "%s/.directory.eap", path);
   if (!ecore_file_exists(path2))
     {
        icon = set_icon(cat);
        if (!icon)
          {
             fprintf(stderr, "ERROR: Cannot Find Icon For %s\n", cat);
             return;
          }
        write_icon(path2, icon);
        write_eap(path2, "app/info/name", cat);
     }
}

char *
get_window_class(char *file)
{
   char *tmp, *cls;
   int i;
   Eet_File *ef;

   if (!ecore_file_exists(file))
      return NULL;

   ef = eet_open(file, EET_FILE_MODE_READ);
   if (!ef)
      return NULL;

   tmp = eet_read(ef, "app/window/class", &i);
   if (!tmp)
     {
        if (ef)
           eet_close(ef);
        return NULL;
     }

   /* Allocate string for window class */
   cls = malloc(i + 1);
   memcpy(cls, tmp, i);
   cls[i] = 0;

   if (tmp)
      free(tmp);
   eet_close(ef);

   if (cls != NULL)
      return strdup(cls);
   return NULL;
}

void
write_icon(char *f, char *i)
{
   Engrave_File *eet;
   Engrave_Image *image;
   Engrave_Group *grp;
   Engrave_Part *part;
   Engrave_Part_State *ps;

   char *idir, *ifile, *icomp;

#ifdef DEBUG
   fprintf(stderr, "\tWriting Icon %s\n", i);
#endif
   if (!i)
      ifile = ecore_file_get_file(DEFAULTICON);
   if (i)
      ifile = ecore_file_get_file(i);

   idir = ecore_file_get_dir(i);

   eet = engrave_file_new();
   engrave_file_image_dir_set(eet, idir);

   icomp = get_icon_compression();
   if (!strcmp(icomp, "COMP"))
      image = engrave_image_new(ifile, ENGRAVE_IMAGE_TYPE_COMP, 0);
   else
      image = engrave_image_new(ifile, ENGRAVE_IMAGE_TYPE_LOSSY, 0);

   engrave_file_image_add(eet, image);

   grp = engrave_group_new();
   engrave_group_name_set(grp, "icon");
   engrave_group_max_size_set(grp, 48, 48);
   engrave_file_group_add(eet, grp);

   part = engrave_part_new(ENGRAVE_PART_TYPE_IMAGE);
   engrave_part_name_set(part, "image");
   engrave_part_mouse_events_set(part, 0);
   engrave_group_part_add(grp, part);

   ps = engrave_part_state_new();
   engrave_part_state_name_set(ps, "default", 0.0);
   engrave_part_state_aspect_set(ps, 1.0, 1.0);
   engrave_part_state_image_normal_set(ps, image);
   engrave_part_state_add(part, ps);

   engrave_edj_output(eet, f);
   engrave_file_free(eet);

   if (icomp)
      free(icomp);
   if (idir)
      free(idir);
   if (ifile)
      free(ifile);
}

void
write_eap(char *file, char *section, char *value)
{
   int i;
   Eet_File *ef;

#ifdef DEBUG
   fprintf(stderr, "\tWriting %s:%s\n", strdup(section), strdup(value));
#endif

   ef = eet_open(file, EET_FILE_MODE_READ_WRITE);
   if (!ef)
      return;

   if (!strcmp(section, "app/info/startup_notify"))
     {
        if (!value)
           eet_delete(ef, section);
        if (value)
          {
             i = atoi(value);
             eet_write(ef, strdup(section), &i, 1, 0);
          }
     }
   else
     {
        if (!value)
           eet_delete(ef, section);
        if (value)
          {
             i = eet_write(ef, strdup(section), strdup(value), strlen(value),
                           0);
             if (i == 0)
                fprintf(stderr, "Failed To Write %s To %s Of %s\n", value,
                        section, file);
          }
     }
   eet_close(ef);
}
