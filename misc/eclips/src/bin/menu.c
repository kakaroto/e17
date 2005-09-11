#include <unistd.h>
#include <math.h>
#include <Evas.h>
#include <Ecore.h>
#include <Ecore_Evas.h>
#include <Ecore_X.h>
#include <Etox.h>
#include <X11/Xatom.h>
#include <X11/Xutil.h>
#include <X11/Xlib.h>
#include <Imlib2.h>
#include <limits.h>
#include "eclipse.h"
#include "menu.h"
#include "bg.h"

extern Ecore_Evas *ee;
extern Eclipse_Options *eo;
Eclips_Menu *menu_ptr;

/**
 * menu_fade_in - Fade in a menu
 * @m the Eclips_Menu
 */
int menu_fade_in(void* m)
{

   Eclips_Menu *menu = (Eclips_Menu*)m;
   evas_object_fade_in(menu->text);   
   return evas_object_fade_in(menu->bg);
}

/**
 * menu_fade_out - Fade out a menu
 * @m the Eclips_Menu
 */
int menu_fade_out(void* m)
{
   Eclips_Menu *menu = (Eclips_Menu*)m;
   if(evas_object_fade_out(menu->bg) == 0)
     {	
	evas_object_del(menu->bg);
	evas_free(ecore_evas_get(menu->window));
	ecore_evas_free(menu->window);
	return 0;
     }
   return 1;
}


/**
 * eclips_menu_new - Create a new empty menu
 * @view the view we're working on
 * @w initial menu width
 * @h initial menu height
 */
Eclips_Menu *eclips_menu_new(Evas_Event_Mouse_Down *ev, Eclipse_View *view, char *filename, int total_w, int total_h)
{   
   Evas        *evas;
   Ecore_Timer *timer;
   Eclips_Menu *menu;
   
   int x,y,w,h;
   int a,r,g,b;
   
   /* create the menu window */
   menu = (Eclips_Menu*)calloc(1,sizeof(Eclips_Menu));
   ecore_evas_geometry_get(ee, &x, &y, &w, &h);   
   
   menu->window = ecore_evas_software_x11_new(0, 0, x+ev->output.x-40, y+ev->output.y-10,  total_w, total_h);
   ecore_evas_title_set(menu->window, "eclipse root menu");
   ecore_evas_borderless_set(menu->window, 1);
   ecore_evas_shaped_set(menu->window, 1);
   ecore_evas_override_set(menu->window, 1);
   evas = ecore_evas_get(menu->window);      
   
   /* create bg for menu */
   menu->bg = evas_object_image_add(evas);
   evas_object_image_file_set(menu->bg, filename, NULL);
   evas_object_resize(menu->bg,  total_w, total_h);
   evas_object_image_fill_set(menu->bg, 0, 0,  total_w, total_h);
   evas_object_image_alpha_set(menu->bg, 1);
   
   /* fade in the menu */
   evas_object_color_get(menu->bg, &r, &g, &b, &a);
   evas_object_color_set(menu->bg, r, g, b, 0);
   timer = ecore_timer_add(eo->transition_timer, menu_fade_in, (void*)menu);
   menu_ptr = menu;
   
   evas_object_layer_set(menu->bg, 1);
   evas_object_show(menu->bg);
   
   return menu;
}

/**
 * root_menu_show - Show the root menu when middle clicking
 * @ev The evas mouse event
 * @view The eclips view we're working on
 */
void root_menu_show(Evas_Event_Mouse_Down *ev,
		    Eclipse_View *view)
{

   Evas_Object *background;
   Eclips_Menu *menu;
   Evas *evas;
   int x,y,w,h,total_w=160,total_h=200,hor_sp=5,ver_sp=5;
      
   ecore_evas_geometry_get(ee, &x, &y, &w, &h);
   
   /* create menu */
   menu = eclips_menu_new(ev,view,DATADIR"menubg.png",w,h);
      
   /* add menu items */
   evas = ecore_evas_get(menu->window);   
   menu->text = etox_new_all(evas, 5, 5, total_w, total_h , 255, ETOX_ALIGN_LEFT);
   etox_context_set_align(etox_get_context(menu->text), ETOX_ALIGN_LEFT);
   etox_context_set_font(etox_get_context(menu->text), DATADIR"sinon.ttf", 12);
   etox_context_set_style(etox_get_context(menu->text), "shadow");
   etox_context_set_color(etox_get_context(menu->text), 255, 255, 255, 255);
   etox_set_soft_wrap(menu->text, 1);
   etox_set_alpha(menu->text, 255);

     {
	char *str = strdup("Option1\nOption2\nOption3)");
	etox_set_text(menu->text, str);//(char*)evas_list_data(view->images)
     }
   
   evas_object_layer_set(menu->text, 1000);
   evas_object_show(menu->text);
   
   /* final resize */
   if(total_w<5*strlen((char*)evas_list_data(view->images)))
     total_w = 5*strlen((char*)evas_list_data(view->images));
   ecore_evas_resize(menu->window,total_w+hor_sp, total_h+ver_sp);
   evas_object_image_fill_set(menu->bg, 0, 0, total_w+hor_sp, total_h+ver_sp);
   evas_object_resize(menu->bg, total_w+hor_sp, total_h+ver_sp);
   evas_object_resize(menu->text, total_w+hor_sp, total_h+ver_sp);

   /*
     {
	// make sure popups stay inside the screen
	// change this code and place it above menu creation
	int rw,rh;
	int x2,y2,w2,h2;
	ecore_x_window_size_get(0, &rw, &rh);
	ecore_evas_geometry_get(menu->window,&x2,&y2,&w2,&h2);
	w2 = total_w+hor_sp;
	h2 = total_h+ver_sp;
	printf("menu has: x=%d y=%d w=%d h=%d\n",x2,y2,w2,h2);
	printf("win  has: w=%d h=%d\n",rw,rh);
	if(x2+w2 > rw)
	  ecore_evas_move(menu->window,rw-(w2),y);
	if(y2+h2 > rh)
	  ecore_evas_move(menu->window,x2,rh-(h2));
     }
    */
   
   /* menu transparency */
   background = transparency_get_pixmap_with_windows(evas,NULL,x+ev->output.x-40,y+ev->output.y-10, (int)total_w+hor_sp, (int)total_h+ver_sp);
   evas_object_layer_set(background, -9999);
   evas_object_show(background);		    
   
   /* show the menu */
   ecore_evas_show(menu->window);   
}

void root_menu_hide()
{   
   if(menu_ptr != NULL)
     {
	Ecore_Timer *timer;
	timer = ecore_timer_add(eo->transition_timer, menu_fade_out, (void*)menu_ptr);
     }   
}

void show_image_info_menu(Evas_Event_Mouse_Down *ev,
		    Eclipse_View *view)
{

   Evas_Object *ob,*background;
   Eclips_Menu *menu;
   Evas *evas;
   int x,y,w,h,total_w=160,total_h=200,hor_sp=5,ver_sp=5;
   
   ecore_evas_geometry_get(ee, &x, &y, &w, &h);
   
   /* create menu */
   if ((!strncmp((char*)evas_list_data(view->images), "http://", 7)) ||
       (!strncmp((char*)evas_list_data(view->images), "https://", 8)) ||
       (!strncmp((char*)evas_list_data(view->images), "ftp://", 6)))
     {
	char outfilename[PATH_MAX];
	snprintf(outfilename, PATH_MAX, "/tmp/eclips%d%s",getpid(),get_shortname_for((char*)evas_list_data(view->images)));
	menu = eclips_menu_new(ev,view,outfilename,w,h);
     }
   else 
     menu = eclips_menu_new(ev,view,(char*)evas_list_data(view->images),w,h);
          
   evas_object_image_size_get(menu->bg, &w, &h);
   evas_object_resize(menu->bg, 320, (int)((320.0/(float)w)*(float)h));
   evas_object_image_fill_set(menu->bg, 1, 0,   320, (int)((320.0/(float)w)*(float)h));
   total_w = 320+1;
   total_h = (int)((320.0/(float)w)*(float)h)+1;   
      
   /* add menu items */
   evas = ecore_evas_get(menu->window);   
   menu->text = etox_new_all(evas, 5, 5, total_w, total_h , 255, ETOX_ALIGN_LEFT);
   etox_context_set_align(etox_get_context(menu->text), ETOX_ALIGN_LEFT);
   etox_context_set_font(etox_get_context(menu->text), DATADIR"sinon.ttf", 12);
   etox_context_set_style(etox_get_context(menu->text), "shadow");
   etox_context_set_color(etox_get_context(menu->text), 255, 255, 255, 255);
   etox_set_soft_wrap(menu->text, 1);
   etox_set_text(menu->text, (char*)evas_list_data(view->images));
   etox_set_alpha(menu->text, 255);
   evas_object_layer_set(menu->text, 1000);
   evas_object_show(menu->text);
   
   /* final resize */
   if(total_w<5*strlen((char*)evas_list_data(view->images)))
     total_w = 5*strlen((char*)evas_list_data(view->images));
   ecore_evas_resize(menu->window,total_w+hor_sp, total_h+ver_sp);
   evas_object_image_fill_set(menu->bg, 0, 0, total_w+hor_sp, total_h+ver_sp);
   evas_object_resize(menu->bg, total_w+hor_sp, total_h+ver_sp);
   evas_object_resize(menu->text, total_w+hor_sp, total_h+ver_sp);

   /* menu transparency */
   background = transparency_get_pixmap_with_windows(evas,NULL,x+ev->output.x-40,y+ev->output.y-10, (int)total_w+hor_sp, (int)total_h+ver_sp);
   evas_object_layer_set(background, -9999);
   evas_object_show(background);		    
   
   /* show the menu */
   ecore_evas_show(menu->window);   
}

void show_menu()
{
}

