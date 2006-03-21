
#include <e.h>
#include "e_mod_main.h"
#include "e_mod_volume.h"
#include "e_mod_face.h"
#include "e_mod_slider.h"
#include "e_mod_pack.h"
#include "e_mod_mixer.h"
#include "e_mod_util.h"

static int _volume_count = 0;


static void _volume_face_cb_gmc_change(void *data, E_Gadman_Client* gmc __UNUSED__,
		E_Gadman_Change change)
{
	Volume_Face* face;
	int x, y, w, h;


	face = data;
	switch (change)
	{
		case E_GADMAN_CHANGE_MOVE_RESIZE:
			e_gadman_client_geometry_get(face->gmc, &x, &y, &w, &h);
			evas_object_move(face->main_object, x, y);
			evas_object_resize(face->main_object, w, h);

#if 0
			evas_object_geometry_get(face->table_object, &x, &y, &w, &h);
			DBG(stderr,"x = %d, y = %d, w = %d, h = %d\n",
					x,y,w,h);
#endif
			break;
		default:
			break;
	}

}

#define MOVE_EXCHANGE 0
#define MOVE_MOVE 1

static Mixer_Slider* src_slider;
static int m_action;

static Mixer_Slider* _get_mixer_slider_under_pointer(int x, int y,
		Volume_Face* face)
{
	Evas_List *l;
	Mixer_Slider *slider;

	for(l = face->sliders; l; l = evas_list_next(l))
	{
		int px, py, pw, ph;
		slider = evas_list_data(l);
		evas_object_geometry_get(slider->name, &px, &py, &pw, &ph);
		/*
		DBG(stderr,"px = %d, py = %d, pw = %d, ph = %d\n",
				px, py, pw, ph);
		*/
		if	((x > px && x < px + pw) &&
			 (y > py && y < py + ph))
			return slider;
	}

	return NULL;
}


static void _volume_face_cb_mouse_down(void *data, Evas* e __UNUSED__,
		Evas_Object* obj __UNUSED__, void* event_info)
{
	Volume_Face* face;
	Evas_Event_Mouse_Down* ev;

	face = data;
	ev = event_info;
	if (ev->button == 3)
	{
		if(src_slider == NULL)
		{
			e_menu_activate_mouse(face->menu, e_zone_current_get(face->con),
					ev->output.x, ev->output.y, 1, 1, E_MENU_POP_DIRECTION_AUTO,
					ev->timestamp);
			e_util_container_fake_mouse_up_all_later(face->con);
		}
		else
		{
			src_slider->conf->active = 0;
			e_volume_pack_unpack(face, src_slider);
			e_volume_slider_free(src_slider, 1);
			src_slider=NULL;
			e_config_save_queue();
		}
	}
	if (ev->button == 2)
	{
		Evas_List *l;
		Mixer_Slider *slider;
		Mixer_Elem *melem;

		for (l = face->sliders; l; l = evas_list_next(l))
		{
			slider = evas_list_data(l);

			melem = slider->melem;
			if (melem)
			{
				int mute;
				char buf[32];

				mute = mixer_get_mute(melem);
				mixer_set_mute(melem, !mute);
				snprintf(buf, 32, "mute,set,%d", mute);
				edje_object_signal_emit(slider->drag_volume, buf, "evolume_bin");
			}
		}

		e_volume_face_mixers_update(face);
	}
	if(ev->button == 1)
	{
		Mixer_Slider *slider;
		/* Try to get slider */
		slider = _get_mixer_slider_under_pointer(ev->output.x,
				ev->output.y, face);

		if(slider == NULL)
			return;

		if(src_slider == NULL)
			src_slider = slider;
		else if(m_action == MOVE_EXCHANGE)
		{

			if(src_slider == slider)
			{
				/* Cancel moving */
				src_slider = NULL;
				edje_object_signal_emit(slider->name, "choosen,0", "evolume_bin");
				return;
			}

			edje_object_signal_emit(src_slider->name, "choosen,0",
					"evolume_bin");
			e_volume_pack_swap(face, src_slider, slider);
			src_slider = NULL;
			e_config_save_queue();
		}

	}
}



static void _volume_face_cb_mouse_up(void *data, Evas* e __UNUSED__,
		Evas_Object* obj __UNUSED__, void* event_info)
{
	Volume_Face* face;
	Evas_Event_Mouse_Down* ev;

	face = data;
	ev = event_info;
	if(ev->button == 1)
	{
		Mixer_Slider	*slider;
		/* Try to get slider */
		slider = _get_mixer_slider_under_pointer(ev->output.x,
				ev->output.y, face);

		if(slider == NULL)
			return;

		if(src_slider == slider)
		{
			edje_object_signal_emit(slider->name, "choosen,1", "evolume_bin");
			m_action = MOVE_EXCHANGE;
		}
		else if(src_slider != NULL && slider != NULL)
		{
			/* Move code there */
			src_slider->conf->weight = slider->conf->weight - 1;
			/* FIXME: THIS IS SLOW, SHOULD USE REPACK INSTEAD */
//			e_volume_mixers_recreate(face);
			src_slider = NULL;
		}
	}
}

#ifndef MAX
#	define MAX(a,b) (a)>(b)?(a):(b)
#endif

static int _mixer_update_mixer_slider(Mixer_Slider* slider)
{
	Mixer_Elem* melem;
	int vl, vr;
	int vol, pvol;

	if (slider->locked)
		return 0;

	melem = slider->melem;

	if(melem)
		mixer_get_volume(melem, &vl, &vr);

	vol  = MAX(vl, vr);
	pvol = MAX(melem->pvl, melem->pvr);

	if (vl != melem->pvl || vr != melem->pvr || slider->first)
	{
		if (vol != pvol || slider->first)
		{
			/* Update volume */
			if (slider->orient == ORIENT_HORIZ)
				edje_object_part_drag_value_set(slider->drag_volume, 
						"volume_bar_drag", (float)vol/100.0f, 0);

			if (slider->orient == ORIENT_VERT && !slider->balance)
				edje_object_part_drag_value_set(slider->drag_volume,
						"volume_bar_drag", 0.5f,
						1.0f - (double)vol/100.0f);
		}
		
#if 0
		if (vl > melem->pvl || vr > melem->pvr)
		if (vol != pvol && (vl > melem->pvl || vr > melem->pvr))
#endif
		/* Now we use locking mechanism and checking is not needed */
		if (slider->drag_balance)
		{
			double bal;

			if (vr > vl)
				slider->balance = (int) rint(((double) vr / vl) * 100) - 100;
			else if (vl > vr)
				slider->balance = 100 - (int) rint(((double) vl / vr) * 100);
			else
				slider->balance = 0;

			bal = ((double)slider->balance/200.0f) + 0.5f;

			//	DBG(stderr,"balance = %d, bal = %f\n", balance, bal);

			edje_object_part_drag_value_set(slider->drag_balance,
					"volume_bar_drag", bal, 1.0f - (double)vol/100.0f);

		}

	}
#if 0
/*	if (((vl != melem->pvl || vr != melem->pvr)
				&& slider->drag_balance && vol == pvol)
				|| slider->first)*/
	/*
	DBG(stderr,"vol = %d, pvol = %d\nvl = %d, pvl = %d\n"
					"vr = %d, pvr = %d\n", vol, pvol, vl, melem->pvl,
					vr, melem->pvr);
					*/
	//if ( vol == pvol || slider->first )
/*		|| vl > melem->pvl
			|| vr > melem->pvr || slider->first)  && 1)
			*/
#endif

	melem->pvr = vr;
	melem->pvl = vl;

	if(slider->first)
		slider->first = 0;

	return 1;
}
		
void	e_volume_face_mixers_update(Volume_Face* face)
{
	Evas_List *l;
	Mixer_Slider *m;
	for(l = face->sliders; l; l = evas_list_next(l))
	{
		m = (Mixer_Slider*)evas_list_data(l);
		if(m)
			_mixer_update_mixer_slider(m);

		//DBG(stderr, "m = %p\n", m);
		//DBG(stderr,"mixer = %p\n", evas_object_data_get(m, "mixer"));
	}
}


void e_volume_face_mixer_update(Volume_Face* face, 
		Config_Mixer* mixer_conf)
{
	Evas_List *l;
	Mixer_System_Name* msn;
	Mixer_Name* name;

	for(l = face->volume->mixer_system_names; l; l = evas_list_data(l))
	{
		msn = evas_list_data(l);
		if(SYSTEM_ID(msn->system_id) == SYSTEM_ID(mixer_conf->mixer_id))
			break;
	}

	if(!l)
		return;

	for(l = msn->mixer_names; l; l = evas_list_next(l))
	{
		name = evas_list_data(l);
		if(MIXER_ID(name->mixer_id) == MIXER_ID(mixer_conf->mixer_id))
			break;
	}

	if(!l || !name)
		return;


	/* Activating mixer */
	if(mixer_conf->active)
	{
		Mixer* mixer;
		mixer = e_volume_mixer_open(face->volume, name);
		if(mixer)
		{
			e_volume_mixers_create(face, mixer, face->con->bg_evas, 1);
			e_volume_face_mixers_update(face);
		}
	}
	else
	{
		Mixer *mixer;

		mixer = e_util_search_mixer_by_name(face->volume->mixers,
				name);

		if(mixer)
			e_volume_mixers_free(face, mixer, 1);
	}

}

void e_volume_face_mixer_elem_update(Volume_Face* face, 
		Config_Mixer_Elem* elem_conf,
		Config_Mixer* mixer_conf)
{

#if 0
	Mixer_Elem *melem;
	Mixer* mixer;

	Evas_List *l, *l1, *l2;
	for(l = face->volume->mixers; l; l = evas_list_next(l))
	{
		mixer = evas_list_data(l);
		if (MIXER_ID(mixer->name->mixer_id) == MIXER_ID(mixer_conf->mixer_id) &&
				SYSTEM_ID(mixer->name->mixer_id) == SYSTEM_ID(mixer_conf->mixer_id))
			break;
	}

	if(!l)
		return;

	for(l = mixer->elems; l; l = evas_list_next(l))
	{
		melem = evas_list_data(l);
		if(melem->elem_id == elem_conf->elem_id)
			break;
	}

	if(!l)
		return;
#endif

	if(elem_conf->active)
	{
		Mixer_Slider* slider;
		slider = e_volume_slider_create_from_conf(elem_conf, face, 1);
		if(slider)
		{
			slider->conf = elem_conf;
			e_volume_pack_pack_weight(face, slider);
			_mixer_update_mixer_slider(slider);
		}
	}
	else
	{
		Mixer_Slider* slider;
		Evas_List *l;
		for(l = face->sliders; l; l = evas_list_next(l))
		{
			slider = evas_list_data(l);
			if(slider->conf->elem_id == elem_conf->elem_id)
				break;
		}

		if(l)
		{
			e_volume_pack_unpack(face, slider);
			e_volume_slider_free(slider, 1);
		}

	}


	e_config_save_queue();

}

void	e_volume_face_mixers_create(Volume_Face* face)
{
	Evas_List* l;
	Volume* volume;
	E_Container* con;

	volume = face->volume;
	con = face->con;
	int c;

	face->sliders = NULL;
	for(c = 0, l = volume->mixers; l; l = evas_list_next(l))
	{
		Mixer* mixer;
//		DBG(stderr, "creating %d\n", c++);
		mixer = evas_list_data(l);
		e_volume_mixers_create(face, mixer, con->bg_evas, 1);
	}

	//DBG(stderr,"face->sliders = %p\n", face->sliders);

	if(face->conf)
	/* Also resize mixer object */
	{
		int h;
		int c;
		c = evas_list_count(face->sliders);
		h = (c + 1)* face->entry_height;
		//DBG(stderr,"face->entry_height = %d\n", face->entry_height);
		e_gadman_client_auto_size_set(face->gmc, 128, h);
	}
}

Volume_Face* e_volume_face_new(E_Container* con, Volume* volume)
{
	Volume_Face* face;
	Evas_Object* o;
	char buf[1024];

	face = E_NEW(Volume_Face, 1);

	face->volume = volume;

	face->con = con;
	e_object_ref(E_OBJECT(con));
	evas_event_freeze(con->bg_evas);

	o = edje_object_add(con->bg_evas);
	face->main_object = o;
	snprintf(buf, 1024, "%s/volume.edj", module_root);
	edje_object_file_set(o, buf,
			"volume/main");

	evas_object_event_callback_add(o, EVAS_CALLBACK_MOUSE_DOWN, 
			_volume_face_cb_mouse_down, face);

	evas_object_event_callback_add(o, EVAS_CALLBACK_MOUSE_UP, 
			_volume_face_cb_mouse_up, face);

	evas_key_modifier_add(evas_object_evas_get(o), "Control");
	evas_key_modifier_add(evas_object_evas_get(o), "Shift");

	evas_object_show(o);

	o = e_box_add(con->bg_evas);
	face->box_object = o;
	e_box_homogenous_set(o, 1);
	edje_object_part_swallow(face->main_object, "items", face->box_object);
	evas_object_show(o);


	/* setup gadman */
	face->gmc = e_gadman_client_new(con->gadman);
	e_gadman_client_domain_set(face->gmc, "module.evolume", _volume_count++);
	e_gadman_client_policy_set(face->gmc,
			E_GADMAN_POLICY_ANYWHERE |
			E_GADMAN_POLICY_HMOVE |
			E_GADMAN_POLICY_VMOVE |
			E_GADMAN_POLICY_HSIZE |
			E_GADMAN_POLICY_VSIZE);
	e_gadman_client_min_size_set(face->gmc, 14, 7);
	e_gadman_client_align_set(face->gmc, 1.0, 0.5);
			

	e_gadman_client_change_func_set(face->gmc, _volume_face_cb_gmc_change, 
			face);
	e_gadman_client_load(face->gmc);

	_volume_face_cb_gmc_change((void*)face, face->gmc,
			E_GADMAN_CHANGE_MOVE_RESIZE);

	evas_event_thaw(con->bg_evas);

	return face;
}

int e_volume_face_free(Volume_Face* face)
{
	Evas_List *l;

	if(!face)
		return 0;

	e_object_unref(E_OBJECT(face->con));
	e_object_del(E_OBJECT(face->gmc));

	evas_object_del(face->box_object);
	evas_object_del(face->main_object);

	for(l = face->sliders; l; l = evas_list_next(l))
	{
		Mixer_Slider* slider;
		slider = evas_list_data(l);

		if(slider)
			e_volume_slider_free(slider, 1);
	}

	_volume_count--;

	return 1;
}
