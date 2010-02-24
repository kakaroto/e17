#ifndef __ENGINE_SOFTWARE_16_DDRAW_H__
#define __ENGINE_SOFTWARE_16_DDRAW_H__


#ifdef __cplusplus
extern "C" {
#endif


Eina_Bool engine_software_16_ddraw_args(const char *engine, int width, int height);
void engine_software_16_ddraw_loop(void);
void engine_software_16_ddraw_shutdown(void);


#ifdef __cplusplus
}
#endif


#endif /* __ENGINE_SOFTWARE_16_DDRAW_H__ */
