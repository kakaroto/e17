#ifndef EKE
#define EKE

#include <stdio.h>
#include <stdlib.h>

#include <Ecore.h>
#include <Evas.h>
#include <Ewl.h>

typedef enum Eke_Gui_Type Eke_Gui_Type;
enum Eke_Gui_Type {
    EKE_GUI_TYPE_EWL = 0,
    EKE_GUI_TYPE_EDJE
};

typedef struct Eke Eke;
struct Eke {
    struct {
        Eke_Gui_Type type;

        struct {
            Ewl_Widget *win;
            Ewl_Widget *menubar;
            Ewl_Widget *notebook;
            Ewl_Widget *statusbar;
        } ewl;

        struct {
            char *theme;
            Evas_Object *edje;
        } edje;
    } gui;

    Ecore_Hash *feeds;
};

#endif

