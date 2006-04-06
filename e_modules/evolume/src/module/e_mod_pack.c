
#include <e_mixer.h>
#include <e_mod_volume.h>
#include <e_mod_volume.h>
#include <e_mod_face.h>
#include <e_mod_slider.h>
#include <e_mod_pack.h>
#include <e_mod_util.h>

int
e_volume_pack_pack_nth(Volume_Face *face, Mixer_Slider *slider, int nth)
{
   Evas_Object *after = NULL;
   Evas_List *l;

   if (nth <= evas_list_count(face->sliders))
      after = e_box_pack_object_nth(face->box_object, nth);

   if (nth == -1)
      e_box_pack_end(face->box_object, slider->swallow_object);
   else
     {
        if (after)
           e_box_pack_after(face->box_object, slider->swallow_object, after);
        else
           e_box_pack_start(face->box_object, slider->swallow_object);
     }

   e_box_pack_options_set(slider->swallow_object, 1, 1, 1, 1, .0, .0, 0, 0, -1, -1);

   evas_object_layer_set(slider->swallow_object, evas_object_layer_get(face->main_object) + 1);
   evas_object_show(slider->swallow_object);

   evas_object_show(slider->name);
   evas_object_show(slider->drag_volume);

   if (slider->drag_balance)
      evas_object_show(slider->drag_balance);

   face->sliders = evas_list_append(face->sliders, slider);

   return 0;
}

int
e_volume_pack_pack_weight(Volume_Face *face, Mixer_Slider *slider)
{
   Evas_List *l;
   int nth = -1, c;

   l = face->sliders;
   face->sliders = e_util_sliders_sort(l);

   for (l = face->sliders, c = 0; l; l = evas_list_next(l), c++)
     {
        Mixer_Slider *sl;

        sl = evas_list_data(l);

#if DEBUG
        fprintf(stderr, "%s: sl->weight = %d, sl->mixer = %s, "
                "slider->conf->weight = %d, slider->mixer = %s\n",
                __FUNCTION__, sl->conf->weight, sl->melem->name, slider->conf->weight, slider->melem->name);
#endif

        if (sl->conf->weight < slider->conf->weight)
           nth = c;
        else
           break;

     }

   if (nth == -1)
      nth = evas_list_count(face->sliders);
   if (nth == 0)
      nth = -1;

   e_volume_pack_pack_nth(face, slider, nth);

   return 0;
}

int
e_volume_pack_unpack(Volume_Face *face, Mixer_Slider *slider)
{
   /* Unpack it */
   e_box_unpack(slider->swallow_object);

   face->sliders = evas_list_remove(face->sliders, slider);

   evas_object_hide(slider->swallow_object);
   evas_object_hide(slider->name);
   evas_object_hide(slider->drag_volume);

   if (slider->drag_balance)
      evas_object_hide(slider->drag_balance);

   return 0;
}

int
e_volume_pack_swap(Volume_Face *face, Mixer_Slider *src, Mixer_Slider *dest)
{
   int weight;
   Evas_List *l;

   weight = src->conf->weight;
   src->conf->weight = dest->conf->weight;
   dest->conf->weight = weight;

   e_volume_pack_unpack(face, src);
   e_volume_pack_unpack(face, dest);

   e_volume_pack_pack_weight(face, dest);
   e_volume_pack_pack_weight(face, src);

   return 0;
}
