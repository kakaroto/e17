#ifndef EWL_ENGINE_EVAS_BUFFER_H
#define EWL_ENGINE_EVAS_BUFFER_H

#define EWL_ENGINE_EVAS_BUFFER(engine) \
		((Ewl_Engine_Evas_Buffer *)engine)

typedef struct Ewl_Engine_Evas_Buffer Ewl_Engine_Evas_Buffer;
struct Ewl_Engine_Evas_Buffer
{
	Ewl_Engine engine;
};

#endif

