#ifndef ENGINE_XRENDER_X11_H
#define ENGINE_XRENDER_X11_H

Eina_Bool engine_xrender_x11_args(const char *engine, int width, int height);
void engine_xrender_x11_loop(void);
void engine_xrender_x11_shutdown(void);

#endif
