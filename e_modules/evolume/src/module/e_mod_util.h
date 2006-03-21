#ifndef E_MOD_UTIL_H_INCLUDED
#define E_MOD_UTIL_H_INCLUDED

int e_util_hash(const char *key);
int e_util_hash_gen(const char *system, const char *mixer,
		const char *elem);

Mixer* e_util_search_mixer_by_name(Evas_List* mixers, Mixer_Name* name);

int e_volume_first_run();

Evas_List *e_util_wlist_get(Evas_List* melems, Config_Face* conf);
Evas_List *e_util_sliders_sort(Evas_List* sliders);

Mixer_Elem *e_util_melem_get(Config_Mixer_Elem* conf, Evas_List* melems);

#endif //E_MOD_UTIL_H_INCLUDED
