#ifndef _EW_IMAGE_H
#define _EW_IMAGE_H

Entrance_Widget ew_image_new(int, int);
void ew_image_file_load(Entrance_Widget, const char*);
void ew_image_edje_load(Entrance_Widget, const char*, const char*);

#endif
