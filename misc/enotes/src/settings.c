
/**************************************************
 **               E  -  N O T E S                **
 **                                              **
 **  The contents of this file are released to   **
 **  the public under the General Public Licence **
 **  Version 2.                                  **
 **                                              **
 **  By  Thomas Fletcher (www.fletch.vze.com)    **
 **                                              **
 **************************************************/


#include "settings.h"

Settings       *settings = NULL;


/* High Level */

/**
 * @brief: Sets up the settings window.  It checks whether its open first
 *         to make sure we don't get any duplicates.
 */
void
setup_settings(void)
{
	if (settings == NULL) {
		dml("Opening the Settings Window", 2);
		settings = malloc(sizeof(Settings));
		setup_settings_win(settings);
		fill_tree();
	} else {
		dml("Won't Open Another Settings Window", 2);
	}
	return;
}


/**
 * @param s: The Settings typedef structure to put the pointers into.
 * @brief: Sets up the settings window, widgets, callbacks, etc.. and stores
 *         the pointers into the typedef structure s.
 */
void
setup_settings_win(Settings * s)
{
	char           *headers[2];

	/* Setup the Window */
	s->win = ewl_window_new();
	ewl_window_title_set((Ewl_Window *) s->win, "E-Notes Settings");
	ewl_object_fill_policy_set((Ewl_Object*)s->win, EWL_FLAG_FILL_ALL);
	ewl_object_size_request((Ewl_Object*)s->win, 400,350);
	ewl_widget_show(s->win);

	s->vbox = ewl_vbox_new();
	ewl_container_child_append((Ewl_Container *) s->win, s->vbox);
	ewl_object_fill_policy_set((Ewl_Object*)s->vbox, EWL_FLAG_FILL_ALL);
	ewl_widget_show(s->vbox);

	ewl_callback_append(s->win, EWL_CALLBACK_CONFIGURE, settings_move_embed,
			    s->vbox);

	s->tree = ewl_tree_new(2);
	ewl_container_child_append((Ewl_Container *) s->vbox, s->tree);

	headers[0] = strdup("Setting");
	headers[1] = strdup("Value");
	ewl_tree_headers_set((Ewl_Tree *) s->tree, headers);
	free(headers[0]);
	free(headers[1]);

	ewl_widget_show(s->tree);

	s->hbox = ewl_hbox_new();
	ewl_container_child_append((Ewl_Container *) s->vbox, s->hbox);
	ewl_object_fill_policy_set ((Ewl_Object*) s->hbox, EWL_FLAG_FILL_VSHRINK);
	ewl_object_fill_policy_set ((Ewl_Object*) s->hbox, EWL_FLAG_FILL_HFILL);
	ewl_widget_show(s->hbox);

	settings_setup_button(s->hbox, &(s->savebtn), "Save.");
	settings_setup_button(s->hbox, &(s->revertbtn), "Revert.");
	settings_setup_button(s->hbox, &(s->closebtn), "Close.");

	/* EWL Callbacks */
	ewl_callback_append(s->revertbtn, EWL_CALLBACK_CLICKED,
			    (void *) ewl_settings_revert, (void *) s->tree);
	ewl_callback_append(s->closebtn, EWL_CALLBACK_CLICKED,
			    (void *) ewl_settings_close, (void *) s->win);
	ewl_callback_append(s->win, EWL_CALLBACK_DELETE_WINDOW,
			    (void *) ewl_settings_close, (void *) s->win);
	ewl_callback_append(s->savebtn, EWL_CALLBACK_CLICKED,
			    (void *) ewl_settings_save, NULL);

	return;
}

/**
 * @brief: Fills the tree with all of the setup options.  This basically
 *         calls the function setup_settings_opt over and over and stores
 *         the pointers returned from that function into the settings
 *         structure.
 */
void
fill_tree(void)
{
	settings->render_method =
		setup_settings_opt(settings->tree, "Render Method:",
				   main_config->render_method);
	settings->theme =
		setup_settings_opt(settings->tree, "Theme:",
				   main_config->theme);
	settings->intro =
		setup_settings_opt_int(settings->tree, "Intro:",
				       main_config->intro);

	settings->cc =
		setup_settings_opt_int(settings->tree,
				       "Control Centre [0=No 1=Yes]:",
				       main_config->controlcentre);
	settings->debug =
		setup_settings_opt_int(settings->tree, "Debugging Level [0-2]:",
				       main_config->debug);

	settings->autosave =
		setup_settings_opt_int(settings->tree,
				       "Autosaving [0=No 1=Yes]:",
				       main_config->autosave);

	settings->welcome =
		setup_settings_opt_int(settings->tree,
				       "Welcome Screen [0=No 1=Yes]:",
				       main_config->welcome);

	return;
}

/**
 * @param c: The container widget to store the button into.
 * @param b: The button pointer to use (pointer to a pointer).
 * @param label: The label to apply to the button.
 * @brief: This function sets up a button, it points it to b (pointer
 *         to a pointer) and appends it to the container (b), the button
 *         has the label contained in label.
 */
void
settings_setup_button(Ewl_Widget * c, Ewl_Widget ** b, char *label)
{
	*b = ewl_button_new(label);
	ewl_container_child_append((Ewl_Container *) c, *b);
	ewl_widget_show(*b);
	return;
}


/**
 * @param tree: The tree to append it to.
 * @param caption: The caption to be put into the text widget.
 * @param value: The beginning value.
 * @brief: Sets up a row in the tree, it uses the structure Settings_Opt
 *         which contains a text and entry widget.
 */
Settings_Opt
setup_settings_opt(Ewl_Widget * tree, char *caption, char *value)
{
	Settings_Opt    oa;
	Settings_Opt   *o = &oa;
	Ewl_Widget     *entries[2];

	o->caption = ewl_text_new(caption);
	ewl_widget_show(o->caption);

	o->entry = ewl_entry_new(value);
	ewl_widget_show(o->entry);

	entries[0] = o->caption;
	entries[1] = o->entry;

	ewl_tree_row_add((Ewl_Tree *) tree, 0, entries);

	return (oa);
}

/**
 * @param tree: The tree to append it to.
 * @param caption: The caption to set the text widget value to.
 * @param value: The beginning value (integer).
 * @brief: Creates a row in the tree for an option which will take
 *         an integer value.
 */
Settings_Opt
setup_settings_opt_int(Ewl_Widget * tree, char *caption, int value)
{
	Settings_Opt    o;
	char           *retval = malloc(1028);
	snprintf(retval, sizeof(int) + 1, "%d", value);
	o = setup_settings_opt(tree, caption, retval);
	free(retval);
	return (o);
}


/* Callbacks */

/**
 * @param widget: The widget which was clicked (we don't use this).
 * @param ev_data: The event data.  We don't use this either.
 * @param p: The widget supplied during the callback definition, its the tree.
 * @brief: The "revert" button was clicked, so we reset the values (thoroughly
 *         empty and refill the tree from scratch).
 */
void
ewl_settings_revert(Ewl_Widget * widget, void *ev_data, Ewl_Widget * p)
{
	dml("Refreshing the Settings Values", 2);
	ewl_container_reset((Ewl_Container *) p);
	fill_tree();

	return;
}

/**
 * @param o: The widget which was clicked (we don't use this).
 * @param ev_data: The event data.  We don't use this either.
 * @param ee: The Ecore_Evas we supply to the ecore callback.
 * @brief: The ewl close button was clicked, we we'll ask the ecore
 *         callback to do the work.
 */
void
ewl_settings_close(Ewl_Widget * o, void *ev_data, Ecore_Evas * ee)
{
	ewl_widget_destroy(settings->win);
	free(settings);
	settings = NULL;
	return;
}

/**
 * @param o: The widget which was clicked (we don't use this).
 * @param ev_data: The event data.  We don't use this either.
 * @param data: A NULL pointer to keep the compiler happy.  We
 *              don't use this.
 * @brief: The save button is clicked, so we save the configuration
 *         by calling save_settings.
 */
void
ewl_settings_save(Ewl_Widget * o, void *ev_data, void *data)
{
	dml("Saving Settings", 2);
	save_settings();
	return;
}


/* XML */

/**
 * @brief: This function saves the settings.  More specifically, it opens
 *         up the xml file from scratch and appends all of the options according
 *         to the values contained within the window using the storage and xml
 *         backends.
 */
void
save_settings(void)
{
	char           *locfn = malloc(PATH_MAX);
	XmlWriteHandle *p;

	snprintf(locfn, strlen(getenv("HOME")) + strlen(DEF_CONFIG_LOC),
		 DEF_CONFIG_LOC, getenv("HOME"));

	p = xml_write(locfn);

	xml_write_append_entry(p, "render_method",
			       ewl_entry_get_text((Ewl_Entry *) settings->
						  render_method.entry));
	xml_write_append_entry(p, "theme",
			       ewl_entry_get_text((Ewl_Entry *) settings->theme.
						  entry));
	xml_write_append_entry(p, "intro",
			       ewl_entry_get_text((Ewl_Entry *) settings->intro.
						  entry));
	xml_write_append_entry(p, "debug",
			       ewl_entry_get_text((Ewl_Entry *) settings->debug.
						  entry));
	xml_write_append_entry(p, "autosave",
			       ewl_entry_get_text((Ewl_Entry *) settings->
						  autosave.entry));
	xml_write_append_entry(p, "welcome",
			       ewl_entry_get_text((Ewl_Entry *) settings->
						  welcome.entry));

	xml_write_end(p);

	free(locfn);
	return;
}

/**
 * @param w: The widget to size according to the embed.
 * @params ev_data and user_data: Callback info.
 * @brief: Moves embed contents to correct location.
 */
void
settings_move_embed(Ewl_Widget * w, void *ev_data, void *user_data)
{
	ewl_object_geometry_request(EWL_OBJECT(user_data), CURRENT_X(w),
				    CURRENT_Y(w), CURRENT_W(w), CURRENT_H(w));
}
