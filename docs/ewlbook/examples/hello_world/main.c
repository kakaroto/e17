#include <stdio.h>
#include <Ewl.h>

void destroy_cb(Ewl_Widget *w, void *event, void *data) {
    ewl_widget_destroy(w);
    ewl_main_quit();
}

void text_update_cb(Ewl_Widget *w, void *event, void *data) {
    char *s = NULL;
    Ewl_Widget *label = NULL;
    char buf[BUFSIZ];

    s = ewl_text_text_get(EWL_TEXT(w));
    label = (Ewl_Widget *)data;

    snprintf(buf, BUFSIZ, "Hello %s", s);
    ewl_text_text_set(EWL_TEXT(label), buf);

    free(s);
    return;
}

int main(int argc, char ** argv) {
    Ewl_Widget *win = NULL;
    Ewl_Widget *box = NULL;
    Ewl_Widget *label = NULL;
    Ewl_Widget *o = NULL;

    /* init the library */
    if (!ewl_init(&argc, argv)) {
        printf("Unable to initialize EWL\n");
        return 1;
    }

    /* create the window */
    win = ewl_window_new();
    ewl_window_title_set(EWL_WINDOW(win), "Hello world");
    ewl_window_class_set(EWL_WINDOW(win), "hello");
    ewl_window_name_set(EWL_WINDOW(win), "hello");
    ewl_object_size_request(EWL_OBJECT(win), 200, 50);
    ewl_callback_append(win, EWL_CALLBACK_DELETE_WINDOW, destroy_cb, NULL);
    ewl_widget_show(win);
 
    /* create the container */
    box = ewl_vbox_new();
    ewl_container_child_append(EWL_CONTAINER(win), box);
    ewl_object_fill_policy_set(EWL_OBJECT(box), EWL_FLAG_FILL_ALL);
    ewl_widget_show(box);
 
    /* create text label */
    label = ewl_text_new();
    ewl_container_child_append(EWL_CONTAINER(box), label);
    ewl_object_alignment_set(EWL_OBJECT(label), EWL_FLAG_ALIGN_CENTER);
    ewl_text_styles_set(EWL_TEXT(label), EWL_TEXT_STYLE_SOFT_SHADOW);
    ewl_text_color_set(EWL_TEXT(label), 255, 0, 0, 255);
    ewl_text_text_set(EWL_TEXT(label), "Hello");
    ewl_widget_show(label);

    /* create the entry */ 
    o = ewl_entry_new();
    ewl_container_child_append(EWL_CONTAINER(box), o);
    ewl_object_alignment_set(EWL_OBJECT(o), EWL_FLAG_ALIGN_CENTER);
    ewl_object_padding_set(EWL_OBJECT(o), 5, 5, 5, 0);
    ewl_text_color_set(EWL_TEXT(o), 0, 0, 0, 255);
    ewl_callback_append(o, EWL_CALLBACK_VALUE_CHANGED, text_update_cb, label);
    ewl_widget_show(o);

    ewl_main();
    return 0;
}


