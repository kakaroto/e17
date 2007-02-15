#include "ephoto.h"

/*Get the pixels and return them in a string*/
char *image_pixels_string_get(const char *file)
{
        char pixels[PATH_MAX];
        int w, h;
        Evas_Imaging_Image *image;

        image = evas_imaging_image_load(file, NULL);
        evas_imaging_image_size_get(image, &w, &h);
        evas_imaging_image_free(image);

        snprintf(pixels, PATH_MAX, "%d x %d", w, h);

        return strdup(pixels);
}

/*Get the pixels and return them in integers*/
void image_pixels_int_get(const char *file, int *width, int *height)
{
        char pixels[PATH_MAX];
	int w, h;
	Evas_Imaging_Image *image;

        image = evas_imaging_image_load(file, NULL);
        evas_imaging_image_size_get(image, &w, &h);
        evas_imaging_image_free(image);
	
	*width = w;
	*height = h;

	return;
}

/*Get the size of a file*/
char *file_size_get(int size)
{
        double dsize;
        char fsize[256];

        dsize = (double)size;
        if (dsize < 1024.0) snprintf(fsize, sizeof(fsize), _("%'.0f Bytes"), dsize);
        else
        {
                dsize /= 1024.0;
                if (dsize < 1024) snprintf(fsize, sizeof(fsize), _("%'.0f KB"), dsize);
                else
                {
                        dsize /= 1024.0;
                        if (dsize < 1024) snprintf(fsize, sizeof(fsize), _("%'.0f MB"), dsize);
                        else
                        {
                                dsize /= 1024.0;
                                snprintf(fsize, sizeof(fsize), _("%'.1f GB"), dsize);
                        }
                }
        }
        return strdup(fsize);
}
