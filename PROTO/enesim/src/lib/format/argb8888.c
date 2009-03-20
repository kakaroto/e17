#include "Enesim.h"
#include "enesim_private.h"
/*============================================================================*
 *                                  Local                                     *
 *============================================================================*/
static void _create(Enesim_Surface_Data *d, uint32_t w, uint32_t h)
{
	d->plane0 = calloc(w * h, sizeof(uint32_t));
}

static void _delete(Enesim_Surface_Data *d)
{
	free(d->plane0);
}

static void _increment(Enesim_Surface_Data *d, uint32_t len)
{
	d->plane0 += len;
}

static uint32_t _argb_to(Enesim_Surface_Data *d)
{
	return *d->plane0;
}

static void _argb_from(Enesim_Surface_Data *d, uint32_t argb)
{
	*d->plane0 = argb;
}

static uint32_t _pixel_argb_to(Enesim_Surface_Pixel *p)
{
	return p->plane0;
}

static void _pixel_argb_from(Enesim_Surface_Pixel *p, uint32_t argb)
{
	p->plane0 = argb;
}

static void _pixel_get(Enesim_Surface_Data *d, Enesim_Surface_Pixel *p)
{
	p->plane0 = *d->plane0;
}

static void _pixel_set(Enesim_Surface_Data *d, Enesim_Surface_Pixel *p)
{
	*d->plane0 = p->plane0;
}

static Enesim_Format _backend = {
	.name = "argb8888",
	.create = _create,
	.delete = _delete,
	.increment = _increment,
	.argb_from = _argb_from,
	.argb_to = _argb_to,
	.pixel_argb_from = _pixel_argb_from,
	.pixel_argb_to = _pixel_argb_to,
	.pixel_get = _pixel_get,
	.pixel_set = _pixel_set,
	.alpha = { 24, 8, 0 },
	.red = { 16, 8, 0 },
	.green = { 8, 8, 0 },
	.blue = { 0, 8, 0 },
	.premul = EINA_TRUE,
};
/*============================================================================*
 *                                 Global                                     *
 *============================================================================*/
Eina_Bool enesim_format_argb8888_init(void)
{
	return enesim_format_register(&_backend);
}

void enesim_format_argb8888_shutdown(void)
{
	//enesim_format_unregister(&_backend);
}
/*============================================================================*
 *                                   API                                      *
 *============================================================================*/
EAPI Enesim_Format * enesim_format_argb8888_get(void)
{
	static Enesim_Format *f = NULL;

	if (!f)
	{
		Enesim_Format_Component a, r, g, b;

		a.offset = 24, a.length = 8, a.plane = 0;
		r.offset = 16, r.length = 8, r.plane = 0;
		g.offset = 8, g.length = 8, g.plane = 0;
		b.offset = 0, b.length = 8, b.plane = 0;

		f = enesim_format_get(&a, &r, &g, &b, EINA_TRUE);
	}
	return f;
}
