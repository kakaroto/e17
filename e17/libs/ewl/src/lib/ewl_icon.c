#include <Ewl.h>
#include "ewl_debug.h"
#include "ewl_macros.h"
#include "ewl_private.h"

static void ewl_icon_cb_label_mouse_down(Ewl_Widget *w, void *ev, 
							void *data);
static void ewl_icon_cb_entry_focus_out(Ewl_Widget *w, void *ev,
							void *data);
static void ewl_icon_cb_entry_value_changed(Ewl_Widget *w, void *ev,
							void *data);

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
 * @param icon: The widget to initialize
 * @return Returns TRUE on successful initialization, FALSE otherwise
 * @brief Initializes the given Ewl_Icon widget
 */
int
ewl_icon_init(Ewl_Icon *icon)
{
	DENTER_FUNCTION(DLEVEL_STABLE);
	DCHECK_PARAM_PTR_RET("icon", icon, FALSE);

	if (!ewl_box_init(EWL_BOX(icon)))
		DRETURN_INT(FALSE, DLEVEL_STABLE);

	ewl_box_orientation_set(EWL_BOX(icon), EWL_ORIENTATION_VERTICAL);
	ewl_box_spacing_set(EWL_BOX(icon), 4);

	ewl_widget_appearance_set(EWL_WIDGET(icon), EWL_ICON_TYPE);
	ewl_widget_inherit(EWL_WIDGET(icon), EWL_ICON_TYPE);

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
	DCHECK_PARAM_PTR("icon", icon);
	DCHECK_TYPE("icon", icon, EWL_ICON_TYPE);

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
	DCHECK_PARAM_PTR_RET("icon", icon, EWL_ICON_TYPE_SHORT);
	DCHECK_TYPE_RET("icon", icon, EWL_ICON_TYPE, EWL_ICON_TYPE_SHORT);

	DRETURN_INT(icon->type, DLEVEL_STABLE);
}

/**
 * @param icon: The Ewl_Icon to set the image into
 * @param file: The file with the image
 * @param key: The key inside the file if applicable
 * @return Retruns no value
 * @brief set the image to use in the icon
 */
void
ewl_icon_image_set(Ewl_Icon *icon, const char *file, const char *key)
{
	DENTER_FUNCTION(DLEVEL_STABLE);
	DCHECK_PARAM_PTR("icon", icon);
	DCHECK_PARAM_PTR("file", file);
	DCHECK_TYPE("icon", icon, EWL_ICON_TYPE);

	if (!icon->preview)
	{
		icon->preview = ewl_image_new();
		ewl_image_proportional_set(EWL_IMAGE(icon->preview), TRUE);
		ewl_widget_internal_set(icon->preview, TRUE);
		ewl_container_child_prepend(EWL_CONTAINER(icon), 
						icon->preview);
		ewl_widget_show(icon->preview);
	}

	ewl_image_file_set(EWL_IMAGE(icon->preview), file, key);

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
	DCHECK_PARAM_PTR_RET("icon", icon, NULL);
	DCHECK_TYPE_RET("icon", icon, EWL_ICON_TYPE, NULL);

	if (icon->preview)
		file = ewl_image_file_path_get(EWL_IMAGE(icon->preview));

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
	DCHECK_PARAM_PTR("icon", icon);
	DCHECK_TYPE("icon", icon, EWL_ICON_TYPE);

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
	DCHECK_PARAM_PTR_RET("icon", icon, FALSE);
	DCHECK_TYPE_RET("icon", icon, EWL_ICON_TYPE, FALSE);

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
	DCHECK_PARAM_PTR("icon", icon);
	DCHECK_TYPE("icon", icon, EWL_ICON_TYPE);

	if (!label)
	{
		if (icon->label)
			ewl_text_text_set(EWL_TEXT(icon->label), NULL);

		DRETURN(DLEVEL_STABLE);
	}

	if (!icon->label)
	{
		icon->label = ewl_text_new();
		ewl_object_alignment_set(EWL_OBJECT(icon->label), EWL_FLAG_ALIGN_CENTER);

		if (icon->editable)
			ewl_callback_append(icon->label, 
					EWL_CALLBACK_MOUSE_DOWN,
					ewl_icon_cb_label_mouse_down, icon);

		ewl_widget_show(icon->label);

		/* if we have a preview make sure we are after it, but
		 * before anything that is after the preview */
		if (icon->preview && icon->extended)
		{
			int idx;
			idx = ewl_container_child_index_get(EWL_CONTAINER(icon),
							icon->preview);
			ewl_container_child_insert_internal(EWL_CONTAINER(icon),
						icon->label, idx + 1);
		}
		else
			ewl_container_child_append(EWL_CONTAINER(icon),
								icon->label);
	}

	ewl_text_text_set(EWL_TEXT(icon->label), label);

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
	DCHECK_PARAM_PTR_RET("icon", icon, NULL);
	DCHECK_TYPE_RET("icon", icon, EWL_ICON_TYPE, NULL);

	if (!icon->label)
		DRETURN_PTR(NULL, DLEVEL_STABLE);

	DRETURN_PTR(ewl_text_text_get(EWL_TEXT(icon->label)),
						DLEVEL_STABLE);
}

/**
 * @param icon: The icon to set the menu into
 * @param menu: The mneu to set on the icon
 * @return Returns no value
 * @brief Set the menu for the icon
 */
void
ewl_icon_menu_set(Ewl_Icon *icon, Ewl_Widget *menu)
{
	DENTER_FUNCTION(DLEVEL_STABLE);
	DCHECK_PARAM_PTR("icon", icon);
	DCHECK_PARAM_PTR("menu", menu);
	DCHECK_TYPE("icon", icon, EWL_ICON_TYPE);
	DCHECK_TYPE("menu", menu, EWL_MENU_TYPE);

	printf("FIXME: MENUS NOT HOOKED INTO ICONS YET\n");

	DLEAVE_FUNCTION(DLEVEL_STABLE);
}

/** 
 * @param icon: The icon to get the menu from
 * @return Returns the menu set on this icon, or NULL if none set 
 * @brief Retrieve the menu from the icon
 */
Ewl_Widget *
ewl_icon_menu_get(Ewl_Icon *icon)
{
	DENTER_FUNCTION(DLEVEL_STABLE);
	DCHECK_PARAM_PTR_RET("icon", icon, NULL);
	DCHECK_TYPE_RET("icon", icon, EWL_ICON_TYPE, NULL);

	printf("FIXME: MENUS NOT HOOKED INTO ICONS YET\n");

	DRETURN_PTR(NULL, DLEVEL_STABLE);
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
	DCHECK_PARAM_PTR("icon", icon);
	DCHECK_TYPE("icon", icon, EWL_ICON_TYPE);

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
	DCHECK_PARAM_PTR_RET("icon", icon, NULL);
	DCHECK_TYPE_RET("icon", icon, EWL_ICON_TYPE, NULL);

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
	DCHECK_PARAM_PTR("icon", icon);
	DCHECK_TYPE("icon", icon, EWL_ICON_TYPE);

	ewl_image_constrain_set(EWL_IMAGE(icon->preview), val);

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
	unsigned int constrain;

	DENTER_FUNCTION(DLEVEL_STABLE);
	DCHECK_PARAM_PTR_RET("icon", icon, 0);
	DCHECK_TYPE_RET("icon", icon, EWL_ICON_TYPE, 0);

	constrain = ewl_image_constrain_get(EWL_IMAGE(icon->preview));

	DRETURN_INT(constrain, DLEVEL_STABLE);
}

static void
ewl_icon_cb_label_mouse_down(Ewl_Widget *w __UNUSED__, void *ev __UNUSED__, 
						void *data)
{
	Ewl_Icon *icon;
	Ewl_Widget *entry;
	Ewl_Embed *emb;
	int x, y, width, height;

	DENTER_FUNCTION(DLEVEL_STABLE);
	DCHECK_PARAM_PTR("data", data);
	DCHECK_TYPE("data", data, EWL_ICON_TYPE);

	icon = data;
	emb = ewl_embed_widget_find(EWL_WIDGET(icon));

	ewl_widget_hide(icon->label);

	entry = ewl_entry_new();
	ewl_text_text_set(EWL_TEXT(entry),
			ewl_text_text_get(EWL_TEXT(icon->label)));
	ewl_container_child_append(EWL_CONTAINER(emb), entry);

	/* put the entry in the same spot as the label */
	ewl_object_current_geometry_get(EWL_OBJECT(icon->label), &x, &y, 
							&width, &height);
	ewl_object_geometry_request(EWL_OBJECT(entry), x, y, width, height);
	ewl_widget_show(entry);

	ewl_callback_append(entry, EWL_CALLBACK_FOCUS_OUT,
				ewl_icon_cb_entry_focus_out, icon);
	ewl_callback_append(entry, EWL_CALLBACK_VALUE_CHANGED,
				ewl_icon_cb_entry_value_changed, icon);

	ewl_embed_focused_widget_set(emb, entry);

	DLEAVE_FUNCTION(DLEVEL_STABLE);
}

static void
ewl_icon_cb_entry_focus_out(Ewl_Widget *w, void *ev __UNUSED__, 
						void *data __UNUSED__)
{
	Ewl_Icon *icon;

	DENTER_FUNCTION(DLEVEL_STABLE);
	DCHECK_PARAM_PTR("w", w);
	DCHECK_PARAM_PTR("data", data);
	DCHECK_TYPE("w", w, EWL_WIDGET_TYPE);
	DCHECK_TYPE("data", data, EWL_ICON_TYPE);

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
	DCHECK_PARAM_PTR("w", w);
	DCHECK_PARAM_PTR("data", data);
	DCHECK_TYPE("w", w, EWL_WIDGET_TYPE);
	DCHECK_TYPE("data", data, EWL_ICON_TYPE);

	icon = data;
	ewl_icon_label_set(icon, ewl_text_text_get(EWL_TEXT(w)));

	ewl_widget_show(icon->label);
	ewl_widget_destroy(w);

	ewl_callback_call(EWL_WIDGET(icon), EWL_CALLBACK_VALUE_CHANGED);
}


