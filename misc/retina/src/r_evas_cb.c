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
extern Evas_Object e_btn1, e_btn2, e_btn3, e_btn4, e_btn5, e_bs;
extern GtkWidget *area, *window;
extern GtkWidget *bwin;
extern int tb_status;
extern int mouse_button;
extern guint current_idle;


void
r_cb_init()
{
	/* Attack callbacks to objects */
	//evas_callback_add(e_area, e_img, CALLBACK_MOUSE_DOWN, r_cb_m_down, NULL);
	evas_callback_add(e_area, e_btn1, CALLBACK_MOUSE_DOWN, r_b1_click, NULL);
	evas_callback_add(e_area, e_btn2, CALLBACK_MOUSE_DOWN, r_b2_click, NULL);
	evas_callback_add(e_area, e_btn3, CALLBACK_MOUSE_DOWN, r_b3_click, NULL);
	evas_callback_add(e_area, e_btn4, CALLBACK_MOUSE_DOWN, r_b4_click, NULL);
	evas_callback_add(e_area, e_btn5, CALLBACK_MOUSE_DOWN, r_b5_click, NULL);

	//evas_callback_add(e_area, e_img, CALLBACK_MOUSE_UP, r_m_up, NULL);
	evas_callback_add(e_area, e_btn1, CALLBACK_MOUSE_UP, r_m_up, NULL);
	evas_callback_add(e_area, e_btn2, CALLBACK_MOUSE_UP, r_m_up, NULL);
	evas_callback_add(e_area, e_btn3, CALLBACK_MOUSE_UP, r_m_up, NULL);
	evas_callback_add(e_area, e_btn4, CALLBACK_MOUSE_UP, r_m_up, NULL);
	evas_callback_add(e_area, e_btn5, CALLBACK_MOUSE_UP, r_m_up, NULL);
}

void
r_cb_m_down(void *_data, Evas _e, Evas_Object _o, int _b, int _x, int _y)
{
	switch(mouse_button){
		case 1: break;
		case 2: break;
		case 3:
			switch(tb_status){
				case 0:
					evas_show(e_area, e_bs);
					evas_show(e_area, e_btn1);
					evas_show(e_area, e_btn2);
					evas_show(e_area, e_btn3);
					evas_show(e_area, e_btn4);
					evas_show(e_area, e_btn5);
					QUEUE_DRAW;
					tb_status = 1;
					break;
				case 1:
					evas_hide(e_area, e_bs);
					evas_hide(e_area, e_btn1);
					evas_hide(e_area, e_btn2);
					evas_hide(e_area, e_btn3);
					evas_hide(e_area, e_btn4);
					evas_hide(e_area, e_btn5);
					QUEUE_DRAW
					tb_status = 0;
					break;
			}
	}
}

void
r_m_up(void *_data, Evas _e, Evas_Object _o, int _b, int _x, int _y)
{
	/* nothing */
}

void
r_b1_click(void *_data, Evas _e, Evas_Object _o, int _b, int _x, int _y)
{
	r_file_load();
}

void
r_b2_click(void *_data, Evas _e, Evas_Object _o, int _b, int _x, int _y)
{
	r_error("Retina cannot save images yet!");
}

void
r_b3_click(void *_data, Evas _e, Evas_Object _o, int _b, int _x, int _y)
{
  gtk_widget_show(bwin);
}

void
r_b4_click(void *_data, Evas _e, Evas_Object _o, int _b, int _x, int _y)
{
	r_error("Image info hasn't been implemented!");
}

void
r_b5_click(void *_data, Evas _e, Evas_Object _o, int _b, int _x, int _y)
{
	gtk_main_quit();
}
