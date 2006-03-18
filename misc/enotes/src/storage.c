
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


#include "storage.h"

/* Freeing */

/**
 * @param p: The notestor to free.
 * @brief: Free's a notestor typedef structure.
 */
void
free_note_stor(NoteStor * p)
{
	if (p != NULL) {
		if (p->content != NULL)
			free(p->content);
		free(p);
		p = NULL;
	}
	return;
}

/**
 * @return: The NoteStor allocated.
 * @brief: Allocates a new NoteStor variable.
 */
NoteStor       *
alloc_note_stor()
{
	NoteStor       *p = malloc(sizeof(NoteStor));

	p->content = NULL;
	return (p);
}

/* One Shot Functions. :-) */

/**
 * @param p: The information required (about the note we're saving).
 * @brief: Appends a new note to the note storage according to the
 *         information stored in p.
 */
int
append_note_stor(NoteStor * p)
{
	char           *target = malloc(PATH_MAX);
	char           *title;
	char           *string = get_value_from_notestor(p);
	FILE           *fp;
	int             retval = 0;

	title = get_title_by_content(p->content);
	sprintf(target, "%s/.e/apps/enotes/notes/%s", getenv("HOME"), title);
	free(title);

	if ((fp = fopen(target, "w")) != NULL) {
		fputs(string, fp);
		fclose(fp);
		retval = 1;
	}

	free(string);
	free(target);
	return (retval);
}

/**
 * @param p: The NoteStor containing the information required.
 * @brief: Removes the NoteStor corrosponding to the information
 *         inside p.
 */
void
remove_note_stor(NoteStor * p)
{
	char           *target = malloc(PATH_MAX);
	char           *title;

	title = get_title_by_content(p->content);
	sprintf(target, "%s/.e/apps/enotes/notes/%s", getenv("HOME"), title);
	free(title);

	unlink(target);

	free(target);
	return;
}

void
note_load(char *target)
{
	FILE           *fp;
	NoteStor       *p;
	char           *str = malloc(NOTE_LIMIT);
	char           *fullstr = malloc(NOTE_LIMIT * 2);
	Note           *note;

	p = NULL;
	fullstr[0] = '\0';
	if ((fp = fopen(target, "r")) != NULL) {
		while ((str = fgets(str, NOTE_LIMIT, fp)) != NULL) {
			sprintf(fullstr, "%s%s", fullstr, str);
		}
		if (strcmp("", fullstr))
			if ((p = get_notestor_from_value(fullstr)) != NULL) {
				note = new_note_with_values_return(p->x, p->y,
								   p->width,
								   p->height,
								   p->shaded,
								   p->content);
				edje_object_signal_emit(note->edje,
							NOTE_LOADED_SIGNAL, "");
			}
	}

	free(str);
	free(fullstr);
	if (p != NULL)
		free_note_stor(p);
	return;
}


void
process_note_storage_locations()
{
	DIR            *p;
	char           *f = malloc(PATH_MAX);

	sprintf(f, "%s/.e/apps/enotes/notes", getenv("HOME"));
	if ((p = opendir(f)) == NULL) {
		dml("Note Storage Location Doesn't Exist; Creating...", 1);
		if (mkdir(f, (mode_t) 0755) == -1)
			dml("Unable to Create Storage Location.  Expect problems!", 1);
	} else {
		dml("Note Storage Location Found", 1);
		closedir(p);
	}

	free(f);
	return;
}


/* Autosave Functions */

/**
 * @brief: Automatically loads all of the notes.
 */
void
autoload(void)
{				/* FIXME: Rewrite using dirents  */
	DIR            *dir;
	struct dirent  *p;
	char           *target = malloc(PATH_MAX);
	char           *targetf = malloc(PATH_MAX);
	struct stat     buf;

	sprintf(target, "%s/.e/apps/enotes/notes", getenv("HOME"));
	if ((dir = opendir(target)) != NULL) {

		while ((p = readdir(dir)) != NULL) {
			sprintf(targetf, "%s/%s", target, p->d_name);
			stat(targetf, &buf);
			if (S_ISREG(buf.st_mode)) {
				note_load(targetf);
			}
		}
		closedir(dir);
	}

	free(targetf);
	free(target);
}

/**
 * @brief: Automatically saves all open notes into the storage.
 */
void
autosave(void)
{
	int             x, y, w, h;
	Note           *note;
	Evas_List      *tmp = gbl_notes;
	NoteStor       *n;

	dml("Autosaving", 1);

	while (tmp != NULL) {
		note = evas_list_data(tmp);
		ecore_evas_geometry_get(note->win, &x, &y, &w, &h);
		n = alloc_note_stor();
		n->width = w;
		n->height = h;
		n->x = x;
		n->y = y;
		n->shaded = note->shaded;
		n->content = strdup(get_content_by_note(tmp));
		append_note_stor(n);
		free_note_stor(n);
		tmp = evas_list_next(tmp);
	}

	dml("Autosaved Notes", 1);

	return;
}

/* Internal Functions */

/**
 * @param e: The value to parse and build a notestor from.
 * @return: The built NoteStor structure (needs free'ing).
 * @brief: Parses e and builds a NoteStor structure, then returns it.
 */
NoteStor       *
get_notestor_from_value(char *e)
{
	NoteStor       *p = alloc_note_stor();

	if (e == NULL) {
		free(p);
		return (NULL);
	}

	p->content = strdup(strsep(&e, DEF_VALUE_SEPERATION));
	if (&e == NULL) {
		free_note_stor(p);
		return (NULL);
	}
	p->width = atoi(strsep(&e, DEF_VALUE_SEPERATION));
	if (&e == NULL) {
		free_note_stor(p);
		return (NULL);
	}

	p->height = atoi(strsep(&e, DEF_VALUE_SEPERATION));
	if (&e == NULL) {
		free_note_stor(p);
		return (NULL);
	}

	p->x = atoi(strsep(&e, DEF_VALUE_SEPERATION));
	if (&e == NULL) {
		free_note_stor(p);
		return (NULL);
	}

	p->y = atoi(strsep(&e, DEF_VALUE_SEPERATION));
	if (&e == NULL) {
		free_note_stor(p);
		return (NULL);
	}

	/* Add the new shaded field, but make it "optional" */
	if (e) {
		p->shaded = atoi(strsep(&e, DEF_VALUE_SEPERATION));
		if (&e == NULL) {
			p->shaded = 0;
		}
	} else {
		p->shaded = 0;
	}

	return (p);
}

/**
 * @param p: The NoteStor to parse and build a value from.
 * @return: The built string value.
 * @brief: Parses the NoteStor and builds a long string out of it.
 *         Reverse of the above function.
 */
char           *
get_value_from_notestor(NoteStor * p)
{
	char           *retval = malloc(MAX_VALUE);

	snprintf(retval, MAX_VALUE, "%s%s%d%s%d%s%d%s%d%s%d", p->content,
		 DEF_VALUE_SEPERATION, p->width, DEF_VALUE_SEPERATION,
		 p->height, DEF_VALUE_SEPERATION, p->x, DEF_VALUE_SEPERATION,
		 p->y, DEF_VALUE_SEPERATION, p->shaded);

	return (retval);
}
