#include <e.h>
#include <alsa/asoundlib.h>
#include "e_mod_alsa.h"

static snd_mixer_t *hdl;

EAPI int 
e_mod_alsa_init(int card) 
{
   char hw[32];
   int err = 0;

   if (card < 0) return 0;
   snprintf(hw, 32, "hw:%d", card);
   if ((err = snd_mixer_open(&hdl, 0)) < 0) return 0;
   if ((err = snd_mixer_attach(hdl, hw)) < 0) goto error;
   if ((err = snd_mixer_selem_register(hdl, NULL, NULL)) < 0) goto error;
   if ((err = snd_mixer_load(hdl)) < 0) goto error;
   return 1;

   error:
   snd_mixer_close(hdl);
   hdl = 0;
   return 0;
}

EAPI int 
e_mod_alsa_shutdown(void) 
{
   if (hdl > 0) snd_mixer_close(hdl);
   return 1;
}

EAPI Evas_List *
e_mod_alsa_get_cards(void) 
{
   int i, err;
   char buf[256];
   snd_ctl_t *control;
   snd_ctl_card_info_t *hw_info;
   Evas_List *cards = NULL;
   
   snd_ctl_card_info_alloca(&hw_info);
   for (i = 0; i < 32; i++)
     {
	const char *name = NULL;
	
	snprintf(buf, sizeof(buf), "hw:%d", i);
	if ((err = snd_ctl_open(&control, buf, 0)) < 0) break;
	if ((err = snd_ctl_card_info(control, hw_info)) < 0)
	  {
	     printf("Cannot get hardware info: %s: %s\n", buf,
		    snd_strerror(err));
	     snd_ctl_close(control);
	     continue;
	  }
	snd_ctl_close(control);
	name = snd_ctl_card_info_get_name(hw_info);
	if (name)
          cards = evas_list_append(cards, evas_stringshare_add(name));
     }

   return cards;
}

EAPI Evas_List *
e_mod_alsa_get_channels(void) 
{
   Evas_List *channels = NULL;
   snd_mixer_elem_t *elem;
   snd_mixer_selem_id_t *sid;

   if (!hdl) return NULL;

   snd_mixer_selem_id_malloc(&sid);
   for (elem = snd_mixer_first_elem(hdl); elem; 
        elem = snd_mixer_elem_next(elem)) 
     {
        snd_mixer_selem_get_id(elem, sid);
        if (!snd_mixer_selem_is_active(elem)) continue;
        if (!snd_mixer_selem_has_playback_volume(elem)) continue;
        channels = evas_list_append(channels, 
                                    strdup(snd_mixer_selem_id_get_name(sid)));
     }
   snd_mixer_selem_id_free(sid);
   return channels;
}

EAPI char *
e_mod_alsa_get_channel_name(int channel) 
{
   snd_mixer_elem_t *elem;
   snd_mixer_selem_id_t *sid;
   char *name = NULL;
   int i = -1;

   if (!hdl) return NULL;

   snd_mixer_selem_id_malloc(&sid);
   for (elem = snd_mixer_first_elem(hdl); elem; 
        elem = snd_mixer_elem_next(elem)) 
     {
        snd_mixer_selem_get_id(elem, sid);
        if (!snd_mixer_selem_is_active(elem)) continue;
        if (!snd_mixer_selem_has_playback_volume(elem)) continue;
        i++;
        if (i != channel) continue;
        name = strdup(snd_mixer_selem_id_get_name(sid));
        break;
     }
   snd_mixer_selem_id_free(sid);
   return name;
}

EAPI int 
e_mod_alsa_get_volume(const char *channel, int *left, int *right) 
{
   snd_mixer_elem_t *elem;
   snd_mixer_selem_id_t *sid;

   if (!hdl) return 0;
   if (!channel) return 0;

   snd_mixer_handle_events(hdl);
   snd_mixer_selem_id_malloc(&sid);
   for (elem = snd_mixer_first_elem(hdl); elem; 
        elem = snd_mixer_elem_next(elem)) 
     {
        long lvol, rvol, min, max;
        int range;

        snd_mixer_selem_get_id(elem, sid);
        if (!snd_mixer_selem_is_active(elem)) continue;
        if (!snd_mixer_selem_has_playback_volume(elem)) continue;
        if (strcmp(channel, snd_mixer_selem_id_get_name(sid))) continue;
        lvol = rvol = -1;
        snd_mixer_selem_get_playback_volume_range(elem, &min, &max);
        range = max - min;
        /* this alsa function seems arse backword
         * returns 0 on stereo and 1 on mono */
        if (snd_mixer_selem_has_playback_volume_joined(elem)) 
          snd_mixer_selem_get_playback_volume(elem, 0, &lvol);
        else 
          {
             if (snd_mixer_selem_has_playback_channel(elem, 0))
               snd_mixer_selem_get_playback_volume(elem, 0, &lvol);
             if (snd_mixer_selem_has_playback_channel(elem, 1))
               snd_mixer_selem_get_playback_volume(elem, 1, &rvol);
          }
        if (lvol) 
          {
             if (range == 0) lvol = 0;
             else lvol -= min;
             if (left) *left = rint(((double)lvol / ((double)range) * 100));
          }
        else
          if (left) *left = -1;
        if (rvol) 
          {
             if (range == 0) rvol = 0;
             else rvol -= min;
             if (right) *right = rint(((double)rvol / ((double)range) * 100));
          }
        else
          if (right) *right = -1;
        break;
     }

   snd_mixer_selem_id_free(sid);
   return 1;
}

EAPI int 
e_mod_alsa_set_volume(const char *channel, int left, int right) 
{
   snd_mixer_elem_t *elem;
   snd_mixer_selem_id_t *sid;

   if (!hdl) return 0;
   if (!channel) return 0;

   snd_mixer_handle_events(hdl);
   snd_mixer_selem_id_malloc(&sid);
   for (elem = snd_mixer_first_elem(hdl); elem; 
        elem = snd_mixer_elem_next(elem)) 
     {
        long min, max;
        int range;

        snd_mixer_selem_get_id(elem, sid);
        if (!snd_mixer_selem_is_active(elem)) continue;
        if (!snd_mixer_selem_has_playback_volume(elem)) continue;
        if (strcmp(channel, snd_mixer_selem_id_get_name(sid))) continue;
        snd_mixer_selem_get_playback_volume_range(elem, &min, &max);
        range = max - min;
        if (!range) 
          {
             snd_mixer_selem_id_free(sid);
             return 0;
          }
        if (left != -1) 
          {
             left = (((range * left) + (range / 2)) / (100 + min));
             left -= min;
          }
        if (right != -1) 
          {
             right = (((range * right) + (range / 2)) / (100 + min));
             right -= min;
          }
        
        /* this alsa function seems arse backword
         * returns 0 on stereo and 1 on mono */
        if (snd_mixer_selem_has_playback_volume_joined(elem)) 
          {
             if (left != -1) 
               snd_mixer_selem_set_playback_volume(elem, 0, left);
          }
        else 
          {
             if (left != -1) 
               {
                  if (snd_mixer_selem_has_playback_channel(elem, 0))
                    snd_mixer_selem_set_playback_volume(elem, 0, left);
               }
             if (right != -1) 
               {
                  if (snd_mixer_selem_has_playback_channel(elem, 1))
                    snd_mixer_selem_set_playback_volume(elem, 1, right);
               }
          }
        break;
     }

   snd_mixer_selem_id_free(sid);
   return 1;
}

EAPI int 
e_mod_alsa_can_mute(const char *channel) 
{
   snd_mixer_elem_t *elem;
   snd_mixer_selem_id_t *sid;
   int ret = 0;

   if (!hdl) return 0;
   if (!channel) return 0;

   snd_mixer_handle_events(hdl);
   snd_mixer_selem_id_malloc(&sid);
   for (elem = snd_mixer_first_elem(hdl); elem; 
        elem = snd_mixer_elem_next(elem)) 
     {
        snd_mixer_selem_get_id(elem, sid);
        if (!snd_mixer_selem_is_active(elem)) continue;
        if (!snd_mixer_selem_has_playback_volume(elem)) continue;
        if (strcmp(channel, snd_mixer_selem_id_get_name(sid))) continue;
        if (snd_mixer_selem_has_playback_switch(elem))
          ret = 1;
        else if (snd_mixer_selem_has_playback_switch_joined(elem))
          ret = 1;
        else
          ret = 0;
        break;
     }

   snd_mixer_selem_id_free(sid);
   return ret;
}

EAPI int 
e_mod_alsa_get_mute(const char *channel, int *mute) 
{
   snd_mixer_elem_t *elem;
   snd_mixer_selem_id_t *sid;
   int ret = 0, m;

   if (!hdl) return 0;
   if (!channel) return 0;

   snd_mixer_handle_events(hdl);
   snd_mixer_selem_id_malloc(&sid);
   for (elem = snd_mixer_first_elem(hdl); elem; 
        elem = snd_mixer_elem_next(elem)) 
     {
        snd_mixer_selem_get_id(elem, sid);
        if (!snd_mixer_selem_is_active(elem)) continue;
        if (!snd_mixer_selem_has_playback_volume(elem)) continue;
        if (strcmp(channel, snd_mixer_selem_id_get_name(sid))) continue;
        if (snd_mixer_selem_has_playback_switch(elem)) 
          ret = snd_mixer_selem_get_playback_switch(elem, 0, &m);
        else if (snd_mixer_selem_has_playback_switch_joined(elem))
          ret = snd_mixer_selem_get_playback_switch(elem, 0, &m);
        if (mute) *mute = !m;
        break;
     }

   snd_mixer_selem_id_free(sid);
   return ret;
}

EAPI int 
e_mod_alsa_set_mute(const char *channel, int mute) 
{
   snd_mixer_elem_t *elem;
   snd_mixer_selem_id_t *sid;
   int ret = 0;

   if (!hdl) return 0;
   if (!channel) return 0;

   snd_mixer_handle_events(hdl);
   snd_mixer_selem_id_malloc(&sid);
   for (elem = snd_mixer_first_elem(hdl); elem; 
        elem = snd_mixer_elem_next(elem)) 
     {
        snd_mixer_selem_get_id(elem, sid);
        if (!snd_mixer_selem_is_active(elem)) continue;
        if (!snd_mixer_selem_has_playback_volume(elem)) continue;
        if (strcmp(channel, snd_mixer_selem_id_get_name(sid))) continue;
        if (snd_mixer_selem_has_playback_switch(elem)) 
          ret = snd_mixer_selem_set_playback_switch_all(elem, !mute);
        else if (snd_mixer_selem_has_playback_switch_joined(elem))
          ret = snd_mixer_selem_set_playback_switch_all(elem, !mute);
        break;
     }

   snd_mixer_selem_id_free(sid);
   return ret;
}
