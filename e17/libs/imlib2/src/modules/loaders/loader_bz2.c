#ifdef HAVE_CONFIG_H
# include <config.h>
#endif

#include <stdio.h>
#include <bzlib.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <assert.h>

#include "common.h"
#include "image.h"

#define OUTBUF_SIZE 16384
#define INBUF_SIZE 1024

static int uncompress_file (FILE *fp, int dest)
{
	BZFILE *bf;
	DATA8 outbuf[OUTBUF_SIZE];
	int bytes, error;

	bf = BZ2_bzReadOpen (&error, fp, 0, 0, NULL, 0);

	if (error != BZ_OK) {
		BZ2_bzReadClose (NULL, bf);
		return 0;
	}

	error = BZ_OK;

	while (error == BZ_OK) {
		bytes = BZ2_bzRead (&error, bf, &outbuf, OUTBUF_SIZE);

		if (error == BZ_OK)
			write (dest, outbuf, bytes);
	}

	BZ2_bzReadClose (&error, bf);

	return 1;
}

char load (ImlibImage *im, ImlibProgressFunction progress,
           char progress_granularity, char immediate_load)
{
	ImlibLoader *loader;
	FILE *fp;
	int dest, res;
	char *file, tmp[] = "/tmp/imlib2_loader_bz2-XXXXXX", *p;

	assert (im);

	/* we'll need a copy of it later */
	file = im->real_file;
	p = strrchr(im->real_file, '.');
	if (p) {
		if (strcasecmp(p + 1, "bz2")) return 0;
	}
	else
		return 0;
	if (!(fp = fopen (im->real_file, "rb"))) {
		return 0;
	}

	if ((dest = mkstemp (tmp)) < 0) {
		fclose (fp);
		return 0;
	}

	res = uncompress_file (fp, dest);
	fclose (fp);
	close (dest);

	if (!res) {
		unlink (tmp);
		return 0;
	}

	if (!(loader = __imlib_FindBestLoaderForFile (tmp, 0))) {
		unlink (tmp);
		return 0;
	}

	free (im->real_file);
	im->real_file = strdup (tmp);
	loader->load (im, progress, progress_granularity, immediate_load);

	free (im->real_file);
	im->real_file = strdup (file);
	unlink (tmp);

	return 1;
}

void formats (ImlibLoader *l)
{
	/* this is the only bit you have to change... */
	char *list_formats[] = {"bz2"};
	int i;

   /* don't bother changing any of this - it just reads this in
	* and sets the struct values and makes copies
	*/
	l->num_formats = sizeof (list_formats) / sizeof (char *);
	l->formats = malloc (sizeof (char *) * l->num_formats);

	for (i = 0; i < l->num_formats; i++)
		l->formats[i] = strdup (list_formats[i]);
}
