#include <stdio.h>

#include "Emage.h"
#include "png.h"
/*============================================================================*
 *                                  Local                                     *
 *============================================================================*/
/*
 * FIX ALL OF THIS
 */
/* number to checks for magic png info */
#define PNG_BYTES_TO_CHECK 4
/*============================================================================*
 *                          Emage Provider API                                *
 *============================================================================*/
Eina_Bool _png_loadable(const char *file)
{
	FILE *f;
	unsigned char buf[PNG_BYTES_TO_CHECK];
	int ret;

	f = fopen(file, "rb");
	ret = fread(buf, 1, PNG_BYTES_TO_CHECK, f);
	if (ret < 0)
		return EINA_FALSE;
	if (!png_check_sig(buf, PNG_BYTES_TO_CHECK))
	{
		fclose(f);
		return EINA_FALSE;
	}
	return EINA_TRUE;
}

Eina_Bool _png_info_load(const char *file, int *w, int *h, Enesim_Converter_Format *sfmt)
{
	Enesim_Surface *s;
	FILE *f;
	int bit_depth, color_type, interlace_type;
	char hasa, hasg;
	int i;

	png_uint_32 w32, h32;
	png_structp png_ptr = NULL;
	png_infop info_ptr = NULL;

	Eina_Bool ret = EINA_TRUE;

	hasa = 0;
	hasg = 0;
	f = fopen(file, "rb");
	if (!f)
	{
		eina_error_set(EMAGE_ERROR_EXIST);
		return EINA_FALSE;
	}

	rewind(f);
	png_ptr = png_create_read_struct(PNG_LIBPNG_VER_STRING, NULL, NULL,
	NULL);
	if (!png_ptr)
	{
		fclose(f);
		return EINA_FALSE;
	}
	info_ptr = png_create_info_struct(png_ptr);
	if (!info_ptr)
	{
		png_destroy_read_struct(&png_ptr, NULL, NULL);
		fclose(f);
		return EINA_FALSE;
	}
	if (setjmp(png_ptr->jmpbuf))
	{
		png_destroy_read_struct(&png_ptr, &info_ptr, NULL);
		fclose(f);
		return EINA_FALSE;
	}
	png_init_io(png_ptr, f);
	png_read_info(png_ptr, info_ptr);
	png_get_IHDR(png_ptr, info_ptr, (png_uint_32 *) (&w32), (png_uint_32 *) (&h32), &bit_depth, &color_type, &interlace_type, NULL, NULL);
	if (w) *w = w32;
	if (h) *h = h32;
	if (sfmt) *sfmt = ENESIM_CONVERTER_ARGB8888;
}

Eina_Bool _png_load(const char *file, void *data)
{
	FILE *f;
	int bit_depth, color_type, interlace_type;
	unsigned char buf[PNG_BYTES_TO_CHECK];
	unsigned char **lines;
	char hasa, hasg;
	int i;

	png_uint_32 w32, h32;
	png_structp png_ptr = NULL;
	png_infop info_ptr = NULL;

	uint32_t *sdata = data;
	Eina_Bool ret = EINA_TRUE;

	hasa = 0;
	hasg = 0;
	f = fopen(file, "rb");
	if (!f)
	{
		eina_error_set(EMAGE_ERROR_EXIST);
		return EINA_FALSE;
	}
	/* if we havent read the header before, set the header data */
	fread(buf, 1, PNG_BYTES_TO_CHECK, f);
	if (!png_check_sig(buf, PNG_BYTES_TO_CHECK))
	{
		fclose(f);
		return EINA_FALSE;
	}
	rewind(f);
	png_ptr = png_create_read_struct(PNG_LIBPNG_VER_STRING, NULL, NULL,
			NULL);
	if (!png_ptr)
	{
		fclose(f);
		return EINA_FALSE;
	}
	info_ptr = png_create_info_struct(png_ptr);
	if (!info_ptr)
	{
		png_destroy_read_struct(&png_ptr, NULL, NULL);
		fclose(f);
		return EINA_FALSE;
	}
	if (setjmp(png_ptr->jmpbuf))
	{
		png_destroy_read_struct(&png_ptr, &info_ptr, NULL);
		fclose(f);
		return EINA_FALSE;
	}
	png_init_io(png_ptr, f);
	png_read_info(png_ptr, info_ptr);
	png_get_IHDR(png_ptr, info_ptr, (png_uint_32 *) (&w32),
			(png_uint_32 *) (&h32), &bit_depth, &color_type,
			&interlace_type, NULL, NULL);

	if (color_type == PNG_COLOR_TYPE_PALETTE)
		png_set_expand(png_ptr);
	if (info_ptr->color_type == PNG_COLOR_TYPE_RGB_ALPHA)
		hasa = 1;
	if (info_ptr->color_type == PNG_COLOR_TYPE_GRAY_ALPHA)
	{
		hasa = 1;
		hasg = 1;
	}
	if (info_ptr->color_type == PNG_COLOR_TYPE_GRAY)
		hasg = 1;
	if (hasa)
		png_set_expand(png_ptr);
	/* TODO check surface format, ARGB8888 for now */
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

	lines = (unsigned char **) alloca(h32 * sizeof(unsigned char *));

	if (hasg)
	{
		png_set_gray_to_rgb(png_ptr);
		if (png_get_bit_depth(png_ptr, info_ptr) < 8)
			png_set_gray_1_2_4_to_8(png_ptr);
	}
	/* setup the pointers */
	for (i = 0; i < h32; i++)
	{
		lines[i] = ((unsigned char *)(sdata)) + (i * w32
				* sizeof(uint32_t));
	}
	png_read_image(png_ptr, lines);
	png_read_end(png_ptr, info_ptr);

err_setup:
	png_destroy_read_struct(&png_ptr, &info_ptr, (png_infopp) NULL);
	fclose(f);

	return ret;
}

Eina_Bool _png_save(void)
{
#if 0
	FILE *f;
	int num_passes = 1, pass;
	int x, y, j;
	int w, h;
	Enesim_Surface_Data esdata;
	uint32_t *ptr, *data;

	png_structp png_ptr;
	png_infop info_ptr;
	png_bytep row_ptr, png_data = NULL;
	png_color_8 sig_bit;

	f = fopen(file, "wb");
	if (!f) return;

	png_ptr = png_create_write_struct(PNG_LIBPNG_VER_STRING, NULL, NULL, NULL);
	if (!png_ptr)
	goto error_ptr;

	info_ptr = png_create_info_struct(png_ptr);
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
		memcpy(data, esdata.argb8888_unpre.plane0, w * h * sizeof(uint32_t));
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
#endif
}

static Emage_Provider _provider = {
	.name = "png",
	.type = EMAGE_PROVIDER_SW,
	.load = _png_load,
	.info_get = _png_info_load,
	.loadable = _png_loadable,
};
/*============================================================================*
 *                             Module API                                     *
 *============================================================================*/
Eina_Bool png_provider_init(void)
{
	return emage_provider_register(&_provider);
}

void png_provider_shutdown(void)
{
	printf("shutdown\n");
	emage_provider_unregister(&_provider);
}

EINA_MODULE_INIT(png_provider_init);
EINA_MODULE_SHUTDOWN(png_provider_shutdown);

