/* vim: set sw=8 ts=8 sts=8 expandtab: */
#include "ewl_base.h"
#include "ewl_icon.h"
#include "ewl_entry.h"
#include "ewl_image.h"
#include "ewl_icon_theme.h"
#include "ewl_label.h"
#include "ewl_menu.h"
#include "ewl_macros.h"
#include "ewl_private.h"
#include "ewl_debug.h"

static const Ewl_Stock_Funcs const stock_funcs = {
        EWL_STOCK_LABEL_SET(ewl_icon_label_set),
        EWL_STOCK_IMAGE_SET(ewl_icon_image_set),
        NULL
};

/* XXX may want to make this configurable, possibly per icon? */
#define EWL_ICON_COMPRESS_SIZE 10

static void ewl_icon_cb_label_mouse_down(Ewl_Widget *w, void *ev, void *data);
static void ewl_icon_cb_entry_focus_out(Ewl_Widget *w, void *ev, void *data);
static void ewl_icon_cb_entry_value_changed(Ewl_Widget *w, void *ev, void *data);
static void ewl_icon_cb_thumb_value_changed(Ewl_Widget *w, void *ev, void *data);

static void ewl_icon_parts_update(Ewl_Icon *icon);
static void ewl_icon_label_build(Ewl_Icon *icon);
static void ewl_icon_label_text_set(Ewl_Icon *icon, const char *txt);
static void ewl_icon_label_update(Ewl_Icon *icon);

/**
 * @return Returns a new Ewl_Icon widget, or NULL on failure
 * @brief Creates and initializes a new Ewl_Icon widget
 */
Ewl_Widget *
ewl_icon_new(void)
{
        Ewl_Widget *w;

        DENTER_FUNCTION(DLEVEL_STABLE);

        w = NEW(Ewl_Icon, 1);
        if (!w)
                DRETURN_PTR(NULL, DLEVEL_STABLE);

        if (!ewl_icon_init(EWL_ICON(w)))
        {
                ewl_widget_destroy(w);
                w = NULL;
        }

        DRETURN_PTR(w, DLEVEL_STABLE);
}

/**
 * @return Returns a new Ewl_Icon widget, or NULL on failure
 * @brief Creates and initializes a new Ewl_Icon widget
 *
 * The difference to ewl_icon_new() is that it has lighter
 * default values. The defaults are
 * complex_label:    no
 * compressed_label: no
 * editable:         no
 * thumbnailing:     no
 *
 */
Ewl_Widget *
ewl_icon_simple_new(void)
{
        Ewl_Widget *w;

        DENTER_FUNCTION(DLEVEL_STABLE);

        w = ewl_icon_new();
        if (!w)
                DRETURN_PTR(NULL, DLEVEL_STABLE);

        ewl_icon_label_complex_set(EWL_ICON(w), FALSE);
        ewl_icon_label_compressed_set(EWL_ICON(w), FALSE);
        ewl_icon_thumbnailing_set(EWL_ICON(w), FALSE);
        ewl_icon_editable_set(EWL_ICON(w), FALSE);

        DRETURN_PTR(w, DLEVEL_STABLE);
}

/**
 * @param icon: The widget to initialize
 * @return Returns TRUE on successful initialization, FALSE otherwise
 * @brief Initializes the given Ewl_Icon widget
 */
int
ewl_icon_init(Ewl_Icon *icon)
{
        DENTER_FUNCTION(DLEVEL_STABLE);
        DCHECK_PARAM_PTR_RET(icon, FALSE);

        if (!ewl_stock_init(EWL_STOCK((icon))))
                DRETURN_INT(FALSE, DLEVEL_STABLE);

        ewl_stock_functions_set(EWL_STOCK(icon), &stock_funcs);
        ewl_stock_type_set(EWL_STOCK(icon), EWL_STOCK_NONE);

        ewl_object_fill_policy_set(EWL_OBJECT(icon), EWL_FLAG_FILL_NONE);
        ewl_box_orientation_set(EWL_BOX(icon), EWL_ORIENTATION_VERTICAL);
        ewl_box_spacing_set(EWL_BOX(icon), 4);

        ewl_widget_appearance_set(EWL_WIDGET(icon), EWL_ICON_TYPE);
        ewl_widget_inherit(EWL_WIDGET(icon), EWL_ICON_TYPE);

        ewl_callback_prepend(EWL_WIDGET(icon), EWL_CALLBACK_DESTROY,
                                        ewl_icon_cb_destroy, NULL);
        /*
         * set some defaults
         */
        icon->thumbnailing = TRUE;
        icon->complex_label = TRUE;
        icon->constrain = 16;

        DRETURN_INT(TRUE, DLEVEL_STABLE);
}

/**
 * @param icon: The icon to set the type of
 * @param type The type to set on this icon
 * @return Returns no value.
 * @brief Set the type of the icon
 */
void
ewl_icon_type_set(Ewl_Icon *icon, Ewl_Icon_Type type)
{
        DENTER_FUNCTION(DLEVEL_STABLE);
        DCHECK_PARAM_PTR(icon);
        DCHECK_TYPE(icon, EWL_ICON_TYPE);

        if (icon->type == type)
                DRETURN(DLEVEL_STABLE);

        icon->type = type;

        /* if we are no longer extended then clear out the current extended
         * data */
        if (icon->extended)
        {
                if (type == EWL_ICON_TYPE_SHORT)
                        ewl_widget_hide(icon->extended);
                else
                        ewl_widget_show(icon->extended);
        }

        DLEAVE_FUNCTION(DLEVEL_STABLE);
}

/**
 * @param icon: The icon to get the type from
 * @return Returns the Ewl_Icon_Type of the icon
 * @brief Retrieve the type of the icon
 */
Ewl_Icon_Type
ewl_icon_type_get(Ewl_Icon *icon)
{
        DENTER_FUNCTION(DLEVEL_STABLE);
        DCHECK_PARAM_PTR_RET(icon, EWL_ICON_TYPE_SHORT);
        DCHECK_TYPE_RET(icon, EWL_ICON_TYPE, EWL_ICON_TYPE_SHORT);

        DRETURN_INT(icon->type, DLEVEL_STABLE);
}

/**
 * @param icon: The icon to set the part to hide of
 * @param part: The part to hide
 * @return Returns no value.
 * @brief Hide the given part of the icon.
 *
 * The give part of the icon will be hidden.
 *
 * @note You can only hide one part.
 */
void
ewl_icon_part_hide(Ewl_Icon *icon, Ewl_Icon_Part part)
{
        DENTER_FUNCTION(DLEVEL_STABLE);
        DCHECK_PARAM_PTR(icon);
        DCHECK_TYPE(icon, EWL_ICON_TYPE);

        if (icon->hidden == part)
                DRETURN(DLEVEL_STABLE);

        icon->hidden = part;

        ewl_icon_parts_update(icon);

        DLEAVE_FUNCTION(DLEVEL_STABLE);
}

/**
 * @param icon: The Ewl_Icon to set the image into
 * @param file: The file with the image
 * @param key: The key inside the file if applicable
 * @return Returns no value
 * @brief set the image to use in the icon
 */
void
ewl_icon_image_set(Ewl_Icon *icon, const char *file, const char *key)
{
        Ewl_Widget *img;

        DENTER_FUNCTION(DLEVEL_STABLE);
        DCHECK_PARAM_PTR(icon);
        DCHECK_PARAM_PTR(file);
        DCHECK_TYPE(icon, EWL_ICON_TYPE);

        if (icon->image)
                ewl_widget_destroy(icon->image);

        img = ewl_image_new();
        ewl_image_file_set(EWL_IMAGE(img), file, key);

        if (icon->thumbnailing)
        {
                icon->image = ewl_image_thumbnail_get(EWL_IMAGE(img));
                ewl_callback_append(icon->image, EWL_CALLBACK_VALUE_CHANGED,
                                        ewl_icon_cb_thumb_value_changed, icon);
        }
        else
                icon->image = img;

        ewl_icon_constrain_set(icon, icon->constrain);
        ewl_image_proportional_set(EWL_IMAGE(icon->image), TRUE);
        ewl_object_alignment_set(EWL_OBJECT(icon->image),
                                                EWL_FLAG_ALIGN_CENTER);
        ewl_widget_internal_set(icon->image, TRUE);
        ewl_container_child_prepend(EWL_CONTAINER(icon), icon->image);

        if (icon->hidden == EWL_ICON_PART_IMAGE)
                DRETURN(DLEVEL_STABLE);

        if (!icon->thumbnailing)
                ewl_icon_parts_update(icon);
        else if (!icon->alt)
        {
                const char *path;

                path = ewl_icon_theme_icon_path_get(
                                        EWL_ICON_IMAGE_LOADING, 0),
                ewl_image_file_set(EWL_IMAGE(icon->image), path,
                                        EWL_ICON_IMAGE_LOADING);
                ewl_widget_show(icon->image);
        }
        else
                ewl_widget_show(icon->alt);

        DLEAVE_FUNCTION(DLEVEL_STABLE);
}

/**
 * @param icon: The Ewl_Icon to get the image file from
 * @return Returns the image file associated with this icon, or NULL if
 * none.
 * @brief Retrieve the image to used in the icon
 */
const char *
ewl_icon_image_file_get(Ewl_Icon *icon)
{
        const char *file = NULL;

        DENTER_FUNCTION(DLEVEL_STABLE);
        DCHECK_PARAM_PTR_RET(icon, NULL);
        DCHECK_TYPE_RET(icon, EWL_ICON_TYPE, NULL);

        if (icon->image)
                file = ewl_image_file_path_get(EWL_IMAGE(icon->image));

        DRETURN_PTR(file, DLEVEL_STABLE);
}

/**
 * @param icon: The icon to set if it is editable or not
 * @param e: The value to set as the editable flag
 * @return Returns no value.
 * @brief Set if the icon is editable or not
 */
void
ewl_icon_editable_set(Ewl_Icon *icon, unsigned int e)
{
        DENTER_FUNCTION(DLEVEL_STABLE);
        DCHECK_PARAM_PTR(icon);
        DCHECK_TYPE(icon, EWL_ICON_TYPE);

        if (icon->editable == e)
                DRETURN(DLEVEL_STABLE);

        icon->editable = e;
        if (icon->editable && icon->label)
                ewl_callback_append(icon->label, EWL_CALLBACK_MOUSE_DOWN,
                                        ewl_icon_cb_label_mouse_down, icon);
        else if (icon->label)
                ewl_callback_del(icon->label, EWL_CALLBACK_MOUSE_DOWN,
                                        ewl_icon_cb_label_mouse_down);

        DLEAVE_FUNCTION(DLEVEL_STABLE);
}

/**
 * @param icon: The icon to check if it's editable
 * @return Returns TRUE if the icon is editable, FALSE otherwise
 * @brief Retrieve if the icon is editable or not
 */
unsigned int
ewl_icon_editable_get(Ewl_Icon *icon)
{
        DENTER_FUNCTION(DLEVEL_STABLE);
        DCHECK_PARAM_PTR_RET(icon, FALSE);
        DCHECK_TYPE_RET(icon, EWL_ICON_TYPE, FALSE);

        DRETURN_INT(icon->editable, DLEVEL_STABLE);
}

/**
 * @param icon: The icon to set the label onto
 * @param label: The label to set on the icon
 * @return Returns no value
 * @brief Set the label of the icon
 */
void
ewl_icon_label_set(Ewl_Icon *icon, const char *label)
{
        DENTER_FUNCTION(DLEVEL_STABLE);
        DCHECK_PARAM_PTR(icon);
        DCHECK_TYPE(icon, EWL_ICON_TYPE);

        if (!label)
        {
                if (icon->label)
                {
                        ewl_icon_label_text_set(icon, NULL);
                        IF_FREE(icon->label_text);
                }

                DRETURN(DLEVEL_STABLE);
        }

        if (!icon->label)
                ewl_icon_label_build(icon);

        icon->label_text = strdup(label);
        ewl_icon_label_update(icon);

        DLEAVE_FUNCTION(DLEVEL_STABLE);
}

/**
 * @param icon: The icon to get the label from
 * @return Returns the icons label or NULL if none set
 * @brief Retrieve the label from the icon
 */
const char *
ewl_icon_label_get(Ewl_Icon *icon)
{
        DENTER_FUNCTION(DLEVEL_STABLE);
        DCHECK_PARAM_PTR_RET(icon, NULL);
        DCHECK_TYPE_RET(icon, EWL_ICON_TYPE, NULL);

        if (!icon->label)
                DRETURN_PTR(NULL, DLEVEL_STABLE);

        DRETURN_PTR(icon->label_text, DLEVEL_STABLE);
}

/**
 * @param icon: The icon to set the extended data into
 * @param ext: The extended data to set in the icon
 * @return Returns no value
 * @brief Set the extended data into the icon
 *
 * @note The widget passed in here becomes internal to the icon, you should
 * not delete it after this. You can pack widgets as needed but the icon
 * will handle the show/hide of the widget after this
 */
void
ewl_icon_extended_data_set(Ewl_Icon *icon, Ewl_Widget *ext)
{
        DENTER_FUNCTION(DLEVEL_STABLE);
        DCHECK_PARAM_PTR(icon);
        DCHECK_TYPE(icon, EWL_ICON_TYPE);

        if (icon->extended)
                ewl_widget_destroy(icon->extended);

        icon->extended = ext;
        ewl_widget_internal_set(icon->extended, TRUE);
        ewl_container_child_append(EWL_CONTAINER(icon), icon->extended);

        if (icon->type == EWL_ICON_TYPE_SHORT)
                ewl_widget_hide(icon->extended);
        else
                ewl_widget_show(icon->extended);

        DLEAVE_FUNCTION(DLEVEL_STABLE);
}

/**
 * @param icon: The icon to get the extended data from
 * @return Returns the extended data on the icon, or NULL if none set
 * @brief Retrieve the extended data from the icon
 */
Ewl_Widget *
ewl_icon_extended_data_get(Ewl_Icon *icon)
{
        DENTER_FUNCTION(DLEVEL_STABLE);
        DCHECK_PARAM_PTR_RET(icon, NULL);
        DCHECK_TYPE_RET(icon, EWL_ICON_TYPE, NULL);

        DRETURN_PTR(icon->extended, DLEVEL_STABLE);
}

/**
 * @param icon: The Ewl_Icon to constrain
 * @param val: The val to constrain too
 * @return Returns no value.
 * @brief Set the constrain value on the icon
 */
void
ewl_icon_constrain_set(Ewl_Icon *icon, unsigned int val)
{
        DENTER_FUNCTION(DLEVEL_STABLE);
        DCHECK_PARAM_PTR(icon);
        DCHECK_TYPE(icon, EWL_ICON_TYPE);

        icon->constrain = val;
        if (icon->image)
                ewl_image_constrain_set(EWL_IMAGE(icon->image), val);

        DLEAVE_FUNCTION(DLEVEL_STABLE);
}

/**
 * @param icon: The icon to get the constrain from
 * @return Returns the current constrain value of the icon
 * @brief Retrieve the constrain value set on the icon
 */
unsigned int
ewl_icon_constrain_get(Ewl_Icon *icon)
{
        DENTER_FUNCTION(DLEVEL_STABLE);
        DCHECK_PARAM_PTR_RET(icon, 0);
        DCHECK_TYPE_RET(icon, EWL_ICON_TYPE, 0);

        DRETURN_INT(icon->constrain, DLEVEL_STABLE);
}

/**
 * @param icon: The icon to work with
 * @param compress: The compression setting to use
 * @return Returns no value
 * @brief Sets the compressions setting for the icon to the given value
 */
void
ewl_icon_label_compressed_set(Ewl_Icon *icon, unsigned int compress)
{
        DENTER_FUNCTION(DLEVEL_STABLE);
        DCHECK_PARAM_PTR(icon);
        DCHECK_TYPE(icon, EWL_ICON_TYPE);

        /* nothing to do if no compression change */
        if (compress == icon->compress_label)
                DRETURN(DLEVEL_STABLE);

        icon->compress_label = !!compress;
        ewl_icon_label_update(icon);

        DLEAVE_FUNCTION(DLEVEL_STABLE);
}

/**
 * @param icon: The ewl_icon to work with
 * @return Returns the current compression setting for the icon
 * @brief Retrieves the current compressiion setting for the icon
 */
unsigned int
ewl_icon_label_compressed_get(Ewl_Icon *icon)
{
        DENTER_FUNCTION(DLEVEL_STABLE);
        DCHECK_PARAM_PTR_RET(icon, FALSE);
        DCHECK_TYPE_RET(icon, EWL_ICON_TYPE, FALSE);

        DRETURN_INT(icon->compress_label, DLEVEL_STABLE);
}

/**
 * @param icon: The icon to set if it has a complex label or not
 * @param c: The value to set as the complex flag
 * @return Returns no value.
 * @brief Set if the icon use a label or a text widget for the label
 * part
 */
void
ewl_icon_label_complex_set(Ewl_Icon *icon, unsigned int c)
{
        DENTER_FUNCTION(DLEVEL_STABLE);
        DCHECK_PARAM_PTR(icon);
        DCHECK_TYPE(icon, EWL_ICON_TYPE);

        if (icon->complex_label == !!c)
                DRETURN(DLEVEL_STABLE);

        icon->complex_label = !!c;

        if (!icon->label)
                DRETURN(DLEVEL_STABLE);

        /*
         * we are now switching from Ewl_Label to Ewl_Text
         * or vice-verse, so first of all destroy the current
         * widget
         */
        ewl_widget_destroy(icon->label);
        icon->label = NULL;

        ewl_icon_label_build(icon);
        ewl_icon_label_update(icon);

        DLEAVE_FUNCTION(DLEVEL_STABLE);
}

/**
 * @param icon: The icon to check if its label is complex
 * @return Returns TRUE if the label is complex, FALSE otherwise
 * @brief Retrieve if the icon has a complex label or not
 */
unsigned int
ewl_icon_label_complex_get(Ewl_Icon *icon)
{
        DENTER_FUNCTION(DLEVEL_STABLE);
        DCHECK_PARAM_PTR_RET(icon, FALSE);
        DCHECK_TYPE_RET(icon, EWL_ICON_TYPE, FALSE);

        DRETURN_INT(icon->complex_label, DLEVEL_STABLE);
}


/**
 * @param icon: The icon to set if the image gets thumbnailed
 * @param t: The value to set as the thumbnail flag
 * @return Returns no value.
 * @brief Set if the icon thumbnailed the image or use it directly
 */
void
ewl_icon_thumbnailing_set(Ewl_Icon *icon, unsigned int t)
{
        DENTER_FUNCTION(DLEVEL_STABLE);
        DCHECK_PARAM_PTR(icon);
        DCHECK_TYPE(icon, EWL_ICON_TYPE);

        if (icon->thumbnailing == !!t)
                DRETURN(DLEVEL_STABLE);

        icon->thumbnailing = !!t;

        DLEAVE_FUNCTION(DLEVEL_STABLE);
}

/**
 * @param icon: The icon to check if it's thumbnails the image
 * @return Returns TRUE if the label use thumbnailing, FALSE otherwise
 * @brief Retrieve if the icon use thumbnailing
 */
unsigned int
ewl_icon_thumbnailing_get(Ewl_Icon *icon)
{
        DENTER_FUNCTION(DLEVEL_STABLE);
        DCHECK_PARAM_PTR_RET(icon, FALSE);
        DCHECK_TYPE_RET(icon, EWL_ICON_TYPE, FALSE);

        DRETURN_INT(icon->thumbnailing, DLEVEL_STABLE);
}

/**
 * @param icon: The icon to work with
 * @param txt: The text to set as the alternate text
 * @return Returns no value
 * @brief Sets the given text as the alternate text for the icon
 */
void
ewl_icon_alt_text_set(Ewl_Icon *icon, const char *txt)
{
        const char *img, *file;

        DENTER_FUNCTION(DLEVEL_STABLE);
        DCHECK_PARAM_PTR(icon);
        DCHECK_TYPE(icon, EWL_ICON_TYPE);

        if (icon->alt)
        {
                ewl_label_text_set(EWL_LABEL(icon->alt), txt);
                DRETURN(DLEVEL_STABLE);
        }

        icon->alt = ewl_label_new();
        ewl_label_text_set(EWL_LABEL(icon->alt), txt);
        ewl_object_alignment_set(EWL_OBJECT(icon->alt), EWL_FLAG_ALIGN_CENTER);
        ewl_object_fill_policy_set(EWL_OBJECT(icon->alt), EWL_FLAG_FILL_VFILL);
        ewl_container_child_prepend(EWL_CONTAINER(icon), icon->alt);

        /* if the image displayed is the loading image then we switch to the
         * alt text. Also switch to alt text if the image doesn't exist */
        img = ewl_icon_theme_icon_path_get(EWL_ICON_IMAGE_LOADING, 0);
        file = ewl_icon_image_file_get(icon);
        if ((!file || !ecore_file_exists(ewl_icon_image_file_get(icon))) ||
                        (icon->image && !strcmp(img, file)))
        {
                if (icon->image) ewl_widget_hide(icon->image);
                ewl_widget_show(icon->alt);
        }

        DLEAVE_FUNCTION(DLEVEL_STABLE);
}

/**
 * @param icon: The icon to work with
 * @return Returns the alternate text set on the icon
 * @brief Retrieves the alternate text set on the icon
 */
const char *
ewl_icon_alt_text_get(Ewl_Icon *icon)
{
        DENTER_FUNCTION(DLEVEL_STABLE);
        DCHECK_PARAM_PTR_RET(icon, NULL);
        DCHECK_TYPE_RET(icon, EWL_ICON_TYPE, NULL);

        DRETURN_PTR(ewl_label_text_get(EWL_LABEL(icon->alt)), DLEVEL_STABLE);
}

/**
 * @internal
 * @param w: The widget to work with
 * @param ev: UNUSED
 * @param data: UNUSED
 * @return Returns no value
 * @brief The destroy callback
 */
void
ewl_icon_cb_destroy(Ewl_Widget *w, void *ev __UNUSED__, void *data __UNUSED__)
{
        Ewl_Icon *icon;

        DENTER_FUNCTION(DLEVEL_STABLE);
        DCHECK_PARAM_PTR(w);
        DCHECK_TYPE(w, EWL_ICON_TYPE);

        icon = EWL_ICON(w);
        IF_FREE(icon->label_text);

        DLEAVE_FUNCTION(DLEVEL_STABLE);
}

static void
ewl_icon_cb_label_mouse_down(Ewl_Widget *w __UNUSED__, void *ev __UNUSED__,
                                                void *data)
{
        Ewl_Icon *icon;
        Ewl_Widget *entry;
        Ewl_Embed *emb;
        int x, y;

        DENTER_FUNCTION(DLEVEL_STABLE);
        DCHECK_PARAM_PTR(data);
        DCHECK_TYPE(data, EWL_ICON_TYPE);

        icon = data;
        emb = ewl_embed_widget_find(EWL_WIDGET(icon));

        ewl_widget_hide(icon->label);

        entry = ewl_entry_new();
        ewl_text_text_set(EWL_TEXT(entry), icon->label_text);
        ewl_container_child_append(EWL_CONTAINER(emb), entry);

        /* put the entry in the same spot as the label */
        ewl_object_current_geometry_get(EWL_OBJECT(icon->label), &x, &y,
                                                        NULL, NULL);
        ewl_object_position_request(EWL_OBJECT(entry), x, y);
        ewl_widget_show(entry);

        ewl_callback_append(entry, EWL_CALLBACK_FOCUS_OUT,
                                ewl_icon_cb_entry_focus_out, icon);
        ewl_callback_append(entry, EWL_CALLBACK_VALUE_CHANGED,
                                ewl_icon_cb_entry_value_changed, icon);

        ewl_embed_focused_widget_set(emb, entry);

        DLEAVE_FUNCTION(DLEVEL_STABLE);
}

static void
ewl_icon_cb_entry_focus_out(Ewl_Widget *w, void *ev __UNUSED__,        void *data)
{
        Ewl_Icon *icon;

        DENTER_FUNCTION(DLEVEL_STABLE);
        DCHECK_PARAM_PTR(w);
        DCHECK_PARAM_PTR(data);
        DCHECK_TYPE(w, EWL_WIDGET_TYPE);
        DCHECK_TYPE(data, EWL_ICON_TYPE);

        icon = data;

        ewl_widget_show(icon->label);
        ewl_widget_destroy(w);

        DLEAVE_FUNCTION(DLEVEL_STABLE);
}

static void
ewl_icon_cb_entry_value_changed(Ewl_Widget *w, void *ev __UNUSED__, void *data)
{
        Ewl_Icon *icon;

        DENTER_FUNCTION(DLEVEL_STABLE);
        DCHECK_PARAM_PTR(w);
        DCHECK_PARAM_PTR(data);
        DCHECK_TYPE(w, EWL_WIDGET_TYPE);
        DCHECK_TYPE(data, EWL_ICON_TYPE);

        icon = data;
        ewl_icon_label_set(icon, ewl_text_text_get(EWL_TEXT(w)));

        ewl_widget_show(icon->label);
        ewl_widget_destroy(w);

        ewl_callback_call(EWL_WIDGET(icon), EWL_CALLBACK_VALUE_CHANGED);
}

static void
ewl_icon_parts_update(Ewl_Icon *icon)
{
        DENTER_FUNCTION(DLEVEL_STABLE);
        DCHECK_PARAM_PTR(icon);
        DCHECK_TYPE(icon, EWL_ICON_TYPE);

        /*
         * show both the label and the image
         */
        if (icon->hidden == EWL_ICON_PART_NONE) {
                if (icon->label)
                        ewl_widget_show(icon->label);
                if (icon->image)
                        ewl_widget_show(icon->image);
        }
        /*
         * show only the label
         */
        else if (icon->hidden == EWL_ICON_PART_IMAGE) {
                if (icon->label && icon->image) {
                        ewl_widget_show(icon->label);
                        ewl_widget_hide(icon->image);
                }
                else if (icon->label)
                        ewl_widget_show(icon->label);
                /*
                 * show the image if there is no label
                 */
                else if (icon->image)
                        ewl_widget_show(icon->image);
        }
        /*
         * show only the image
         */
        else if (icon->hidden == EWL_ICON_PART_LABEL) {
                if (icon->label && icon->image) {
                        ewl_widget_show(icon->image);
                        ewl_widget_hide(icon->label);
                }
                else if (icon->image)
                        ewl_widget_show(icon->image);
                /*
                 * show label if there is no image
                 */
                else if (icon->label)
                        ewl_widget_show(icon->label);
        }

        DLEAVE_FUNCTION(DLEVEL_STABLE);
}

static void
ewl_icon_label_build(Ewl_Icon *icon)
{
        DENTER_FUNCTION(DLEVEL_STABLE);
        DCHECK_PARAM_PTR(icon);
        DCHECK_TYPE(icon, EWL_ICON_TYPE);

        if (icon->complex_label)
                icon->label = ewl_text_new();
        else
                icon->label = ewl_label_new();

        ewl_object_fill_policy_set(EWL_OBJECT(icon->label),
                                        EWL_FLAG_FILL_NONE);
        ewl_object_alignment_set(EWL_OBJECT(icon->label),
                                        EWL_FLAG_ALIGN_CENTER);

        if (icon->editable)
                ewl_callback_append(icon->label,
                                EWL_CALLBACK_MOUSE_DOWN,
                                ewl_icon_cb_label_mouse_down, icon);

        if (icon->hidden != EWL_ICON_PART_LABEL)
                ewl_widget_show(icon->label);

        /* if we have a image make sure we are after it, but
         * before anything that is after the image */
        if (icon->image && icon->extended)
        {
                int idx;
                idx = ewl_container_child_index_get(EWL_CONTAINER(icon),
                                                icon->image);
                ewl_container_child_insert_internal(EWL_CONTAINER(icon),
                                        icon->label, idx + 1);
        }
        else
                ewl_container_child_append(EWL_CONTAINER(icon),
                                                        icon->label);

        DLEAVE_FUNCTION(DLEVEL_STABLE);
}

static void
ewl_icon_label_text_set(Ewl_Icon *icon, const char *txt)
{
        DENTER_FUNCTION(DLEVEL_STABLE);
        DCHECK_PARAM_PTR(icon);
        DCHECK_TYPE(icon, EWL_ICON_TYPE);

        if (icon->complex_label)
                ewl_text_text_set(EWL_TEXT(icon->label), txt);
        else
                ewl_label_text_set(EWL_LABEL(icon->label), txt);

        DLEAVE_FUNCTION(DLEVEL_STABLE);
}

static void
ewl_icon_label_update(Ewl_Icon *icon)
{
        DENTER_FUNCTION(DLEVEL_STABLE);
        DCHECK_PARAM_PTR(icon);
        DCHECK_TYPE(icon, EWL_ICON_TYPE);

        /* nothing to do if no label set */
        if (!icon->label_text)
                DRETURN(DLEVEL_STABLE);

        if (icon->compress_label &&
                        (strlen(icon->label_text) > EWL_ICON_COMPRESS_SIZE))
        {
                char *c;

                c = NEW(char, EWL_ICON_COMPRESS_SIZE + 4);
                strncpy(c, icon->label_text, EWL_ICON_COMPRESS_SIZE);
                strcat(c, "...");

                ewl_icon_label_text_set(icon, c);
                FREE(c);
        }
        else
                ewl_icon_label_text_set(icon, icon->label_text);

        DLEAVE_FUNCTION(DLEVEL_STABLE);
}

static void
ewl_icon_cb_thumb_value_changed(Ewl_Widget *w __UNUSED__, void *ev __UNUSED__,
                                                                void *data)
{
        Ewl_Icon *icon;

        DENTER_FUNCTION(DLEVEL_STABLE);
        DCHECK_PARAM_PTR(data);
        DCHECK_TYPE(data, EWL_ICON_TYPE);

        icon = EWL_ICON(data);
        if (icon->alt && VISIBLE(icon->alt))
                ewl_widget_hide(icon->alt);

        if (!VISIBLE(icon->image))
                ewl_widget_show(icon->image);

        DLEAVE_FUNCTION(DLEVEL_STABLE);
}



