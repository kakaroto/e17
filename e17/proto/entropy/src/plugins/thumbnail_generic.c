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

int entropy_stat_thumbnail(char* path) {
	struct stat buf;
	int code;

	code = lstat(path, &buf);
	
	if (code == -1) {
		return 0;
	} else {
		return 1;
	}
}

void entropy_thumbnail_add_text(entropy_thumbnail* thumb, char* key, char* value) {
	
	char* ikey;
	char* ival;

	ikey = strdup(key);
	ival = strdup(value);
	
	ecore_hash_set(thumb->text, ikey, ival);
	thumb->keys++;
}

static char* md5_for_string(char* string) {
	static char md5[33];
	md5_state_t state;
	md5_byte_t digest[16];
	static const char hex[]="0123456789abcdef";
	int i;

       md5_init(&state);
       md5_append(&state, (const md5_byte_t*)string, strlen(string));
       md5_finish(&state, digest);
       for (i=0;i<16;i++) {
	       md5[2*i] = hex[digest[i]>>4];
	       md5[2*i+1]=hex[digest[i]&0x0f];
       }
       md5[2*i] = '\0';

       return md5;

	
}

entropy_thumbnail* entropy_thumbnail_create(entropy_generic_file* e_file) {
	Imlib_Image image;
	Imlib_Image thumbnail;
	Imlib_Load_Error error;
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

	int iw,ih;

	/*If we don't get have a thumbnail path, make one*/
	if (!thumbnail_path) {
		thumbnail_path = malloc( (sizeof(char)* strlen(getenv("HOME"))) + strlen("/.thumbnails/normal/") + 2);
		sprintf(thumbnail_path, "%s%s", getenv("HOME"), "/.thumbnails/normal/");
		//printf("Set thumbnail path to %s\n", thumbnail_path);	
	}


	
	strcpy(thumb_path_and_name, e_file->path);
	pos = thumb_path_and_name + strlen(thumb_path_and_name);
	strcpy(pos, "/");
	pos +=1;
	strcpy(pos, e_file->filename);

	
	/*printf("MD5: '%s'\n", md5_for_string(thumb_path_and_name));*/

	/*Now set up the thumbname*/
	strncpy(thumbname, thumbnail_path, strlen(thumbnail_path));
	pos = thumbname + strlen(thumbnail_path);
	strcpy(pos, md5_for_string(thumb_path_and_name));
	pos += 32;
	strcpy(pos, ".png");



	
	/*printf("Writing thumb: '%s'\n", thumbname);*/
	
	thumb_struct = entropy_thumbnail_new();

	entropy_thumbnail_add_text(thumb_struct,"TesT","Value");

	/*First check if this image already exists.. */
	if (entropy_stat_thumbnail(thumbname)) {
		goto already_exists; /* Image already exists */
		/*TODO check the create date etc */
	}

	
	
	//printf("Thumbnailing: '%s'\n", thumb_path_and_name);	
	image = imlib_load_image_immediately_without_cache(thumb_path_and_name);

	if (!image) { goto error; }
	
	imlib_context_set_image(image);

	imlib_context_set_cliprect(0,0,w,h);

	iw = imlib_image_get_width();
	ih = imlib_image_get_height();
	
	thumbnail = imlib_create_cropped_scaled_image(0,0,iw,ih,w,h);
	imlib_free_image(); /*Free the master image*/
	imlib_context_set_image(thumbnail);



	/* Now write the thumbnail */
	png_ptr = png_create_write_struct(PNG_LIBPNG_VER_STRING,NULL,NULL,NULL);
	info_ptr = png_create_info_struct(png_ptr);

	fp = fopen(thumbname,"wb");
	png_init_io(png_ptr, fp);	

	/* Apply attribs */
	text_ptr = entropy_malloc(sizeof(png_text)*thumb_struct->keys);
	keys = ecore_hash_keys(thumb_struct->text);
	ecore_list_goto_first(keys);
	i=0;
	/*printf ("Allocated enough space for %d keys\n", thumb_struct->keys);*/
	while ( (key = ecore_list_next(keys)) ) {
		text_ptr[i].key = key;
		text_ptr[i].text = ecore_hash_get(thumb_struct->text, key);
		text_ptr[i].compression=PNG_TEXT_COMPRESSION_NONE;

		i++;
	}

	png_set_text(png_ptr, info_ptr, text_ptr, thumb_struct->keys);

	png_set_IHDR(png_ptr,info_ptr,w,h,8,PNG_COLOR_TYPE_RGB_ALPHA,
                   PNG_INTERLACE_NONE,PNG_COMPRESSION_TYPE_BASE,
                   PNG_FILTER_TYPE_BASE);

	png_set_bgr(png_ptr);

	/*png_set_swap_alpha(png_ptr);*/
	sig_bit.red=8;
	sig_bit.green=8;
	sig_bit.blue=8;
	sig_bit.alpha=8;
	png_set_sBIT(png_ptr,info_ptr,&sig_bit);

      
	png_set_compression_level(png_ptr,9);
	png_write_info(png_ptr,info_ptr);

    	png_set_shift(png_ptr,&sig_bit);
	png_set_packing(png_ptr);

	ptr = imlib_image_get_data_for_reading_only();
	/*row_data=malloc(w*3*sizeof(char));*/
	for(i=0;i<h;i++) {
	      row_ptr=(png_bytep) ptr;
	      png_write_row(png_ptr,row_ptr);
	      ptr += w; 
	  }

	if (row_data) {
		entropy_free(row_data);
	}

	

	png_write_end(png_ptr,info_ptr);
	png_destroy_write_struct(&png_ptr,&info_ptr);
	png_destroy_info_struct(png_ptr,&info_ptr);
	entropy_free(text_ptr);
	imlib_free_image_and_decache(); /*Free the thumbnail*/
	

	fclose(fp);


	already_exists:

	strcpy(thumb_struct->thumbnail_filename, thumbname);
	return thumb_struct;

	error:

	entropy_thumbnail_destroy(thumb_struct);
	
	//printf("Thumbnailing error!\n");
	return NULL;
}
