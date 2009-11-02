#undef FNAME
#undef NAME
#undef ICON

/* metadata */
#define FNAME image_map_3d_flow_start
#define NAME "Image Map 3D Flow"
#define ICON "bulb.png"

#ifndef PROTO
# ifndef UI
#  include "main.h"

/* standard var */
static int done = 0;

#define CNUM 48

/* private data */
static Evas_Object *o_images[CNUM];
static Evas_Object *o_reflec[CNUM];

/* setup */
static void _setup(void)
{
   int i;
   Evas_Object *o;
   for (i = 0; i < CNUM; i++)
     {
        char buf[256];
        
	o = evas_object_image_add(evas);
	o_images[i] = o;
        snprintf(buf, sizeof(buf), "cube%i.png", (i % 6) + 1);
        evas_object_image_file_set(o, build_path(buf), NULL);
	evas_object_image_fill_set(o, 0, 0, 256, 256);
	evas_object_resize(o, 256, 256);
        evas_object_image_smooth_scale_set(o, 0);
	evas_object_show(o);
        
	o = evas_object_image_add(evas);
	o_reflec[i] = o;
        snprintf(buf, sizeof(buf), "cube%i.png", (i % 6) + 1);
        evas_object_image_file_set(o, build_path(buf), NULL);
	evas_object_image_fill_set(o, 0, 0, 256, 256);
	evas_object_resize(o, 256, 256);
        evas_object_image_smooth_scale_set(o, 0);
	evas_object_show(o);
     }
   done = 0;
}

/* cleanup */
static void _cleanup(void)
{
   int i;
   for (i = 0; i < CNUM; i++) evas_object_del(o_images[i]);
   for (i = 0; i < CNUM; i++) evas_object_del(o_reflec[i]);
}

/* loop - do things */
static void _loop(double t, int f)
{
   int i;
   Evas_Map *m;
   Evas_Coord x, y, xx, yy, w, h, c, z;
   int center = 150;
   double ang, n;
   
   m = evas_map_new(4);
   evas_map_smooth_set(m, 0);
   
   for (i = 0; i < CNUM; i++)
     {
        x = (win_w / 2);
        y = (win_h / 2);
        w = 256;
        h = 256;
        
        c = (i - (CNUM / 2)) * (w / 4);
        c += (f % (6 * (w / 4)));
        
        ang = (double)c / (double)center;
        if (ang > 0.0) ang = ang * ang;
        else           ang = -ang * ang;
        ang *= -90.0;
        if (ang > 90.0) ang = 90.0;
        else if (ang < -90.0) ang = -90.0;

        if ((c > (-center)) && (c <= (center)))
          {
             n = (double)c / (double)center;
             if (c > 0)
               {
                  n = 1.0 - n;
                  n = 1.0 - (n * n);
               }
             else
               {
                  n = -(-1.0 - n);
                  n = -(1.0 - (n * n));
               }
             c = n * center * 2.0;
          }
        else if (c <= (-center))
          c = (c + center) - (center * 2);
        else if (c > (center))
          c = (c - center) + (center * 2);

        z = 0;
        if ((c > (-center)) && (c <= (center)))
          {
             z = (c * (w / 2)) / center;
             if (z < 0) z = -z;
             z = (w / 2) - z;
          }
        
        x += c;
        
        x -= (w / 2);
        xx = x + w;

        y -= (h / 2);
        yy = y + h;
        
        if (c <= 0)
          {
             evas_object_raise(o_images[i]);
             evas_object_raise(o_reflec[i]);
          }
        else
          {
             evas_object_lower(o_images[i]);
             evas_object_lower(o_reflec[i]);
          }
        
        evas_map_point_coord_set   (m, 0, x, y, -z);
        evas_map_point_image_uv_set(m, 0, 0, 0);
        evas_map_point_color_set   (m, 0, 255, 255, 255, 255);
        
        evas_map_point_coord_set   (m, 1, xx, y, -z);
        evas_map_point_image_uv_set(m, 1, 256, 0);
        evas_map_point_color_set   (m, 1, 255, 255, 255, 255);
        
        evas_map_point_coord_set   (m, 2, xx, yy, -z);
        evas_map_point_image_uv_set(m, 2, 256, 256);
        evas_map_point_color_set   (m, 2, 255, 255, 255, 255);
        
        evas_map_point_coord_set   (m, 3, x, yy, -z);
        evas_map_point_image_uv_set(m, 3, 0, 256);
        evas_map_point_color_set   (m, 3, 255, 255, 255, 255);
        
        evas_map_util_3d_rotate(m, 0, ang, 0, 
                                x + (w / 2), y + (h / 2), 0);
        evas_map_util_3d_perspective(m, (win_w / 2), (win_h / 2), -256, 512);
        
        evas_object_map_enable_set(o_images[i], 1);
        evas_object_map_set(o_images[i], m);

        evas_map_point_coord_set   (m, 0, x, yy, -z);
        evas_map_point_image_uv_set(m, 0, 0, 256);
        evas_map_point_color_set   (m, 0, 128, 128, 128, 128);
        
        evas_map_point_coord_set   (m, 1, xx, yy, -z);
        evas_map_point_image_uv_set(m, 1, 256, 256);
        evas_map_point_color_set   (m, 1, 128, 128, 128, 128);
        
        evas_map_point_coord_set   (m, 2, xx, yy + h, -z);
        evas_map_point_image_uv_set(m, 2, 256, 0);
        evas_map_point_color_set   (m, 2, 0, 0, 0, 0);
        
        evas_map_point_coord_set   (m, 3, x, yy + h, -z);
        evas_map_point_image_uv_set(m, 3, 0, 0);
        evas_map_point_color_set   (m, 3, 0, 0, 0, 0);
        
        evas_map_util_3d_rotate(m, 0, ang, 0, 
                                x + (w / 2), y + (h / 2), 0);
        evas_map_util_3d_perspective(m, (win_w / 2), (win_h / 2), -256, 512);
        
        evas_object_map_enable_set(o_reflec[i], 1);
        evas_object_map_set(o_reflec[i], m);
     }
   evas_map_free(m);
   
   FPS_STD(NAME);
}

/* prepend special key handlers if interactive (before STD) */
static void _key(char *key)
{
   KEY_STD;
}












/* template stuff - ignore */
# endif
#endif

#ifdef UI
_ui_menu_item_add(ICON, NAME, FNAME);
#endif

#ifdef PROTO
void FNAME(void);
#endif

#ifndef PROTO
# ifndef UI
void FNAME(void)
{
   ui_func_set(_key, _loop);
   _setup();
}
# endif
#endif
#undef FNAME
#undef NAME
#undef ICON
