
#include <e.h>
#include <e_mixer.h>
#include <e_mod_main.h>
#include <e_mod_volume.h>
#include <e_mod_slider.h>
#include <e_mod_mixer.h>
#include <e_mod_util.h>

static void _volume_slider_volume_set(Mixer_Slider *slider)
{
	double	vol, vol1;
	int		left, right;
	Mixer_Elem *melem;

	edje_object_part_drag_value_get(slider->drag_volume, "volume_bar_drag",
			&vol, &vol1);

//	DBG(stderr,"vol = %f, vol1 = %f\n", vol, vol1);
	if(slider->orient == ORIENT_VERT)
		vol = 1.0f - vol1;
	
	melem = slider->melem;

	left = right = vol*100;

	if(slider->drag_balance)
	{
		if(slider->balance < 0)
			right = (right * (100 - abs(slider->balance))) / 100;
		if(slider->balance > 0)
			left = (left * (100 - abs(slider->balance))) / 100;
	}
	else if (slider->orient == ORIENT_VERT)
		edje_object_part_drag_value_set(slider->drag_volume, "volume_bar_drag",
				0.5f, vol1);

//	DBG(stderr,"Setting volume: (%d, %d) = 100.0*(%f)", left, right, vol);
	mixer_set_volume(melem, left, right);
	
}

static void _volume_slider_level_drag_state_cb(void *data,
		Evas_Object* object __UNUSED__,
		const char *emission, const char *source __UNUSED__)
{
	Mixer_Slider* slider;
	slider = (Mixer_Slider*)data;

	if(strstr(emission, "start"))
		slider->locked = 1;
	if(strstr(emission, "stop"))
		slider->locked = 0;
}


static void _volume_slider_level_drag_cb(void *data,
		Evas_Object* object __UNUSED__,
		const char* emission __UNUSED__, const char* source __UNUSED__)
{
	_volume_slider_volume_set((Mixer_Slider*)data);
	return;
}

static void _volume_slider_balance_drag_cb(void *data,
		Evas_Object* object __UNUSED__,
		const char* emission __UNUSED__, const char* source __UNUSED__)
{
	Mixer_Slider *slider;
	slider = data;
	double bal;

	/* Balance is are always X axis */
	edje_object_part_drag_value_get(slider->drag_balance,
			"volume_bar_drag", &bal, NULL);

	slider->balance = rint((bal - 0.5f) * 200.0f);

	_volume_slider_volume_set(slider);
	return;
}

static void _volume_slider_level_mouse_wheel_cb(void *data, Evas* e __UNUSED__,
		Evas_Object* obj __UNUSED__, void* event_info)
{
	Evas_Event_Mouse_Wheel* ev;
	int l, r;
	Mixer_Elem *melem;
	Mixer_Slider *slider;

	ev = event_info;
	slider = data;
#if 0
	melem = evas_object_data_get(object, "melem");
#else
	melem = slider->melem;
#endif
	if (!melem)
		return;

	mixer_get_volume(melem, &l, &r);

	if(ev->z < 0)
	{
		l += 5;
		r += 5;
	} else if(ev->z > 0)
	{
		l -= 5;
		r -= 5;
	}

	melem->pvl = -1;
	melem->pvr = -1;

	mixer_set_volume(melem, l, r);

	return;
}

static void _volume_slider_balance_mouse_wheel_cb(void *data __UNUSED__,
		Evas* e __UNUSED__,
		Evas_Object* obj __UNUSED__, void* event_info __UNUSED__)
{
#if 0
	Evas_Event_Mouse_Wheel* ev;
	int l, r;
	Mixer_Elem *melem;
	Mixer_Slider *slider;

	ev = event_info;
	slider = data;
#if 0
	melem = evas_object_data_get(object, "melem");
#else
	melem = slider->melem;
#endif
	if (!melem)
		return;

	mixer_get_volume(melem, &l, &r);

	if(ev->z < 0)
	{
		if (slider->balance > 0)
			r += 5;
		l -= 5;
	} else if(ev->z > 0)
	{
		if (slider->balance < 0)
			l += 5;
		r -= 5;
	}

	melem->pvl = -1;
	melem->pvr = -1;

	mixer_set_volume(melem, l, r);

	return;
#endif
}

/** Check if Edje group exists
 * @param file A valid file path to Edje
 * @param part The part name to check
 * @return 0 on Error\n
 * 1 if Edje part exists
 *
 * CODE OF THIS FUNCTION MAY BE USED IN E17
 */
static int
edje_file_group_exists(const char *file, const char *part)
{
	Evas_List *collections;
	collections = edje_file_collection_list(file);
	for (; collections; collections = evas_list_next(collections))
	{
		if (!strcmp(part,collections->data))
			return 1;
	}
	return 0;
}

static int _edje_object_file_set_orient(Evas_Object* obj,
		const char *file, const char* part, int orient)
{
	char buf[1024];
	/* First check for required orient */
	
	strncpy(buf, part, 1024);
	switch(orient) {
		case ORIENT_VERT:
			strncat(buf, "/vert", 1024);
			if (edje_file_group_exists(file, buf))
				break;

		case ORIENT_HORIZ:
			strncat(buf, "/horiz", 1024);
			if (edje_file_group_exists(file, buf))
				break;

		default:
			strncpy(buf, part, 1024);
			break;
	}

//	DBG(stderr, "loading %s\n", buf);
	/* Ok, we get group name, try to load it */
	return edje_object_file_set(obj, file, buf);
}

Mixer_Slider* e_volume_slider_create(Mixer_Elem* melem, Evas* ev, int orient,
		int balance,
		Volume_Face* face, int ref)
{
	Mixer_Slider* slider;

	slider = calloc(sizeof(Mixer_Slider), 1);

	if(!slider)
		return NULL;

	/* It is not initialized */
	slider->first = 1;
	/* Set it there, as it used in reordering callbacks */
	slider->face = face;

	/* Update name object first */
	slider->name = (Evas_Object*)edje_object_add(ev);

	/* Swallow object */
	slider->swallow_object = (Evas_Object*)edje_object_add(ev);
	fprintf(stderr,"%s\n", module_theme);
	edje_object_file_set(slider->swallow_object,
			module_theme,
			"volume/mixer");
	

	slider->orient = orient;
	_edje_object_file_set_orient(slider->name, 
			module_theme, 
			"volume/mixer/name", orient);
	edje_object_part_text_set(slider->name, "mixer_name",
			melem->name);

	/* Num num, sweet part :-) */
	edje_object_part_swallow(slider->swallow_object, 
			"label",
			slider->name);
		

	/* Update volume drag object */
	{
		slider->drag_volume = (Evas_Object*)edje_object_add(ev);
		_edje_object_file_set_orient(slider->drag_volume, 
				module_theme, 
				"volume/mixer/drag", orient);

		edje_object_signal_callback_add(slider->drag_volume, "drag",
				"volume_bar_drag",
				_volume_slider_level_drag_cb,
				(void*)slider );

		edje_object_signal_callback_add(slider->drag_volume, "drag,*",
				"volume_bar_drag",
				_volume_slider_level_drag_state_cb,
				(void*)slider );

		evas_object_event_callback_add(slider->drag_volume,
				EVAS_CALLBACK_MOUSE_WHEEL, _volume_slider_level_mouse_wheel_cb,
				slider);

		evas_object_data_set(slider->drag_volume, "melem", (void*)melem);

		/* Num num, sweet part :-) */
		if(!balance)
			edje_object_part_swallow(slider->swallow_object, 
					"level",
					slider->drag_volume);
		else
			edje_object_part_swallow(slider->swallow_object, 
					"level_b",
					slider->drag_volume);
		
	}

	if (balance)
	{
		slider->drag_balance = (Evas_Object*)edje_object_add(ev);
		_edje_object_file_set_orient(slider->drag_balance, 
				module_theme, 
				"volume/mixer/drag", orient);

		edje_object_signal_callback_add(slider->drag_balance, "drag",
				"volume_bar_drag",
				_volume_slider_balance_drag_cb,
				(void*)slider );

		evas_object_event_callback_add(slider->drag_balance,
				EVAS_CALLBACK_MOUSE_WHEEL,
				_volume_slider_balance_mouse_wheel_cb,
				slider);

		evas_object_data_set(slider->drag_balance, "melem", (void*)melem);

		/* Num num, sweet part :-) */
		edje_object_part_swallow(slider->swallow_object, 
				"balance",
				slider->drag_balance);
	}
		
	slider->melem = melem;
	/* Refing to mixer */
	if(ref)
		mixer_ref(melem->mixer);

	return slider;
}

Mixer_Slider* e_volume_slider_create_from_conf(Config_Mixer_Elem* conf,
		Volume_Face* face, int ref)
{
	Mixer_Elem* melem;
	Mixer_Name* mixer_name = NULL;
	Mixer_System_Name* msn;

	Evas_List	*l, *l1, *l2;
	
	for(l = face->volume->mixer_system_names; l; l = evas_list_next(l))
	{
		msn = evas_list_data(l);
		fprintf(stderr,"conf->elem_id = %x, system_id = %x\n",
				conf->elem_id, msn->system_id);

		if(SYSTEM_ID(conf->elem_id) == msn->system_id)
		{
			for(l1 = msn->mixer_names; l1; l1 = evas_list_next(l1))
			{
				mixer_name = evas_list_data(l1);

				fprintf(stderr,"conf->elem_id = %x, mixer_id = %x\n",
						conf->elem_id, mixer_name->mixer_id);

				if(MIXER_ID(conf->elem_id) == mixer_name->mixer_id);
					break;
			}

			if(l1)
				break;
		}
	}

	if(l1 && mixer_name)
	{
		Mixer* mixer;
		mixer = e_volume_mixer_open(face->volume, mixer_name);

		for(l2 = mixer->elems; l2; l2 = evas_list_next(l2))
		{
			melem = evas_list_data(l2);

			fprintf(stderr,"conf->hash = %x, e_util_hash_gen = %x\n",
					conf->elem_id, GET_ELEM_ID(melem));

			if(conf->elem_id == GET_ELEM_ID(melem))
			{
				fprintf(stderr,"Calling e_volume_slider_create\n");
				return e_volume_slider_create(melem, face->con->bg_evas, 
						face->conf->orient, conf->balance, 
						face, ref);
			}
		}
	}

	fprintf(stderr,"conf->elem_id = %x, not found\n", conf->elem_id);
	return NULL;

}

void e_volume_slider_free(Mixer_Slider* slider, int unref)
{
	if(!slider)
		return;

	if(slider->drag_balance)
		evas_object_hide(slider->drag_balance);

	evas_object_del(slider->name);
	evas_object_del(slider->drag_volume);

	if(slider->drag_balance)
		evas_object_del(slider->drag_balance);

	/* Unrefing mixer */
	if(unref)
		e_volume_mixer_unref_close(slider->face->volume, slider->melem->mixer);
	free(slider);
}

