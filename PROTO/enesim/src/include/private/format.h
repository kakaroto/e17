#ifndef FORMAT_H_
#define FORMAT_H_

void enesim_format_init(void);
void enesim_format_shutdown(void);

Eina_Bool enesim_format_argb8888_init(void);
void enesim_format_argb8888_shutdown(void);

#if BUILD_SURFACE_ARGB8888_UNPRE
Eina_Bool enesim_format_argb8888_unpre_init(void);
void enesim_format_argb8888_unpre_shutdown(void);
#endif

#endif /* FORMAT_H_ */
