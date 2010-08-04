#include "ewl_base.h"
#include "ewl_stock.h"
#include "ewl_icon_theme.h"
#include "ewl_macros.h"
#include "ewl_private.h"
#include "ewl_debug.h"
#include "ewl_intl.h"

/*
 * this array needs to have it's items in the same order as they
 * appear in the Ewl_Stock_Type enum
 */
struct
{
        const char * const label;
        const char * const image_key;
} ewl_stock_items[] = {
        {N_("STOCK|About"),             EWL_ICON_HELP_ABOUT},
        {N_("STOCK|Add"),               EWL_ICON_LIST_ADD},
        {N_("STOCK|Apply"),             EWL_ICON_DIALOG_APPLY},
        {/*Arrow*/N_("STOCK|Bottom"),   EWL_ICON_GO_BOTTOM},
        {/*Arrow*/N_("STOCK|Down"),     EWL_ICON_GO_DOWN},
        {/*Arrow*/N_("STOCK|First"),    EWL_ICON_GO_FIRST},
        {/*Arrow*/N_("STOCK|Last"),     EWL_ICON_GO_LAST},
        {/*Arrow*/N_("STOCK|Left"),     EWL_ICON_GO_PREVIOUS},
        {/*Arrow*/N_("STOCK|Right"),    EWL_ICON_GO_NEXT},
        {/*Arrow*/N_("STOCK|Top"),      EWL_ICON_GO_TOP},
        {/*Arrow*/N_("STOCK|Up"),       EWL_ICON_GO_UP},
        {N_("STOCK|Bold"),              EWL_ICON_FORMAT_TEXT_BOLD},
        {N_("STOCK|Cancel"),            EWL_ICON_DIALOG_CANCEL},
        {N_("STOCK|Clear"),             EWL_ICON_EDIT_DELETE},
        {N_("STOCK|Close"),             EWL_ICON_DIALOG_CLOSE},
        {N_("STOCK|Copy"),              EWL_ICON_EDIT_COPY},
        {N_("STOCK|Cut"),               EWL_ICON_EDIT_CUT},
        {N_("STOCK|Delete"),            EWL_ICON_USER_TRASH},
        {N_("STOCK|Edit"),              EWL_ICON_ACCESSORIES_TEXT_EDITOR},
        {N_("STOCK|Execute"),           EWL_ICON_SYSTEM_RUN},
        {N_("STOCK|Find"),              EWL_ICON_EDIT_FIND},
        {N_("STOCK|Find and replace"),  EWL_ICON_EDIT_FIND_REPLACE},
        {N_("STOCK|Flip horizontal"),   EWL_ICON_OBJECT_FLIP_HORIZONTAL},
        {N_("STOCK|Flip vertical"),     EWL_ICON_OBJECT_FLIP_VERTICAL},
        {N_("STOCK|Fullscreen"),        EWL_ICON_VIEW_FULLSCREEN},
        {N_("STOCK|Help"),              EWL_ICON_HELP_CONTENTS},
        {N_("STOCK|Home"),              EWL_ICON_GO_HOME},
        {N_("STOCK|Indent"),            EWL_ICON_FORMAT_INDENT_MORE},
        {N_("STOCK|Italic"),            EWL_ICON_FORMAT_TEXT_ITALIC},
        {N_("STOCK|Justify center"),    EWL_ICON_FORMAT_JUSTIFY_CENTER},
        {N_("STOCK|Justify fill"),      EWL_ICON_FORMAT_JUSTIFY_FILL},
        {N_("STOCK|Justify left"),      EWL_ICON_FORMAT_JUSTIFY_LEFT},
        {N_("STOCK|Justify right"),     EWL_ICON_FORMAT_JUSTIFY_RIGHT},
        {N_("STOCK|FF"),                EWL_ICON_MEDIA_SEEK_FORWARD},
        {N_("STOCK|Next"),              EWL_ICON_MEDIA_SKIP_FORWARD},
        {N_("STOCK|Pause"),             EWL_ICON_MEDIA_PLAYBACK_PAUSE},
        {N_("STOCK|Play"),              EWL_ICON_MEDIA_PLAYBACK_START},
        {N_("STOCK|Previous"),          EWL_ICON_MEDIA_SKIP_BACKWARD},
        {N_("STOCK|Record"),            EWL_ICON_MEDIA_RECORD},
        {N_("STOCK|Rewind"),            EWL_ICON_MEDIA_SEEK_BACKWARD},
        {N_("STOCK|Stop"),              EWL_ICON_MEDIA_PLAYBACK_STOP},
        {N_("STOCK|New"),               EWL_ICON_DOCUMENT_NEW},
        {N_("STOCK|Ok"),                EWL_ICON_DIALOG_OK},
        {N_("STOCK|Open"),              EWL_ICON_DOCUMENT_OPEN},
        {N_("STOCK|Paste"),             EWL_ICON_EDIT_PASTE},
        {N_("STOCK|Preferences"),       EWL_ICON_PREFERENCES_SYSTEM},
        {N_("STOCK|Print"),             EWL_ICON_DOCUMENT_PRINT},
        {N_("STOCK|Print preview"),     EWL_ICON_DOCUMENT_PRINT_PREVIEW},
        {N_("STOCK|Properties"),        EWL_ICON_DOCUMENT_PROPERTIES},
        {N_("STOCK|Quit"),              EWL_ICON_SYSTEM_LOG_OUT},
        {N_("STOCK|Redo"),              EWL_ICON_EDIT_REDO},
        {N_("STOCK|Refresh"),           EWL_ICON_VIEW_REFRESH},
        {N_("STOCK|Remove"),            EWL_ICON_LIST_REMOVE},
        {N_("STOCK|Rotate left"),       EWL_ICON_OBJECT_ROTATE_LEFT},
        {N_("STOCK|Rotate right"),      EWL_ICON_OBJECT_ROTATE_RIGHT},
        {N_("STOCK|Save"),              EWL_ICON_DOCUMENT_SAVE},
        {N_("STOCK|Save as"),           EWL_ICON_DOCUMENT_SAVE_AS},
        {N_("STOCK|Select all"),        EWL_ICON_EDIT_SELECT_ALL},
        {N_("STOCK|Sort ascending"),    EWL_ICON_VIEW_SORT_ASCENDING},
        {N_("STOCK|Sort descending"),   EWL_ICON_VIEW_SORT_DESCENDING},
        {N_("STOCK|Spell check"),       EWL_ICON_TOOLS_CHECK_SPELLING},
        {N_("STOCK|Strikethrough"),     EWL_ICON_FORMAT_TEXT_STRIKETHROUGH},
        {N_("STOCK|Underline"),         EWL_ICON_FORMAT_TEXT_UNDERLINE},
        {N_("STOCK|Undo"),              EWL_ICON_EDIT_UNDO},
        {N_("STOCK|Unindent"),          EWL_ICON_FORMAT_INDENT_LESS},
        {N_("STOCK|Zoom 1:1"),          EWL_ICON_ZOOM_ORIGINAL},
        {N_("STOCK|Zoom fit"),          EWL_ICON_ZOOM_BEST_FIT},
        {N_("STOCK|Zoom in"),           EWL_ICON_ZOOM_IN},
        {N_("STOCK|Zoom out"),          EWL_ICON_ZOOM_OUT}
};

/**
 * @param s: the stock widget to initialize
 * @return Returns TRUE on success, FALSE on failure.
 * @brief Initialize the stock widget and inherited fields
 *
 * Clears the contents of the stock widget and stores the
 * default values.
 */
int
ewl_stock_init(Ewl_Stock *s)
{
        DENTER_FUNCTION(DLEVEL_STABLE);
        DCHECK_PARAM_PTR_RET(s, FALSE);

        if (!ewl_box_init(EWL_BOX(s)))
                DRETURN_INT(FALSE, DLEVEL_STABLE);

        ewl_widget_inherit(EWL_WIDGET(s), EWL_STOCK_TYPE);

        DRETURN_INT(TRUE, DLEVEL_STABLE);
}

/**
 * @internal
 * @param s: The stock to work with
 * @param funcs: The stock functions to set.
 * @return Returns no value
 * @brief Sets the given stock functions onto the stock widget
 */
void
ewl_stock_functions_set(Ewl_Stock *s, const Ewl_Stock_Funcs * const funcs)
{
        DENTER_FUNCTION(DLEVEL_STABLE);
        DCHECK_PARAM_PTR(s);
        DCHECK_PARAM_PTR(funcs);
        DCHECK_TYPE(s, EWL_STOCK_TYPE);

        s->stock_funcs = funcs;

        DLEAVE_FUNCTION(DLEVEL_STABLE);
}

/**
 * @param s: The stock to set the stock type on
 * @param stock: The Ewl_Stock_Type to set on the stock widget
 * @return Returns no value.
 * @brief Set the type of the stock to use
 */
void
ewl_stock_type_set(Ewl_Stock *s, Ewl_Stock_Type stock)
{
        DENTER_FUNCTION(DLEVEL_STABLE);
        DCHECK_PARAM_PTR(s);
        DCHECK_TYPE(s, EWL_STOCK_TYPE);

        if (stock == s->stock_type)
                DRETURN(DLEVEL_STABLE);

        s->stock_type = stock;

        /* we're done if it's none */
        if (s->stock_type == EWL_STOCK_NONE)
                DRETURN(DLEVEL_STABLE);

        /* Can't do anything without the stock funcs */
        if (!s->stock_funcs)
                DRETURN(DLEVEL_STABLE);

        /* set the label */
        if (s->stock_funcs->label_set)
                s->stock_funcs->label_set(s,
                         SD_(ewl_stock_items[s->stock_type].label));

        /* set the image */
        if (s->stock_funcs->image_set) {
                const char *data;

                /* check for an image key */
                data = ewl_icon_theme_icon_path_get(
                                ewl_stock_items[s->stock_type].image_key,
                                EWL_ICON_SIZE_MEDIUM);

                s->stock_funcs->image_set(s, data,
                                ewl_stock_items[s->stock_type].image_key);
        }

        DLEAVE_FUNCTION(DLEVEL_STABLE);
}

/**
 * @param s: The stock widget to get the stock type from
 * @return Returns the Ewl_Stock_Type of the stock widget
 * @brief Get the stock type in use
 */
Ewl_Stock_Type
ewl_stock_type_get(Ewl_Stock *s)
{
        DENTER_FUNCTION(DLEVEL_STABLE);
        DCHECK_PARAM_PTR_RET(s, EWL_STOCK_NONE);
        DCHECK_TYPE_RET(s, EWL_STOCK_TYPE, EWL_STOCK_NONE);

        DRETURN_INT(s->stock_type, DLEVEL_STABLE);
}


