/*
 * vim:ts=3:sw=3:sts=8:noexpandtab:cino=>5n-3f0^-2{2
 */
/*

   Copyright (C) 2005-2006 Alex Taylor

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
#include <limits.h>
#include <extractor.h>

static  EXTRACTOR_ExtractorList *_extractors = NULL;
static EXTRACTOR_KeywordList   *keywords;
Eina_List* evfs_file_meta_retrieve(evfs_client* client, evfs_command* command);
const char              *key;
static char buffer[PATH_MAX];


Ecore_List* evfs_plugin_meta_types_get()
{
	Ecore_List* list = ecore_list_new();

	ecore_list_append(list, "image/png");
	ecore_list_append(list, "images/jpeg");
	ecore_list_append(list, "image/gif");
	ecore_list_append(list, "video/x-ms-wmv");
	ecore_list_append(list, "application/msword");
	ecore_list_append(list, "application/pdf");
	ecore_list_append(list, "application/vnd.ms-excel");
	ecore_list_append(list, "application/x-gtar");
	ecore_list_append(list, "text/html");
	ecore_list_append(list, "video/mpeg");
	ecore_list_append(list, "video/x-msvideo");
	ecore_list_append(list, "application/x-gtar");
	ecore_list_append(list, "application/x-bzip2");
	ecore_list_append(list, "video/quicktime");
	ecore_list_append(list, "video/x-ms-asf");
 	ecore_list_append(list, "object/undefined");

	



	return list;
}

evfs_plugin_functions_meta* evfs_plugin_init() 
{
	evfs_plugin_functions_meta* functions = calloc(1, sizeof(evfs_plugin_functions_meta));
	functions->evfs_file_meta_retrieve = evfs_file_meta_retrieve;

	_extractors = EXTRACTOR_loadDefaultLibraries();

	return functions;
}


Eina_List* evfs_file_meta_retrieve(evfs_client* client, evfs_command* command)
{
	Eina_List* ret_list = NULL;
	EvfsMetaObject* obj;
	char* key;
	EvfsFilereference* ref;

	ref = evfs_command_first_file_get(command);

	keywords=EXTRACTOR_getKeywords(_extractors, ref->path);
	keywords=EXTRACTOR_removeDuplicateKeywords(keywords,0);

	while(keywords) {

		key=EXTRACTOR_getKeywordTypeAsString(keywords->keywordType);
		
		obj = calloc(1,sizeof(EvfsMetaObject));
		obj->key = strdup(key);
		obj->value = strdup(keywords->keyword);
		ret_list = eina_list_append(ret_list, obj);

		keywords = keywords->next;
	}
	EXTRACTOR_freeKeywords(keywords);

	return ret_list;
}

