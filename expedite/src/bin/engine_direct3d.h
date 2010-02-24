#ifndef __ENGINE_DIRECT3D_H__
#define __ENGINE_DIRECT3D_H__


#ifdef __cplusplus
extern "C" {
#endif


Eina_Bool engine_direct3d_args(const char *engine, int width, int height);
void engine_direct3d_loop(void);
void engine_direct3d_shutdown(void);


#ifdef __cplusplus
}
#endif


#endif /* __ENGINE_DIRECT3D_H__ */
