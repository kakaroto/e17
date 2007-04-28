#include "ephoto.h"

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

void save_image(Ewl_Widget *image, const char *file)
{
	pid_t pid;

	if(image && file)
	{
		pid = fork();
		if(pid == 0)
		{
			evas_object_image_save(EWL_IMAGE(image)->image, file, NULL, NULL);
			exit(0);
		}
	}
	return;
}
