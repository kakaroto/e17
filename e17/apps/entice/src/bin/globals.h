#ifndef __GLOBAL_H__
#define __GLOBAL_H__

/* globals */
extern Evas_Object *o_bg;
extern Evas_Object *o_logo;
extern Evas_Object *o_panel;
extern Evas_Object *o_showpanel;
extern Evas_Object *o_hidepanel;
extern Evas_Object *o_showbuttons;
extern Evas_Object *o_arrow_l;
extern Evas_Object *o_arrow_r;
extern Evas_Object *o_arrow_t;
extern Evas_Object *o_arrow_b;
extern Evas_Object *o_image;
extern Evas_Object *o_mini_image;
extern Evas_Object *o_mini_select;
extern Evas_Object *o_list_select;
extern Evas_Object *o_panel_arrow_u;
extern Evas_Object *o_panel_arrow_d;
extern Evas_Object *o_bt_close;
extern Evas_Object *o_bt_delete;
extern Evas_Object *o_bt_expand;
extern Evas_Object *o_bt_full;
extern Evas_Object *o_bt_next;
extern Evas_Object *o_bt_prev;
extern Evas_Object *o_bt_zoom_in;
extern Evas_Object *o_bt_zoom_normal;
extern Evas_Object *o_bt_zoom_out;
extern Evas_Object *o_txt_info[10];
extern Evas_Object *o_txt_tip[5];
extern Evas_Object *icon_drag;
extern Evas_Object *o_trash;
extern Evas_Object *o_trash_can;

extern char         txt_info[2][4096];

extern Evas_List   *images;
extern Evas_List   *current_image;
extern Image       *generating_image;

extern Evas        *evas;
extern int          render_method;
extern int          max_colors;
extern int          smoothness;
extern int          win_w;
extern int          win_h;
extern int          win_x;
extern int          win_y;
extern Window       main_win;
extern Window       ewin;

extern int          icon_x;
extern int          icon_y;

extern int          dragging;
extern int          panel_active;
extern int          buttons_active;
extern int          scroll_x;
extern int          scroll_y;
extern int          scroll_sx;
extern int          scroll_sy;
extern int          arrow_l;
extern int          arrow_r;
extern int          arrow_t;
extern int          arrow_b;
extern int          down_x;
extern int          down_y;
extern int          down_sx;
extern int          down_sy;
extern double       scale;
extern double       focus_pos;
extern int          need_thumbs;
extern int          dnd_num_files;
extern char       **dnd_files;

#endif /* __GLOBAL_H__ */
