#ifndef ENTICE_KEYS_H
#define ENTICE_KEYS_H

#include <Evas.h>
/**
 * Ok, yes I know this could be done A LOT better, but it's abstracted
 * into its own little thing now and we should be able to modify it
 * without throwing the rest of the app into limbo
 * 
 * Currently keys are loaded via the config system, it loads key ->
 * signal emission mappings and sends a request to trap that key here.
 */
struct _Entice_Key
{
   char *key;
   char *signal;
};
typedef struct _Entice_Key Entice_Key;

struct _Entice_Key_Keeper
{
   Evas_Object *obj;            /* our object grabbing keys in evas */
   Evas_Hash *up, *down;        /* different hashes for different key events */
};
typedef struct _Entice_Key_Keeper Entice_Key_Keeper;

/**
 *
 */
Entice_Key *entice_key_new(const char *key, const char *signal);
void entice_key_free(Entice_Key * e);

/**
 *
 */
void entice_keys_init(void);
void entice_keys_callback_init(Evas_Object * edje);
void entice_keys_free(void);
void entice_keys_up_add(Entice_Key * key);
void entice_keys_down_add(Entice_Key * key);

#endif
