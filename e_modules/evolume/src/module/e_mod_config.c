#include "e_mod_config.h"
#include <e.h>

#include <e_mod_util.h>
#include <e_mod_main.h>

E_Config_DD *conf_edd;
static E_Config_DD *conf_container_edd;
static E_Config_DD *conf_face_edd;
static E_Config_DD *conf_mixer_system_edd;
static E_Config_DD *conf_mixer_edd;
static E_Config_DD *conf_elem_edd;

/* This will copy only list, not data */
Evas_List *
_evas_list_copy(Evas_List *src)
{
   Evas_List *l, *ret;

   if (!src)
      return NULL;

   ret = NULL;

   for (l = src; l; l = evas_list_next(l))
      ret = evas_list_append(ret, evas_list_data(l));

   return ret;
}

void
e_volume_config_init()
{

//      DBG(stderr,"in start of : %s\n", __FUNCTION__);
   conf_elem_edd = E_CONFIG_DD_NEW("Volume_Config_Elem", Config_Mixer_Elem);

#undef T
#undef D
#define T Config_Mixer_Elem
#define D conf_elem_edd
   E_CONFIG_VAL(D, T, name, STR);
   E_CONFIG_VAL(D, T, elem_id, INT);
   E_CONFIG_VAL(D, T, active, INT);
   E_CONFIG_VAL(D, T, balance, INT);
   E_CONFIG_VAL(D, T, weight, INT);

   conf_mixer_edd = E_CONFIG_DD_NEW("Volume_Config_Mixer", Config_Mixer);

#undef T
#undef D
#define T Config_Mixer
#define D conf_mixer_edd
   E_CONFIG_VAL(D, T, real, STR);
   E_CONFIG_VAL(D, T, card, STR);
   E_CONFIG_VAL(D, T, system, STR);
   E_CONFIG_VAL(D, T, active, INT);
   E_CONFIG_VAL(D, T, mixer_id, INT);
   E_CONFIG_LIST(D, T, elems, conf_elem_edd);

   conf_mixer_system_edd = E_CONFIG_DD_NEW("Volume_Config_Mixer_System", Config_Mixer_System);
#undef T
#undef D
#define T Config_Mixer_System
#define D conf_mixer_system_edd
   E_CONFIG_VAL(D, T, name, STR);
   E_CONFIG_VAL(D, T, system, STR);
   E_CONFIG_VAL(D, T, active, INT);
   E_CONFIG_VAL(D, T, system_id, INT);
   E_CONFIG_LIST(D, T, mixers, conf_mixer_edd);

   conf_face_edd = E_CONFIG_DD_NEW("Volume_Config_Face", Config_Face);

#undef T
#undef D
#define T Config_Face
#define D conf_face_edd
   E_CONFIG_LIST(D, T, systems, conf_mixer_system_edd);
   E_CONFIG_VAL(D, T, orient, INT);

   conf_container_edd = E_CONFIG_DD_NEW("Volume_Config_Container", Config_Container);
#undef T
#undef D
#define T Config_Container
#define D conf_container_edd
   E_CONFIG_LIST(D, T, faces, conf_face_edd);

   conf_edd = E_CONFIG_DD_NEW("Volume_Config", Config);

#undef T
#undef D
#define T Config
#define D conf_edd
   E_CONFIG_LIST(D, T, containers, conf_container_edd);

//      DBG(stderr,"conf_edd = %p\n", conf_edd);

//      DBG(stderr,"in end of : %s\n", __FUNCTION__);
   return;
}

Config_Mixer_System *
e_volume_config_mixer_system_get(Mixer_System_Name *msn, Config_Face *conf)
{
   Evas_List *l;

   for (l = conf->systems; l; l = evas_list_next(l))
     {
        Config_Mixer_System *cms;

        cms = evas_list_data(l);
#if 0
        DBG(stderr, "cms->name = %s, cms->system = %s\n", cms->name, cms->system);
        DBG(stderr, "msn->name = %s, msn->system = %s\n", msn->name, msn->system);
#endif

#if 0
        DBG(stderr, "cms->system_id = %x, msn->system_id = %x\n", cms->system_id, msn->system_id);
#endif
        if (SYSTEM_ID(cms->system_id) == SYSTEM_ID(msn->system_id))
           return cms;
     }
   if (!l)
     {
        Config_Mixer_System *cms;

//              DBG(stderr,"Creating new systme: %s\n", msn->name);

        cms = calloc(1, sizeof(Config_Mixer_System));
        cms->name = evas_stringshare_add(msn->name);
        cms->system = evas_stringshare_add(msn->system);
        cms->active = 1;
        cms->system_id = msn->system_id;
        cms->mixers = NULL;

        conf->systems = evas_list_append(conf->systems, cms);

        return cms;
     }

   return NULL;
}

Config_Mixer *
e_volume_config_mixer_get(Mixer_Name *mixer_name, Config_Face *conf)
{
   Evas_List *l;

   Config_Mixer_System *cms;

   cms = e_volume_config_mixer_system_get(mixer_name->system_name, conf);

   for (l = cms->mixers; l; l = evas_list_next(l))
     {
        Config_Mixer *mixer_conf;

        mixer_conf = evas_list_data(l);

        DBG(stderr, "mixer_id1 = %x, mixer_id2 = %x\n", mixer_conf->mixer_id, mixer_name->mixer_id);
        if (MIXER_ID(mixer_conf->mixer_id) == MIXER_ID(mixer_name->mixer_id))
          {
//                      DBG(stderr,"using mixer_conf = %p\n", mixer_conf);
             return mixer_conf;
          }
	else if ((!strcmp(mixer_conf->real, mixer_name->real)) && 
		 (!strcmp(mixer_conf->card, mixer_name->card))) 
	  {
	     return mixer_conf;
	  }	
     }
   if (!l)
     {
        /* Ok, create new one */
        Config_Mixer *mixer_conf;

        mixer_conf = calloc(1, sizeof(Config_Mixer));
        mixer_conf->real = evas_stringshare_add(mixer_name->real);
        mixer_conf->card = evas_stringshare_add(mixer_name->card);
        mixer_conf->system = evas_stringshare_add(cms->name);
        mixer_conf->mixer_id = mixer_name->mixer_id | cms->system_id;
        mixer_conf->active = 1;
        mixer_conf->elems = NULL;

//              DBG(stderr,"Creating new mixer_conf\n");
        cms->mixers = evas_list_append(cms->mixers, mixer_conf);

        return mixer_conf;
     }

   return NULL;
}

Config_Mixer_Elem *
e_volume_config_melem_get(Mixer_Elem *melem, Config_Face *conf)
{
   Config_Mixer *mixer_conf;
   Config_Mixer_Elem *elem;
   Evas_List *l;
   int hash;
   char fullname[1024];

   //      DBG( stderr, "fullname = %s\n", fullname);

   mixer_conf = e_volume_config_mixer_get(melem->mixer->name, conf);

   /*
    * DBG(stderr,"mixer_conf->name = %s, mixer->name = %s\n",
    * mixer_conf->name, melem->mixer->name);
    */

   for (l = mixer_conf->elems; l; l = evas_list_next(l))
     {
        elem = evas_list_data(l);

#if 0
        DBG(stderr, "elem->hash = %d, hash = %d, elem->name = %s\n", elem->hash, hash, elem->name);
#endif

#if 0
        DBG(stderr, "elem->elem_id = %x, elem->elem_id = %x\n", ELEM_ID(elem->elem_id), elem->elem_id);
#endif
        if (ELEM_ID(elem->elem_id) == ELEM_ID(melem->elem_id))
          {
             DBG(stderr, "found: %x\n", elem->elem_id);
             return elem;
          }
     }

   /* We lookup all elem entries and don't found */
   /* requested: create one new and return it */
   if (!l)
     {
        DBG(stderr, "`elem' is not found\n");
        DBG(stderr, "name = %s, hash = %d\n", melem->name, hash);
        elem = calloc(1, sizeof(Config_Mixer_Elem));

        elem->name = evas_stringshare_add(melem->name);
        elem->active = 1;
        elem->weight = (evas_list_count(mixer_conf->elems) + 1) * 10;
        elem->elem_id = melem->elem_id | mixer_conf->mixer_id;  //GET_ELEM_ID(melem);

        mixer_conf->elems = evas_list_append(mixer_conf->elems, elem);

        return elem;
     }

   DBG(stderr, "MIXER CONFIG NOT CREATED FOR %s->%s->%s\n",
       melem->mixer->system->name->system, melem->mixer->name->real, melem->name);
   return NULL;
}

Config *
e_volume_config_load()
{
   Config *conf;

//      DBG(stderr,"in start of : %s\n", __FUNCTION__);
   conf = e_config_domain_load("module.evolume", conf_edd);

   if (!conf)
     {
        conf = calloc(1, sizeof(Config));
        conf->containers = NULL;
     }
   if (!conf->containers)
      e_volume_first_run();

//      DBG(stderr,"in end of : %s\n", __FUNCTION__);
   return conf;
}

void
e_volume_config_shutdown()
{
//      DBG(stderr,"in start of : %s\n", __FUNCTION__);
   E_CONFIG_DD_FREE(conf_edd);
   E_CONFIG_DD_FREE(conf_container_edd);
   E_CONFIG_DD_FREE(conf_face_edd);
   E_CONFIG_DD_FREE(conf_mixer_edd);
   E_CONFIG_DD_FREE(conf_elem_edd);
//      DBG(stderr,"in end of : %s\n", __FUNCTION__);
}
