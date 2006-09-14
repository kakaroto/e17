#include <Ewl.h>
#include "ewl_private.h"
#include "ewl_debug.h"
#include "ewl_macros.h"

Ewl_Widget *
ewl_io_manager_plugin_read(const char *uri)
{
	Ewl_Widget *ret = NULL;
	FILE *file;

	DENTER_FUNCTION(DLEVEL_STABLE);
	DCHECK_PARAM_PTR_RET("uri", uri, NULL);

	file = fopen(uri, "r");
	if (file) 
	{
		struct stat buf;
		char *str;
		Ecore_List *nodes;
		Ewl_Text_Tree *node;

		ret = ewl_text_new();

		stat(uri, &buf);
		str = malloc(sizeof(char) * (buf.st_size + 1));
		fread(str, buf.st_size, 1, file);
		str[buf.st_size] = '\0';
		fclose(file);

		nodes = ecore_list_new();

		node = ewl_text_tree_new();
		node->length.bytes = buf.st_size;
		node->tx = ewl_text_context_default_create(EWL_TEXT(ret));

		ecore_list_append(nodes, node);
		ewl_text_deserialize(EWL_TEXT(ret), nodes, str);

		ecore_list_destroy(nodes);
		FREE(str);
	}

	DRETURN_PTR(ret, DLEVEL_STABLE);
}

void
ewl_io_manager_plugin_write(Ewl_Widget *data, const char *uri)
{
	FILE *file;

	DENTER_FUNCTION(DLEVEL_STABLE);
	DCHECK_PARAM_PTR("data", data);
	DCHECK_PARAM_PTR("uri", uri);
	DCHECK_TYPE("data", data, EWL_WIDGET_TYPE);

	file = fopen(uri, "w");
	if (file)
	{
		char *txt;

		txt = ewl_text_text_get(EWL_TEXT(data));
		fwrite(txt, sizeof(char), strlen(txt), file);

		FREE(txt);
		fclose(file);
	}

	DLEAVE_FUNCTION(DLEVEL_STABLE);
}

