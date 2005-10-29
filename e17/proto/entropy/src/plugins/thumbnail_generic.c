#include <X11/Xlib.h>
#include <Imlib2.h>
#include <Ecore.h>
#include <stdio.h>
#include <stdlib.h>
#include <png.h>
#include "entropy.h"
#include "md5.h"
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <Epsilon.h>


static char* thumbnail_path = NULL; //Make this central to the core, so it can be configurable

entropy_thumbnail* entropy_thumbnail_new() {
	entropy_thumbnail* thumb = malloc(sizeof(entropy_thumbnail));
	
	thumb->text = ecore_hash_new(ecore_str_hash, ecore_str_compare);
	thumb->keys = 0;

	allocated_thumbnails++;
	print_allocation();

	return thumb;
}

void entropy_thumbnail_destroy(entropy_thumbnail* thumb) {
	if (thumb) {
		if (thumb->text) ecore_hash_destroy(thumb->text);
		entropy_free(thumb);
		allocated_thumbnails--;

		print_allocation();
	}
}

entropy_thumbnail* entropy_thumbnail_create(entropy_generic_file* e_file) {
	Imlib_Image image;
	Imlib_Image thumbnail;
  	png_infop    info_ptr;
	png_structp	png_ptr;
	png_text	*text_ptr = NULL;
	png_bytep    row_ptr,
        row_data=NULL;
	DATA32 *ptr = NULL;
	char* key;
	int i,w=64,h=64;
	  png_color_8  sig_bit;
	entropy_thumbnail* thumb_struct;
	Ecore_List* keys;
	char thumb_path_and_name[255];
	char *pos;
	FILE* fp = NULL;
	char thumbname[255];
	Epsilon* e;

	int iw,ih;

	
	strcpy(thumb_path_and_name, e_file->path);
	pos = thumb_path_and_name + strlen(thumb_path_and_name);
	strcpy(pos, "/");
	pos +=1;
	strcpy(pos, e_file->filename);

	e = epsilon_new(thumb_path_and_name);
	if (epsilon_exists(e) == EPSILON_FAIL) {
		epsilon_generate(e);
	}

	
	if (epsilon_exists(e)) {
		thumb_struct = entropy_thumbnail_new();
		strcpy(thumb_struct->thumbnail_filename, epsilon_thumb_file_get(e));
		return thumb_struct;
	} else return NULL;

	
}
