#ifndef _EON_ENESIM_H_
#define _EON_ENESIM_H_

typedef struct _Eon_Enesim_Paint Eon_Enesim_Paint;
typedef struct _Eon_Enesim_Layout Eon_Enesim_Layout;
typedef struct _Eon_Enesim_Buffer Eon_Enesim_Buffer;

struct _Eon_Enesim_Paint
{
	Enesim_Renderer *r;
	Eon_Paint *p;
	void (*style_setup)(Eon_Enesim_Paint *p, Eon_Enesim_Paint *rel);
};

struct _Eon_Enesim_Layout
{
	Eon_Enesim_Paint p;
	Enesim_Surface *s;
	void *data;
};

struct _Eon_Enesim_Buffer
{
	Eon_Enesim_Paint p;
	Enesim_Surface *s;
	Enesim_Renderer *imp;
};


EAPI void eon_engine_enesim_setup(Eon_Engine *e);
EAPI void eon_engine_enesim_cleanup(Eon_Engine *e);

Eon_Enesim_Layout * eon_engine_enesim_layout_create(Eon_Layout *el, int w, int h, void *data);
void eon_engine_enesim_layout_delete(Eon_Enesim_Layout *l);

#endif
