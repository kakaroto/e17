/*
 * $Id$
 *
 * Copyright (C) 2004 Embrace project.
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of version 2 of the GNU General Public License as
 * published by the Free Software Foundation.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software Foundation,
 * Inc., 59 Temple Place, Suite 330, Boston, MA 02111-1307, USA.
 */

/*
 * Parts of this code are derived from John Fleck's tutorial on libxml
 * (http://www.xmlsoft.org/tutorial/index.html)
 */

#ifdef HAVE_CONFIG_H
# include <config.h>
#endif

#include <assert.h>
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>

#include <libxml/parser.h>
#include <libxml/xpath.h>

#include <embrace_plugin.h>

static xmlXPathObjectPtr get_nodeset (xmlDocPtr doc, xmlChar *xpath)
{
	xmlXPathContextPtr context;
	xmlXPathObjectPtr result;

	context = xmlXPathNewContext (doc);
	if (!context) {
		fprintf (stderr, "Error in xmlXPathNewContext()\n");

		return NULL;
	}

	result = xmlXPathEvalExpression (xpath, context);
	xmlXPathFreeContext (context);

	if (!result) {
		fprintf (stderr, "Error in xmlXPathEvalExpression()\n");

		return NULL;
	}

	if (xmlXPathNodeSetIsEmpty (result->nodesetval)) {
		xmlXPathFreeObject (result);
		result = NULL;
	}

	return result;
}

static bool sylpheed_load_config (MailBox *mb, E_DB_File *edb,
                                  const char *root)
{
	char key[32], *str, *path, *mailbox, *folder;

	assert (mb);
	assert (edb);
	assert (root);

	if (!(path = malloc ((PATH_MAX + 1) * sizeof (char))))
		return false;

	/* get the path */
	snprintf (key, sizeof (key), "%s/path", root);

	if (!(str = e_db_str_get (edb, key))) {
		fprintf (stderr, "[sylpheed] 'path' not specified!\n");
		return false;
	}

	embrace_expand_path (str, path, PATH_MAX + 1);
	free (str);

	mailbox_property_set (mb, "path", path);

	/* get the mailbox */
	snprintf (key, sizeof (key), "%s/mailbox", root);

	if (!(mailbox = e_db_str_get (edb, key))) {
		fprintf (stderr, "[sylpheed] 'mailbox' not specified!\n");
		return false;
	}

	mailbox_property_set (mb, "mailbox", mailbox);

	/* get the folder */
	snprintf (key, sizeof (key), "%s/folder", root);

	if (!(folder = e_db_str_get (edb, key))) {
		fprintf (stderr, "[sylpheed] 'folder' not specified!\n");
		return false;
	}

	mailbox_property_set (mb, "folder", folder);

	return true;
}

static bool sylpheed_check (MailBox *mb)
{
	xmlDocPtr xml;
	const char *tags[] = {"new", "total"};
	int i;

	assert (mb);

	xml = xmlParseFile ((char *) mailbox_property_get (mb, "path"));
	if (!xml) {
		fprintf (stderr, "[sylpheed] Document not parsed successfully.\n");
		return false;
	}

	for (i = 0; i < 2; i++) {
		xmlXPathObjectPtr set;
		xmlNodePtr node;
		xmlChar *ch;
		char xpath[256];
		int n;

		snprintf (xpath, sizeof (xpath),
		          "//folderlist/folder[@name=\'%s\']"
		          "/folderitem[@name=\'%s\']/@%s",
		          (char *) mailbox_property_get (mb, "mailbox"),
		          (char *) mailbox_property_get (mb, "folder"),
		          tags[i]);

		set = get_nodeset (xml, xpath);
		if (!set)
			continue;

		node = set->nodesetval->nodeTab[0]->xmlChildrenNode;
		assert (node);

		ch = xmlNodeListGetString (xml, node, 1);
		if (ch) {
			n = xmlXPathCastStringToNumber (ch);

			if (!i)
				mailbox_unseen_set (mb, n);
			else
				mailbox_total_set (mb, n);

			xmlFree (ch);
		}

		xmlXPathFreeObject (set);
	}

	xmlFreeDoc (xml);
	xmlCleanupParser ();

	return true;
}

static int on_timer (void *udata)
{
	sylpheed_check (udata);

	return 1;
}

static bool sylpheed_add_mailbox (MailBox *mb)
{
	Ecore_Timer *timer;
	int interval;

	assert (mb);

	interval = mailbox_poll_interval_get (mb);

	if (!(timer = ecore_timer_add (interval, on_timer, mb)))
		return false;

	mailbox_property_set (mb, "timer", timer);

	return true;
}

static bool sylpheed_remove_mailbox (MailBox *mb)
{
	Ecore_Timer *timer;
	assert (mb);

	if ((timer = mailbox_property_get (mb, "timer")))
		ecore_timer_del (timer);

	free (mailbox_property_get (mb, "path"));
	free (mailbox_property_get (mb, "mailbox"));
	free (mailbox_property_get (mb, "folder"));

	return true;
}

bool embrace_plugin_init (EmbracePlugin *ep)
{
	snprintf (ep->name, sizeof (ep->name), "%s", "sylpheed");

	ep->check = sylpheed_check;
	ep->load_config = sylpheed_load_config;

	ep->add_mailbox = sylpheed_add_mailbox;
	ep->remove_mailbox = sylpheed_remove_mailbox;

	return true;
}
