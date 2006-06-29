#include "emphasis.h"

int
main (int argc, char **argv)
{
	Emphasis_Gui *gui;

	if (!etk_init(&argc, argv))
	{
		printf("Unable to init etk\n");
		return 1;
	}
	if (!eet_init())
	{
		printf("Unable to init eet\n");
		return 1;
	}
	if (!ecore_file_init())
	{
		printf("Unable to init ecore_file\n");
		return 3;
	}
	
	gui = malloc(sizeof(Emphasis_Gui));
	if (!gui)
	{
		printf("Memory full\n");
		exit(1);
	}
	
	emphasis_init_gui(gui);
	
	emphasis_init_connect(gui);
	emphasis_try_connect(gui);
	
	etk_main();

	return 0;
}

void
emphasis_init_connect(Emphasis_Gui *gui)
{
	gui->timer = ecore_timer_add(0.2, emphasis_try_connect, gui);
}

int
emphasis_try_connect(void *data)
{
	Emphasis_Gui *gui;
	Evas_List *artist_list;
	Emphasis_Config *config;
	Ecore_Timer *timer;
	
	gui = data;
	config = config_load();
	etk_window_resize(ETK_WINDOW(gui->window), config->geometry.w, config->geometry.h);
	etk_object_data_set(ETK_OBJECT(gui->window), "config", config);
	
	timer = mpc_init(config->hostname, config->port, config->password);
	
	if (!timer)
	{
		emphasis_clear(gui);
		return 1;
	}
	else
	{
		ecore_timer_del(gui->timer);
		gui->timer = timer;

		artist_list = mpc_mlib_artist_get();
		Emphasis_Data *data;
		data = evas_list_data(artist_list);
		emphasis_tree_mlib_set(ETK_TREE(gui->tree_artist), artist_list, MPD_DATA_TYPE_TAG);
		etk_tree_row_select(etk_tree_first_row_get(ETK_TREE(gui->tree_artist)));
	
		mpc_signal_connect_status_changed(gui);
		return 0;
	}
}
