#include "eke_macros.h"
#include "eke_feed.h"
#include "eke_gui.h"
#include "eke_gui_edje.h"
#include "eke_gui_ewl.h"

static int init_count = 0;

int
eke_gui_init(Eke *eke, int *argc, char ** argv)
{
    init_count ++;
    if (eke->gui.type == EKE_GUI_TYPE_EDJE) {
        if (!eke_gui_edje_init()) return 0;

    } else if (eke->gui.type == EKE_GUI_TYPE_EWL) {
        if (!eke_gui_ewl_init(argc, argv)) return 0;
    }

    eke->feeds = ecore_hash_new(ecore_direct_hash, ecore_direct_compare);

    return init_count;
}

int
eke_gui_shutdown(Eke *eke)
{
    if (init_count > 0) {
        init_count --;
        if (init_count > 0) return init_count;

        if (eke->gui.type == EKE_GUI_TYPE_EDJE) 
            eke_gui_edje_shutdown();

        else if (eke->gui.type == EKE_GUI_TYPE_EWL)
            eke_gui_ewl_shutdown();

        ecore_hash_destroy(eke->feeds);
    }
    return 0;
}

void
eke_gui_create(Eke *eke)
{
    if (eke->gui.type == EKE_GUI_TYPE_EDJE)
        eke_gui_edje_create(eke);

    else if (eke->gui.type == EKE_GUI_TYPE_EWL)
        eke_gui_ewl_create(eke);
}

void
eke_gui_feed_register(Eke *eke, Eke_Feed *feed)
{
    if (eke->gui.type == EKE_GUI_TYPE_EDJE) 
        eke_gui_edje_feed_register(eke, feed);

    else if (eke->gui.type == EKE_GUI_TYPE_EWL)
        eke_gui_ewl_feed_register(eke, feed);
}

int
eke_gui_value_changed_cb(void *data, int type, void *ev)
{
    Eke_Feed_Event *feed_ev;
    Eke_Feed *feed;
    Eke *eke;

    feed_ev = ev;
    eke = data;

    feed = feed_ev->feed;

    if (feed->data.type == EKE_FEED_DATA_RESPONSE_ERROR) {
        if (eke->gui.type == EKE_GUI_TYPE_EDJE) 
            eke_gui_edje_feed_error(eke, feed);

        else if (eke->gui.type == EKE_GUI_TYPE_EWL)
            eke_gui_ewl_feed_error(eke, feed);

    } else {
        if (eke->gui.type == EKE_GUI_TYPE_EDJE) 
            eke_gui_edje_feed_change(eke, feed);

        else if (eke->gui.type == EKE_GUI_TYPE_EWL)
            eke_gui_ewl_feed_change(eke, feed);
    }

    return 1;
    type = 0;
}

void
eke_gui_begin(Eke *eke)
{
    if (eke->gui.type == EKE_GUI_TYPE_EDJE)
        eke_gui_edje_begin();

    else if (eke->gui.type == EKE_GUI_TYPE_EWL)
        eke_gui_ewl_begin();
}


