#include <assert.h>
#include "main.h"

#include <SDL/SDL.h>
#include <Evas_Engine_SDL.h>
#include <Evas_Engine_Buffer.h>

static void *
_expedite_sdl_switch_buffer(void *data, void *dest __UNUSED__)
{
   SDL_Flip(data);
   return ((SDL_Surface*)data)->pixels;
}

Eina_Bool
engine_software_sdl_args(const char *engine, int width, int height)
{
   int ok = 0;

   if (!strcmp(engine, "sdl")) ok = 1;
   if (!strcmp(engine, "sdl-16")) ok = 2;
   if (!ok) return EINA_FALSE;

   if (ok == 1)
     {
        Evas_Engine_Info_Buffer *einfo;

        evas_output_method_set(evas, evas_render_method_lookup("buffer"));

        einfo = (Evas_Engine_Info_Buffer *) evas_engine_info_get(evas);
        if (einfo)
          {
             SDL_Init(SDL_INIT_NOPARACHUTE);

             if (SDL_InitSubSystem(SDL_INIT_VIDEO) < 0)
               {
                  printf("SDL_Init failed with %s", SDL_GetError());
                  SDL_Quit();
                  return EINA_FALSE;
               }

             einfo->info.depth_type = EVAS_ENGINE_BUFFER_DEPTH_RGB32;
             einfo->info.switch_data = SDL_SetVideoMode(width, height, 32, SDL_HWSURFACE | SDL_DOUBLEBUF);
             if (!einfo->info.switch_data)
               {
                  printf("SDL_SetVideoMode failed !");
                  return EINA_FALSE;
               }

             SDL_SetAlpha(einfo->info.switch_data, SDL_SRCALPHA, 0);
             SDL_FillRect(einfo->info.switch_data, NULL, 0);

             einfo->info.dest_buffer = ((SDL_Surface*)einfo->info.switch_data)->pixels;
             einfo->info.dest_buffer_row_bytes = width * sizeof (int);
             einfo->info.use_color_key = 0;
             einfo->info.alpha_threshold = 0;
             einfo->info.func.new_update_region = NULL;
             einfo->info.func.free_update_region = NULL;
             einfo->info.func.switch_buffer = _expedite_sdl_switch_buffer;
             if (!evas_engine_info_set(evas, (Evas_Engine_Info *) einfo))
               {
                  printf("evas_engine_info_set() for engine 'sdl' with 'buffer' backend failed.");
                  return EINA_FALSE;
               }
          }
     }
   else
     {
        Evas_Engine_Info_SDL *einfo;

        evas_output_method_set(evas, evas_render_method_lookup("software_16_sdl"));

        einfo = (Evas_Engine_Info_SDL *) evas_engine_info_get(evas);

        /* the following is specific to the engine */
        einfo->info.fullscreen = fullscreen;
        einfo->info.noframe = 0;

        if (!evas_engine_info_set(evas, (Evas_Engine_Info *) einfo))
          {
             printf("Evas can not setup the informations of the Software SDL Engine\n");
             return EINA_FALSE;
          }
     }

   return EINA_TRUE;
}

void
engine_software_sdl_loop(void)
{
   SDL_Event event;
   int rmethod;

   while(SDL_PollEvent(&event))
     {
        switch(event.type)
          {
          case SDL_MOUSEMOTION:
             evas_event_feed_mouse_move(evas, event.motion.x, event.motion.y, 0, NULL);
             break;
          case SDL_MOUSEBUTTONDOWN:
             evas_event_feed_mouse_move(evas, event.button.x, event.button.y, 0, NULL);
             evas_event_feed_mouse_down(evas, event.button.button, EVAS_BUTTON_NONE, 0, NULL);
             break;
          case SDL_MOUSEBUTTONUP:
             evas_event_feed_mouse_move(evas, event.button.x, event.button.y, 0, NULL);
             evas_event_feed_mouse_up(evas, event.button.button, EVAS_BUTTON_NONE, 0, NULL);
             break;
          case SDL_VIDEORESIZE:
             rmethod = evas_output_method_get(evas);
             if (rmethod == evas_render_method_lookup("buffer"))
               {
                  Evas_Engine_Info_Buffer *einfo;

                  einfo = (Evas_Engine_Info_Buffer *) evas_engine_info_get(evas);
                  if (einfo)
                    {
                       einfo->info.depth_type = EVAS_ENGINE_BUFFER_DEPTH_RGB32;
                       einfo->info.switch_data = SDL_SetVideoMode(event.resize.w, event.resize.h, 32, SDL_HWSURFACE | SDL_DOUBLEBUF);
                       if (!einfo->info.switch_data)
                         {
                            return ;
                         }

                       SDL_SetAlpha(einfo->info.switch_data, SDL_SRCALPHA, 0);
                       SDL_FillRect(einfo->info.switch_data, NULL, 0);

                       einfo->info.dest_buffer = ((SDL_Surface*)einfo->info.switch_data)->pixels;
                       einfo->info.dest_buffer_row_bytes = event.resize.w * sizeof (int);
                       einfo->info.use_color_key = 0;
                       einfo->info.alpha_threshold = 0;
                       einfo->info.func.new_update_region = NULL;
                       einfo->info.func.free_update_region = NULL;
                       einfo->info.func.switch_buffer = _expedite_sdl_switch_buffer;
                       if (!evas_engine_info_set(evas, (Evas_Engine_Info *) einfo))
                         {
                            return ;
                         }
                    }
               }

             evas_output_viewport_set(evas, 0, 0,
                                      event.resize.h, event.resize.w);
             evas_output_size_set(evas, event.resize.h, event.resize.w);
             evas_output_size_get(evas, &win_w, &win_h);
             break;
          case SDL_VIDEOEXPOSE:
             evas_output_size_get(evas, &win_w, &win_h);
             evas_damage_rectangle_add(evas, 0, 0, win_w, win_h);
             break;
          case SDL_KEYDOWN:
             switch (event.key.keysym.sym)
               {
               case SDLK_LSHIFT:
               case SDLK_RSHIFT:
                  evas_key_modifier_on(evas, "Shift");
                  break;
               case SDLK_RCTRL:
               case SDLK_LCTRL:
                  evas_key_modifier_on(evas, "Control");
                  break;
               case SDLK_MENU:
               case SDLK_RALT:
               case SDLK_LALT:
                  evas_key_modifier_on(evas, "Alt");
                  break;
               case SDLK_LSUPER:
               case SDLK_RSUPER:
                  evas_key_modifier_on(evas, "Super");
                  break;
               case SDLK_CAPSLOCK:
                  if (evas_key_lock_is_set(evas_key_lock_get(evas), "Caps_Lock"))
                    evas_key_lock_off(evas, "Caps_Lock");
                  else
                    evas_key_lock_on(evas, "Caps_Lock");
                  break;
               case SDLK_NUMLOCK:
                  if (evas_key_lock_is_set(evas_key_lock_get(evas), "Num_Lock"))
                    evas_key_lock_off(evas, "Num_Lock");
                  else
                    evas_key_lock_on(evas, "Num_Lock");
                  break;
               case SDLK_SCROLLOCK:
                  if (evas_key_lock_is_set(evas_key_lock_get(evas), "Scroll_Lock"))
                    evas_key_lock_off(evas, "Scroll_Lock");
                  else
                    evas_key_lock_on(evas, "Scroll_Lock");
                  break;
               case SDLK_s:
               case SDLK_ESCAPE:
                  evas_event_feed_key_down(evas, "Escape", "Escape", NULL, NULL, 0, NULL);
                  break;
               case SDLK_KP_ENTER:
               case SDLK_RETURN:
                  evas_event_feed_key_down(evas, "Return", "Return", NULL, NULL, 0, NULL);
                  break;
               case SDLK_LEFT: evas_event_feed_key_down(evas, "Left", "Left", NULL, NULL, 0, NULL); break;
               case SDLK_RIGHT: evas_event_feed_key_down(evas, "Right", "Right", NULL, NULL, 0, NULL); break;
               case SDLK_q:
                  if (evas_key_lock_is_set(evas_key_lock_get(evas), "Caps_Lock"))
                    evas_event_feed_key_down(evas, "Q", "Q", NULL, NULL, 0, NULL);
                  else
                    evas_event_feed_key_down(evas, "q", "q", NULL, NULL, 0, NULL);
                  break;
	       default:
		  break;
               }
             break;
          case SDL_KEYUP:
             switch (event.key.keysym.sym)
               {
               case SDLK_LSHIFT:
               case SDLK_RSHIFT:
                  evas_key_modifier_off(evas, "Shift");
                  break;
               case SDLK_RCTRL:
               case SDLK_LCTRL:
                  evas_key_modifier_off(evas, "Control");
                  break;
               case SDLK_MENU:
               case SDLK_RALT:
               case SDLK_LALT:
                  evas_key_modifier_off(evas, "Alt");
                  break;
               case SDLK_LSUPER:
               case SDLK_RSUPER:
                  evas_key_modifier_off(evas, "Super");
                  break;
               case SDLK_ESCAPE: evas_event_feed_key_up(evas, "Escape", "Escape", NULL, NULL, 0, NULL); break;
               case SDLK_RETURN: evas_event_feed_key_up(evas, "Return", "Return", NULL, NULL, 0, NULL); break;
               case SDLK_LEFT: evas_event_feed_key_up(evas, "Left", "Left", NULL, NULL, 0, NULL); break;
               case SDLK_RIGHT: evas_event_feed_key_up(evas, "Right", "Right", NULL, NULL, 0, NULL); break;
               case SDLK_q:
                  if (evas_key_lock_is_set(evas_key_lock_get(evas), "Caps_Lock"))
                    evas_event_feed_key_up(evas, "Q", "Q", NULL, NULL, 0, NULL);
                  else
                    evas_event_feed_key_up(evas, "q", "q", NULL, NULL, 0, NULL);
                  break;
	       default:
		  break;
               }
             break;
          case SDL_QUIT:
          case SDL_SYSWMEVENT:
          case SDL_USEREVENT:
          case SDL_ACTIVEEVENT:
          case SDL_JOYAXISMOTION:
          case SDL_JOYBALLMOTION:
          case SDL_JOYHATMOTION:
          case SDL_JOYBUTTONDOWN:
          case SDL_JOYBUTTONUP:
          default:
             break;
          }
     }
}

void
engine_software_sdl_shutdown(void)
{
}
