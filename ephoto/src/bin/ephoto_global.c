#include "ephoto.h"

void ephoto_set_main_window(Ewl_Widget *w) {
	em->win = w;
}

Ewl_Widget *ephoto_get_main_window(void) {
	return em->win;
}

void ephoto_set_effects_window(Ewl_Widget *w) {
	        em->ewin = w;
}

Ewl_Widget *ephoto_get_effects_window(void) {
	        return em->ewin;
}

void ephoto_set_view_box(Ewl_Widget *w) {
	em->view_box = w;
}

Ewl_Widget *ephoto_get_view_box(void) {
	return em->view_box;
}

void ephoto_set_normal_vbox(Ewl_Widget *w) {
	em->normal_vbox = w;
}

Ewl_Widget *ephoto_get_normal_vbox(void) {
	return em->normal_vbox;
}

void ephoto_set_fbox(Ewl_Widget *w) {
	em->fbox = w;
}

Ewl_Widget *ephoto_get_fbox(void) {
	return em->fbox;
}

void ephoto_set_ftree(Ewl_Widget *w) {
	em->ftree = w;
}

Ewl_Widget *ephoto_get_ftree(void) {
	return em->ftree;
}

void ephoto_set_single_vbox(Ewl_Widget *w) {
	em->single_vbox = w;
}

Ewl_Widget *ephoto_get_single_vbox(void) {
	return em->single_vbox;
}

void ephoto_set_single_image(Ewl_Widget *w) {
	em->single_image = w;
}

Ewl_Widget *ephoto_get_single_image(void) {
	return em->single_image;
}

void ephoto_set_fsystem(Ecore_List *l) {
	em->fsystem = l;
}

Ecore_List *ephoto_get_fsystem(void) {
	return em->fsystem;
}

void ephoto_set_images(Ecore_List *l) {
	em->images = l;
}

Ecore_List *ephoto_get_images(void) {
	return em->images;
}

void ephoto_set_current_directory(char *dir) {
	em->current_directory = dir;
}

char *ephoto_get_current_directory(void) {
	return em->current_directory;
}

void ephoto_set_thumb_size(int i) {
	em->thumb_size = i;
}

int ephoto_get_thumb_size(void) {
	return em->thumb_size;
}

