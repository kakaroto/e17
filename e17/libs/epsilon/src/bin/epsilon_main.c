#include<stdlib.h>
#include<stdio.h>
#include<string.h>
#include"Epsilon.h"
#include"../config.h"

int
main (int argc, char *argv[])
{
  Epsilon *e = NULL;

  epsilon_init ();
  while (--argc)
    {
      if ((e = epsilon_new ((const char *) argv[argc])))
	{
	  char *str;
	  str = strrchr (argv[argc], '.');
	  str++;
	  if (!strcmp (str, "edj"))
	    {
	      epsilon_key_set (e, "desktop/background");
	      //epsilon_resolution_set(e, 1600, 1200);        
	      epsilon_resolution_set (e, 800, 600);
	    }
	  if (epsilon_exists (e) == EPSILON_FAIL)
	    {
	      fprintf (stderr,
		       "Thumbnail for %s needs to be generated: ",
		       argv[argc]);
	      if (epsilon_generate (e) == EPSILON_OK)
		fprintf (stderr, "OK - thumb is %s\n", (char*)epsilon_thumb_file_get(e));
	      else
		fprintf (stderr, "FAILED\n");
	    }
	  else
	    {
	      Epsilon_Info *info;
	      fprintf (stderr, "\nThumbnail already exists\n%s\n",
		       epsilon_thumb_file_get (e));
	      fprintf (stderr, "Thumbnail already exists\n");
	      if ((info = epsilon_info_get (e)))
		{
		  if (info->uri)
		    printf ("URI: %s\n", info->uri);
		  if (info->mimetype)
		    printf ("MimeType: %s\n", info->mimetype);
		  printf ("Source Image Width: %d\n", info->w);
		  printf ("Source Image Height: %d\n", info->h);
		  printf ("Source Image Mtime: %d\n", (int) info->mtime);
		  fprintf (stderr, "Trying EXIF Info: ");
		  if (epsilon_info_exif_get (info))
		    {
		      fprintf (stderr, "Found!\n");
#if 0
		      fprintf (stderr, "%d is direction\n",
			       epsilon_exif_info_props_as_int_get (eei,
								   0x0112));
#endif
		      epsilon_info_exif_props_print (info);
		    }
		  else
		    {
		      fprintf (stderr, "Not Found!\n");
		    }
		  epsilon_info_free (info);
		}
	      else
		{
		  fprintf (stderr, "Meta Info Not Found!\n");
		}
	    }
	  epsilon_free (e);
	}
    }
  return (0);
}
