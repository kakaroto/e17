#ifdef HAVE_CONFIG_H
# include <config.h>
#endif

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <fcntl.h>
#include <assert.h>
#include <errno.h>

#include "image.h"

#include <id3tag.h>

int extract_pic (struct id3_frame* frame, int dest, char* ext, int extlen)
{
	union id3_field* field;
	unsigned char const * data;
	long length;
	int done = 0;

	field = id3_frame_field (frame, 1);
	data = id3_field_getlatin1 (field);
	if (! data) {
		fprintf (stderr, "No mime type data found for image frame\n");
		return 0;
	}
	if (strncmp (data, "image/", 6)) {
		fprintf (stderr,
			 "Picture frame with unknown mime-type %s found\n",
			 data);
		return 0;
	}
	strncpy (ext, data + 6, extlen);
	field = id3_frame_field (frame, 4);
	data = id3_field_getbinarydata (field, &length);
	if (! data) {
		fprintf (stderr, "No image data found for frame\n");
		return 0;
	}
	while (length > 0) {
		ssize_t res;
		if ((res = write (dest, data + done, length)) < 0) {
			if (errno == EINTR)
				continue;
			perror ("Unable to write to file");
			return 0;
		}
		length -= res;
		done += res;
	}
	return 1;
}

/* Loader for ID3v2 tags in audio files.
 * ID3v2 allows for 'Attached Picture' frames to be embedded in the file.
 * A numeric key is supported, and indicates the zero-based frame index
 * to be extracted, in case more than one such frame is found.
 * Defaults to 0, or the first picture frame.
 */
char load (ImlibImage *im, ImlibProgressFunction progress,
           char progress_granularity, char immediate_load)
{
	ImlibLoader *loader;
	char *file, tmp[] = "/tmp/imlib2_loader_id3-XXXXXX", *p;
	char real_ext[16];
	int res, dest, pic_index = 0;
	struct id3_file* tagfile;
	struct id3_tag* tag;
	struct id3_frame* frame;

	assert (im);

	p = strrchr(im->real_file, '.');
	if (! (p && p != im->real_file && !strcmp (p + 1, "mp3"))) {
		return 0;
	}

	if (im->key)
		pic_index = atoi (im->key);

	tagfile = id3_file_open (im->real_file, ID3_FILE_MODE_READONLY);
	if (! tagfile) {
		fprintf (stderr, "Unable to open tagged file %s: %s",
			 im->real_file, strerror (errno));
		return 0;
	}
	tag = id3_file_tag (tagfile);
	if (! tag) {
		fprintf (stderr, "Unable to find ID3v2 tags in file\n");
		return 0;
	}
	frame = id3_tag_findframe (tag, "APIC", pic_index);
	if (! frame) {
		fprintf (stderr, "No picture frame # %d found\n", pic_index);
		return 0;
	}

	if ((dest = mkstemp (tmp)) < 0) {
		fprintf (stderr, "Unable to create a temporary file\n");
		id3_file_close (tagfile);
		return 0;
	}

	real_ext[15] = '\0';
	real_ext[0] = '.';
	res = extract_pic (frame, dest, real_ext + 1, 14);
	close (dest);
	id3_file_close (tagfile);

	if (!res) {
		unlink (tmp);
		return 0;
	}

	if (!(loader = __imlib_FindBestLoaderForFile (real_ext, 0))) {
		fprintf (stderr, "No loader found for extension %s\n", real_ext);
		unlink (tmp);
		return 0;
	}

	/* remember the original filename */
	file = strdup (im->real_file);

	free (im->real_file);
	im->real_file = strdup (tmp);
	loader->load (im, progress, progress_granularity, immediate_load);

	free (im->real_file);
	im->real_file = file;
	unlink (tmp);

	return 1;
}

void formats (ImlibLoader *l)
{
	/* this is the only bit you have to change... */
	char *list_formats[] = {"mp3"};
	int i;

	/* don't bother changing any of this - it just reads this in
	 * and sets the struct values and makes copies
	 */
	l->num_formats = sizeof (list_formats) / sizeof (char *);
	l->formats = (char**) malloc (sizeof (char *) * l->num_formats);

	for (i = 0; i < l->num_formats; i++)
		l->formats[i] = strdup (list_formats[i]);
}
