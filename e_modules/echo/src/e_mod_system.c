#include <e.h>
#include "config.h"
#include "e_mod_main.h"
#include "e_mod_system.h"
#if defined(HAVE_ALSA)
# include "e_mod_alsa.h"
#endif

System *echo_sys = NULL;

EAPI int 
e_mod_system_init(int card) 
{
   echo_sys = E_NEW(System, 1);
   if (!echo_sys) return 0;

#if defined(HAVE_ALSA)
   echo_sys->init = e_mod_alsa_init;
   echo_sys->shutdown = e_mod_alsa_shutdown;
   echo_sys->get_cards = e_mod_alsa_get_cards;
   echo_sys->get_channels = e_mod_alsa_get_channels;
   echo_sys->get_channel_name = e_mod_alsa_get_channel_name;
   echo_sys->get_volume = e_mod_alsa_get_volume;
   echo_sys->set_volume = e_mod_alsa_set_volume;
   echo_sys->can_mute = e_mod_alsa_can_mute;
   echo_sys->get_mute = e_mod_alsa_get_mute;
   echo_sys->set_mute = e_mod_alsa_set_mute;
#endif

   if (echo_sys->init) return echo_sys->init(card);
   return 1;
}

EAPI int 
e_mod_system_shutdown(void) 
{
   if (echo_sys->shutdown) echo_sys->shutdown();
   E_FREE(echo_sys);
   return 1;
}
