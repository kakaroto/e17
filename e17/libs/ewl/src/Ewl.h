#include <Ebits.h>
#include <Ecore.h>
#include <Edb.h>
#include <Evas.h>
#include <Etox.h>
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

#include <ewl_debug.h>
#include <ewl_macros.h>

#include <ewl_enums.h>

#include <ewl_object.h>
#include <ewl_widget.h>
#include <ewl_container.h>

#include <ewl_callback.h>
#include <ewl_events.h>

#include <ewl_misc.h>

#include <ewl_box.h>
#include <ewl_table.h>

#include <ewl_config.h>
#include <ewl_theme.h>

#include <ewl_button.h>
#include <ewl_window.h>
#include <ewl_text.h>
#include <ewl_entry.h>

/* Playing around */
#include <ewl_fx.h>

#include <ewl_seeker.h>
#include <ewl_spinner.h>
#include <ewl_list.h>
#include <ewl_image.h>
#include <ewl_checkbutton.h>
#include <ewl_radiobutton.h>
#include <ewl_separator.h>
