#include <Ebits.h>
#include <Ecore.h>
#include <Edb.h>
#include <Evas.h>
#define LIBRARY_NAME "EWL"
#include <Ewd.h>

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>

#ifndef PATH_LEN
#define PATH_LEN 4096
#endif

#ifndef EWL_USER_DIR
#define EWL_USER_DIR ".e/ewl"
#endif

enum _ewl_callback_type {
    Ewl_Callback_Realize,
    Ewl_Callback_Show,
    Ewl_Callback_Hide,
    Ewl_Callback_Destroy,
    Ewl_Callback_Configure,
	Ewl_Callback_Key_Down,
	Ewl_Callback_Key_Up,
	Ewl_Callback_Mouse_Down,
	Ewl_Callback_Mouse_Up,
	Ewl_Callback_Focus_In,
	Ewl_Callback_Focus_Out,
	Ewl_Callback_Select,
	Ewl_Callback_Unselect,
	Ewl_Callback_Clicked,
	Ewl_Callback_Released,
	Ewl_Callback_Hilited,
    Ewl_Callback_Max
};

typedef enum _ewl_callback_type Ewl_Callback_Type;

typedef struct _ewl_container Ewl_Container;

/* Ewl Base Classes */
#include <ewl_object.h>
#include <ewl_widget.h>
#include <ewl_container.h>

#include <ewl_callback.h>
#include <ewl_events.h>

#include <ewl_macros.h>
#include <ewl_misc.h>
#include <ewl_box.h>

#include <ewl_prefs.h>
#include <ewl_theme.h>

#include <ewl_button.h>
#include <ewl_window.h>
#include <ewl_text.h>
#include <ewl_entry.h>
