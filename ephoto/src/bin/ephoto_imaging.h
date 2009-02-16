#ifndef _EPHOTO_IMAGING_H_
#define _EPHOTO_IMAGING_H_

#include <Evas.h>
#include <stdio.h>
#include <stdlib.h>

unsigned int *rotate_left(Ewl_Widget *image);
unsigned int *rotate_right(Ewl_Widget *image);
unsigned int *flip_horizontal(Ewl_Widget *image);
unsigned int *flip_vertical(Ewl_Widget *image);
unsigned int *blur_image(Ewl_Widget *image);
unsigned int *sharpen_image(Ewl_Widget *image);
unsigned int *grayscale_image(Ewl_Widget *image);
unsigned int *sepia_image(Ewl_Widget *image);
void update_image(Ewl_Widget *image, int w, int h,
                                unsigned int *data);

#endif
