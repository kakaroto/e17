#include "dEvian.h"

static void _devian_source_del(DEVIANN *devian);

/* Public functions */

/**
 * Change de source of a dEvian
 *
 * @param devian The dEvian
 * @return 0 on success, 1 on fail
 */
int DEVIANF(source_change)(DEVIANN *devian, int source)
{
  if (devian->container_func.is_in_transition(devian))
    {
      DMAIN(("--- Wanna source change, but already in transition ---"));
      return 1;
    }

  /* CHECK: if already one source, remove it, except if its the one we want */
  if (devian->source)
    {
      if (devian->conf->source_type == source)
	return 1;

      _devian_source_del(devian);
      
      if (devian->dialog_conf)
	DEVIANF(config_dialog_devian_shutdown)(devian);
    }

  /* Add the new container to the devian */
  switch (source)
    {

#ifdef HAVE_PICTURE
    case SOURCE_PICTURE:
      {
	if (!DEVIANM->source_picture_count)
	  {
	    /* Init pictures subsystem */
	    if ( DEVIANF(data_picture_list_local_init)() )
	      {
		fprintf(stderr, MODULE_NAME ": can't init picture list (local) !\n");
		return 1;
	      }
	    if ( !DEVIANF(data_picture_cache_init)() )
	      {
		fprintf(stderr, "can't init picture cache !\n");
		return 1;
	      }
	  }
	/* Add the picture */
	if ( DEVIANF(source_picture_add)(devian) )
	  return 1;
	/* Chech good theme */
	DEVIANF(container_theme_check)(devian, &DEVIANM->conf->theme_picture);
	break;
      }
#endif

#ifdef HAVE_RSS
    case SOURCE_RSS:
      {
	/* Add the rss */
	if ( !DEVIANF(source_rss_add)(devian) )
	  return 1;
	/* Chech good theme */
	DEVIANF(container_theme_check)(devian, &DEVIANM->conf->theme_rss);
	break;
      }
#endif

#ifdef HAVE_FILE
    case SOURCE_FILE:
      {
	/* Add the file */
	if ( !DEVIANF(source_file_add)(devian) )
	  return 1;
	/* Chech good theme */
	DEVIANF(container_theme_check)(devian, &DEVIANM->conf->theme_file);
	break;
      }
#endif

    case SOURCE_NO:
      {
	/* I want no source attached, thank you */
	return 0;
      }

    default : return 1;
    }

  DEVIANF(menu_refresh)(devian);
  
  return 0;
}

void DEVIANF(source_detach)(DEVIANN *devian, int part)
{
  switch (devian->conf->source_type)
    {
#ifdef HAVE_PICTURE
    case SOURCE_PICTURE:
      DEVIANF(data_picture_cache_detach)(devian->source, part);
      break;
#endif
#ifdef HAVE_RSS
    case SOURCE_RSS:
      DEVIANF(data_rss_prev_detach)(devian->source, part);
      break;
#endif
#ifdef HAVE_FILE
    case SOURCE_FILE:
      break;
#endif
    }
  return;
}

/*
  Get the evas objects of a source
  Return:
    - 1 on success
    - 0 on fail
*/
int DEVIANF(source_evas_object_get)(DEVIANN *devian,
				    Evas_Object **obj0,
				    Evas_Object **obj1)
{
  switch (devian->conf->source_type)
    {
#ifdef HAVE_PICTURE
    case SOURCE_PICTURE:
      return DEVIANF(source_picture_evas_object_get)(devian, obj0, obj1);
#endif
#ifdef HAVE_RSS
    case SOURCE_RSS:
      return DEVIANF(source_rss_evas_object_get)(devian, obj0, obj1);
#endif
#ifdef HAVE_FILE
    case SOURCE_FILE:
      return DEVIANF(source_file_evas_object_get)(devian, obj0, obj1);
#endif
    case SOURCE_NO:
	break;
    }

  return 0;
}

int DEVIANF(source_original_size_get)(DEVIANN *devian,
				      int part, int *w, int *h)
{
  switch (devian->conf->source_type)
    {
#ifdef HAVE_PICTURE
    case SOURCE_PICTURE:
      return DEVIANF(source_picture_original_size_get)(devian->source,
						       part, w, h);
#endif
#ifdef HAVE_RSS
    case SOURCE_RSS:
      return DEVIANF(source_rss_original_size_get)(devian->source,
						   part, w, h);
#endif
#ifdef HAVE_FILE
    case SOURCE_FILE:
      return DEVIANF(source_file_original_size_get)(devian->source,
						    w, h);
	break;
#endif
    case SOURCE_NO:
	break;
    }

  return 0;
}

void DEVIANF(source_dialog_infos)(DEVIANN *devian)
{
  switch (devian->conf->source_type)
    {
#ifdef HAVE_PICTURE
    case SOURCE_PICTURE:
      DEVIANF(source_picture_dialog_infos)(devian->source);
      break;
#endif
#ifdef HAVE_RSS
    case SOURCE_RSS:
      DEVIANF(source_rss_dialog_infos)(devian->source);
      break;
#endif
#ifdef HAVE_FILE
    case SOURCE_FILE:
      DEVIANF(source_file_dialog_infos)(devian->source);
      break;
#endif
    }
  
  return;
}

char *DEVIANF(source_name_get)(DEVIANN *devian, int part)
{
  switch (devian->conf->source_type)
    {
#ifdef HAVE_PICTURE
    case SOURCE_PICTURE:
      return DEVIANF(source_picture_name_get)(devian->source, part);
#endif
#ifdef HAVE_RSS
    case SOURCE_RSS:
      return DEVIANF(source_rss_name_get)(devian->source);
#endif
#ifdef HAVE_FILE
    case SOURCE_FILE:
      return DEVIANF(source_file_name_get)(devian->source);
      break;
#endif
    }
  return NULL;
}

char *DEVIANF(source_infos_get)(DEVIANN *devian, int part)
{
  switch (devian->conf->source_type)
    {
#ifdef HAVE_PICTURE
    case SOURCE_PICTURE:
      return DEVIANF(source_picture_infos_get)(devian->source, part);
#endif
#ifdef HAVE_RSS
    case SOURCE_RSS:
      return DEVIANF(source_rss_infos_get)(devian->source);
#endif
#ifdef HAVE_FILE
    case SOURCE_FILE:
      return DEVIANF(source_file_infos_get)(devian->source);
      break;
#endif
    }

  return NULL;
}

void DEVIANF(source_idle_timer_change)(DEVIANN *devian, int active, int time)
{
  return;
}
int DEVIANF(source_idle_refresh)(DEVIANN *devian, int option)
{
  return 0;
}
int DEVIANF(source_idle_set_bg)(DEVIANN *devian)
{
  return 0;
}
int DEVIANF(source_idle_viewer)(DEVIANN *devian)
{
  return 0;
}
int DEVIANF(source_idle_gui_update)(DEVIANN *devian)
{
  return 0;
}


/* Private functions */

static void _devian_source_del(DEVIANN *devian)
{
  switch (devian->conf->source_type)
    {
    case SOURCE_NO: break;
#ifdef HAVE_PICTURE
    case SOURCE_PICTURE:
      DEVIANF(source_picture_del)(devian->source);
      if (!DEVIANM->source_picture_count)
	{
	  /* Shutdown pictures subsystem */
	  DEVIANF(data_picture_cache_shutdown)();
	  DEVIANF(data_picture_list_local_shutdown)();
	}
      break;
#endif
#ifdef HAVE_RSS
    case SOURCE_RSS:
      DEVIANF(source_rss_del)(devian->source);
      break;
#endif
#ifdef HAVE_FILE
    case SOURCE_FILE:
      DEVIANF(source_file_del)(devian->source);
      break;
#endif
    }

  devian->source = NULL;
  devian->conf->source_type = SOURCE_NO;

  /* Actions */
  devian->source_func.timer_change = DEVIANF(source_idle_timer_change);
  devian->source_func.refresh = DEVIANF(source_idle_refresh);
  devian->source_func.set_bg = DEVIANF(source_idle_set_bg);
  devian->source_func.viewer = DEVIANF(source_idle_viewer);
  devian->source_func.gui_update = DEVIANF(source_idle_gui_update);
}
