#include <assert.h>
#include "main.h"

#include <SDL/SDL.h>
#include <Evas_Engine_SDL.h>

int
engine_software_sdl_args(int argc, char **argv)
{
   Evas_Engine_Info_SDL *einfo;
   int                  i;
   int                  ok = 0;

   for (i = 1; i < argc; ++i)
     {
        if ((!strcmp(argv[i], "-e") && (i < (argc - 1))))
          {
             i++;
             if (!strcmp(argv[i], "sdl")) ok = 1;
             if (!strcmp(argv[i], "sdl-16")) ok = 2;
          }
     }
   if (!ok) return 0;

   if (ok == 1)
   evas_output_method_set(evas, evas_render_method_lookup("software_sdl"));
   else
     evas_output_method_set(evas, evas_render_method_lookup("software_16_sdl"));

   einfo = (Evas_Engine_Info_SDL *) evas_engine_info_get(evas);

   /* the following is specific to the engine */
   einfo->info.fullscreen = 0;
   einfo->info.noframe = 0;

   evas_engine_info_set(evas, (Evas_Engine_Info *) einfo);
   return 1;
}

void
engine_software_sdl_loop(void)
{
   SDL_Event event;

   while(SDL_PollEvent(&event))
     {
        switch(event.type)
          {
          case SDL_MOUSEMOTION:
             evas_event_feed_mouse_move(evas, event.motion.x, event.motion.y, 0, NULL);
             break;
          case SDL_MOUSEBUTTONDOWN:
             evas_event_feed_mouse_move(evas, event.button.x, event.button.y, 0, NULL);
             evas_event_feed_mouse_down(evas, event.button.state, EVAS_BUTTON_NONE, 0, NULL);
             break;
          case SDL_MOUSEBUTTONUP:
             evas_event_feed_mouse_move(evas, event.button.x, event.button.y, 0, NULL);
             evas_event_feed_mouse_up(evas, event.button.state, EVAS_BUTTON_NONE, 0, NULL);
             break;
          case SDL_VIDEORESIZE:
             evas_output_size_set(evas, event.resize.w, event.resize.w);
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
