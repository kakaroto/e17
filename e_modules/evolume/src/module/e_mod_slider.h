
#ifndef E_MOD_SLIDER_H_INCLUDED
#define E_MOD_SLIDER_H_INCLUDED

typedef struct _Mixer_Slider	Mixer_Slider;

struct _Mixer_Slider
{
	Mixer_Elem			*melem;
	Config_Mixer_Elem	*conf;

	Evas_Object	*name;
	Evas_Object	*swallow_object;
	Evas_Object	*drag_volume;
	Evas_Object	*drag_balance;
	Volume_Face	*face;

	/* Orientation */
	int			orient;
	/* If first time updated by _mixer_update_mixer_pair */
	int			first;

	int			locked;

	int			balance;
};


Mixer_Slider* e_volume_slider_create(Mixer_Elem* melem, Evas* ev,
		int balance, int orient, Volume_Face* face, int ref);

Mixer_Slider* e_volume_slider_create_from_conf(Config_Mixer_Elem* conf,
		Volume_Face* face, int ref);

void		e_volume_slider_free(Mixer_Slider* slider, int unref);



#endif // E_MOD_SLIDER_H_INCLUDED
