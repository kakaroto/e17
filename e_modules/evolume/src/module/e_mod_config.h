
#ifndef E_MOD_CONFIG_H_INCLUDED
#define E_MOD_CONFIG_H_INCLUDED

#include "e_mod_volume.h"

#define ORIENT_VERT 0x01
#define ORIENT_HORIZ 0x02

//typedef struct _Config_Mixer Config_Mixer;
//typedef struct _Config_Mixer_Enabled Config_Mixer_Enabled;

extern E_Config_DD *conf_edd;

struct _Config_Mixer_Elem
{
   const char *name;
   int active;
   int balance;
   int weight;

   int elem_id;
};

struct _Config_Mixer
{
   /* Nice name of mixer */
   const char *real;

   /* Card (e.g. local) name of mixer */
   const char *card;

   /* Name of mixer system, this is to avoid back referencing */
   const char *system;
   /* If mixer is active */
   int active;
   /* List of Config_Mixer_Elem */
   Evas_List *elems;

   /* Mixer id */
   int mixer_id;

   /* Oh, this is not criminal ? */
//      Config_Mixer_System *mixer_system;
};

struct _Config_Mixer_System
{
   /* Real name of Mixer System */
   const char *name;
   /* System name of mixer system (used to match soname) */
   const char *system;
   /* Mixers by this System */
   Evas_List *mixers;
   /* If this mixer system is active */
   int active;
   /* System id */
   int system_id;
};

struct _Config_Face
{
   int orient;
   Evas_List *systems;
};

struct _Config_Container
{
   Evas_List *faces;
};

struct _Config
{
   Evas_List *containers;
};

void e_volume_config_init();
void e_volume_config_shutdown();

Config *e_volume_config_load();
Config_Mixer_Elem *e_volume_config_melem_get(Mixer_Elem *melem, Config_Face *conf);
Config_Mixer *e_volume_config_mixer_get(Mixer_Name *melem, Config_Face *conf);
Config_Mixer_System *e_volume_config_mixer_system_get(Mixer_System_Name *msn, Config_Face *conf);

#define SYSTEM_ID(a) ((a) & 0x00FF0000)
#define MIXER_ID(a)  ((a) & 0x0000FF00)
#define ELEM_ID(a)   ((a) & 0x000000FF)

#endif //E_MOD_CONFIG_H_INCLUDED
