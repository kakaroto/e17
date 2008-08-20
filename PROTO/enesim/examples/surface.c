#include "enesim_test.h"

Enesim_Surface * surface_new(int w, int h, Enesim_Surface_Format fmt)
{
	Enesim_Surface *s = NULL;
	Enesim_Surface_Data sdata;
	
	switch(fmt)
	{
		case ENESIM_SURFACE_ARGB8888:
		sdata.argb8888.plane0 = calloc(1, sizeof(unsigned int) * w * h);
		s = enesim_surface_new_data_from(fmt, w, h, &sdata);
		break;

		case ENESIM_SURFACE_ARGB8888_UNPRE:
		sdata.argb8888_unpre.plane0 = calloc(1, sizeof(unsigned int) * w * h);
		s = enesim_surface_new_data_from(fmt, w, h, &sdata);
		break;
	
		case ENESIM_SURFACE_RGB565_XA5:
		sdata.rgb565_xa5.plane0 = calloc(1, sizeof(unsigned short int) * w * h);
		sdata.rgb565_xa5.plane1 = calloc(1, sizeof(unsigned char) * w * h);
		s = enesim_surface_new_data_from(fmt, w, h, &sdata);
		break;
		
		default:
		break;
	}
	return s;
}

void surface_free(Enesim_Surface *s)
{
	Enesim_Surface_Format fmt;
	Enesim_Surface_Data sdata;
	
	fmt = enesim_surface_format_get(s);
	enesim_surface_data_get(s, &sdata);
	
	switch(fmt)
	{
		case ENESIM_SURFACE_ARGB8888:
		free(sdata.argb8888.plane0);
		break;
	
		case ENESIM_SURFACE_ARGB8888_UNPRE:
		free(sdata.argb8888_unpre.plane0);
		break;
		
		case ENESIM_SURFACE_RGB565_XA5:
		free(sdata.rgb565_xa5.plane0);
		free(sdata.rgb565_xa5.plane1);
		break;
		
		default:
		break;
	}
	free(s);
}

void surface_blt(Enesim_Surface *s, SDL_Surface *sdl)
{
	SDL_Surface *tmp;
	Enesim_Surface_Data sdata;
	Enesim_Surface_Format fmt;
	void *data;
	unsigned int amask, rmask, gmask, bmask, pitch, bpp;  
	int w, h;
	
	enesim_surface_size_get(s, &w, &h);
	enesim_surface_data_get(s, &sdata);
	fmt = enesim_surface_format_get(s);
	switch (fmt)
	{
		
		case ENESIM_SURFACE_ARGB8888:
		amask = 0xff000000;
		rmask = 0x00ff0000;
		gmask = 0x0000ff00;
		bmask = 0x000000ff;
		pitch = 4 * w;
		bpp = 32;
		data = malloc(sizeof(unsigned int) * w * h);
		/* TODO we have to fix this with the converter? */
		{
			unsigned int argb;
			int i;
			unsigned int *ptr = data;
			
			for (i = 0; i < w * h; i++)
			{
				argb = enesim_surface_data_to_argb(&sdata, fmt);
				enesim_surface_data_increment(&sdata, fmt, 1);
				*ptr = argb;
				ptr++;
			}
			
		}
		break;
		
		case ENESIM_SURFACE_ARGB8888_UNPRE:
		amask = 0xff000000;
		rmask = 0x00ff0000;
		gmask = 0x0000ff00;
		bmask = 0x000000ff;
		pitch = 4 * w;
		bpp = 32;
		data = sdata.argb8888_unpre.plane0;
		break;
		
		case ENESIM_SURFACE_RGB565_XA5:
		amask = 0x00000000;
		rmask = 0x0000f800;
		gmask = 0x000007e0;
		bmask = 0x0000001f;
		pitch = 2 * w;
		bpp = 16;
		data = sdata.rgb565_xa5.plane0;
		break;

		default:
		return;
	}
	tmp = SDL_CreateRGBSurfaceFrom(data, w, h, bpp, pitch, rmask, gmask, bmask, amask);
	SDL_BlitSurface(tmp, NULL, sdl, NULL);
	SDL_FreeSurface(sdl);
}
