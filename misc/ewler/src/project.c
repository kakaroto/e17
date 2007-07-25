#include <Ewl.h>

#include "ewler.h"

static Ewler_Project *active_project;

static Ewl_Widget *file_tree;
static Ewl_Widget *project_win;
static Ewl_Widget *open_project_win;
static Ewl_Widget *options_win;
static Ewl_Widget *confirm_win;
static Ewl_Widget *file_menu;
static Ewl_Widget *settings_win;

struct {
	Ewl_Widget *name_entry;
	Ewl_Widget *path_entry;
	Ewl_Widget *filename_entry;
} active_project_options;

struct {
	Ewl_Widget *name_entry;
	Ewl_Widget *path_entry;
} file_settings;

static void project_new( void );
static int project_open( char *filename );
static void project_close( Ewler_Project *p );
static Ewler_Project *project_copy( void );
static void project_file_settings( char *name );

/**
 * callback for exit confirmation dialog
 */
static EWL_CALLBACK_DEFN(project_exit_cb)
{
	int *response = ev_data;

	if( *response == EWL_RESPONSE_OK )
		ewler_shutdown();

	ewl_widget_destroy(confirm_win);
	confirm_win = NULL;
}

/**
 * exit confirmation launcher, triggered by file->exit or window delete
 */
static EWL_CALLBACK_DEFN(project_exit)
{
	Ewl_Widget *text, *yes, *no;

	if( !active_project->dirty )
		ewler_shutdown();

	if( confirm_win )
		return;

	confirm_win = ewl_dialog_new(EWL_POSITION_BOTTOM);
	if( !confirm_win )
		return;

	text = ewl_text_new("Project has not been saved, continue?");
	ewl_dialog_widget_add(EWL_DIALOG(confirm_win), text);
	ewl_widget_show(text);

	yes = ewl_dialog_button_add(EWL_DIALOG(confirm_win),
															EWL_STOCK_OK, EWL_RESPONSE_OK);
	ewl_button_label_set(EWL_BUTTON(yes), "Yes");
	ewl_callback_append(yes, EWL_CALLBACK_VALUE_CHANGED,
											project_exit_cb, NULL);
	ewl_widget_show(yes);

	no = ewl_dialog_button_add(EWL_DIALOG(confirm_win),
														 EWL_STOCK_CANCEL, EWL_RESPONSE_CANCEL);
	ewl_button_label_set(EWL_BUTTON(no), "No");
	ewl_callback_append(no, EWL_CALLBACK_VALUE_CHANGED,
											project_exit_cb, NULL);
	ewl_widget_show(no);

	ewl_widget_show(confirm_win);
}

/**
 * keystroke accelerator for project window
 */
static EWL_CALLBACK_DEFN(project_key_down)
{
	Ecore_List *selected;
	Ewl_Event_Key_Down *ev = ev_data;
	Ewl_Widget *text, *row;
	char *name;

	if( !strcmp(ev->keyname, "Delete") ) {
		selected = ewl_tree_selected_get(EWL_TREE(file_tree));

		if( ecore_list_count(selected) > 0 ) {
			ecore_list_first_goto(selected);

			while( (row = ecore_list_next(selected)) ) {
				text = ewl_tree_row_column_get(EWL_ROW(row), 0);
				name = ewl_text_text_get(EWL_TEXT(text));
				ecore_hash_remove(active_project->files, name);
				FREE(name);
				active_project->dirty = true;
			}

			project_update();
		}
	}
}

/**
 * file->save
 */
static EWL_CALLBACK_DEFN(save)
{
	/* check for ever saved, request info */
	if( active_project->dirty ) {
		file_project_save(active_project);

		active_project->dirty = false;
	}
}

/**
 * callback for open filedialog
 */
static EWL_CALLBACK_DEFN(open_cb)
{
	char *filename;
	int *response = ev_data;

	if( response && *response == EWL_RESPONSE_OPEN ) {
		filename = ewl_filedialog_file_get(EWL_FILEDIALOG(w));

		if( project_open(filename) < 0 )
			/* issue a crazy warning */;
	}

	ewl_widget_destroy(open_project_win);
	open_project_win = NULL;
}

/**
 * file->open
 */
static EWL_CALLBACK_DEFN(open)
{
	Ewl_Widget *fd;

	if( open_project_win )
		return;

	open_project_win = ewl_window_new();

	fd = ewl_filedialog_new(EWL_FILEDIALOG_TYPE_OPEN);
	ewl_filedialog_path_set(EWL_FILEDIALOG(fd), active_project->path);
	ewl_callback_append(fd, EWL_CALLBACK_VALUE_CHANGED, open_cb, NULL);
	ewl_callback_append(fd, EWL_CALLBACK_DELETE_WINDOW, open_cb, NULL);
	ewl_container_child_append(EWL_CONTAINER(open_project_win), fd);
	ewl_widget_show(fd);

	ewl_widget_show(open_project_win);
}

/**
 * callback for project options dialog
 */
static EWL_CALLBACK_DEFN(options_cb)
{
	int *response_id = ev_data;

	if( *response_id == EWL_RESPONSE_SAVE ) {
		IF_FREE(active_project->path);
		active_project->path =
			ewl_text_text_get(EWL_TEXT(active_project_options.path_entry));

		IF_FREE(active_project->name);
		active_project->name =
			ewl_text_text_get(EWL_TEXT(active_project_options.name_entry));

		IF_FREE(active_project->filename);
		active_project->filename =
			ewl_text_text_get(EWL_TEXT(active_project_options.filename_entry));

		active_project->dirty = true;
	}

	ewl_widget_destroy(options_win);
	options_win = NULL;
}

/**
 * settings->options
 */
static EWL_CALLBACK_DEFN(options)
{
	Ewl_Widget *save, *cancel;
	Ewl_Widget *label_vbox, *entry_vbox, *hbox;
	Ewl_Widget *label, *entry;

	if( options_win )
		return;

	options_win = ewl_dialog_new(EWL_POSITION_BOTTOM);
	if( !options_win )
		return;

	hbox = ewl_hbox_new();
	ewl_dialog_widget_add(EWL_DIALOG(options_win), hbox);
	ewl_widget_show(hbox);

	label_vbox = ewl_vbox_new();
	ewl_container_child_append(EWL_CONTAINER(hbox), label_vbox);
	ewl_widget_show(label_vbox);

	label = ewl_text_new("Project Path:");
	ewl_container_child_append(EWL_CONTAINER(label_vbox), label);
	ewl_widget_show(label);

	label = ewl_text_new("Project Name:");
	ewl_container_child_append(EWL_CONTAINER(label_vbox), label);
	ewl_widget_show(label);

	label = ewl_text_new("Project File:");
	ewl_container_child_append(EWL_CONTAINER(label_vbox), label);
	ewl_widget_show(label);

	entry_vbox = ewl_vbox_new();
	ewl_container_child_append(EWL_CONTAINER(hbox), entry_vbox);
	ewl_widget_show(entry_vbox);

	entry = ewl_entry_new(active_project->path);
	ewl_container_child_append(EWL_CONTAINER(entry_vbox), entry);
	ewl_widget_show(entry);
	active_project_options.path_entry = entry;

	entry = ewl_entry_new(active_project->name);
	ewl_container_child_append(EWL_CONTAINER(entry_vbox), entry);
	ewl_widget_show(entry);
	active_project_options.name_entry = entry;

	entry = ewl_entry_new(active_project->filename);
	ewl_container_child_append(EWL_CONTAINER(entry_vbox), entry);
	ewl_widget_show(entry);
	active_project_options.filename_entry = entry;

	save = ewl_dialog_button_add(EWL_DIALOG(options_win),
															 EWL_STOCK_SAVE, EWL_RESPONSE_SAVE);
	ewl_callback_append(save, EWL_CALLBACK_VALUE_CHANGED, options_cb, NULL);
	ewl_widget_show(save);

	cancel = ewl_dialog_button_add(EWL_DIALOG(options_win),
																 EWL_STOCK_CANCEL, EWL_RESPONSE_CANCEL);
	ewl_callback_append(cancel, EWL_CALLBACK_VALUE_CHANGED, options_cb, NULL);
	ewl_widget_show(cancel);

	ewl_widget_show(options_win);
}

/**
 * file menu action callback
 */
static EWL_CALLBACK_DEFN(project_file_menu_cb)
{
	int action;
	char *name = user_data;
	Ewler_Form *form;

	action = (int) ewl_widget_data_get(w, "Action");

	switch( action ) {
		case EWLER_FILE_SAVE:
			form = form_get(name);
			form->filename = ecore_hash_get(active_project->files, name);
			if( form->dirty ) {
				if( file_form_save(form) < 0 )
					ewler_error("Unable to save file: %s", form->filename);
				else
					form->dirty = false;
			}
			break;
		case EWLER_FILE_DELETE:
			/* add history jiggies */
			form = form_get(name);
			if( form )
				form_close(form);
			project_file_delete(name);
			break;
		case EWLER_FILE_SETTINGS:
			project_file_settings(name);
			break;
	}

	ewl_widget_destroy(file_menu);
	file_menu = NULL;
}

/**
 * hack for getting the file popup menu to configure
 */
static EWL_CALLBACK_DEFN(project_file_menu_configure)
{
	ewl_callback_del(w, EWL_CALLBACK_CONFIGURE, project_file_menu_configure);
	ewl_widget_hide(w);
}

/**
 * popup menu for managing files in the project
 */
static EWL_CALLBACK_DEFN(project_file_menu)
{
	Ewl_Widget *item;
	Ewl_Event_Mouse_Up *ev = ev_data;

	if( file_menu ) {
		ewl_widget_destroy(file_menu);
		file_menu = NULL;
	} else if( ev->button == 3 ) {
		ewl_window_raise(EWL_WINDOW(project_win));

		file_menu = ewl_imenu_new(NULL, "File Options");
		ewl_object_position_request(EWL_OBJECT(file_menu), ev->x, ev->y);
		ewl_object_fill_policy_set(EWL_OBJECT(file_menu), EWL_FLAG_FILL_NONE);
		ewl_container_child_append(EWL_CONTAINER(project_win), file_menu);
		ewl_callback_append(file_menu, EWL_CALLBACK_CONFIGURE,
												project_file_menu_configure, NULL);
		ewl_callback_call(file_menu, EWL_CALLBACK_SELECT);
		ewl_widget_show(file_menu);

		item = ewl_menu_item_new(NULL, "Save");
		/* HACK FOR NON-STRING DATA HASHES */
		item->data = ecore_hash_new(ecore_str_hash, ecore_str_compare);
		ewl_widget_data_set(item, "Action", (void *) EWLER_FILE_SAVE);
		ewl_callback_append(item, EWL_CALLBACK_SELECT,
												project_file_menu_cb, user_data);
		ewl_container_child_append(EWL_CONTAINER(file_menu), item);
		ewl_widget_show(item);

		item = ewl_menu_item_new(NULL, "Settings...");
		/* HACK FOR NON-STRING DATA HASHES */
		item->data = ecore_hash_new(ecore_str_hash, ecore_str_compare);
		ewl_widget_data_set(item, "Action", (void *) EWLER_FILE_SETTINGS);
		ewl_callback_append(item, EWL_CALLBACK_SELECT,
												project_file_menu_cb, user_data);
		ewl_container_child_append(EWL_CONTAINER(file_menu), item);
		ewl_widget_show(item);

		item = ewl_menu_item_new(NULL, "Delete");
		/* HACK FOR NON-STRING DATA HASHES */
		item->data = ecore_hash_new(ecore_str_hash, ecore_str_compare);
		ewl_widget_data_set(item, "Action", (void *) EWLER_FILE_DELETE);
		ewl_callback_append(item, EWL_CALLBACK_SELECT,
												project_file_menu_cb, user_data);
		ewl_container_child_append(EWL_CONTAINER(file_menu), item);
		ewl_widget_show(item);
	}
}

/**
 * Now we're into the meat of it.  The following functions provide access to
 * project logic, and are used by the static callbacks above as well as
 * external modules
 */

static EWL_CALLBACK_DEFN(project_file_settings_cb)
{
	int *response = ev_data;
	char *name = user_data;
	char *new_name, *new_path;

	if( *response == EWL_RESPONSE_SAVE ) {
		Ewler_Form *form;

		new_name = ewl_text_text_get(EWL_TEXT(file_settings.name_entry));
		new_path = ewl_text_text_get(EWL_TEXT(file_settings.path_entry));

		if( (form = form_get(name)) )
			form_name_set(form, new_name);

		ecore_hash_remove(active_project->files, name);
		ecore_hash_set(active_project->files, new_name, new_path);

		project_update();
	}

	ewl_widget_destroy(settings_win);
	settings_win = NULL;
}

/**
 * spawn a dialog to alter the settings on a particular file
 */
static void
project_file_settings( char *name )
{
	Ewl_Widget *hbox, *vbox;
	Ewl_Widget *label, *entry, *button;

	if( settings_win )
		return;

	settings_win = ewl_dialog_new(EWL_POSITION_BOTTOM);
	if( !settings_win )
		return;

	hbox = ewl_hbox_new();
	ewl_dialog_widget_add(EWL_DIALOG(settings_win), hbox);
	
	vbox = ewl_vbox_new();
	ewl_container_child_append(EWL_CONTAINER(hbox), vbox);

	label = ewl_text_new("Object Name");
	ewl_container_child_append(EWL_CONTAINER(vbox), label);
	ewl_widget_show(label);

	label = ewl_text_new("File Name");
	ewl_container_child_append(EWL_CONTAINER(vbox), label);
	ewl_widget_show(label);

	ewl_widget_show(vbox);

	vbox = ewl_vbox_new();
	ewl_container_child_append(EWL_CONTAINER(hbox), vbox);

	entry = ewl_entry_new(name);
	ewl_container_child_append(EWL_CONTAINER(vbox), entry);
	ewl_widget_show(entry);
	file_settings.name_entry = entry;

	entry = ewl_entry_new(project_file_get(name));
	ewl_container_child_append(EWL_CONTAINER(vbox), entry);
	ewl_widget_show(entry);
	file_settings.path_entry = entry;

	ewl_widget_show(vbox);
	ewl_widget_show(hbox);

	button = ewl_dialog_button_add(EWL_DIALOG(settings_win),
																 EWL_STOCK_SAVE, EWL_RESPONSE_SAVE);
	ewl_callback_append(button, EWL_CALLBACK_VALUE_CHANGED,
											project_file_settings_cb, name);
	ewl_widget_show(button);

	button = ewl_dialog_button_add(EWL_DIALOG(settings_win),
																 EWL_STOCK_CANCEL, EWL_RESPONSE_CANCEL);
	ewl_callback_append(button, EWL_CALLBACK_VALUE_CHANGED,
											project_file_settings_cb, name);
	ewl_widget_show(button);

	ewl_widget_show(settings_win);
}

/**
 * add a file to the project listing
 */
void
project_file_add( char *name, char *filename )
{
	/* TODO check for dups */
	name = strdup(name);
	filename = strdup(filename);

	if( !name || !filename ) {
		ewler_error("project_file_add: out of memory!");
		IF_FREE(name);
		return;
	}

	ecore_hash_set(active_project->files, name, filename);
	active_project->dirty = true;
	project_update();
}

void
project_file_delete( char *name )
{
	ecore_hash_remove(active_project->files, name);
	active_project->dirty = true;
	project_update();
}

/**
 * retreive a filename from the project listing keyed by name
 */
char *
project_file_get( char *name )
{
	return ecore_hash_get(active_project->files, name);
}

/**
 * initialize this project
 */
static void
project_new( void )
{
	active_project->dirty = false;
	active_project->ever_saved = false;
	active_project->name = strdup("untitled");
	active_project->path = strdup("./");
	active_project->filename = strdup("untitled.ewl");

	project_update();
}

/**
 * open an existing project from a file.
 */
static int
project_open( char *filename )
{
	Ewler_Project *p;

	p = project_copy();
	if( !p ) {
		ewler_error("project_open: out of memory!");
		return -1;
	}

	active_project->filename = strdup(filename);
	active_project->files = ecore_hash_new(ecore_str_hash, ecore_str_compare);

	if( file_project_open(active_project) == 0 ) {
		project_close(p);
		active_project->dirty = false;
		active_project->ever_saved = true;
		project_update();
	} else {
		project_close(active_project);
		active_project = p;
		ewler_error("Unable to open project file: %s", active_project->filename);
		return -1;
	}

	return 0;
}

/**
 * close the current project
 */
static void
project_close( Ewler_Project *p )
{
	IF_FREE(p->name);
	IF_FREE(p->path);
	IF_FREE(p->filename);

	ecore_hash_destroy(p->files);
	FREE(p);
}

/**
 * saves a copy of the current project
 */
static Ewler_Project *
project_copy( void )
{
	Ewler_Project *p = NULL;

	p = NEW(Ewler_Project);
	if( !p )
		return NULL;

	memcpy(p, active_project, sizeof(Ewler_Project));

	return p;
}

/**
 * initialize this project manager.  called from main
 */
int
projects_init( char *filename )
{
	Ewl_Widget *vbox, *menu, *project_menu, *settings_menu, *windows_menu, *item;

	/* ui phase... kick on some phat window action */
	project_win = ewl_window_new();
	ewl_window_title_set(EWL_WINDOW(project_win), "Ewler");
	ewl_object_size_request(EWL_OBJECT(project_win), 200, 320);
	ewl_callback_append(project_win, EWL_CALLBACK_DELETE_WINDOW,
											project_exit, NULL);
	ewl_callback_append(project_win, EWL_CALLBACK_KEY_DOWN,
											project_key_down, NULL);

	vbox = ewl_vbox_new();
	ewl_container_child_append(EWL_CONTAINER(project_win), vbox);

	menu = ewl_hmenubar_new();
	ewl_container_child_append(EWL_CONTAINER(vbox), menu);

	project_menu = ewl_menubar_menu_add(EWL_MENUBAR(menu), NULL, "Project");

	item = ewl_menu_item_new(NULL, "New");
	ewl_container_child_append(EWL_CONTAINER(project_menu), item);
	ewl_widget_show(item);

	item = ewl_menu_item_new(NULL, "New Form");
	ewl_container_child_append(EWL_CONTAINER(project_menu), item);
	ewl_callback_append(item, EWL_CALLBACK_CLICKED, form_new, NULL);
	ewl_widget_show(item);

	item = ewl_menu_item_new(NULL, "Open...");
	ewl_container_child_append(EWL_CONTAINER(project_menu), item);
	ewl_callback_append(item, EWL_CALLBACK_CLICKED, open, NULL);
	ewl_widget_show(item);

	item = ewl_menu_separator_new();
	ewl_container_child_append(EWL_CONTAINER(project_menu), item);
	ewl_widget_show(item);

	item = ewl_menu_item_new(NULL, "Save");
	ewl_callback_append(item, EWL_CALLBACK_CLICKED, save, NULL);
	ewl_container_child_append(EWL_CONTAINER(project_menu), item);
	ewl_widget_show(item);

	item = ewl_menu_separator_new();
	ewl_container_child_append(EWL_CONTAINER(project_menu), item);
	ewl_widget_show(item);

	item = ewl_menu_item_new(NULL, "Exit");
	ewl_callback_append(item, EWL_CALLBACK_CLICKED,
											project_exit, NULL);
	ewl_container_child_append(EWL_CONTAINER(project_menu), item);
	ewl_widget_show(item);

	ewl_widget_show(project_menu);

	settings_menu = ewl_menubar_menu_add(EWL_MENUBAR(menu), NULL, "Settings");

	item = ewl_menu_item_new(NULL, "Options...");
	ewl_container_child_append(EWL_CONTAINER(settings_menu), item);
	ewl_callback_append(item, EWL_CALLBACK_CLICKED,
											options, NULL);
	ewl_widget_show(item);

	ewl_widget_show(settings_menu);

	windows_menu = ewl_menubar_menu_add(EWL_MENUBAR(menu), NULL, "Windows");

	item = ewl_menu_item_new(NULL, "Toolbar");
	ewl_container_child_append(EWL_CONTAINER(windows_menu), item);
	ewl_callback_append(item, EWL_CALLBACK_CLICKED, tools_toggle, NULL);
	ewl_widget_show(item);

	item = ewl_menu_item_new(NULL, "Inspector");
	ewl_container_child_append(EWL_CONTAINER(windows_menu), item);
	ewl_callback_append(item, EWL_CALLBACK_CLICKED, inspector_toggle, NULL);
	ewl_widget_show(item);

	item = ewl_menu_item_new(NULL, "Callbacks");
	ewl_container_child_append(EWL_CONTAINER(windows_menu), item);
	ewl_callback_append(item, EWL_CALLBACK_CLICKED, callbacks_toggle, NULL);
	ewl_widget_show(item);

	ewl_widget_show(windows_menu);

	ewl_widget_show(menu);

	file_tree = ewl_tree_new(1);
	ewl_tree_mode_set(EWL_TREE(file_tree), EWL_TREE_MODE_SINGLE);
	ewl_container_child_append(EWL_CONTAINER(vbox), file_tree);
	ewl_widget_show(file_tree);

	ewl_widget_show(vbox);
	ewl_widget_show(project_win);
	/* end ui phase */

	/* project phase */
	active_project = NEW(Ewler_Project);
	if( !active_project ) {
		ewler_error("projects_init: out of memory!");
		return -1;
	}
	active_project->files = ecore_hash_new(ecore_str_hash, ecore_str_compare);
	ecore_hash_free_key_cb_set(active_project->files, free);
	ecore_hash_free_value_cb_set(active_project->files, free);

	if( !filename || project_open(filename) < 0 ) {
		project_new();
	}
	/* end project phase */

	return 0;
}

/**
 * update the view of this project based on its contents
 */
void
project_update( void )
{
	Ewl_Widget *w;
	Ecore_List *names;
	Ecore_Sheap *snames;
	int i, nodes;
	char *name;

	ewl_tree_selected_clear(EWL_TREE(file_tree));
	ewl_container_reset(EWL_CONTAINER(file_tree));

	if( !active_project->files->nodes )
		return;

	names = ecore_hash_keys(active_project->files);
	nodes = ecore_list_count(names);
	snames = ecore_sheap_new(ecore_str_compare, nodes);

	while( (name = ecore_list_remove(names)) )
		ecore_sheap_insert(snames, name);

	ecore_sheap_sort(snames);

	for( i=0;i<nodes;i++ ) {
		Ewl_Widget *row;

		name = ecore_sheap_item(snames, i);

		w = ewl_text_new(name);
		row = ewl_tree_row_add(EWL_TREE(file_tree), NULL, &w);
		ewl_callback_append(row, EWL_CALLBACK_DOUBLE_CLICKED,
												form_open, name);
		ewl_callback_append(row, EWL_CALLBACK_CLICKED,
												project_file_menu, name);
		ewl_widget_show(w);
	}

	ecore_sheap_destroy(snames);
	ecore_list_destroy(names);
}
