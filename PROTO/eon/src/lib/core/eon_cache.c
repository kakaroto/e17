/* EON - Canvas and Toolkit library
 * Copyright (C) 2008-2009 Jorge Luis Zapata
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2.1 of the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with this library.
 * If not, see <http://www.gnu.org/licenses/>.
 */
#include <string.h>

#include "Eon.h"
#include "eon_private.h"

/* Disable the eshm backend for now until we find a good allocator
 * which can receive a memory chunk and good LUT algorithm
 */

#ifdef BUILD_CACHE_ESHM
#undef BUILD_CACHE_ESHM
#endif

#ifdef BUILD_CACHE_ESHM
#include "Eshm.h"
#endif
/*============================================================================*
 *                                  Local                                     *
 *============================================================================*/

typedef struct _Eon_Cache_Image_Load
{
	Emage_Load_Callback cb;
	void *data;
	char key[1024];
} Eon_Cache_Image_Load;

typedef struct _Eon_Cache_Image
{
	int w;
	int h;
	Enesim_Format f;
	void *data;
} Eon_Cache_Image;


typedef struct _Eon_Cache
{
	Eina_Mempool *pool;
#ifdef BUILD_CACHE_ESHM
	int count;
	Eshm_Segment *lut;
	Eshm_Segment *data;
#else
	Eina_Hash *elements;
#endif
} Eon_Cache;

static Eon_Cache *_images;

#ifdef BUILD_CACHE_ESHM
/* TODO add this function to ESHM */
static Eshm_Segment * _segment_get(const char *name, size_t size)
{
	Eshm_Segment *s;

	s = eshm_segment_new(name, size);
	/* if the segment already exists request it */
	if (!s)
	{
		s = eshm_segment_get("eon:images");
		if (!s)
			printf("[Eon_Cache] Image segment can't be created or retrieved\n");
	}
}
#endif

Eon_Cache * eon_cache_image_get(void)
{
	static Eon_Cache *c = NULL;
	if (!c)
	{
#ifdef BUILD_CACHE_ESHM
		Eina_Mempool *mp;
		Eshm_Segment *s;
#endif
		c = malloc(sizeof(Eon_Cache));
		if (!c)
			return NULL;
#ifdef BUILD_CACHE_ESHM
		/* initialize eshm */
		eshm_init();
		c->data = _segment_get("eon:images:data", 64 * 1024 * 1024);
		c->lut = _segment_get("eon:images:lut", sizeof(Eon_Cache_Image) * 256);
		//c->pool = eina_mempool_new("pass_through", NULL, NULL);
#else
		c->elements = eina_hash_string_superfast_new(NULL);
		c->pool = eina_mempool_new("pass_through", NULL, NULL);
#endif
	}
	return c;
}

static void _loader_callback(Enesim_Surface *s, void *data, int error)
{
	Eon_Cache_Image_Load *l = data;

	//printf("Callback called\n");
	/* store the image metadata in case everything went ok */
	if (!error)
	{

		Eon_Cache *c;

		c = eon_cache_image_get();
		//printf("Storing image with key %s on the cache\n", l->key);
#ifdef BUILD_CACHE_ESHM
#else
		Eon_Cache_Image *cim;

		cim = calloc(1, sizeof(Eon_Cache_Image));

		enesim_surface_size_get(s, &cim->w, &cim->h);
		cim->data = enesim_surface_data_get(s);
		cim->f = enesim_surface_format_get(s);
		eina_hash_add(c->elements, l->key, cim);
#endif
	}
	//printf("OUTSIDE %p %p %p\n", l, l->data, l->cb);
	/* TODO call the real callback registered */
	l->cb(s, l->data, error);
	free(l);
}

/*============================================================================*
 *                                 Global                                     *
 *============================================================================*/
void eon_cache_init(void)
{
#ifdef BUILD_CACHE_ESHM
	_image_init();
#endif
}

void eon_cache_shutdown(void)
{
#ifdef BUILD_CACHE_ESHM
	/* shutdown eshm */
	eshm_shutdown();
#endif
}


/* FIXME
 * if we receive a request A for file F and before the thread completes
 * the request we receive another request B for the same file
 * this creates two entries on the cache which is wrong!
 */
void eon_cache_image_load(const char *file, Enesim_Surface **s,
		Enesim_Format f, Emage_Load_Callback cb, void *data,
		const char *options)
{
	Eon_Cache *c;
	Eon_Cache_Image *im;
	Eon_Cache_Image_Load *il;
	char key[1024];

	printf("[Eon_Cache] Trying to load image at %s\n", file);
	c = eon_cache_image_get();
	/* TODO create the key based on more properties */
	strncpy(key, file, 1024);
#ifdef BUILD_CACHE_ESHM
#else
	printf("Searching for image %s on the cache\n", key);
	im = eina_hash_find(c->elements, key);
	if (im)
	{
		printf("[Eon_Cache] Image found on the cache\n");
		/* create the surface from external data */
		*s = enesim_surface_new_data_from(im->w, im->h, im->data);
		cb(*s, data, 0);
		return;
	}
#endif

	il = malloc(sizeof(Eon_Cache_Image_Load));
	il->cb = cb;
	il->data = data;
	strncpy(il->key, key, 1024);

	/* here instead of calling directly the async function
	 * first store the im on the hash with state LOADING and register
	 * the same il on it */
	printf("[Eon_Cache] Image not found\n");
	emage_load_async(file, s, f, c->pool, _loader_callback, il, options);
}
/*============================================================================*
 *                                   API                                      *
 *============================================================================*/

