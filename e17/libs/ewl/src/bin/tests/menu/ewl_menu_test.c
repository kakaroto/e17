/* vim: set sw=8 ts=8 sts=8 expandtab: */
#include "Ewl_Test.h"
#include "ewl_test_private.h"
#include "ewl_label.h"
#include "ewl_menu.h"

#include <stdio.h>

static int create_test(Ewl_Container *box);
static void cb_menu_clicked(Ewl_Widget * w, void *ev, void *data);

void
test_info(Ewl_Test *test)
{
        test->name = "Menu";
        test->tip = "Defines the basic menu classes that\n"
                        "are extended to an actual menu\n"
                        "implementation by inheriting classes\n"
                        "such as Ewl_Menu and Ewl_IMenu.";
        test->filename = __FILE__;
        test->func = create_test;
        test->type = EWL_TEST_TYPE_CONTAINER;
}

static int
create_test(Ewl_Container *box)
{
        Ewl_Widget *menu1, *menu2, *menu3, *item;

        item = ewl_label_new();
        ewl_label_text_set(EWL_LABEL(item), "");
        ewl_widget_name_set(item, "menu_label");
        ewl_container_child_append(EWL_CONTAINER(box), item);
        ewl_widget_show(item);

        menu1 = ewl_menu_new();
        ewl_button_image_set(EWL_BUTTON(menu1),
                                PACKAGE_DATA_DIR "/ewl/images/Draw.png", NULL);
        ewl_button_label_set(EWL_BUTTON(menu1), "Test Menu");
        ewl_container_child_append(EWL_CONTAINER(box), menu1);
        ewl_object_fill_policy_set(EWL_OBJECT(menu1), EWL_FLAG_FILL_NONE);
        ewl_widget_show(menu1);

        item = ewl_menu_item_new();
        ewl_button_image_set(EWL_BUTTON(item),
                                PACKAGE_DATA_DIR "/ewl/images/Open.png", NULL);
        ewl_button_label_set(EWL_BUTTON(item), "Dia");
        ewl_container_child_append(EWL_CONTAINER(menu1), item);
        ewl_callback_append(item, EWL_CALLBACK_CLICKED, cb_menu_clicked, NULL);
        ewl_widget_show(item);

        item = ewl_menu_item_new();
        ewl_button_image_set(EWL_BUTTON(item),
                                PACKAGE_DATA_DIR "/ewl/images/Package.png", NULL);
        ewl_button_label_set(EWL_BUTTON(item), "Gimp");
        ewl_container_child_append(EWL_CONTAINER(menu1), item);
        ewl_callback_append(item, EWL_CALLBACK_CLICKED, cb_menu_clicked, NULL);
        ewl_widget_show(item);

        item = ewl_menu_item_new();
        ewl_stock_type_set(EWL_STOCK(item), EWL_STOCK_OK);
        ewl_container_child_append(EWL_CONTAINER(menu1), item);
        ewl_callback_append(item, EWL_CALLBACK_CLICKED, cb_menu_clicked, NULL);
        ewl_widget_show(item);

        item = ewl_menu_item_new();
        ewl_button_label_set(EWL_BUTTON(item), "Button");
        ewl_container_child_append(EWL_CONTAINER(menu1), item);
        ewl_callback_append(item, EWL_CALLBACK_CLICKED, cb_menu_clicked, NULL);
        ewl_widget_show(item);

        /* Create a sub-menu */
        menu2 = ewl_menu_new();
        ewl_button_label_set(EWL_BUTTON(menu2), "Sub Menu");
        ewl_container_child_append(EWL_CONTAINER(menu1), menu2);
        ewl_widget_show(menu2);

        item = ewl_menu_item_new();
        ewl_button_label_set(EWL_BUTTON(item), "Button 1");
        ewl_container_child_append(EWL_CONTAINER(menu2), item);
        ewl_callback_append(item, EWL_CALLBACK_CLICKED, cb_menu_clicked, NULL);
        ewl_widget_show(item);

        item = ewl_menu_item_new();
        ewl_button_label_set(EWL_BUTTON(item), "Button 2");
        ewl_container_child_append(EWL_CONTAINER(menu2), item);
        ewl_callback_append(item, EWL_CALLBACK_CLICKED, cb_menu_clicked, NULL);
        ewl_widget_show(item);

        /* create a sub-sub-menu */
        menu3 = ewl_menu_new();
        ewl_button_label_set(EWL_BUTTON(menu3), "Sub Sub Menu");
        ewl_container_child_append(EWL_CONTAINER(menu2), menu3);
        ewl_widget_show(menu3);

        item = ewl_menu_item_new();
        ewl_button_label_set(EWL_BUTTON(item), "Sub Button 1");
        ewl_container_child_append(EWL_CONTAINER(menu3), item);
        ewl_callback_append(item, EWL_CALLBACK_CLICKED, cb_menu_clicked, NULL);
        ewl_widget_show(item);

        item = ewl_menu_item_new();
        ewl_button_label_set(EWL_BUTTON(item), "Sub Button 2");
        ewl_container_child_append(EWL_CONTAINER(menu3), item);
        ewl_callback_append(item, EWL_CALLBACK_CLICKED, cb_menu_clicked, NULL);
        ewl_widget_show(item);

        /* Create a sub-menu */
        menu2 = ewl_menu_new();
        ewl_button_label_set(EWL_BUTTON(menu2), "Sub Menu2");
        ewl_container_child_append(EWL_CONTAINER(menu1), menu2);
        ewl_widget_show(menu2);

        item = ewl_menu_item_new();
        ewl_button_label_set(EWL_BUTTON(item), "Button 2-1");
        ewl_container_child_append(EWL_CONTAINER(menu2), item);
        ewl_callback_append(item, EWL_CALLBACK_CLICKED, cb_menu_clicked, NULL);
        ewl_widget_show(item);

        item = ewl_menu_item_new();
        ewl_button_label_set(EWL_BUTTON(item), "Button 2-2");
        ewl_container_child_append(EWL_CONTAINER(menu2), item);
        ewl_callback_append(item, EWL_CALLBACK_CLICKED, cb_menu_clicked, NULL);
        ewl_widget_show(item);

        return 1;
}

static void
cb_menu_clicked(Ewl_Widget * w, void *ev __UNUSED__, void *data __UNUSED__)
{
        Ewl_Widget *o;
        const char *text;
        char buf[1024];

        o = ewl_widget_name_find("menu_label");
        text = ewl_button_label_get(EWL_BUTTON(w));

        snprintf(buf, 1024, "%s item clicked.", (char *)text);
        ewl_label_text_set(EWL_LABEL(o), buf);
}

