/* --------------------------------------
 * Retina - Evas powered image viewer
 * (C) 2000, Joshua Deere
 * dphase@dphase.net
 * --------------------------------------
 * See COPYING for license information
 * --------------------------------------
 */

#include "retina.h"

extern Evas e_area;
extern Evas_Object e_img;
extern Evas_Object e_checks;
extern Evas_Object e_btn1;
extern GtkWidget *area, *window;
extern int tb_status;
extern int mouse_button;

static void r_cb_m_down(void *_data, Evas _e, Evas_Object _o, int _b, int _x, int _y);
static void r_b1_click(void *_data, Evas _e, Evas_Object _o, int _b, int _x, int _y);

void
r_cb_init()
{
	/* Attack callbacks to objects */
	evas_callback_add(e_area, e_img, CALLBACK_MOUSE_DOWN, r_cb_m_down, NULL);
	evas_callback_add(e_area, e_btn1, CALLBACK_MOUSE_DOWN, r_b1_click, NULL);
}

static void
r_cb_m_down(void *_data, Evas _e, Evas_Object _o, int _b, int _x, int _y)
{
	switch(mouse_button){
		case 1: break;
		case 2: break;
		case 3:
			switch(tb_status){
				case 0:
					evas_show(e_area, e_btn1);
					evas_update_rect(e_area, 0, 0, 77, 23);
					evas_render(e_area);
					tb_status = 1;
					break;
				case 1:
					evas_hide(e_area, e_btn1);
					evas_update_rect(e_area, 0, 0, 77, 23);
					evas_render(e_area);
					tb_status = 0;
					break;
			}
	}
}

static void
r_b1_click(void *_data, Evas _e, Evas_Object _o, int _b, int _x, int _y)
{
	g_print("toolbar clicked\n");
}
