#include "entropy.h"

Ecore_Hash *mime_hash;
int
entropy_plugin_type_get ()
{
  return ENTROPY_PLUGIN_MIME;
}

char *
entropy_plugin_identify ()
{
  return (char *) "Simple MIME identifier";
}

int
entropy_mime_plugin_priority_get ()
{
  return ENTROPY_MIME_PLUGIN_PRIORITY_HIGH;
}

Entropy_Plugin*
entropy_plugin_init (entropy_core * core)
{
  Entropy_Plugin_Mime* plugin;
	
  mime_hash = ecore_hash_new (ecore_str_hash, ecore_str_compare);
 

  ecore_hash_set (mime_hash, ".png", "image/png");
  ecore_hash_set (mime_hash, ".jpg", "image/jpeg");
  ecore_hash_set (mime_hash, ".gif", "image/gif");
  ecore_hash_set (mime_hash, ".pl", "text/x-perl");
  ecore_hash_set (mime_hash, ".wmv", "video/x-ms-wmv");
  ecore_hash_set (mime_hash, ".doc", "application/msword");
  ecore_hash_set (mime_hash, ".pdf", "application/pdf");
  ecore_hash_set (mime_hash, ".xls", "application/vnd.ms-excel");
  ecore_hash_set (mime_hash, ".gz", "application/x-gtar");
  ecore_hash_set (mime_hash, ".mp3", "audio/x-mp3");
  ecore_hash_set (mime_hash, ".java", "text/x-java");
  ecore_hash_set (mime_hash, ".jar", "application/x-jar");
  ecore_hash_set (mime_hash, ".xml", "text/xml");
  ecore_hash_set (mime_hash, ".htm", "text/html");
  ecore_hash_set (mime_hash, ".html", "text/html");
  ecore_hash_set (mime_hash, ".c", "text/csrc");
  ecore_hash_set (mime_hash, ".cpp", "text/x-c++src");
  ecore_hash_set (mime_hash, ".mpg", "video/mpeg");
  ecore_hash_set (mime_hash, ".mpe", "video/mpeg");
  ecore_hash_set (mime_hash, ".mpeg", "video/mpeg");
  ecore_hash_set (mime_hash, ".avi", "video/x-msvideo");
  ecore_hash_set (mime_hash, ".tgz", "application/x-gtar");
  ecore_hash_set (mime_hash, ".bz2", "application/x-bzip2");
  ecore_hash_set (mime_hash, ".tar", "application/x-tar");
  ecore_hash_set (mime_hash, ".txt", "text/plain");
  ecore_hash_set (mime_hash, ".mov", "video/quicktime");
  ecore_hash_set (mime_hash, ".asf", "video/x-ms-asf");
  ecore_hash_set (mime_hash, ".tga" ,"image/tga");
  ecore_hash_set (mime_hash, ".pdf", "application/pdf");

  plugin = entropy_malloc(sizeof(Entropy_Plugin_Mime));

  return ENTROPY_PLUGIN(plugin);
}

char *
entropy_mime_plugin_identify_file (char *path, char *filename)
{
  char fullname[1024];

  char *ifile;
  char *type = NULL;
  char *pos;
  int stat_res = 0;
  struct stat st;


  if (!filename || strlen (filename) == 0) {
    return NULL;
  }

  ifile = strdup (filename);
  entropy_core_string_lowcase (ifile);

  sprintf (fullname, "%s/%s", path, filename);


  stat_res = stat (fullname, &st);
  if (!stat_res && S_ISDIR (st.st_mode)) {
    /*printf("It's a folder..\n"); */
    entropy_free (ifile);
    return "file/folder";
  }



  /*Check if it's a folder.. */
  /*printf("Checking MIME %s\n", ifile);  */
  pos = rindex (ifile, '.');
  /*printf ("MIME: %s\n", pos); */
  if (pos) {
    /*printf("Finding extension %s\n", pos); */

    type = ecore_hash_get (mime_hash, pos);

  }
  free (ifile);
  return type;
}
