#include "ephoto.h"

static void close_dialog(Ewl_Widget *w, void *event, void *data);
static void save_image(Ewl_Widget *w, void *event, void *data);

static Ewl_Widget *save_win, *qseek, *cseek;

unsigned int *flip_horizontal(Ewl_Widget *image)
{
	unsigned int *im_data, *im_data_new;
	int index, ind, i, j, ni, nj, ew, eh;

	im_data = evas_object_image_data_get(EWL_IMAGE(image)->image, FALSE);
	evas_object_image_size_get(EWL_IMAGE(image)->image, &ew, &eh);

	index = 0;

	im_data_new = malloc(sizeof(unsigned int) * ew * eh);

	for (i = 0; i < eh; i ++)
	{
		for (j = 0; j < ew; j++)
		{
			ni = i;
			nj = ew - j - 1;

			ind = ni * ew + nj;

			im_data_new[index] = im_data[ind];

			index++;
		}
	}
	return im_data_new;
}

unsigned int *flip_vertical(Ewl_Widget *image)
{
	unsigned int *im_data, *im_data_new;
	int index, ind, i, j, ni, nj, ew, eh;

	im_data = evas_object_image_data_get(EWL_IMAGE(image)->image, FALSE);
	evas_object_image_size_get(EWL_IMAGE(image)->image, &ew, &eh);

	index = 0;

	im_data_new = malloc(sizeof(unsigned int) * ew * eh);

	for (i = 0; i < eh; i++)
	{
		for (j = 0; j < ew; j++)
		{
			ni = eh - i - 1;
			nj = j;

			ind = ni * ew + nj;

			im_data_new[index] = im_data[ind];

			index++;
		}
	}
	return im_data_new;
}

unsigned int *rotate_left(Ewl_Widget *image)
{
	unsigned int *im_data, *im_data_new;
	int index, ind, i, j, ni, nj, ew, eh, nw, nh;

	im_data = evas_object_image_data_get(EWL_IMAGE(image)->image, FALSE);
	evas_object_image_size_get(EWL_IMAGE(image)->image, &ew, &eh);
	
	index = 0;

	im_data_new = malloc(sizeof(unsigned int) * ew * eh);

	nw = eh;
	nh = ew;

	for (i = 0; i < nh; i++)
	{
		for (j = 0; j < nw; j++)
		{
			ni = j;
			nj = nh - i - 1;

			ind = ni * nh + nj;

			im_data_new[index] = im_data[ind];

			index++;
		}
	}
	return im_data_new;
}

unsigned int *rotate_right(Ewl_Widget *image)
{
	unsigned int *im_data, *im_data_new;
	int index, ind, i, j, ni, nj, ew, eh, nw, nh;

	im_data = evas_object_image_data_get(EWL_IMAGE(image)->image, FALSE);
	evas_object_image_size_get(EWL_IMAGE(image)->image, &ew, &eh);

	index = 0;

	im_data_new = malloc(sizeof(unsigned int) * ew * eh);

	nw = eh;
	nh = ew;

	for (i = 0; i < nh; i++)
	{
		for (j = 0; j < nw; j++)
		{
			ni = nw - j - 1;
			nj = i;

			ind = ni * nh + nj;

			im_data_new[index] = im_data[ind];
			
			index++;
		}
	}
	return im_data_new;
}

void update_image(Ewl_Widget *image, int w, int h, unsigned int *data)
{
	if (w && h && !data)
	{
		ewl_image_size_set(EWL_IMAGE(image), w, h);
		ewl_widget_configure(image);
	}
	if (w && h && data)
	{
		evas_object_image_size_set(EWL_IMAGE(image)->image, w, h);
		evas_object_image_data_set(EWL_IMAGE(image)->image, data);
		evas_object_image_data_update_add(EWL_IMAGE(image)->image, 0, 0, w, h);
	}
}

static void close_dialog(Ewl_Widget *w, void *event, void *data)
{
	ewl_widget_destroy(save_win);
}

static void save_image(Ewl_Widget *w, void *event, void *data)
{
	const char *file;
	char flags[PATH_MAX];
	pid_t pid;

	file = ewl_text_text_get(EWL_TEXT(data)); 
	snprintf(flags, PATH_MAX, "quality=%f compression=%f", ewl_range_value_get(EWL_RANGE(qseek)), 
							       ewl_range_value_get(EWL_RANGE(cseek)));

	if(!file) return;

	if(VISIBLE(em->eimage) && file)
	{
		pid = fork();
		if(pid == 0)
		{
			evas_object_image_save(EWL_IMAGE(em->eimage)->image, file, NULL, flags);
			exit(0);
		}
	}

	ewl_widget_destroy(save_win);

	return;
}

void save_dialog(const char *file)
{
        Ewl_Widget *vbox, *hbox, *button, *entry;
        
	save_win = add_window("Save Image", 300, 100, close_dialog, NULL);
        
	vbox = add_box(save_win, EWL_ORIENTATION_VERTICAL, 5);
	ewl_object_fill_policy_set(EWL_OBJECT(vbox), EWL_FLAG_FILL_ALL);

	add_label(vbox, "Save As:");

	entry = add_entry(vbox, "default.jpg", NULL, NULL);

	add_label(vbox, "Quality:");

	qseek = ewl_hseeker_new();
	ewl_range_minimum_value_set(EWL_RANGE(qseek), 0);
	ewl_range_maximum_value_set(EWL_RANGE(qseek), 100);
	ewl_range_step_set(EWL_RANGE(qseek), 10);
	ewl_range_value_set(EWL_RANGE(qseek), 80);
	ewl_container_child_append(EWL_CONTAINER(vbox), qseek);
	ewl_widget_show(qseek);

	add_label(vbox, "Compression:");

	cseek = ewl_hseeker_new();
	ewl_range_minimum_value_set(EWL_RANGE(cseek), 0);
	ewl_range_maximum_value_set(EWL_RANGE(cseek), 9);
	ewl_range_step_set(EWL_RANGE(cseek), 1);
	ewl_range_value_set(EWL_RANGE(cseek), 9);
	ewl_container_child_append(EWL_CONTAINER(vbox), cseek);
	ewl_widget_show(cseek);	
	
	hbox = add_box(vbox, EWL_ORIENTATION_HORIZONTAL, 5);
	ewl_object_alignment_set(EWL_OBJECT(hbox), EWL_FLAG_ALIGN_CENTER);
	ewl_object_fill_policy_set(EWL_OBJECT(hbox), EWL_FLAG_FILL_SHRINK);

	button = add_button(hbox, "Save", PACKAGE_DATA_DIR "/images/stock_save.png", save_image, entry);
	ewl_button_image_size_set(EWL_BUTTON(button), 25, 25);	

	button = add_button(hbox, "Close", PACKAGE_DATA_DIR "/images/dialog-close.png", close_dialog, NULL);
        ewl_button_image_size_set(EWL_BUTTON(button), 25, 25);

	return;
}
   
