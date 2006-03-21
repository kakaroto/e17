
#ifndef E_MOD_VOLUME_INCLUDED
#define E_MOD_VOLUME_INCLUDED

#include <e.h>
#include "e_mixer.h"

typedef struct _Config_Mixer_Elem	Config_Mixer_Elem;
typedef struct _Config_Mixer		Config_Mixer;
typedef struct _Config_Mixer_System		Config_Mixer_System;
typedef struct _Config_Face		Config_Face;
typedef struct _Config_Container	Config_Container;


typedef struct _Volume Volume;
typedef struct _Config Config;

struct _Volume
{
	Evas_List *mixer_system_names;

	Evas_List *mixers;
	Evas_List *faces;

	E_Menu* config_menu;

	Config*		conf;
};

#include "e_mod_face.h"
#include "e_mod_config.h"
	
Volume* e_volume_init(void* data);
int e_volume_shutdown(Volume* vol);

int e_volume_mixers_create(Volume_Face* face, Mixer* mixer, Evas* ev, int ref);
int e_volume_mixers_update(Volume* volume);


#endif // E_MOD_VOLUME_INCLUDED
