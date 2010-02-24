#ifndef ENGINE_SOFTWARE_SDL_H
#define ENGINE_SOFTWARE_SDL_H

Eina_Bool engine_software_sdl_args(const char *engine, int width, int height);
void engine_software_sdl_loop(void);
void engine_software_sdl_shutdown(void);

#endif
