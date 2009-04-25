/* vim: set sw=8 ts=8 sts=8 expandtab: */
#include "ewl_base.h"
#include "ewl_stock.h"
#include "ewl_icon_theme.h"
#include "ewl_macros.h"
#include "ewl_private.h"
#include "ewl_debug.h"

/*
 * this array needs to have it's items in the same order as they
 * appear in the Ewl_Stock_Type enum
 */
struct
{
        const char * const label;
        const char * const image_key;
} ewl_stock_items[] = {
        {"About",               EWL_ICON_HELP_ABOUT},
        {"Add",                 EWL_ICON_LIST_ADD},
        {"Apply",               EWL_ICON_DIALOG_APPLY},
        {/*Arrow*/"Bottom",     EWL_ICON_GO_BOTTOM},
        {/*Arrow*/"Down",       EWL_ICON_GO_DOWN},
        {/*Arrow*/"First",      EWL_ICON_GO_FIRST},
        {/*Arrow*/"Last",       EWL_ICON_GO_LAST},
        {/*Arrow*/"Left",       EWL_ICON_GO_PREVIOUS},
        {/*Arrow*/"Right",      EWL_ICON_GO_NEXT},
        {/*Arrow*/"Top",        EWL_ICON_GO_TOP},
        {/*Arrow*/"Up",         EWL_ICON_GO_UP},
        {"Bold",                EWL_ICON_FORMAT_TEXT_BOLD},
        {"Cancel",              EWL_ICON_DIALOG_CANCEL},
        {"Clear",               EWL_ICON_EDIT_DELETE},
        {"Close",               EWL_ICON_DIALOG_CLOSE},
        {"Copy",                EWL_ICON_EDIT_COPY},
        {"Cut",                 EWL_ICON_EDIT_CUT},
        {"Delete",              EWL_ICON_USER_TRASH},
        {"Edit",                EWL_ICON_ACCESSORIES_TEXT_EDITOR},
        {"Execute",             EWL_ICON_SYSTEM_RUN},
        {"Find",                EWL_ICON_EDIT_FIND},
        {"Find and replace",    EWL_ICON_EDIT_FIND_REPLACE},
        {"Flip horizontal",     EWL_ICON_OBJECT_FLIP_HORIZONTAL},
        {"Flip vertical",       EWL_ICON_OBJECT_FLIP_VERTICAL},
        {"Fullscreen",          EWL_ICON_VIEW_FULLSCREEN},
        {"Help",                EWL_ICON_HELP_CONTENTS},
        {"Home",                EWL_ICON_GO_HOME},
        {"Indent",              EWL_ICON_FORMAT_INDENT_MORE},
        {"Italic",              EWL_ICON_FORMAT_TEXT_ITALIC},
        {"Justify center",      EWL_ICON_FORMAT_JUSTIFY_CENTER},
        {"Justify fill",        EWL_ICON_FORMAT_JUSTIFY_FILL},
        {"Justify left",        EWL_ICON_FORMAT_JUSTIFY_LEFT},
        {"Justify right",       EWL_ICON_FORMAT_JUSTIFY_RIGHT},
        {"FF",                  EWL_ICON_MEDIA_SEEK_FORWARD},
        {"Next",                EWL_ICON_MEDIA_SKIP_FORWARD},
        {"Pause",               EWL_ICON_MEDIA_PLAYBACK_PAUSE},
        {"Play",                EWL_ICON_MEDIA_PLAYBACK_START},
        {"Previous",            EWL_ICON_MEDIA_SKIP_BACKWARD},
        {"Record",              EWL_ICON_MEDIA_RECORD},
        {"Rewind",              EWL_ICON_MEDIA_SEEK_BACKWARD},
        {"Stop",           	EWL_ICON_MEDIA_PLAYBACK_STOP},
        {"New",            	EWL_ICON_DOCUMENT_NEW},
        {"Ok",             	EWL_ICON_DIALOG_OK},
        {"Open",           	EWL_ICON_DOCUMENT_OPEN},
        {"Paste",          	EWL_ICON_EDIT_PASTE},
        {"Preferences",    	EWL_ICON_PREFERENCES_SYSTEM},
        {"Print",          	EWL_ICON_DOCUMENT_PRINT},
        {"Print preview",  	EWL_ICON_DOCUMENT_PRINT_PREVIEW},
        {"Properties",     	EWL_ICON_DOCUMENT_PROPERTIES},
        {"Quit",           	EWL_ICON_SYSTEM_LOG_OUT},
        {"Redo",           	EWL_ICON_EDIT_REDO},
        {"Refresh",        	EWL_ICON_VIEW_REFRESH},
        {"Remove",         	EWL_ICON_LIST_REMOVE},
        {"Rotate left",    	EWL_ICON_OBJECT_ROTATE_LEFT},
        {"Rotate right",   	EWL_ICON_OBJECT_ROTATE_RIGHT},
        {"Save",           	EWL_ICON_DOCUMENT_SAVE},
        {"Save as",        	EWL_ICON_DOCUMENT_SAVE_AS},
        {"Select all",     	EWL_ICON_EDIT_SELECT_ALL},
        {"Sort ascending", 	EWL_ICON_VIEW_SORT_ASCENDING},
        {"Sort descending",	EWL_ICON_VIEW_SORT_DESCENDING},
        {"Spell check",    	EWL_ICON_TOOLS_CHECK_SPELLING},
        {"Strikethrough",  	EWL_ICON_FORMAT_TEXT_STRIKETHROUGH},
        {"Underline",      	EWL_ICON_FORMAT_TEXT_UNDERLINE},
        {"Undo",           	EWL_ICON_EDIT_UNDO},
        {"Unindent",       	EWL_ICON_FORMAT_INDENT_LESS},
        {"Zoom 1:1",       	EWL_ICON_ZOOM_ORIGINAL},
        {"Zoom fit",       	EWL_ICON_ZOOM_BEST_FIT},
        {"Zoom in",        	EWL_ICON_ZOOM_IN},
        {"Zoom out",       	EWL_ICON_ZOOM_OUT}
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
                         ewl_stock_items[s->stock_type].label);

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


