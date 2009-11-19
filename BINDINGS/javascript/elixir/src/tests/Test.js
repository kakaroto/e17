var FN = "/.fonts/";
var test = true;

test &= elx.load("evas");
test &= elx.load("ecore");
test &= elx.load("ecore-evas");

var x = 50;
var y = 50;
var dx = +20;
var dy = +10;

function anim_cb(obj)
{
   x += dx;
   y += dy;

   if (x + 250 > 720 || x < 0)
     {
	dx = -dx;
	x += 2 * dx;
     }

   if (y + 200 > 576 || y < 0)
     {
	dy = -dy;
	y += 2 * dy;
     }

   evas_object_move(obj, x, y);

   return 1;
}

function start()
{
   var obj;

   ecore_init();
   ecore_evas_init();

   ecore_animator_frametime_set(1 / 20);

   ee = ecore_evas_new(null, 0, 0, 720, 576, "name=Kuubi;double_buffer=1;");
   ecore_evas_fullscreen_set(ee, 1);

   var evas = ecore_evas_get(ee);

   evas_image_cache_set(evas, 10 * 1024 * 1024);
   evas_font_path_prepend(evas, FN);
   evas_font_cache_set(evas, 512 * 1024);

   obj = evas_object_rectangle_add(evas);
   evas_object_resize(obj, 720, 576);
   evas_object_color_set(obj, 0, 0, 0, 255);
   evas_object_show(obj);

   obj = evas_object_rectangle_add(evas);
   evas_object_resize(obj, 250, 200);
   evas_object_color_set(obj, 128, 64, 0, 180);
   evas_object_move(obj, 50, 50);
   evas_object_show(obj);

   ecore_animator_add(anim_cb, obj);

   ecore_evas_show(ee);
   ecore_main_loop_begin();
   evas_object_del(obj);

   ecore_evas_shutdown();
   ecore_shutdown();
}

if (test)
  start();