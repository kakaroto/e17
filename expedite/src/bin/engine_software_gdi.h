#ifndef __ENGINE_SOFTWARE_GDI_H__
#define __ENGINE_SOFTWARE_GDI_H__


#ifdef __cplusplus
extern "C" {
#endif

Eina_Bool engine_software_gdi_args(const char *engine, int width, int height);
void engine_software_gdi_loop(void);
void engine_software_gdi_shutdown(void);


#ifdef __cplusplus
}
#endif


#endif /* __ENGINE_SOFTWARE_GDI_H__ */
