#include "erss.h"
#include "parse.h"

Ewd_List *list = NULL;
Article *item = NULL;
Config *cfg = NULL;
Rc_Config *rc = NULL;

char *get_element (char **buffer, char *type)
{
	char tmp_char;
	char *c;
	char *start_tmp;
	char *end_tmp;
	char *ret_val = NULL;
	int size;

	if (!buffer || !*buffer || !**buffer || !type)
		goto err_clean_none;

	/* Allocate string plus enough to add in "</>". */
	size = strlen (type) + 4;
	c = malloc (size);
	if (!c)
		goto err_clean_none;

	/* Locate the opening tag of the type we're looking for. */
	snprintf (c, size, "<%s", type);
	start_tmp = strstr (*buffer, c);
	if (!start_tmp)
		goto err_clean_c;

	/* Move to the end of the found opening tag. */
	start_tmp += size - 2;

	/* Locate the closing tag of the specified type. */
	snprintf (c, size, "</%s>", type);
	end_tmp = strstr (start_tmp, c);
	if (!end_tmp)
		goto err_clean_c;

	/* Copy the data between the tags into a newly allocated buffer. */
	tmp_char = *end_tmp;
	*end_tmp = '\0';
	ret_val = strdup (start_tmp);
	*end_tmp = tmp_char;
	if (!ret_val)
		goto err_clean_c;

	/* We were incrementing the buffer past the found tags, may want to do
	 * this, but we must have a writable string passed in. */
	/* *buffer = end_tmp + size - 1; */

  err_clean_c:
	free (c);
  err_clean_none:
	return ret_val;
}

int get_new_story (char *buffer)
{
	char c[1024];
	char *ptr;

	/*
	 * First check for <item> cases
	 */
	snprintf (c, sizeof (c), "<%s>", cfg->item_start);

	ptr = strstr (buffer, c);

	if (ptr)
		return TRUE;

	/* 
	 * Second check for <item ....> cases.
	 */
	snprintf (c, sizeof (c), "<%s ", cfg->item_start);
	ptr = strstr (buffer, c);

	if (ptr)
		return TRUE;

	/* 
	 * The item didn't match.
	 */
	return FALSE;
}

int get_end_story (char *buffer)
{
	char c[1024];
	char *ptr;

	snprintf (c, sizeof (c), "</%s>", cfg->item_start);

	ptr = strstr (buffer, c);

	if (ptr)
		return TRUE;
	else
		return FALSE;
}

void remove_garbage (char *c, char *garbage)
{
	char *str;
	char *tmp;

	if (!garbage)
		return;

	str = strstr (c, garbage);

	if (str) {
		tmp = strdup(str + strlen(garbage));
		strcpy (c, tmp);
		free (tmp);
	}
}

void parse_data (char *buf)
{
	char *c;
	char *text;
	char *string;
	int i;
	
	/* 
	 * Some of the feeds may have "garbage" in their titles
	 * we want to remove it.
	 */
	char *garbage [2] = {
		"<![CDATA[",
		"]]>"
	};
	
	if (!buf || !*buf)
		return;

	/*
	printf ("data: %s\n", buf);
	*/

	if (ewd_list_nodes (list) >= cfg->num_stories)
		return;

	if (get_new_story (buf))
	{
		/* 
		 * We have a new story, allocate an item for it
		 */
		item = malloc (sizeof (Article));
		memset(item, 0, sizeof(Article));

		return;
	}

	if (get_end_story (buf))
	{
		ewd_list_append (list, item);
		return;
	}

	/* If the item is not allocated then we dont have a 
	 * real story. So return with an error
	 */
	if (!item)
		return;

	if ((c = get_element (&buf, cfg->item_title)) != NULL)
	{
		for (i = 0; i < 2; i++)
			remove_garbage (c, garbage[i]);
		
		string = ecore_txt_convert (rc->enc_to, rc->enc_from, c);
		
		i = strlen(string) + 3 + strlen(cfg->prefix);
		text = malloc (i);
		
		snprintf (text, i, " %s %s", cfg->prefix, string);
		
		item->obj = edje_object_add (evas);
		edje_object_file_set (item->obj, cfg->theme, "erss_item");
		evas_object_show (item->obj);

		evas_object_event_callback_add (item->obj,
				EVAS_CALLBACK_MOUSE_IN, cb_mouse_in, NULL);
		evas_object_event_callback_add (item->obj,
				EVAS_CALLBACK_MOUSE_OUT, cb_mouse_out, NULL);
		
		e_container_element_append(cont, item->obj);
		edje_object_part_text_set (item->obj, "article", text);

		free (string);
		free (text);
		free (c);

		return; 
	}
	
	if ((c = get_element (&buf, cfg->item_url)) != NULL)
	{
		item->url = strdup (c);

		edje_object_signal_callback_add (item->obj, "exec*", "*", 
				cb_mouse_out_item, item);
		edje_object_signal_emit (item->obj, "mouse,in", "article");
		edje_object_signal_emit (item->obj, "mouse,out", "article");

		free (c);
		return;
	}
	
	if ((c = get_element (&buf, cfg->item_description)) != NULL)
	{
		item->description = strdup (c);
		
		free (c);
		return;
	}
	
}

char *get_next_line (FILE * fp)
{
	int index = 0;
	int bufsize = 512;
	signed char temp;
	char *buf = NULL;

	buf = malloc (bufsize);

	while ((temp = fgetc (fp)) != '\n')
	{
		if (feof (fp))
			return NULL;

		buf[index++] = temp;

		/* Check size after incrimenting to eliminate extra check
		 * outside of loop */
		if (index == bufsize)
		{
			bufsize += 512;
			buf = realloc (buf, bufsize);
		}
	}

	buf[index] = '\0';
	index = strlen(buf) + 1;
	if (bufsize > index)
		buf = realloc (buf, index);

	return buf;
}

int parse_rc_file ()
{
	FILE *fp;
	char *line;
	char *c;
	char file[PATH_MAX];

	snprintf (file, PATH_MAX, "%s/.erssrc", getenv ("HOME"));

	rc = malloc (sizeof (Rc_Config));
	if (!rc)
		return FALSE;

	memset(rc, 0, sizeof(Rc_Config));
	
	fp = fopen (file, "r");

	while (fp && (line = get_next_line (fp)) != NULL)
	{
		if ((c = get_element (&line, "config")) != NULL)
			rc->config = strdup (c);
		else if ((c = get_element (&line, "theme")) != NULL)
			rc->theme = strdup (c);
		else if ((c = get_element (&line, "browser")) != NULL)
			rc->browser = strdup (c);
		else if ((c = get_element (&line, "proxy")) != NULL)
			rc->proxy = strdup (c);
		else if ((c = get_element (&line, "enc_from")) != NULL)
			rc->enc_from = strdup (c);
		else if ((c = get_element (&line, "enc_to")) != NULL)
			rc->enc_to = strdup (c);
		else if ((c = get_element (&line, "proxy_port")) != NULL)
			rc->proxy_port = atoi (c);

		if (c)
			free (c);

		free(line);
	}


	/*
	 * Set sane defaults for unspecified config options.
	 */
	if (!rc->theme)
		rc->theme = strdup(PACKAGE_DATA_DIR "/default.eet");
	if (!rc->config)
		rc->config = strdup(PACKAGE_DATA_DIR "/config/slashdot.cfg");

	if (!rc->browser && getenv("BROWSER"))
		rc->browser = strdup(getenv("BROWSER"));
	if (!rc->browser)
		rc->browser = strdup("mozilla");

	if (!rc->enc_from)
		rc->enc_from = strdup("utf8");
	if (!rc->enc_to)
		rc->enc_to = strdup("iso-8859-1");
	
	/* 
	 * If there is no rc file return false for us to know
	 */
	if (!fp)
		return FALSE;
	
	fclose (fp);

	return TRUE;
}

void parse_config_file (char *file)
{
	FILE *fp;
	char *line;
	char *c;
	char *str;
	int match = FALSE;

	if ((fp = fopen (file, "r")) == NULL) {
		list_config_files (FALSE);

		str = ewd_list_goto_first (config_files);
		while ((str = ewd_list_current (config_files))) {
			if (strstr (str, file))  {
				
				if ((fp = fopen (str, "r")) == NULL) {
					fprintf (stderr, "%s error: Can't open config file %s\n",
							PACKAGE, str);
					exit (-1);
				}

				match = TRUE;
				break;
			} 
			
			ewd_list_next (config_files);
		}

		if (!match) {
			fprintf (stderr, "%s error: No match for %s\n", PACKAGE, file);
			exit (-1);
		} else {
			printf ("%s info: your string '%s' matches %s\n", PACKAGE, file, str);
			printf ("%s info: using %s as config file\n", PACKAGE, str);
		}
			
	}

	cfg = malloc (sizeof (Config));
	memset(cfg, 0, sizeof(Config));

	while ((line = get_next_line (fp)) != NULL)
	{
		if ((c = get_element (&line, "header")) != NULL)
			cfg->header = strdup (c);
		else if ((c = get_element (&line, "hostname")) != NULL)
			cfg->hostname = strdup (c);
		else if ((c = get_element (&line, "url")) != NULL)
			cfg->url = strdup (c);
		else if ((c = get_element (&line, "item_start")) != NULL)
			cfg->item_start = strdup (c);
		else if ((c = get_element (&line, "item_title")) != NULL) 
			cfg->item_title = strdup (c);
		else if ((c = get_element (&line, "item_url")) != NULL)
			cfg->item_url = strdup (c);
		else if ((c = get_element (&line, "item_description")) != NULL)
			cfg->item_description = strdup (c);
		else if ((c = get_element (&line, "prefix")) != NULL)
			cfg->prefix = strdup (c);
		else if ((c = get_element (&line, "update_rate")) != NULL)
			cfg->update_rate = atoi (c);
		else if ((c = get_element (&line, "clock")) != NULL)
		{
			cfg->clock = atoi (c);

			if (cfg->clock != 1 && cfg->clock != 0)
			{
				fprintf (stderr,
						"%s error: Clock option has wrong value - check your config file!\n", PACKAGE);
				exit (-1);
			}
		} else if ((c = get_element (&line, "stories")) != NULL)
		{
			cfg->num_stories = atoi (c);

			if (cfg->num_stories > 10)
			{
				fprintf (stderr,
						 "%s error: Max stories to show is 10 - check your config file!\n", PACKAGE);
				exit (-1);
			}
		} else if ((c = get_element (&line, "borderless")) != NULL)
		{
			cfg->borderless = atoi (c);

			if (cfg->borderless != 1 && cfg->borderless != 0)
			{
				fprintf (stderr,
						 "%s error:: Borderless option has wrong value - check your config file!\n", PACKAGE);
				exit (-1);
			}
		} else if ((c = get_element (&line, "x")) != NULL)
			cfg->x = atoi (c);
		else if ((c = get_element (&line, "y")) != NULL)
			cfg->y = atoi (c);

		if (c)
			free (c);
		
		free (line);
	}

	fclose (fp);

	if (!cfg->prefix)
		cfg->prefix = strdup(" . ");

}
