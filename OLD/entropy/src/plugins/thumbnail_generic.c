#include <X11/Xlib.h>
#include <Imlib2.h>
#include <Ecore.h>
#include <stdio.h>
#include <stdlib.h>
#include <png.h>
#include "entropy.h"
#include "md5.h"
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <Epsilon.h>
#include <X11/Xlib.h>
#include <Ecore_X.h>


entropy_thumbnail *
entropy_thumbnail_new ()
{
  entropy_thumbnail *thumb = entropy_malloc (sizeof (entropy_thumbnail));

  thumb->text = ecore_hash_new (ecore_str_hash, ecore_str_compare);
  thumb->keys = 0;

  return thumb;
}

void
entropy_thumbnail_destroy (entropy_thumbnail * thumb)
{
  if (thumb) {
    if (thumb->text)
      ecore_hash_destroy (thumb->text);
    entropy_free (thumb);
    allocated_thumbnails--;

    print_allocation ();
  }
}

entropy_thumbnail *
entropy_thumbnail_create (entropy_generic_file * e_file)
{
  entropy_thumbnail *thumb_struct;
  char thumb_path_and_name[255];
  char *pos;
  Epsilon *e;

  strcpy (thumb_path_and_name, e_file->path);
  pos = thumb_path_and_name + strlen (thumb_path_and_name);
  strcpy (pos, "/");
  pos += 1;
  strcpy (pos, e_file->filename);

  e = epsilon_new (thumb_path_and_name);
  if (epsilon_exists (e) == EPSILON_FAIL) {
    epsilon_generate (e);
  }

  if (epsilon_exists (e)) {
    thumb_struct = entropy_thumbnail_new ();
    strcpy (thumb_struct->thumbnail_filename, epsilon_thumb_file_get (e));
    epsilon_free (e);

    return thumb_struct;
  }
  else {
    epsilon_free (e);
    return NULL;
  }


}
