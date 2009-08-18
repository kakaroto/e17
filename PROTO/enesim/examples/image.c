#include "enesim_bench_common.h"
#include <png.h>
#include <setjmp.h>

#define PNG_BYTES_TO_CHECK 4

void image_save(Enesim_Surface *s, const char *file, int compress)
{
	FILE *f;
	int y;
	int w, h;
	uint32_t *sdata;
	uint32_t *data;

	png_structp png_ptr;
	png_infop info_ptr;
	png_bytep row_ptr, png_data = NULL;
	png_color_8 sig_bit;

	Enesim_Converter_Data cdata;
	Enesim_Converter_1D conv;

	f = fopen(file, "wb");
	if (!f)
		return;

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
		return;
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
		return;
	}
	cdata.argb8888.plane0 = data;
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

	return;

error_jmp:
	png_destroy_info_struct(png_ptr, (png_infopp)&info_ptr);
error_info:
	png_destroy_write_struct(&png_ptr, (png_infopp)&info_ptr);
error_ptr:
	fclose(f);
	return;
}
