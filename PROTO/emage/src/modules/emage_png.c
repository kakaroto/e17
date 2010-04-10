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

static Eina_Bool _png_format_get(int color_type, Enesim_Converter_Format *fmt)
{
	switch (color_type)
	{
		case PNG_COLOR_TYPE_RGB:
		*fmt = ENESIM_CONVERTER_RGB888;
		break;

		case PNG_COLOR_TYPE_RGB_ALPHA:
		*fmt = ENESIM_CONVERTER_ARGB8888;
		break;

		case PNG_COLOR_TYPE_GRAY:
		*fmt = ENESIM_CONVERTER_A8;
		break;

		default:
		return EINA_FALSE;
	}

	return EINA_TRUE;
}
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

Eina_Bool _png_saveable(const char *file)
{
	char *d;

	d = strrchr(file, '.');
	if (!d) return EINA_FALSE;

	d++;
	if (!strcasecmp(d, "png"))
		return EINA_TRUE;

	return EINA_FALSE;
}

Eina_Error _png_info_load(const char *file, int *w, int *h, Enesim_Converter_Format *sfmt)
{
	Enesim_Surface *s;
	FILE *f;
	int bit_depth, color_type, interlace_type;
	char hasa, hasg;
	int i;

	png_uint_32 w32, h32;
	png_structp png_ptr = NULL;
	png_infop info_ptr = NULL;

	hasa = 0;
	hasg = 0;
	f = fopen(file, "rb");
	if (!f)
	{
		return EMAGE_ERROR_EXIST;
	}

	rewind(f);
	png_ptr = png_create_read_struct(PNG_LIBPNG_VER_STRING, NULL, NULL,
	NULL);
	if (!png_ptr)
	{
		fclose(f);
		return EMAGE_ERROR_LOADING;
	}
	info_ptr = png_create_info_struct(png_ptr);
	if (!info_ptr)
	{
		png_destroy_read_struct(&png_ptr, NULL, NULL);
		fclose(f);
		return EMAGE_ERROR_LOADING;
	}
	if (setjmp(png_ptr->jmpbuf))
	{
		png_destroy_read_struct(&png_ptr, &info_ptr, NULL);
		fclose(f);
		return EMAGE_ERROR_LOADING;
	}
	png_init_io(png_ptr, f);
	png_read_info(png_ptr, info_ptr);
	png_get_IHDR(png_ptr, info_ptr, (png_uint_32 *) (&w32),
			(png_uint_32 *) (&h32), &bit_depth, &color_type,
			&interlace_type, NULL, NULL);
	if (w) *w = w32;
	if (h) *h = h32;
	if (!sfmt)
		return EMAGE_ERROR_LOADING;

	if (!_png_format_get(color_type, sfmt))
	{
		return EMAGE_ERROR_LOADING;
	}

	return 0;
}

Eina_Error _png_load(const char *file, Enesim_Converter_Data *data)
{
	FILE *f;
	int bit_depth, color_type, interlace_type;
	unsigned char buf[PNG_BYTES_TO_CHECK];
	unsigned char **lines;
	char hasa, hasg;
	int i;
	Enesim_Converter_Format fmt;
	int pixel_inc;

	png_uint_32 w32, h32;
	png_structp png_ptr = NULL;
	png_infop info_ptr = NULL;

	uint32_t *sdata = data->pixels.argb8888.plane0;

	hasa = 0;
	hasg = 0;
	f = fopen(file, "rb");
	if (!f)
	{
		return EMAGE_ERROR_EXIST;
	}
	/* if we havent read the header before, set the header data */
	fread(buf, 1, PNG_BYTES_TO_CHECK, f);
	if (!png_check_sig(buf, PNG_BYTES_TO_CHECK))
	{
		fclose(f);
		return EMAGE_ERROR_LOADING;
	}
	rewind(f);
	png_ptr = png_create_read_struct(PNG_LIBPNG_VER_STRING, NULL, NULL,
			NULL);
	if (!png_ptr)
	{
		fclose(f);
		return EMAGE_ERROR_LOADING;
	}
	info_ptr = png_create_info_struct(png_ptr);
	if (!info_ptr)
	{
		png_destroy_read_struct(&png_ptr, NULL, NULL);
		fclose(f);
		return EMAGE_ERROR_LOADING;
	}
	if (setjmp(png_ptr->jmpbuf))
	{
		png_destroy_read_struct(&png_ptr, &info_ptr, NULL);
		fclose(f);
		return EMAGE_ERROR_LOADING;
	}
	png_init_io(png_ptr, f);
	png_read_info(png_ptr, info_ptr);
	png_get_IHDR(png_ptr, info_ptr, (png_uint_32 *) (&w32),
			(png_uint_32 *) (&h32), &bit_depth, &color_type,
			&interlace_type, NULL, NULL);

	if (!_png_format_get(color_type, &fmt))
	{
		png_destroy_read_struct(&png_ptr, &info_ptr, NULL);
		fclose(f);
		return EMAGE_ERROR_LOADING;
	}

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
	/* 16bit color -> 8bit color */
	png_set_strip_16(png_ptr);
	/* pack all pixels to byte boundaires */
	png_set_packing(png_ptr);
	if (png_get_valid(png_ptr, info_ptr, PNG_INFO_tRNS))
		png_set_expand(png_ptr);

	pixel_inc = enesim_converter_format_depth_get(fmt) / 8;
	if (!pixel_inc)
	{
		png_destroy_read_struct(&png_ptr, &info_ptr, (png_infopp) NULL);
		fclose(f);
		return EMAGE_ERROR_LOADING;
	}

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
				* pixel_inc);
	}
	png_read_image(png_ptr, lines);
	png_read_end(png_ptr, info_ptr);


	return 0;
}

Eina_Bool _png_save(const char *file, Enesim_Surface *s)
{
	FILE *f;
	int y;
	int w, h;
	uint32_t *sdata;
	uint32_t *data;
	/* FIXME fix this, it should be part of the options */
	const int compress = 0;

	png_structp png_ptr;
	png_infop info_ptr;
	png_bytep row_ptr, png_data = NULL;
	png_color_8 sig_bit;

	Enesim_Converter_Data cdata;
	Enesim_Converter_1D conv;

	f = fopen(file, "wb");
	if (!f)
		return EINA_FALSE;

	png_ptr = png_create_write_struct(PNG_LIBPNG_VER_STRING, NULL, NULL,NULL);
	if (!png_ptr)
		goto error_ptr;

	info_ptr = png_create_info_struct(png_ptr);
	if (!info_ptr)
		goto error_info;

	if (setjmp(png_ptr->jmpbuf))
		goto error_jmp;

	sdata = enesim_surface_data_get(s);
	enesim_surface_size_get(s, &w, &h);
	data = malloc(w * sizeof(uint32_t));
	if (!data)
	{
		fclose(f);
		png_destroy_write_struct(&png_ptr, (png_infopp) & info_ptr);
		png_destroy_info_struct(png_ptr, (png_infopp) & info_ptr);
		return EINA_FALSE;
	}
	png_init_io(png_ptr, f);
	png_set_IHDR(png_ptr, info_ptr, w, h, 8,
			PNG_COLOR_TYPE_RGB_ALPHA, png_ptr->interlaced,
			PNG_COMPRESSION_TYPE_BASE, PNG_FILTER_TYPE_BASE);
#ifdef WORDS_BIGENDIAN
	png_set_swap_alpha(png_ptr);
#else
	png_set_bgr(png_ptr);
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
	/* setup the operator */
	if (!(conv = enesim_converter_span_get(ENESIM_CONVERTER_ARGB8888, ENESIM_ANGLE_0, ENESIM_FORMAT_ARGB8888)))
	{
		printf("Error calling the converter\n");
		return EINA_FALSE;
	}
	cdata.pixels.argb8888.plane0 = data;
	row_ptr = (png_bytep) data;

	for (y = 0; y < h; y++)
	{
		conv(&cdata, w, sdata);
		png_write_rows(png_ptr, &row_ptr, 1);
		sdata += w;
	}
	png_write_end(png_ptr, info_ptr);
	png_destroy_write_struct(&png_ptr, (png_infopp) & info_ptr);
	png_destroy_info_struct(png_ptr, (png_infopp) & info_ptr);
	free(data);
	fclose(f);

	return EINA_TRUE;

error_jmp:
	png_destroy_info_struct(png_ptr, (png_infopp)&info_ptr);
error_info:
	png_destroy_write_struct(&png_ptr, (png_infopp)&info_ptr);
error_ptr:
	fclose(f);

	return EINA_FALSE;
}

static Emage_Provider _provider = {
	.name = "png",
	.type = EMAGE_PROVIDER_SW,
	.load = _png_load,
	.save = _png_save,
	.info_get = _png_info_load,
	.loadable = _png_loadable,
	.saveable = _png_saveable,
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
	emage_provider_unregister(&_provider);
}

EINA_MODULE_INIT(png_provider_init);
EINA_MODULE_SHUTDOWN(png_provider_shutdown);

