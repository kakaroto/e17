
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
		if (p->title != NULL)
			free(p->title);
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

	p->title = NULL;
	p->content = NULL;
	return (p);
}

/* One Shot Functions. :-) */

/**
 * @param p: The NoteStor containing the required information.
 * @brief: Appends a new autosave note according to whats in p.
 */
void
append_autosave_note_stor(NoteStor * p)
{
	Evas_List      *list;
	Evas_List      *lt;
	XmlReadHandle  *r;
	XmlWriteHandle *w;
	XmlEntry       *e;
	char           *fn;
	char           *value;
	char           *val;

	list = NULL;

	/* Making the strings */
	value = get_value_from_notestor(p);
	fn = make_autosave_fn();

	/* Read */
	r = xml_read(fn);
	if (r != NULL) {
		while (r->cur != NULL) {
			e = xml_read_entry_get_entry(r);
			list = evas_list_append(list, strdup(e->value));
			free_xmlentry(e);
			xml_read_next_entry(r);
		}
		xml_read_end(r);
	} else {
		list = NULL;
	}

	/* Write */
	w = xml_write(fn);
	if (list != NULL) {
		lt = list;
		while (lt != NULL) {
			xml_write_append_entry(w, "NoteStor",
					       (char *) evas_list_data(lt));
			free(evas_list_data(lt));
			list = evas_list_remove(lt, evas_list_data(lt));
			lt = evas_list_next(lt);
		}
	}
	val = get_value_from_notestor(p);
	xml_write_append_entry(w, "NoteStor", val);
	free(val);
	xml_write_end(w);

	/* Leaving */
	evas_list_free(list);
	free(value);
	free(fn);
	return;
}

/**
 * @param p: The information required (about the note we're saving).
 * @brief: Appends a new note to the note storage according to the
 *         information stored in p.
 */
void
append_note_stor(NoteStor * p)
{
	Evas_List      *list;
	Evas_List      *lt;
	XmlReadHandle  *r;
	XmlWriteHandle *w;
	XmlEntry       *e;
	char           *fn;
	char           *value;
	char           *val;

	list = NULL;

	/* Making the strings */
	value = get_value_from_notestor(p);
	fn = make_storage_fn();

	/* Read */
	r = xml_read(fn);
	if (r != NULL) {
		while (r->cur != NULL) {
			e = xml_read_entry_get_entry(r);
			list = evas_list_append(list, strdup(e->value));
			free_xmlentry(e);
			xml_read_next_entry(r);
		}
		xml_read_end(r);
	} else {
		list = NULL;
	}

	/* Write */
	w = xml_write(fn);
	if (list != NULL) {
		lt = list;
		while (lt != NULL) {
			xml_write_append_entry(w, "NoteStor",
					       (char *) evas_list_data(lt));
			free(evas_list_data(lt));
			list = evas_list_remove(lt, evas_list_data(lt));
			lt = evas_list_next(lt);
		}
	}
	val = get_value_from_notestor(p);
	xml_write_append_entry(w, "NoteStor", val);
	free(val);
	xml_write_end(w);

	/* Leaving */
	evas_list_free(list);
	free(value);
	free(fn);
	return;
}

/**
 * @param p: The NoteStor containing the information required.
 * @brief: Removes the NoteStor corrosponding to the information
 *         inside p.
 */
void
remove_note_stor(NoteStor * p)
{
	Evas_List      *list;
	Evas_List      *lt;
	XmlReadHandle  *r;
	XmlWriteHandle *w;
	XmlEntry       *e;
	NoteStor       *tmp;
	char           *fn;
	char           *value;
	char           *val;
	char           *tmpstr;

	list = NULL;

	/* Making the strings */
	value = get_value_from_notestor(p);
	fn = make_storage_fn();

	/* Read */
	r = xml_read(fn);
	if (r != NULL) {
		while (r->cur != NULL) {
			e = xml_read_entry_get_entry(r);
			tmpstr = strdup(e->value);
			tmp = get_notestor_from_value(tmpstr);
			free(tmpstr);
			if (strcmp(p->title, tmp->title)) {
				list = evas_list_append(list, strdup(e->value));
			}
			free_note_stor(tmp);
			free_xmlentry(e);
			xml_read_next_entry(r);
		}
		xml_read_end(r);
	} else {
		list = NULL;
	}

	/* Write */
	w = xml_write(fn);
	if (list != NULL) {
		lt = list;
		while (lt != NULL) {
			xml_write_append_entry(w, "NoteStor",
					       (char *) evas_list_data(lt));
			free(evas_list_data(lt));
			list = evas_list_remove(lt, evas_list_data(lt));
			lt = evas_list_next(lt);
		}
	}
	xml_write_end(w);

	/* Leaving */
	evas_list_free(list);
	free(value);
	free(fn);
	return;
}


/* Cycle Functions */

/**
 * @return: The XmlReadHandle for the beginning of the storage cycle.
 * @brief: Returns a handle for the beginning of the storage cycle (reading).
 */
XmlReadHandle  *
stor_cycle_begin(void)
{
	char           *p;
	XmlReadHandle  *retval;

	p = make_storage_fn();
	retval = xml_read(p);
	free(p);
	return (retval);
}

/**
 * @return: The XmlReadHandle for the beginning of the autosave storage cycle.
 * @brief: Begins the storage cycle for the autosave storage and returns a read
 *         handle.
 */
XmlReadHandle  *
stor_cycle_begin_autosave(void)
{
	char           *p;
	XmlReadHandle  *retval;

	p = make_autosave_fn();
	retval = xml_read(p);
	free(p);
	return (retval);
}

/**
 * @param p: The read handle which is to be ended and free'd.
 * @brief: Ends and free's a storage cycle.
 */
void
stor_cycle_end(XmlReadHandle * p)
{
	xml_read_end(p);
	return;
}


/**
 * @param p: The cycle handle to move forward in.
 * @brief: Move forward in the cycle (next note).
 */
void
stor_cycle_next(XmlReadHandle * p)
{
	xml_read_next_entry(p);
	return;
}

/**
 * @param p: The cycle handle to move backwards in.
 * @brief: Moves backwards in the cycle (previous note).
 */
void
stor_cycle_prev(XmlReadHandle * p)
{
	xml_read_prev_entry(p);
	return;
}


/**
 * @param p: The handle to get the current notestor from.
 * @return: The notestor requested, allocated and with values.
 *          Needs free'ing.
 * @brief: Obtains the NoteStor information from the current stage
 *         in the supplied handle.
 */
NoteStor       *
stor_cycle_get_notestor(XmlReadHandle * p)
{
	NoteStor       *h;
	XmlEntry       *info;

	info = xml_read_entry_get_entry(p);
	h = get_notestor_from_value(info->value);
	free(info);
	return (h);
}

/* Autosave Functions */

/**
 * @brief: Automatically loads all of the "autosave" notes.
 */
void
autoload(void)
{
	XmlReadHandle  *r;
	NoteStor       *p;

	dml("Autoloading Saved Notes", 1);

	r = stor_cycle_begin_autosave();
	if (r != NULL) {
		while (r->cur != NULL) {
			p = stor_cycle_get_notestor(r);
			new_note_with_values(p->width, p->height, p->title,
					     p->content);
			free_note_stor(p);
			stor_cycle_next(r);
		}
		stor_cycle_end(r);
	}
	return;
}

/**
 * @brief: Automatically saves all open notes into the autosave storage.
 */
void
autosave(void)
{
	int             x, y, w, h;
	Note           *note;
	Evas_List      *tmp = gbl_notes;
	NoteStor       *n;
	XmlWriteHandle *p;
	char           *pp;

	dml("Autosaving", 1);

	pp = make_autosave_fn();
	p = xml_write(pp);
	xml_write_end(p);
	free(pp);

	while ((tmp = evas_list_next(tmp)) != NULL) {
		note = evas_list_data(tmp);
		ecore_evas_geometry_get(note->win, &x, &y, &w, &h);
		n = alloc_note_stor();
		n->width = w;
		n->height = h;
		n->title = strdup(get_title_by_note(tmp));
		n->content = strdup(get_content_by_note(tmp));
		append_autosave_note_stor(n);
		free_note_stor(n);
	}

	return;
}

/* Internal Functions */

/**
 * @return: The storage file location string.
 * @brief: Builds up a string containing the location of the storage
 *         xml file.
 */
char           *
make_storage_fn(void)
{
	char           *p = malloc(PATH_MAX);

	snprintf(p, PATH_MAX, DEF_STORAGE_LOC, getenv("HOME"));
	return (p);
}

/**
 * @return: The storage file location string (autosave).
 * @brief: Builds up a string containing the location of the autosave
 *         storage xml file.
 */
char           *
make_autosave_fn(void)
{
	char           *p = malloc(PATH_MAX);

	snprintf(p, PATH_MAX, DEF_AUTOSAVE_LOC, getenv("HOME"));
	return (p);
}

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

	p->title = strdup(strsep(&e, DEF_VALUE_SEPERATION));
	if (&e == NULL) {
		free_note_stor(p);
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

	snprintf(retval, MAX_VALUE, "%s%s%s%s%d%s%d", p->title,
		 DEF_VALUE_SEPERATION, p->content, DEF_VALUE_SEPERATION,
		 p->width, DEF_VALUE_SEPERATION, p->height);

	return (retval);
}
