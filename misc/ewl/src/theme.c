#include "theme.h"

typedef struct _EwlThemeFindClass EwlThemeFindClass;
struct _EwlThemeFindClass	{
	char *widget;
	char *dbpath;
};

EwlBool _cb_ewl_theme_find_db(EwlLL *node, EwlData *data)
{
	EwlBool      r = FALSE;
	struct stat  stats;
	EwlThemeFindClass *find_data = (EwlThemeFindClass*) data;
	char        *app = ewl_get_application_name(),
	            *theme = ewl_get_theme(),
	            *path  = (char*) (node->data),
	            *db = (char*) find_data->widget,
	            *buf = NULL;
	int          len = (path?strlen(path):0) + (theme?strlen(theme):0) +
	                   (app?strlen(app):0) + (db?strlen(db):0) + 16;
	if (ewl_debug_is_active())
		fprintf(stderr,"path = %s\naloocating buf of size %d for filename\n",
		        (char*) path, len);
	buf = malloc(len);
	if (!buf) {
		if (theme) free(theme);
		return FALSE;
	}
	buf[0] = 0;
	if (theme)	{
		if (app)	{
			snprintf(buf,len,"%s/%s/%s-%s.db", path, theme, app, db);
			if (ewl_debug_is_active())
				fprintf(stderr, "0. looking for %s\n", buf);
			if (stat(buf,&stats)!=-1)	{
				if (ewl_debug_is_active())
					fprintf(stderr,"0. found it\n");
				find_data->dbpath = e_string_dup(buf);
				r = TRUE;
			}
		} 

		if (!r)		{
			snprintf(buf,len,"%s/%s/%s.db", path, theme, db);
			if (ewl_debug_is_active())
				fprintf(stderr, "1. looking for %s\n", buf);
			if (stat(buf,&stats)!=-1)	{
				if (ewl_debug_is_active())
					fprintf(stderr,"1. found it\n");
				find_data->dbpath = e_string_dup(buf);
				r = TRUE;
			}
		}
	}

	if (!r&&app)	{
		snprintf(buf,len,"%s/%s-%s.db", path, app, db);
		if (ewl_debug_is_active())
			fprintf(stderr, "2. looking for %s\n", buf);
		if (stat(buf,&stats)!=-1)	{
			if (ewl_debug_is_active())
				fprintf(stderr,"2. found it\n");
			find_data->dbpath = e_string_dup(buf);
			r = TRUE;
		}
	}

	if (!r)	{
		snprintf(buf,len,"%s/%s.db", path, db);
		if (ewl_debug_is_active())
			fprintf(stderr, "3. looking for %s\n", buf);
		if (stat(buf,&stats)!=-1)	{
			if (ewl_debug_is_active())
				fprintf(stderr,"3. found it\n");
			find_data->dbpath = e_string_dup(buf);
			r = TRUE;
		}
	}

	if (buf)   free(buf);
	if (app)   free(app);
	if (theme) free(theme);
	return r;
}

EwlBool _cb_test(EwlLL *node, EwlData *data)
{
	fprintf(stderr,"%s\n", (char*)(node->data));
	return FALSE;
}

char         *ewl_theme_find_db(char *name)
{
	EwlLL *pl = ewl_get_path_list(),
	      *fl = NULL;
	char  *path  = NULL;
	EwlThemeFindClass find_data;
	FUNC_BGN("ewl_theme_find_db");
	if (!pl)	{
		ewl_debug("ewl_theme_find_db", EWL_NULL_ERROR, "pl");
	} else if (!name)	{
		ewl_debug("ewl_theme_find_db", EWL_NULL_ERROR, "name");
	} else {
		find_data.dbpath = NULL;
		find_data.widget = e_string_dup(name);
		fl = ewl_ll_callback_find(pl, _cb_ewl_theme_find_db,
		                          (EwlData*) &find_data);
		if (fl)	{
			path = find_data.dbpath;
		} else {
			/* couldn't find the theme */
			fprintf(stderr,"ERROR: Couldn't locate theme for \"%s\".\n"
			        "Please check your theme path and permissions.\n",
			        find_data.widget);
		}
	}
	FUNC_END("ewl_theme_find_db");
	return path;
}

EwlBool _cb_ewl_theme_find_file(EwlLL *node, EwlData *data)
{
	EwlBool      r = FALSE;
	struct stat  stats;
	EwlThemeFindClass *find_data = (EwlThemeFindClass*) data;
	char        *theme = ewl_get_theme(),
	            *path  = (char*) (node->data),
	            *file = (char*) find_data->widget,
	            *buf = NULL;
	int          len = (path?strlen(path):0) + (theme?strlen(theme):0) +
	                   (file?strlen(file):0) + 16;
	if (ewl_debug_is_active())
		fprintf(stderr,"path = %s\naloocating buf of size %d for filename\n",
		        (char*) path, len);
	buf = malloc(len); 
	if (!buf) {
		if (theme) free(theme);
		return FALSE;
	}
	buf[0] = 0;
	if (theme)	{
		snprintf(buf,len,"%s/%s/%s", path, theme, file);
		if (ewl_debug_is_active())
			fprintf(stderr, "_cb_find_file: 0. looking for %s\n", buf);
		if (stat(buf,&stats)!=-1)	{
			if (ewl_debug_is_active())
				fprintf(stderr,"_cb_find_file: 0. found it\n");
			find_data->dbpath = e_string_dup(buf);
			r = TRUE;
		}
	}

	if (!r)	{
		snprintf(buf,len,"%s/%s", path, file);
		if (ewl_debug_is_active())
			fprintf(stderr, "_cb_find_file: 1. looking for %s\n", buf);
		if (stat(buf,&stats)!=-1)	{
			if (ewl_debug_is_active())
				fprintf(stderr,"_cb_find_file: 1. found it\n");
			find_data->dbpath = e_string_dup(buf);
			r = TRUE;
		}
	}

	if (buf)   free(buf);
	if (theme) free(theme);
	return r;
}

char         *ewl_theme_find_file(char *name)
{
	EwlLL *pl = ewl_get_path_list(),
	      *fl = NULL;
	char  *path  = NULL;
	EwlThemeFindClass find_data;
	FUNC_BGN("ewl_theme_find_db");
	if (!pl)	{
		ewl_debug("ewl_theme_find_db", EWL_NULL_ERROR, "pl");
	} else if (!name)	{
		ewl_debug("ewl_theme_find_db", EWL_NULL_ERROR, "name");
	} else {
		find_data.dbpath = NULL;
		find_data.widget = e_string_dup(name);
		fl = ewl_ll_callback_find(pl, _cb_ewl_theme_find_file,
		                          (EwlData*) &find_data);
		if (fl)	{
			path = find_data.dbpath;
		} else {
			/* couldn't find the theme */
			fprintf(stderr,"ERROR: Couldn't locate file \"%s\".\n"
			        "Please check your theme path and permissions.\n",
			        find_data.widget);
		}
	}
	FUNC_END("ewl_theme_find_db");
	return path;
}


char         *ewl_theme_get_string(char *key)
{
	EwlDB *db   = NULL;
	char  *val  = NULL,
	      *tmp  = NULL,
	      *wid  = NULL,
	      *tkey = NULL,
	      *path = NULL;
	int    len  = 0;
	FUNC_BGN("ewl_theme_get_string");
	if (!key)	{
		ewl_debug("ewl_theme_get_string", EWL_NULL_ERROR, "key");
	} else {
		if (key[0]=='/')	{
			tmp = e_string_dup(key+1);
		} else {
			tmp = e_string_dup(key);
		}
		wid = strtok(tmp,"/");
		tkey = strtok(NULL, "");
		if ((path = ewl_theme_find_db(wid))==NULL)	{
			ewl_debug("ewl_theme_get_string", EWL_NULL_ERROR, "path");
		} else {
			/* THIS IS WRONG, WE NEED TO CACHE THE DATABSES --
			   I'M ONLY DOING THIS FOR TESTING */
			db = ewl_db_open(path);
			if (!db)	{
				ewl_debug("ewl_theme_get_string", EWL_NULL_ERROR, "db");
			} else {
				val = ewl_db_get(db,tkey, &len);
				ewl_db_close(db);
			}
		}
		if (wid)  free(wid);
	}
	FUNC_BGN("ewl_theme_get_string");
	return val;
}

EwlBool       ewl_theme_get_int(char *key, int *val)
{
	EwlBool  r = FALSE;
	char    *temp = NULL;
	FUNC_BGN("ewl_theme_get_int");
	if (!key) {
		ewl_debug("ewl_theme_get_int", EWL_NULL_ERROR, "key");
	} else if (!val) {
		ewl_debug("ewl_theme_get_int", EWL_NULL_ERROR, "val");
	} else {
		temp = ewl_theme_get_string(key);
		if (!temp) {
			ewl_debug("ewl_theme_get_int", EWL_NULL_ERROR, "temp");
		} else {
			*val = atoi(temp);
			r = TRUE;
		}
	}
	FUNC_END("ewl_theme_get_int");
	return r;
}

EwlBool       ewl_theme_get_float(char *key, float *val)
{
	EwlBool  r = FALSE;
	char    *temp = NULL;
	FUNC_BGN("ewl_theme_get_float");
	if (!key) {
		ewl_debug("ewl_theme_get_float", EWL_NULL_ERROR, "key");
	} else if (!val) {
		ewl_debug("ewl_theme_get_float", EWL_NULL_ERROR, "val");
	} else {
		temp = ewl_theme_get_string(key);
		if (!temp) {
			ewl_debug("ewl_theme_get_float", EWL_NULL_ERROR, "temp");
		} else {
			*val = atof(temp);
			r = TRUE;
		}
	}
	FUNC_END("ewl_theme_get_float");
	return r;
}


EwlImage     *ewl_theme_get_image(char *key)
{
	EwlImage *im        = NULL;
	EwlBool   visible   = FALSE;
	char     *path      = NULL,
	          buf[1024] = "",
	         *temp      = NULL;
	FUNC_BGN("ewl_theme_get_image");
	if (!key)	{
		ewl_debug("ewl_theme_get_image", EWL_NULL_ERROR, "key");
	} else {
		snprintf(buf,1023,"%s", ewl_theme_get_string(key));
		path = ewl_theme_find_file(buf);

		if (!path)	{
			ewl_debug("ewl_theme_get_image", EWL_NULL_ERROR, "path");
			if (ewl_debug_is_active())
				fprintf(stderr,"DEBUG: Couldn't find image associated with the following key:\n\t\"%s\".\nTrying the follwing key:\n\t\"%s/path\".\n",
				        key, key);
			
			snprintf(buf,1023,"%s/path", ewl_theme_get_string(key));
			path = ewl_theme_find_file(buf);
		}

		if (!path)	{
			ewl_debug("ewl_theme_get_image", EWL_NULL_ERROR, "path");
			fprintf(stderr, "ERROR: Couldn't find image associated with the following key:\n\t\"%s\".\nPlease check your theme search path.\n", key);
		} else {
			im = ewl_image_load(path);
			if (!im)	{
				ewl_debug("ewl_theme_get_image", EWL_NULL_ERROR, "im");
				fprintf(stderr, "ERROR: Couldn't load the following image:\n\t\"%s\".\nPlease check your permissions and your theme search path.\n", path);
			} else {
				fprintf(stderr,"ewl_theme_get_imlayer(): "
				        "Image \"%s\" loaded okay.\n", path);
				snprintf(buf, 1023, "%s/visible", key);
				temp = ewl_theme_get_string(buf);
				visible = e_check_bool_string(temp);
				if (visible) ewl_image_show(im);
			}
		}
	}
	FUNC_END("ewl_theme_get_image");
	return im;
}

EwlImLayer   *ewl_theme_get_imlayer(char *key)
{
	EwlImLayer *imlay   = NULL;
	EwlBool     alpha   = FALSE,
	            visible = FALSE,
	            gw      = FALSE,
	            gh      = FALSE,
	            gn      = FALSE;
	char       *buf     = NULL,
	           *temp    = NULL,
	           *name    = NULL;
	int         i       = 0,
	            num     = 0,
	            w, h;
	FUNC_BGN("ewl_theme_get_imlayer");
	if (!key)	{
		ewl_debug("ewl_theme_get_imlayer", EWL_NULL_ERROR, "key");
	} else {
		buf = malloc(strlen(key)+16+1);
		if (!buf)	{
			ewl_debug("ewl_theme_get_imlayer", EWL_NULL_ERROR, "buf");
		} else {
			sprintf(buf, "%s/name", key);
			name = ewl_theme_get_string(buf);
			sprintf(buf, "%s/width", key);
			gw = ewl_theme_get_int(buf, &w);
			sprintf(buf, "%s/height", key); 
			gh = ewl_theme_get_int(buf, &h);
			sprintf(buf,"%s/num_images", key);
			gn = ewl_theme_get_int(buf, &num);

			fprintf(stderr,"%s/width = %d\n%s/num_images = %d\n", key, w, key, num);

			sprintf(buf, "%s/alpha", key);  
			temp = ewl_theme_get_string(buf);
			alpha = e_check_bool_string(temp);
			if (temp) free(temp);

			sprintf(buf, "%s/visible", key);
			temp = ewl_theme_get_string(buf);
			visible = e_check_bool_string(temp);
			if (temp) free(temp);

			imlay = ewl_imlayer_new_with_values(name,
			                                    (gw)?(&w):0, (gh)?(&h):0,
			                                    FALSE, alpha);
			if (!imlay)	{
				ewl_debug("ewl_theme_get_imlayer", EWL_NULL_ERROR, "imlay");
			} else {
				if (visible) ewl_imlayer_show(imlay);
				if (ewl_debug_is_active())
					fprintf(stderr,"ewl_theme_get_imlayer(): "
					        "Image Layer \"%s\" loaded okay.\n",
					        (name)?name:"Untitled ImLayer");
				if (!gn)	{
					if (ewl_debug_is_active())
						fprintf(stderr,"ewl_theme_get_imlayer(): "
						        "No Images to load for ImLayer \"%s\".\n",
						        (name)?name:"Untitled ImLayer");
				} else {
					if (ewl_debug_is_active())
						fprintf(stderr,"ewl_widget_get_theme(): "
						        "Loading %d Images for ImLayer \"%s\".\n",
						        num, (name)?name:"Untitled ImLayer");
					for (i=0; i<num; i++)	{
						sprintf(buf,"%s/image-%02d", key, i);
						ewl_imlayer_image_insert(imlay,
						                         ewl_theme_get_image(buf));
					}
				}
			}
			free(buf);
		}
	}
	FUNC_END("ewl_theme_get_imlayer");
	return imlay;
}


void          ewl_widget_get_theme(EwlWidget *wid, char *key)
{
	EwlBool   gt = FALSE;
	char     *buf = NULL,
	         *tmp = NULL;
	int       i = 0,
	          t = 0;
	FUNC_BGN("ewl_widget_get_theme");
	if (!wid) {
		ewl_debug("ewl_widget_get_theme", EWL_NULL_WIDGET_ERROR, "wid");
	} else if (!key) {
		ewl_debug("ewl_widget_get_theme", EWL_NULL_ERROR, "key");
	} else {
		buf = malloc(strlen(key)+65);
		if (!buf) {
			ewl_debug("ewl_widget_get_theme", EWL_NULL_ERROR, "buf");
		} else {
			sprintf(buf,"%s/name", key);
			if (key) wid->name = ewl_theme_get_string(key);

			sprintf(buf,"%s/padding/left", key); 
			gt = ewl_theme_get_int(buf,&t);
			if (gt) ewl_widget_set_padding(wid,&t,0,0,0);
			sprintf(buf,"%s/padding/top", key); 
			gt = ewl_theme_get_int(buf,&t);
			if (gt) ewl_widget_set_padding(wid,0,&t,0,0);
			sprintf(buf,"%s/padding/right", key); 
			gt = ewl_theme_get_int(buf,&t);
			if (gt) ewl_widget_set_padding(wid,0,0,&t,0);
			sprintf(buf,"%s/padding/bottom", key); 
			gt = ewl_theme_get_int(buf,&t);
			if (gt) ewl_widget_set_padding(wid,0,0,0,&t);
			
			sprintf(buf,"%s/background", key);
			tmp = ewl_theme_find_file(ewl_theme_get_string(buf));
			if (tmp)	{
				/*ewl_widget_set_background(wid,ewl_imlib_load_image(tmp));*/
				ewl_widget_set_background(wid,
				    evas_add_image_from_file(ewl_widget_get_evas(wid), tmp));
			}

			/*sprintf(buf,"%s/num_layers", key); 
			gt = ewl_theme_get_int(buf,&t);
			if (!gt) {
				/ no layers /
					if (ewl_debug_is_active())
						fprintf(stderr,"ewl_widget_get_themer(): "
						        "No Image Layers to load.\n");
			} else {
				if (ewl_debug_is_active())
					fprintf(stderr,"ewl_widget_get_theme(): "
					        "Attempting to load %d Image Layers.\n", t);
				for (i=0; i<t; i++)	{
					sprintf(buf,"%s/layer-%02d", key, i);
					ewl_widget_imlayer_insert(wid,
					                          ewl_theme_get_imlayer(buf));
				}
			}*/
			free(buf);
		}
	}
	FUNC_END("ewl_widget_get_theme");
	return;
}

