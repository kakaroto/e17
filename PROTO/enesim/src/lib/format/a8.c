#include "Enesim.h"
#include "enesim_private.h"
/*============================================================================*
 *                                  Local                                     *
 *============================================================================*/
static void _create(Enesim_Surface_Data *d, uint32_t w, uint32_t h)
{
	d->plane0 = calloc(w * h, sizeof(uint8_t));
}

static void _delete(Enesim_Surface_Data *d)
{
	free(d->plane0);
}

static void _increment(Enesim_Surface_Data *d, uint32_t len)
{
	uint8_t *p = (uint8_t *)d->plane0;
	d->plane0 = (uint32_t *)(p + len);
}

static uint32_t _argb_to(Enesim_Surface_Data *d)
{
	uint8_t a;

	a = *(uint8_t *)d->plane0;
	return (a << 24);
}

static void _argb_from(Enesim_Surface_Data *d, uint32_t argb)
{
	uint8_t *a;

	a = (uint8_t *)d->plane0;
	*a  = (argb >> 24);
}

static uint32_t _pixel_argb_to(Enesim_Surface_Pixel *p)
{
	uint8_t a;

	a = (uint8_t)p->plane0;
	return (a << 24);
}

static void _pixel_argb_from(Enesim_Surface_Pixel *p, uint32_t argb)
{
	uint8_t *a;

	a = (uint8_t *)&p->plane0;
	*a  = (argb >> 24);
}

static void _pixel_get(Enesim_Surface_Data *d, Enesim_Surface_Pixel *p)
{
	uint8_t *a;

	a = (uint8_t *)&p->plane0;
	*a = *(uint8_t *)d->plane0;
}

static void _pixel_set(Enesim_Surface_Data *d, Enesim_Surface_Pixel *p)
{
	*(uint8_t *)d->plane0 = (uint8_t)p->plane0;
}

static Enesim_Format _backend = {
	.name = "a8",
	.create = _create,
	.delete = _delete,
	.increment = _increment,
	.argb_from = _argb_from,
	.argb_to = _argb_to,
	.pixel_argb_from = _pixel_argb_from,
	.pixel_argb_to = _pixel_argb_to,
	.pixel_get = _pixel_get,
	.pixel_set = _pixel_set,
	.alpha = { 0, 8, 0 },
	.premul = EINA_FALSE,
};
/*============================================================================*
 *                                 Global                                     *
 *============================================================================*/
Eina_Bool enesim_format_a8_init(void)
{
	return enesim_format_register(&_backend);
}

void enesim_format_a8_shutdown(void)
{
	//enesim_format_unregister(&_backend);
}
/*============================================================================*
 *                                   API                                      *
 *============================================================================*/
EAPI Enesim_Format * enesim_format_a8_get(void)
{
	static Enesim_Format *f = NULL;

	if (!f)
	{
		Enesim_Format_Component a;

		a.offset = 0, a.length = 8, a.plane = 0;

		f = enesim_format_get(&a, NULL, NULL, NULL, EINA_FALSE);
	}
	return f;
}
