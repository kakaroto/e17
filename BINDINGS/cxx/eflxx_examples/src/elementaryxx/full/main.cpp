#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include "test.h"

#ifndef ELM_LIB_QUICKLAUNCH
static void my_win_del(Evasxx::Object &obj, void *event_info);
static void my_win_main(void);

//
void test_bg_plain (void *data, Evas_Object *obj, void *event_info);
void test_bg_image (void *data, Evas_Object *obj, void *event_info);
void test_icon (void *data, Evas_Object *obj, void *event_info);
void test_box_horiz (void *data, Evas_Object *obj, void *event_info);
void test_box_vert (void *data, Evas_Object *obj, void *event_info);
void test_button (void *data, Evas_Object *obj, void *event_info);
void test_toggle (void *data, Evas_Object *obj, void *event_info);
void test_clock (void *data, Evas_Object *obj, void *event_info);
void test_layout (void *data, Evas_Object *obj, void *event_info);
void test_hover (void *data, Evas_Object *obj, void *event_info);
void test_hover2 (void *data, Evas_Object *obj, void *event_info);
void test_entry (void *data, Evas_Object *obj, void *event_info);
void test_entry_scolled (void *data, Evas_Object *obj, void *event_info);
void test_check (void *data, Evas_Object *obj, void *event_info);
void test_radio (void *data, Evas_Object *obj, void *event_info);
//
/*
void test_anchorview(void *data, Evas_Object *obj, void *event_info);
void test_anchorblock(void *data, Evas_Object *obj, void *event_info);
void test_toolbar(void *data, Evas_Object *obj, void *event_info);
void test_hoversel(void *data, Evas_Object *obj, void *event_info);
void test_list(void *data, Evas_Object *obj, void *event_info);
void test_list2(void *data, Evas_Object *obj, void *event_info);
void test_list3(void *data, Evas_Object *obj, void *event_info);
void test_carousel(void *data, Evas_Object *obj, void *event_info);
void test_inwin(void *data, Evas_Object *obj, void *event_info);
void test_inwin2(void *data, Evas_Object *obj, void *event_info);
void test_scaling(void *data, Evas_Object *obj, void *event_info);
void test_scaling2(void *data, Evas_Object *obj, void *event_info);*/
void test_slider(void *data, Evas_Object *obj, void *event_info);
void test_genlist(void *data, Evas_Object *obj, void *event_info);
void test_genlist2(void *data, Evas_Object *obj, void *event_info);
void test_genlist3(void *data, Evas_Object *obj, void *event_info);
void test_genlist4(void *data, Evas_Object *obj, void *event_info);
void test_genlist5(void *data, Evas_Object *obj, void *event_info);
void test_genlist6(void *data, Evas_Object *obj, void *event_info);
void test_table(void *data, Evas_Object *obj, void *event_info);
/*void test_pager(void *data, Evas_Object *obj, void *event_info);
void test_win_state(void *data, Evas_Object *obj, void *event_info);
*/
void test_progressbar(void *data, Evas_Object *obj, void *event_info);
/*void test_fileselector(void *data, Evas_Object *obj, void *event_info);
void test_separator(void *data, Evas_Object *obj, void *event_info);*/
void test_scroller(void *data, Evas_Object *obj, void *event_info);
void test_spinner(void *data, Evas_Object *obj, void *event_info);
/*void test_index(void *data, Evas_Object *obj, void *event_info);
void test_photocam(void *data, Evas_Object *obj, void *event_info);
void test_photo(void *data, Evas_Object *obj, void *event_info);
void test_icon_desktops(void *data, Evas_Object *obj, void *event_info);
void test_notify(void *data, Evas_Object *obj, void *event_info);
void test_slideshow(void *data, Evas_Object *obj, void *event_info);
void test_menu(void *data, Evas_Object *obj, void *event_info);
 */
void test_panel (void *data, Evas_Object *obj, void *event_info);

Application *elmApp;


static void
my_win_del (Evasxx::Object &obj, void *event_info)
{
  // TODO
  /* called when my_win_main is requested to be deleted */
  elmApp->exit(); /* exit the program's main loop that runs in elm_run() */
}

#endif // ELM_LIB_QUICKLAUNCH

int main (int argc, const char **argv)
{
  elmApp = new Application (argc, argv);

  /* 1 create an elm window - it returns an evas object. this is a little
   * special as the object lives in the canvas that is inside the window
   * so what is returned is really inside the window, but as you manipulate
   * the evas object returned - the window will respond. elm_win makes sure
   * of that so you can blindly treat it like any other evas object
   * pretty much, just as long as you know it has special significance */

  /* the first parameter is a "parent" window - eg for a dialog you want to
   * have a main window it is related to, here it is NULL meaning there
   * is no parent. "main" is the name of the window - used by the window
   * manager for identifying the window uniquely amongst all the windows
   * within this application (and all instances of the application). the
   * type is a basic window (the final parameter) */
  Window *win = Window::factory ("main", ELM_WIN_BASIC);

  /* set the title of the window - this is in the titlebar */
  win->setTitle ("Elementary Tests");

  /* set a callback on the window when "delete-request" is emitted as
   * a callback. when this happens my_win_del() is called and the
   * data pointer (first param) is passed the final param here (in this
   * case it is NULL). This is how you can pass specific things to a
   * callback like objects or data layered on top */
  win->getEventSignal ("delete-request")->connect (sigc::ptr_fun (&my_win_del));

  win->setAutoDel (true);

  /* add a background to our window. this just uses the standard theme set
   * background. without a backgorund, you could make a window seem
   * transparent with elm_win_alpha_set(win, 1); for example. if you have
   * a compositor running this will make the window able to be
   * semi-transparent and any space not filled by object/widget pixels will
   * be transparent or translucent based on alpha. if you do not have a
   * comnpositor running this should fall back to using shaped windows
   * (which have a mask). both these features will be slow and rely on
   * a lot more resources, so only use it if you need it. */
  Background *bg = Background::factory (*win);
  /* set weight to 1.0 x 1.0 == expand in both x and y direction */
  bg->setWeightHintSize (EVAS_HINT_EXPAND, EVAS_HINT_EXPAND);
  /* tell the window that this object is to be resized along with the window.
   * also as a result this object will be one of several objects that
   * controls the minimum/maximum size of the window */
  win->addObjectResize (*bg);
  /* and show the background */
  bg->show ();

  /* add a box layout widget to the window */
  Box *bx0 = Box::factory (*win);
  /* allow base box (bx0) to expand in x and y */
  bx0->setWeightHintSize (EVAS_HINT_EXPAND, EVAS_HINT_EXPAND);
  /* tell the window that the box affects window size and also will be
   * resized when the window is */
  win->addObjectResize (*bx0);
  bx0->show ();

  Frame *fr = Frame::factory (*win);
  fr->setLabel ("Information");
  bx0->packEnd (*fr);
  fr->show ();

  Label *lb = Label::factory (*win);
  lb->setLabel("Please select a test from the list below<br>"
              "by clicking the test button to show the<br>"
              "test window.");
  fr->setContent (*lb);
  lb->show ();

  List *li = List::factory (*win);
  li->setAlwaysSelectMode (true);
  li->setWeightHintSize (EVAS_HINT_EXPAND, EVAS_HINT_EXPAND);
  li->setAlignHintSize (EVAS_HINT_FILL, EVAS_HINT_FILL);
  bx0->packEnd (*li);
  li->show ();

  li->append ("Bg Plain", test_bg_plain, NULL);
  li->append ("Bg Image", test_bg_image, NULL);
  li->append ("Icon Transparent", test_icon, NULL);
  li->append ("Box Vert", test_box_vert, NULL);
  li->append ("Box Horiz", test_box_horiz, NULL);
  li->append ("Buttons", test_button, NULL);
  li->append ("Toggles", test_toggle, NULL);
  li->append ("Table", test_table, NULL);
  li->append ("Clock", test_clock, NULL);
  li->append ("Layout", test_layout, NULL);
  li->append ("Hover", test_hover, NULL);
  li->append ("Hover 2", test_hover2, NULL);
  li->append ("Entry", test_entry, NULL);
  li->append ("Entry Scrolled", test_entry_scolled, NULL);
  /*li.append("Anchorview", NULL, NULL, my_bt_16, NULL);
  li.append("Anchorblock", NULL, NULL, my_bt_17, NULL);
  li.append("Toolbar", NULL, NULL, my_bt_18, NULL);
  li.append("Hoversel", NULL, NULL, my_bt_19, NULL);
  li.append("List", NULL, NULL, my_bt_20, NULL);
  li.append("List 2", NULL, NULL, my_bt_21, NULL);
  li.append("List 3", NULL, NULL, my_bt_22, NULL);
  li.append("Carousel", NULL, NULL, my_bt_23, NULL);
  li.append("Inwin", NULL, NULL, my_bt_24, NULL);
  li.append("Inwin 2", NULL, NULL, my_bt_25, NULL);
  li.append("Scaling", NULL, NULL, my_bt_26, NULL);
  li.append("Scaling 2", NULL, NULL, my_bt_27, NULL);*/
  li->append ("Slider", test_slider, NULL);
  li->append("Genlist", test_genlist, NULL);
  li->append("Genlist 2", test_genlist2, NULL);
  li->append("Genlist 3",test_genlist3, NULL);
  li->append("Genlist 4", test_genlist4, NULL);
  li->append("Genlist 5",test_genlist5, NULL);
  li->append("Genlist Tree", test_genlist6, NULL);
  li->append ("Checks", test_check, NULL);
  li->append ("Radios", test_radio, NULL);
  /*li.append("Pager", NULL, NULL, my_bt_35, NULL);
  li.append("Genlist Tree", NULL, NULL, my_bt_36, NULL);
  li.append("Genlist 5", NULL, NULL, my_bt_37, NULL);
  li.append("Window States", NULL, NULL, my_bt_38, NULL);
  */
  li->append ("Progressbar", test_progressbar, NULL);
  /*
  elm_list_item_append(li, "File Selector", NULL, NULL, test_fileselector, NULL);
  elm_list_item_append(li, "Separator", NULL, NULL, test_separator, NULL);
  elm_list_item_append(li, "Scroller", NULL, NULL, test_scroller, NULL);
  */
  li->append ("Spinner", test_spinner, NULL);
  /*
  elm_list_item_append(li, "Index", NULL, NULL, test_index, NULL);
  elm_list_item_append(li, "Photocam", NULL, NULL, test_photocam, NULL);
  elm_list_item_append(li, "Photo", NULL, NULL, test_photo, NULL);
  elm_list_item_append(li, "Icon Desktops", NULL, NULL, test_icon_desktops, NULL);
  elm_list_item_append(li, "Notify", NULL, NULL, test_notify, NULL);
  elm_list_item_append(li, "Slideshow", NULL, NULL, test_slideshow, NULL);
  elm_list_item_append(li, "Menu", NULL, NULL, test_menu, NULL);
  */
  li->append ("Panel", test_panel, NULL);

  li->go ();

  /* set an initial window size */
  win->resize (Size (240, 480));
  /* show the window */
  win->show ();

  elmApp->run ();
}
