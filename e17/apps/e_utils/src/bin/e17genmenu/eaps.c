#include "global.h"
#include "config.h"
#include "icons.h"
#include "eaps.h"
#include "parse.h"

#define DEBUG 1

static void _write_eap(Eet_File *ef, char *section, char *value);


/* Create a .directory.eap for this dir */
void
create_dir_eap(char *path, char *cat)
{
   char path2[MAX_PATH];
   G_Eap eap;

   memset(&eap, 0, sizeof(G_Eap));
   eap.name = cat;
   snprintf(path2, sizeof(path2), "%s/.directory.eap", path);
   if (!ecore_file_exists(path2))
     {
        eap.icon_path = set_icon(cat);
        if (!eap.icon_path)
          {
             fprintf(stderr, "ERROR: Cannot Find Icon For %s\n", cat);
             return;
          }
        write_icon(path2, &eap);
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
write_icon(char *file, G_Eap *eap)
{
   Engrave_File *eet;
   Engrave_Image *image;
   Engrave_Group *grp;
   Engrave_Part *part;
   Engrave_Part_State *ps;
   Eet_File *ef;

   char *idir, *ifile, *icomp, *exec;

#ifdef DEBUG
   fprintf(stderr, "\tWriting file %s\n", file);
   fprintf(stderr, "\t\tIcon %s\n", eap->icon_path);
#endif
   /* FIXME: This does not seem to be catching all the problems.  Further head scratching is needed. */
   if ((!eap->icon_path) || (eap->icon_path[0] == '\0'))
      eap->icon_path = DEFAULTICON;
   ifile = ecore_file_get_file(eap->icon_path);
   idir = ecore_file_get_dir(eap->icon_path);

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

   engrave_edj_output(eet, file);
   engrave_file_free(eet);

//   if (icomp)
//      free(icomp);
//   if (idir)
//      free(idir);
//   if (ifile)
//      free(ifile);

   /* FIXME: This is probably why creating eaps takes so long.  
    * We should just create it right in the first place, rather 
    * than creating a basic one, then rewriting it several times
    * each time we find a new little bit of info.
    */
   ef = eet_open(file, EET_FILE_MODE_READ_WRITE);
   if (ef)
   {
        /* Set Eap Values. Trap For Name Not Being Set */
        if (eap->name != NULL)
           _write_eap(ef, "app/info/name", eap->name);
        else if (eap->eap_name != NULL)
           _write_eap(ef, "app/info/name", eap->eap_name);

        if (eap->generic != NULL)
           _write_eap(ef, "app/info/generic", eap->generic);
        if (eap->comment != NULL)
           _write_eap(ef, "app/info/comments", eap->comment);

        /* Parse Exec string for %'s that messup eap write */
        exec = NULL;
        if (eap->exec != NULL)
          {
             exec = parse_exec(eap->exec);
             if (exec != NULL)
               {
                  _write_eap(ef, "app/info/exe", exec);
                  _write_eap(ef, "app/icon/class", exec);
               }
          }

        if (eap->startup != NULL)
           _write_eap(ef, "app/info/startup_notify", eap->startup);
        if (eap->window_class != NULL)
           _write_eap(ef, "app/window/class", eap->window_class);
      eet_close(ef);
   }
}


static void
_write_eap(Eet_File *ef, char *section, char *value)
{
   int i;

#ifdef DEBUG
   fprintf(stderr, "\t\t%s:%s\n", strdup(section), strdup(value));
#endif

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
             i = eet_write(ef, strdup(section), strdup(value), strlen(value), 0);
             if (i == 0)
                fprintf(stderr, "Failed To Write %s To %s\n", value, section);
          }
     }
}
