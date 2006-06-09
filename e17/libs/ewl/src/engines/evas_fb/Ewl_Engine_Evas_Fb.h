#ifndef EWL_ENGINE_EVAS_FB_H
#define EWL_ENGINE_EVAS_FB_H

#define EWL_ENGINE_EVAS_FB(engine) \
		((Ewl_Engine_Evas_Fb *)engine)

typedef struct Ewl_Engine_Evas_Fb Ewl_Engine_Evas_Fb;
struct Ewl_Engine_Evas_Fb
{
	Ewl_Engine engine;

	/* There can be only one window in fb land */
	Ewl_Window *window; 
}

#endif

