#ifndef ENGINE_XRENDER_XCB_H
#define ENGINE_XRENDER_XCB_H

Eina_Bool engine_xrender_xcb_args(const char *engine, int width, int height);
void engine_xrender_xcb_loop(void);
void engine_xrender_xcb_shutdown(void);

#endif
