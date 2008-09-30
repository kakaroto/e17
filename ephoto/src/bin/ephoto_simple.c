#include "ephoto.h"

typedef struct image_data image_data;
struct image_data {
	Evas_Object *clip;
	Evas_Object *image;
};

static void 
image_smart_add(Evas_Object *obj)
{
	Evas *e;
	image_data *id;

	id = NULL;
	id = calloc(1, sizeof(image_data));

	e = evas_object_evas_get(obj);
	id->clip = evas_object_rectangle_add(e);
	id->image = evas_object_image_add(e);

	evas_object_clip_set(id->image, id->clip);
	evas_object_show(id->image);

	evas_object_smart_member_add(id->clip, obj);
	evas_object_smart_member_add(id->image, obj);

	evas_object_smart_data_set(obj, id);
}

static void
image_smart_del(Evas_Object *obj)
{
	image_data *id;

	id = evas_object_smart_data_get(obj);
	evas_object_del(id->clip);
	evas_object_del(id->image);

	free(id);
}

static void
image_smart_move(Evas_Object *obj, Evas_Coord x, Evas_Coord y)
{
	image_data *id;

	id = evas_object_smart_data_get(obj);
	evas_object_move(id->clip, x, y);
	evas_object_move(id->image, x, y);
}

static void
image_smart_recalculate(Evas_Object *img, Evas_Coord x, Evas_Coord y, Evas_Coord w, Evas_Coord h)
{
	int iw, ih, fw, fh;
	double is, nw, nh, ns;

	evas_object_image_size_get(img, &iw, &ih);
	if (w == 0 || h == 0 || iw == 0 || ih == 0)
	{
		evas_object_image_fill_set(img, 0, 0, 1, 1);
		evas_object_resize(img, 0, 0);
		return;
	}
	is = (double)iw / (double)ih;
	nw = (double)w / (double)iw;
	nh = (double)h / (double)ih;
	ns = (nw < nh) ? nw : nh;

	fw = iw * ns;
	fh = fw / is;

	evas_object_move(img, x+(w-fw)/2, y+(h-fh)/2);
	evas_object_resize(img, fw, fh);
	evas_object_image_fill_set(img, 0, 0, fw, fh);
}

static void
image_smart_resize(Evas_Object *obj, Evas_Coord w, Evas_Coord h)
{
	image_data *id;
	Evas_Coord x, y;

	id = evas_object_smart_data_get(obj);

	evas_object_resize(id->clip, w, h);
	evas_object_geometry_get(obj, &x, &y, NULL, NULL);
	image_smart_recalculate(id->image, x, y, w, h);
}

static void
image_smart_show(Evas_Object *obj)
{
	image_data *id;

	id = evas_object_smart_data_get(obj);
	
	evas_object_show(id->clip);
}

static void
image_smart_hide(Evas_Object *obj)
{
	image_data *id;

	id = evas_object_smart_data_get(obj);

	evas_object_hide(id->clip);
}

static void
image_smart_clip_set(Evas_Object *obj, Evas_Object *clip)
{
	image_data *id;

	id = evas_object_smart_data_get(obj);

	evas_object_clip_set(id->clip, clip);
}

static void
image_smart_clip_unset(Evas_Object *obj)
{
	image_data *id;

	id = evas_object_smart_data_get(obj);

	evas_object_clip_unset(id->clip);
}

static Evas_Smart *
image_smart_class(void)
{
	static Evas_Smart *s;
	static const Evas_Smart_Class sc = {
		"Image_Smart",
		EVAS_SMART_CLASS_VERSION,
		image_smart_add,
		image_smart_del,
		image_smart_move,
		image_smart_resize,
		image_smart_show,
		image_smart_hide,
		NULL,
		image_smart_clip_set,
		image_smart_clip_unset,
		NULL
	};
	s = evas_smart_class_new(&sc);
	return(s);
}

static Evas_Object *
image_add(Evas *e)
{
	Evas_Smart *smart;

	smart = image_smart_class();
	return evas_object_smart_add(e, smart);
}

static void 
image_file_set(Evas_Object *obj, const char *img)
{
	Evas_Coord x, y, w, h;
	image_data *id;

	id = evas_object_smart_data_get(obj);

	evas_object_image_file_set(id->image, img, NULL);
	evas_object_geometry_get(obj, &x, &y, &w, &h);
	image_smart_recalculate(id->image, x, y, w, h);
}

static void 
window_resize(Ecore_Evas *ee)
{
	Evas_Object *interface;
	Evas_Coord w, h;
	Evas *e;

	ecore_evas_geometry_get(ee, NULL, NULL, &w, &h);

	e = ecore_evas_get(ee);
	interface = evas_object_name_find(e, "interface");
	evas_object_resize(interface, w, h);
}

static void
close_ephoto_simple(Ecore_Evas *ee)
{
      ecore_evas_shutdown();
      ecore_main_loop_quit();
      edje_shutdown();
}

void 
show_ephoto_simple(const char *img)
{
	Ecore_Evas *ee;
        Evas *e;
        Evas_Object *interface, *i;

	if (!ecore_init())
	{
		printf("Error - Could not initialize Ecore.\n");
		return;
	}

        if (!ecore_evas_init())
        {
                printf("Error - Could not initialize Ecore_Evas.\n");
                return;
        }

        ee = ecore_evas_software_x11_new(0, 0, 0, 0, 255, 255);
        if (!ee)
        {
                printf("Error - Could not create the Ecore_Evas window.\n");
                return;
        }
	ecore_evas_size_min_set(ee, 400, 400);
        ecore_evas_title_set(ee, "Ephoto Simple Viewer");
        ecore_evas_name_class_set(ee, "Ephoto Simple Viewer", "Ephoto Simple Viewer");
	ecore_evas_callback_resize_set(ee, window_resize);
	ecore_evas_callback_delete_request_set(ee, close_ephoto_simple);
	ecore_evas_callback_destroy_set(ee, close_ephoto_simple);
	ecore_evas_borderless_set(ee, 1);
	ecore_evas_shaped_set(ee, 1);

        edje_init();

        e = ecore_evas_get(ee);

        interface = edje_object_add(e);
        edje_object_file_set(interface, "/home/okra/coding/dev/e/ephoto/data/themes/ephoto/ephoto.edj", "ephoto");
        evas_object_name_set(interface, "interface");
        evas_object_move(interface, 0, 0);
	evas_object_resize(interface, 400, 400);
        evas_object_show(interface);

	i = image_add(e);
        image_file_set(i, img);
	evas_object_name_set(i, "image_smart");
        edje_object_part_swallow(interface, "image", i);

	ecore_evas_resize(ee, 400, 400);
        ecore_evas_show(ee);

	return;
}
