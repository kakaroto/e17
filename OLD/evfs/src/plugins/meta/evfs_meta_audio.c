/*
 * vim:ts=3:sw=3:sts=8:noexpandtab:cino=>5n-3f0^-2{2
 */
/*

   Copyright (C) 2005 Alex Taylor

   Permission is hereby granted, free of charge, to any person obtaining a copy
   of this software and associated documentation files (the "Software"), to
   deal in the Software without restriction, including without limitation the
   rights to use, copy, modify, merge, publish, distribute, sublicense, and/or
   sell copies of the Software, and to permit persons to whom the Software is
   furnished to do so, subject to the following conditions:

   The above copyright notice and this permission notice shall be included in
   all copies of the Software and its documentation and acknowledgment shall be
   given in the documentation and software packages that this Software was
   used.

   THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
   IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
   FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL
   THE AUTHORS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER
   IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN
   CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
   */

#if HAVE_CONFIG_H
# include <config.h>
#endif

#include <evfs.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <errno.h>
#include <dirent.h>
#include <Ecore_File.h>
#include <sys/stat.h>
#include <tag_c.h>


Eina_List* evfs_file_meta_retrieve(evfs_client* client, evfs_command* command);


Ecore_List* evfs_plugin_meta_types_get()
{
	Ecore_List* list = ecore_list_new();

	ecore_list_append(list, "audio/x-mp3");

	return list;
}

evfs_plugin_functions_meta* evfs_plugin_init() 
{
	evfs_plugin_functions_meta* functions = calloc(1, sizeof(evfs_plugin_functions_meta));
	functions->evfs_file_meta_retrieve = evfs_file_meta_retrieve;

	return functions;
}


Eina_List* evfs_file_meta_retrieve(evfs_client* client, evfs_command* command)
{
	TagLib_File *taglib_file;
	TagLib_Tag *taglib_tag;
	const TagLib_AudioProperties *taglib_props;
	char* tmp;
	char buf[100];
	Eina_List* ret_list = NULL;
	
	EvfsMetaObject* obj;

	taglib_file = taglib_file_new(evfs_command_first_file_get(command)->path);
	if (!taglib_file) goto faildone;

	taglib_tag = taglib_file_tag(taglib_file);
	if (!taglib_tag) {
		taglib_file_free(taglib_file);
		goto faildone;
	}

	tmp = taglib_tag_artist(taglib_tag);
	if (tmp) {
		obj = calloc(1,sizeof(EvfsMetaObject));
		obj->key = strdup("artist");
		obj->value = strdup(tmp);
		ret_list = eina_list_append(ret_list, obj);

	} else {
		obj = calloc(1,sizeof(EvfsMetaObject));
		obj->key = strdup("artist");
		obj->value = NULL;
		ret_list = eina_list_append(ret_list, obj);		
	}

	tmp = taglib_tag_title(taglib_tag);
	if (tmp) {
		obj = calloc(1,sizeof(EvfsMetaObject));
		obj->key = strdup("title");
		obj->value = strdup(tmp);
		ret_list = eina_list_append(ret_list, obj);
	} else {
		obj = calloc(1,sizeof(EvfsMetaObject));
		obj->key = strdup("title");
		obj->value = NULL;
		ret_list = eina_list_append(ret_list, obj);
	}
	
	tmp = taglib_tag_album(taglib_tag);
	if (tmp) {
		obj = calloc(1,sizeof(EvfsMetaObject));
		obj->key = strdup("album");
		obj->value = strdup(tmp);
		ret_list = eina_list_append(ret_list, obj);
	} else {
		obj = calloc(1,sizeof(EvfsMetaObject));
		obj->key = strdup("album");
		obj->value = NULL;
		ret_list = eina_list_append(ret_list, obj);
	}

	taglib_props = taglib_file_audioproperties(taglib_file);
	if(taglib_props) {
		snprintf(buf, 100, "%d", taglib_audioproperties_length(taglib_props));
		
		obj = calloc(1,sizeof(EvfsMetaObject));
		obj->key = strdup("length");
		obj->value = strdup(buf);
		ret_list = eina_list_append(ret_list, obj);

	} else {
		obj = calloc(1,sizeof(EvfsMetaObject));
		obj->key = strdup("length");
		obj->value = NULL;
		ret_list = eina_list_append(ret_list, obj);
	}

	taglib_file_free(taglib_file);

	goto done;

	faildone:
	obj = calloc(1,sizeof(EvfsMetaObject));
	obj->key = strdup("file");
	obj->value = strdup("invalid");
	ret_list = eina_list_append(ret_list, obj);
	
	done:
	return ret_list;
}

