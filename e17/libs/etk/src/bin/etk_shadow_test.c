#include "etk_test.h"
#include "config.h"

#define NUM_COLS 3
#define NUM_PICTURES 6

static void _etk_test_shadow_offset_x_changed_cb(Etk_Object *object, double value, void *data);
static void _etk_test_shadow_offset_y_changed_cb(Etk_Object *object, double value, void *data);
static void _etk_test_shadow_radius_changed_cb(Etk_Object *object, double value, void *data);
static void _etk_test_shadow_border_toggled_cb(Etk_Object *object, void *data);

static Etk_Widget *_etk_test_picture_shadows[NUM_PICTURES];

/* Creates the window for the shadow test */
void etk_test_shadow_window_create(void *data)
{
   static Etk_Widget *win = NULL;
   Etk_Widget *vpaned;
   Etk_Widget *alignment;
   Etk_Widget *vbox;
   Etk_Widget *hbox;
   Etk_Widget *label;
   Etk_Widget *frame;
   Etk_Widget *slider;
   Etk_Widget *shadow;
   Etk_Widget *table;
   Etk_Widget *image;
   Etk_Widget *check;
   char image_path[4096];
   int i;

   if (win)
   {
      etk_widget_show_all(ETK_WIDGET(win));
      return;
   }

   win = etk_window_new();
   etk_window_title_set(ETK_WINDOW(win), "Etk Shadow Test");
   etk_signal_connect("delete-event", ETK_OBJECT(win), ETK_CALLBACK(etk_window_hide_on_delete), NULL);
   
   vpaned = etk_vpaned_new();
   etk_container_add(ETK_CONTAINER(win), vpaned);
   
   
   /**********************
    * The "Pictures" pane
    **********************/
   
   vbox = etk_vbox_new(ETK_FALSE, 0);
   etk_container_border_width_set(ETK_CONTAINER(vbox), 5);
   etk_paned_child1_set(ETK_PANED(vpaned), vbox, ETK_TRUE);
   
   label = etk_label_new("<title>Pictures:</title>");
   etk_box_append(ETK_BOX(vbox), label, ETK_BOX_START, ETK_BOX_FILL, 0);
   
   alignment = etk_alignment_new(0.5, 0.0, 0.0, 0.0);
   etk_box_append(ETK_BOX(vbox), alignment, ETK_BOX_START, ETK_BOX_EXPAND_FILL, 0);
   
   /* Create the table containing the pictures */
   table = etk_table_new(NUM_COLS, (NUM_PICTURES + NUM_COLS - 1) / NUM_COLS, ETK_TRUE);
   etk_container_add(ETK_CONTAINER(alignment), table);
   
   for (i = 0; i < NUM_PICTURES; i++)
   {
      _etk_test_picture_shadows[i] = etk_shadow_new();
      /* Here, we use a shadow container to make the image have a border and cast a shadow */
      etk_shadow_border_set(ETK_SHADOW(_etk_test_picture_shadows[i]), 1);
      etk_shadow_shadow_set(ETK_SHADOW(_etk_test_picture_shadows[i]),
         ETK_SHADOW_OUTSIDE, ETK_SHADOW_ALL, 15, 3, 3, 200);
      etk_table_attach(ETK_TABLE(table), _etk_test_picture_shadows[i],
         i % NUM_COLS, i % NUM_COLS, i / NUM_COLS, i / NUM_COLS,
         0, 0, ETK_TABLE_NONE);
      
      sprintf(image_path, PACKAGE_DATA_DIR "/images/picture%d.png", i + 1);
      image = etk_image_new_from_file(image_path, NULL);
      etk_container_add(ETK_CONTAINER(_etk_test_picture_shadows[i]), image);
   }

   
   /**********************
    * The "Settings" pane
    **********************/
   
   /* We use a shadow container to make the paned separator cast a shadow on the lower part */
   shadow = etk_shadow_new();
   etk_shadow_shadow_set(ETK_SHADOW(shadow), ETK_SHADOW_INSIDE, ETK_SHADOW_TOP, 15, 0, 3, 200);
   //etk_shadow_border_set(ETK_SHADOW(shadow), 1);
   etk_container_border_width_set(ETK_CONTAINER(shadow), 5);
   etk_paned_child2_set(ETK_PANED(vpaned), shadow, ETK_FALSE);
   
   /* Create the widgets controlling the settings of the shadows cast by the pictures */
   vbox = etk_vbox_new(ETK_FALSE, 0);
   etk_container_add(ETK_CONTAINER(shadow), vbox);
   
   /* TODO: FIXME: why "Settings:" is not displayed? */
   label = etk_label_new("<title>Shadow Settings:</title>");
   etk_box_append(ETK_BOX(vbox), label, ETK_BOX_START, ETK_BOX_FILL, 0);
   hbox = etk_hbox_new(ETK_FALSE, 0);
   etk_box_append(ETK_BOX(vbox), hbox, ETK_BOX_START, ETK_BOX_EXPAND_FILL, 0);
   
   frame = etk_frame_new("Offsets");
   etk_box_append(ETK_BOX(hbox), frame, ETK_BOX_START, ETK_BOX_EXPAND_FILL, 0);
   table = etk_table_new(2, 2, ETK_FALSE);
   etk_container_add(ETK_CONTAINER(frame), table);
   
   label = etk_label_new("X");
   etk_table_attach(ETK_TABLE(table), label, 0, 0, 0, 0, 0, 0, ETK_TABLE_HFILL);
   slider = etk_hslider_new(-15.0, 15.0, 3.0, 1.0, 5.0);
   etk_table_attach(ETK_TABLE(table), slider, 1, 1, 0, 0, 0, 0, ETK_TABLE_HFILL | ETK_TABLE_HEXPAND);
   etk_signal_connect("value-changed", ETK_OBJECT(slider), _etk_test_shadow_offset_x_changed_cb, NULL);
   
   label = etk_label_new("Y");
   etk_table_attach(ETK_TABLE(table), label, 0, 0, 1, 1, 0, 0, ETK_TABLE_HFILL);
   slider = etk_hslider_new(-15.0, 15.0, 3.0, 1.0, 5.0);
   etk_table_attach(ETK_TABLE(table), slider, 1, 1, 1, 1, 0, 0, ETK_TABLE_HFILL | ETK_TABLE_HEXPAND);
   etk_signal_connect("value-changed", ETK_OBJECT(slider), _etk_test_shadow_offset_y_changed_cb, NULL);
   
   etk_box_append(ETK_BOX(hbox), etk_vseparator_new(), ETK_BOX_START, ETK_BOX_NONE, 3);
   
   vbox = etk_vbox_new(ETK_FALSE, 0);
   etk_box_append(ETK_BOX(hbox), vbox, ETK_BOX_START, ETK_BOX_EXPAND_FILL, 0);
   
   frame = etk_frame_new("Radius");
   etk_box_append(ETK_BOX(vbox), frame, ETK_BOX_START, ETK_BOX_EXPAND_FILL, 0);
   slider = etk_hslider_new(0.0, 100.0, 15.0, 1.0, 10.0);
   etk_container_add(ETK_CONTAINER(frame), slider);
   etk_signal_connect("value-changed", ETK_OBJECT(slider), _etk_test_shadow_radius_changed_cb, NULL);
   
   check = etk_check_button_new_with_label("Show the border");
   etk_toggle_button_active_set(ETK_TOGGLE_BUTTON(check), ETK_TRUE);
   etk_box_append(ETK_BOX(vbox), check, ETK_BOX_START, ETK_BOX_NONE, 0);
   etk_signal_connect("toggled", ETK_OBJECT(check), _etk_test_shadow_border_toggled_cb, NULL);
   
   
   etk_widget_show_all(win);
}

/* Called when the x offset is changed */
static void _etk_test_shadow_offset_x_changed_cb(Etk_Object *object, double value, void *data)
{
   Etk_Shadow *shadow;
   int radius;
   int offset_y;
   int opacity;
   int i;
   
   for (i = 0; i < NUM_PICTURES; i++)
   {
      shadow = ETK_SHADOW(_etk_test_picture_shadows[i]);
      etk_shadow_shadow_get(shadow, NULL, NULL, &radius, NULL, &offset_y, &opacity);
      etk_shadow_shadow_set(shadow, ETK_SHADOW_OUTSIDE, ETK_SHADOW_ALL, radius, value, offset_y, opacity);
   }
}
/* Called when the y offset is changed */
static void _etk_test_shadow_offset_y_changed_cb(Etk_Object *object, double value, void *data)
{
   Etk_Shadow *shadow;
   int radius;
   int offset_x;
   int opacity;
   int i;
   
   for (i = 0; i < NUM_PICTURES; i++)
   {
      shadow = ETK_SHADOW(_etk_test_picture_shadows[i]);
      etk_shadow_shadow_get(shadow, NULL, NULL, &radius, &offset_x, NULL, &opacity);
      etk_shadow_shadow_set(shadow, ETK_SHADOW_OUTSIDE, ETK_SHADOW_ALL, radius, offset_x, value, opacity);
   }
}

/* Called when the radius value is changed */
static void _etk_test_shadow_radius_changed_cb(Etk_Object *object, double value, void *data)
{
   Etk_Shadow *shadow;
   int offset_x;
   int offset_y;
   int opacity;
   int i;
   
   for (i = 0; i < NUM_PICTURES; i++)
   {
      shadow = ETK_SHADOW(_etk_test_picture_shadows[i]);
      etk_shadow_shadow_get(shadow, NULL, NULL, NULL, &offset_x, &offset_y, &opacity);
      etk_shadow_shadow_set(shadow, ETK_SHADOW_OUTSIDE, ETK_SHADOW_ALL, value, offset_x, offset_y, opacity);
   }
}

/* Called when tge "Show the border" check button is toggled */
static void _etk_test_shadow_border_toggled_cb(Etk_Object *object, void *data)
{
   Etk_Toggle_Button *check;
   Etk_Bool has_border;
   int i;
   
   if (!(check = ETK_TOGGLE_BUTTON(object)))
      return;
   
   has_border = etk_toggle_button_active_get(check);
   for (i = 0; i < NUM_PICTURES; i++)
      etk_shadow_border_set(ETK_SHADOW(_etk_test_picture_shadows[i]), has_border ? 1 : 0);
}
