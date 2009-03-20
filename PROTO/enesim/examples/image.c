#include "enesim_test.h"

#define PNG_BYTES_TO_CHECK 4

/* TODO
 * fix all this mess */
void image_load(Enesim_Surface *s, const char *file)
{
	png_uint_32 w32, h32;
	int w, h;
	FILE *f;
	png_structp png_ptr = NULL;
	png_infop info_ptr = NULL;
	int bit_depth, color_type, interlace_type;
	unsigned char buf[PNG_BYTES_TO_CHECK];
	unsigned char **lines;
	char hasa, hasg;
	int i;
	Enesim_Surface_Data sdata;

	if ((!file)) return;
	hasa = 0;
	hasg = 0;
	f = fopen(file, "rb");
	if (!f) return;

	/* if we havent read the header before, set the header data */
	fread(buf, 1, PNG_BYTES_TO_CHECK, f);
	if (!png_check_sig(buf, PNG_BYTES_TO_CHECK))
	{
		fclose(f);
		return;
	}
	rewind(f);
	png_ptr = png_create_read_struct(PNG_LIBPNG_VER_STRING, NULL, NULL, NULL);
	if (!png_ptr)
	{
		fclose(f);
		return;
	}
	info_ptr = png_create_info_struct(png_ptr);
	if (!info_ptr)
	{
		png_destroy_read_struct(&png_ptr, NULL, NULL);
		fclose(f);
		return;
	}
	if (setjmp(png_ptr->jmpbuf))
	{
		png_destroy_read_struct(&png_ptr, &info_ptr, NULL);
		fclose(f);
		return;
	}
	png_init_io(png_ptr, f);
	png_read_info(png_ptr, info_ptr);
	png_get_IHDR(png_ptr, info_ptr, (png_uint_32 *) (&w32),
		(png_uint_32 *) (&h32), &bit_depth, &color_type,
		&interlace_type, NULL, NULL);
	enesim_surface_size_get(s, &w, &h);
	if ((w32 != w) || (h32 != h))
	{
		png_destroy_read_struct(&png_ptr, &info_ptr, (png_infopp) NULL);
		fclose(f);
		return;
	}
	if (color_type == PNG_COLOR_TYPE_PALETTE) png_set_expand(png_ptr);
	if (info_ptr->color_type == PNG_COLOR_TYPE_RGB_ALPHA) hasa = 1;
	if (info_ptr->color_type == PNG_COLOR_TYPE_GRAY_ALPHA)
	{
		hasa = 1;
		hasg = 1;
	}
	if (info_ptr->color_type == PNG_COLOR_TYPE_GRAY) hasg = 1;
	//if (hasa) im->flags |= RGBA_IMAGE_HAS_ALPHA;

	if (hasa) png_set_expand(png_ptr);
	/* we want ARGB */
#ifdef WORDS_BIGENDIAN
	png_set_swap_alpha(png_ptr);
	png_set_filler(png_ptr, 0xff, PNG_FILLER_BEFORE);
#else
	png_set_bgr(png_ptr);
	png_set_filler(png_ptr, 0xff, PNG_FILLER_AFTER);
#endif
	/* 16bit color -> 8bit color */
	png_set_strip_16(png_ptr);
	/* pack all pixels to byte boundaires */
	png_set_packing(png_ptr);
	if (png_get_valid(png_ptr, info_ptr, PNG_INFO_tRNS))
		png_set_expand(png_ptr);
	/*evas_common_image_surface_alloc(im->image);
	if (!im->image->data)
	{
		evas_common_image_surface_free(im->image);
		png_destroy_read_struct(&png_ptr, &info_ptr, (png_infopp) NULL);
		fclose(f);
		return 0;
	}*/
	lines = (unsigned char **) alloca(h * sizeof(unsigned char *));

	if (hasg)
	{
		png_set_gray_to_rgb(png_ptr);
		if (png_get_bit_depth(png_ptr, info_ptr) < 8)
			png_set_gray_1_2_4_to_8(png_ptr);
	}
	enesim_surface_data_get(s, &sdata);
	for (i = 0; i < h; i++)
		lines[i] = ((unsigned char *)(sdata.plane0)) + (i * w * sizeof(uint32_t));
	png_read_image(png_ptr, lines);
	png_read_end(png_ptr, info_ptr);
	png_destroy_read_struct(&png_ptr, &info_ptr, (png_infopp) NULL);
	fclose(f);
        //enesim_color_data_argb_premul(sdata, w * h);
}

void image_save(Enesim_Surface *s, const char *file, int compress)
{
	FILE *f;
	int num_passes = 1, pass;
	int x, y, j;
	int w, h;
	Enesim_Surface_Data esdata;
	uint32_t *ptr, *data;

	png_structp         png_ptr;
	png_infop           info_ptr;
	png_bytep row_ptr, png_data = NULL;
	png_color_8 sig_bit;

	f = fopen(file, "wb");
	if (!f) return;

	png_ptr = png_create_write_struct(PNG_LIBPNG_VER_STRING, NULL, NULL, NULL);
	if (!png_ptr)
		goto error_ptr;

	info_ptr  = png_create_info_struct(png_ptr);
	if (!info_ptr)
		goto error_info;

	if (setjmp(png_ptr->jmpbuf))
		goto error_jmp;

//	if (s->flags & RGBA_SURFACE_HAS_ALPHA)
	{
		enesim_surface_data_get(s, &esdata);
		enesim_surface_size_get(s, &w, &h);
		data = malloc(w * h * sizeof(uint32_t));
        if (!data)
          {
            fclose(f);
            png_destroy_write_struct(&png_ptr, (png_infopp) & info_ptr);
            png_destroy_info_struct(png_ptr, (png_infopp) & info_ptr);
            return;
          }
	memcpy(data, esdata.plane0, w * h * sizeof(uint32_t));
        //enesim_color_data_argb_unpremul(data, w * h);
        png_init_io(png_ptr, f);
        png_set_IHDR(png_ptr, info_ptr, w, h, 8,
                     PNG_COLOR_TYPE_RGB_ALPHA, png_ptr->interlaced,
                     PNG_COMPRESSION_TYPE_BASE, PNG_FILTER_TYPE_BASE);

#ifdef WORDS_BIGENDIAN
		png_set_swap_alpha(png_ptr);
#else
		png_set_bgr(png_ptr);
#endif
	}
#if 0
	else
	{
        data = s->data;
        png_init_io(png_ptr, f);
        png_set_IHDR(png_ptr, info_ptr, s->w, s->h, 8,
                     PNG_COLOR_TYPE_RGB, png_ptr->interlaced,
                     PNG_COMPRESSION_TYPE_BASE, PNG_FILTER_TYPE_BASE);
        png_data = alloca(s->w * 3 * sizeof(char));

	}
#endif
	sig_bit.red = 8;
	sig_bit.green = 8;
	sig_bit.blue = 8;
	sig_bit.alpha = 8;
	png_set_sBIT(png_ptr, info_ptr, &sig_bit);

	png_set_compression_level(png_ptr, compress);
	png_write_info(png_ptr, info_ptr);
	png_set_shift(png_ptr, &sig_bit);
	png_set_packing(png_ptr);
	for (pass = 0; pass < num_passes; pass++)
	{
		ptr = data;

        for (y = 0; y < h; y++)
          {
            // if (s->flags & RGBA_SURFACE_HAS_ALPHA)
               row_ptr = (png_bytep) ptr;
	     #if 0
             else
               {
                  for (j = 0, x = 0; x < s->w; x++)
                    {
                       png_data[j++] = (ptr[x] >> 16) & 0xff;
                       png_data[j++] = (ptr[x] >> 8) & 0xff;
                       png_data[j++] = (ptr[x]) & 0xff;
                    }
                  row_ptr = (png_bytep) png_data;
               }
	       #endif
             png_write_rows(png_ptr, &row_ptr, 1);
             ptr += w;
          }


	}
	png_write_end(png_ptr, info_ptr);
	png_destroy_write_struct(&png_ptr, (png_infopp) & info_ptr);
	png_destroy_info_struct(png_ptr, (png_infopp) & info_ptr);
	//if (s->flags & RGBA_SURFACE_HAS_ALPHA)
	free(data);
	fclose(f);
	return;
error_jmp:
	png_destroy_info_struct(png_ptr, (png_infopp)&info_ptr);
error_info:
	png_destroy_write_struct(&png_ptr, (png_infopp)&info_ptr);
error_ptr:
	fclose(f);
	return;
}
