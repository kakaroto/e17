#ifndef EKE
#define EKE

#include <stdio.h>
#include <stdlib.h>

#include <Ecore.h>

#if BUILD_EDJE_GUI
#include <Ecore_Evas.h>
#include <Evas.h>
#endif

#if BUILD_EWL_GUI
#include <Ewl.h>
#endif

#include "eke_feed.h"

typedef enum Eke_Gui_Type Eke_Gui_Type;
enum Eke_Gui_Type {
#if BUILD_EWL_GUI
    EKE_GUI_TYPE_EWL = 0,
#endif

#if BUILD_EDJE_GUI
    EKE_GUI_TYPE_EDJE,
#endif

    EKE_GUI_TYPE_NULL
};

typedef struct Eke Eke;
struct Eke {
    struct {
        Eke_Gui_Type type;

#if BUILD_EWL_GUI
        struct {
            Ewl_Widget *win;
            Ewl_Widget *menubar;
            Ewl_Widget *notebook;
            Ewl_Widget *statusbar;
        } ewl;
#endif

#if BUILD_EDJE_GUI
        struct {
            char *theme;
            Ecore_Evas *ee;
            Evas_Object *edje;
        } edje;
#endif
    } gui;

    Eke_Feed *current_feed;
    Ecore_Hash *feeds;
};

#endif

