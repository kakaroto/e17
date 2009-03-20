#ifndef ENESIM_FORMAT_H_
#define ENESIM_FORMAT_H_

typedef struct Enesim_Format_Component
{
	uint8_t offset;
	uint8_t length;
	uint8_t plane;
} Enesim_Format_Component;

/**
 * The format definition
 * TODO handle YUV formats too
 * TODO all this functions should go away, there's no point on using them on
 * any algo as it is slow as hell, just provide inline versions
 */
struct _Enesim_Format
{
	const char *name;
	void (*create)(Enesim_Surface_Data *d, uint32_t w, uint32_t h);
	void (*delete)(Enesim_Surface_Data *d);
	void (*increment)(Enesim_Surface_Data *d, uint32_t len);
	uint32_t (*argb_to)(Enesim_Surface_Data *d);
	void (*argb_from)(Enesim_Surface_Data *d, uint32_t argb);
	uint32_t (*pixel_argb_to)(Enesim_Surface_Pixel *p);
	void (*pixel_argb_from)(Enesim_Surface_Pixel *p, uint32_t argb);
	void (*pixel_get)(Enesim_Surface_Data *d, Enesim_Surface_Pixel *p);
	void (*pixel_set)(Enesim_Surface_Data *d, Enesim_Surface_Pixel *p);
	Enesim_Format_Component alpha;
	Enesim_Format_Component red;
	Enesim_Format_Component green;
	Enesim_Format_Component blue;
	Eina_Bool premul;
};

extern Enesim_Format ENESIM_FORMAT_ARGB8888; /** The basic format of Enesim */

EAPI const char * enesim_format_name_get(Enesim_Format *f);
EAPI Eina_Iterator * enesim_format_iterator_new(void);
EAPI Eina_Bool enesim_format_register(Enesim_Format *f);
EAPI Enesim_Format * enesim_format_get(Enesim_Format_Component *a,
		Enesim_Format_Component *r, Enesim_Format_Component *g,
		Enesim_Format_Component *b, Eina_Bool premul);
EAPI Enesim_Format * enesim_format_argb8888_get(void);
EAPI Enesim_Format * enesim_format_argb8888_unpre_get(void);
EAPI Enesim_Format * enesim_format_a8_get(void);


#endif /* ENESIM_FORMAT_H_ */
