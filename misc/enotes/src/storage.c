
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

NoteStor       *
alloc_note_stor()
{
	NoteStor       *p = malloc(sizeof(NoteStor));

	p->title = NULL;
	p->content = NULL;
	return (p);
}

/* One Shot Functions. :-) */
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

void
stor_cycle_end(XmlReadHandle * p)
{
	xml_read_end(p);
	return;
}


void
stor_cycle_next(XmlReadHandle * p)
{
	xml_read_next_entry(p);
	return;
}

void
stor_cycle_prev(XmlReadHandle * p)
{
	xml_read_prev_entry(p);
	return;
}


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

/* Internal Functions */
char           *
make_storage_fn(void)
{
	char           *p = malloc(PATH_MAX);

	snprintf(p, PATH_MAX, DEF_STORAGE_LOC, getenv("HOME"));
	return (p);
}

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

char           *
get_value_from_notestor(NoteStor * p)
{
	char           *retval = malloc(MAX_VALUE);

	snprintf(retval, MAX_VALUE, "%s%s%s%s%d%s%d", p->title,
		 DEF_VALUE_SEPERATION, p->content, DEF_VALUE_SEPERATION,
		 p->width, DEF_VALUE_SEPERATION, p->height);

	return (retval);
}
