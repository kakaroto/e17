
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


#include "config.h"

/**
 * @return: Returns an allocated MainConfig variable with some default values
 *          set.
 * @brief: Allocate and initialize a new MainConfig variable.
 */
MainConfig     *
mainconfig_new(void)
{
	MainConfig     *p;

	p = malloc(sizeof(MainConfig));

	p->render_method = NULL;
	p->theme = NULL;

	p->debug = 0;
	p->intro = 0;
	p->controlcentre = 1;
	p->autosave = 0;

	return (p);
}

/**
 * @param p: The MainConfig variable to free.
 * @brief: Free's an allocated MainConfig variable.
 */
void
mainconfig_free(MainConfig * p)
{
	if (!p) {
		if (p->render_method != NULL)
			free(p->render_method);
		if (p->theme != NULL)
			free(p->theme);

		free(p);
	}
	return;
}

/**
 * @param p: The MainConfig variable to store the read settings into.
 * @brief: Reads the global configuration settings and stores them into p.
 */
void
read_global_configuration(MainConfig * p)
{
	char           *locfn = malloc(PATH_MAX);

	snprintf(locfn, PATH_MAX, "%s/data/config.xml", PACKAGE_DATA_DIR);
	read_configuration(p, locfn);
	free(locfn);
	return;
}

/**
 * @param p: The MainConfig variable to store the read settings into.
 * @brief: Reads the local configuration and stores the settings into p.
 */
void
read_local_configuration(MainConfig * p)
{
	char           *locfn = malloc(PATH_MAX);

	snprintf(locfn, PATH_MAX, DEF_CONFIG_LOC, getenv("HOME"));
	read_configuration(p, locfn);
	free(locfn);
	return;
}

/**
 * @param p:  The MainConfig variable to store the read settings into.
 * @param fn: The location of the configuration file to read.
 * @brief: Reads the configuration file pointed to by fn, and stores the
 *         settings into p.
 */
void
read_configuration(MainConfig * p, char *fn)
{
	XmlReadHandle  *h;
	XmlEntry       *tmp;

	h = xml_read(fn);
	while (h->cur != NULL) {
		tmp = xml_read_entry_get_entry(h);
		processopt(tmp, p);
		free_xmlentry(tmp);
		xml_read_next_entry(h);
	}
	xml_read_end(h);
	return;
}

/**
 * @param info: The xml tag to process.
 * @param p: The MainConfig variable to apply the value to.
 * @brief: Processed an xml tag and applies the individual setting
 *         it reads to p.
 */
void
processopt(XmlEntry * info, MainConfig * p)
{
	if (!strcmp(info->name, "render_method")) {
		if (p->render_method != NULL)
			free(p->render_method);
		p->render_method = strdup(info->value);
	} else if (!strcmp(info->name, "theme")) {
		if (p->theme != NULL)
			free(p->theme);
		p->theme = strdup(info->value);
	} else if (!strcmp(info->name, "controlcentre")) {
		p->controlcentre = atoi(info->value);
	} else if (!strcmp(info->name, "intro")) {
		p->intro = atoi(info->value);
	} else if (!strcmp(info->name, "debug")) {
		p->debug = atoi(info->value);
	} else if (!strcmp(info->name, "autosave")) {
		p->autosave = atoi(info->value);
	}

	return;
}

/**
 * @brief: Check whether a local configuration exists, and if not it
 *         will create the necessary files and folders so the user can
 *         immediately begin to edit his/her configuration.
 */
void
check_local_configuration(void)
{
	char           *homedir_e = malloc(PATH_MAX);
	char           *homedir_e_notes = malloc(PATH_MAX);
	char           *homedir_e_notes_config = malloc(PATH_MAX);
	char           *global_config = malloc(PATH_MAX);
	char           *execstr = malloc(PATH_MAX * 2);
	FILE           *input;
	char            buf;
	XmlReadHandle  *p;

	snprintf(homedir_e, PATH_MAX, "%s/.e", getenv("HOME"));
	snprintf(homedir_e_notes, PATH_MAX, "%s/.e/notes", getenv("HOME"));
	snprintf(homedir_e_notes_config, PATH_MAX,
		 "%s/.e/notes/config.xml", getenv("HOME"));
	snprintf(global_config, PATH_MAX,
		 "%s/data/config.xml", PACKAGE_DATA_DIR);

	mkdir(homedir_e, 0700);
	mkdir(homedir_e_notes, 0700);

	snprintf(execstr, PATH_MAX * 2, "%s %s %s", COPY_COMMAND,
		 global_config, homedir_e_notes_config);

	if ((input = fopen(homedir_e_notes_config, "r")) == NULL) {
		system(execstr);
	} else {
		fclose(input);
	}

	free(homedir_e);
	free(homedir_e_notes);
	free(homedir_e_notes_config);
	free(global_config);
	free(execstr);

	return;
}
