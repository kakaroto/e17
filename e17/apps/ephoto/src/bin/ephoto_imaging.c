#include "ephoto.h"

#define R_CMOD(r) \
        red[(int)(r)]
#define G_CMOD(g) \
        green[(int)(g)]
#define B_CMOD(b) \
        blue[(int)(b)] \
/*#define A_CMOD(a) \
        alpha[(int)(a)]*/

#define A_VAL(p) ((unsigned char *)(p))[3]
#define R_VAL(p) ((unsigned char *)(p))[2]
#define G_VAL(p) ((unsigned char *)(p))[1]
#define B_VAL(p) ((unsigned char *)(p))[0]

//static unsigned int *set_contrast(unsigned int *data, int ew, int eh, float v);

static void close_dialog(Ewl_Widget *w, void *event, void *data);
static void close_progress(Ewl_Widget *w, void *event, void *data);
static void save_clicked(Ewl_Widget *w, void *event, void *data);
static void save_image(Ewl_Widget *w, void *event, void *data);

static Ewl_Widget *save_win, *qseek;

/*static unsigned int *set_contrast(unsigned int *data, int ew, int eh, float v)
{
	int i, val;
	unsigned int *p;
	unsigned char red[256], green[256], blue[256], alpha[256];

        for (i = 0; i < 256; i++)
        {
                red[i] = (unsigned char)i;
                green[i] = (unsigned char)i;
                blue[i] = (unsigned char)i;
                alpha[i] = (unsigned char)i;
        }
        for (i = 0; i < 256; i++)
        {
                val = (int)(((double)red[i] - 127) * v) + 127;
                if (val < 0) val = 0;
                if (val > 255) val = 255;
                red[i] = (unsigned char)val;

                val = (int)(((double)green[i] - 127) * v) + 127;
                if (val < 0) val = 0;
                if (val > 255) val = 255;
                green[i] = (unsigned char)val;

                val = (int)(((double)blue[i] - 127) * v) + 127;
                if (val < 0) val = 0;
                if (val > 255) val = 255;
                blue[i] = (unsigned char)val;

                val = (int)(((double)alpha[i] - 127) * v) + 127;
                if (val < 0) val = 0;
                if (val > 255) val = 255;
                alpha[i] = (unsigned char)val;
        }
        for (i = 0; i < (ew * eh); i++)
        {
                p = &data[i];

                R_VAL(p) = R_CMOD(R_VAL(p));
                G_VAL(p) = G_CMOD(G_VAL(p));
                B_VAL(p) = B_CMOD(B_VAL(p));
  //              A_VAL(p) = A_CMOD(A_VAL(p));
	}
	return data;
}*/

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

unsigned int *blur_image(Ewl_Widget *image)
{
	unsigned int *im_data, *im_data_new, *p1, *p2;
	int rad = 2;
	int x, y, w, h, mx, my, mw, mh, mt, xx, yy;
	int a, r, g, b;
	int *as, *rs, *gs, *bs;

	im_data = evas_object_image_data_get(EWL_IMAGE(image)->image, FALSE);
        evas_object_image_size_get(EWL_IMAGE(image)->image, &w, &h);

	im_data_new = malloc(sizeof(unsigned int) * w * h);
	as = malloc(sizeof(int) * w);
	rs = malloc(sizeof(int) * w);
	gs = malloc(sizeof(int) * w);
	bs = malloc(sizeof(int) * w);

	for (y = 0; y < h; y++)
	{
		my = y - rad;
		mh = (rad << 1) + 1;
		if (my < 0)
		{
			mh += my;
			my = 0;
		}
		if ((my + mh) > h)
		{
			mh = h - my;
		}
		p1 = im_data_new + (y * w);
		memset(as, 0, w * sizeof(int));
		memset(rs, 0, w * sizeof(int));
		memset(gs, 0, w * sizeof(int));
		memset(bs, 0, w * sizeof(int));

		for (yy = 0; yy < mh; yy++)
		{
			p2 = im_data + ((yy + my) * w);
			for (x = 0; x < w; x++)
			{
				as[x] += (*p2 >> 24) & 0xff;
				rs[x] += (*p2 >> 16) & 0xff;
				gs[x] += (*p2 >> 8) & 0xff;
				bs[x] += *p2 & 0xff;
				p2++;
			}
		}
		if (w > ((rad << 1) + 1))
		{
			for (x = 0; x < w; x++)
			{
				a = 0;
				r = 0;
				g = 0;
				b = 0;
				mx = x - rad;
				mw = (rad << 1) + 1;
				if (mx < 0)
				{
					mw += mx;
					mx = 0;
				}
				if ((mx + mw) > w)
				{
					mw = w - mx;
				}
				mt = mw * mh;
				for (xx = mx; xx < (mw + mx); xx++)
				{
					a += as[xx];
					r += rs[xx];
					g += gs[xx];
					b += bs[xx];
				}
				a = a / mt;
				r = r / mt;
				g = g / mt;
				b = b / mt;
				*p1 = (a << 24) | (r << 16) | (g << 8) | b;
				p1 ++;
			}
		}
	}
	free(as);
	free(rs);
	free(gs);
	free(bs);
	
	return im_data_new;
}

unsigned int *sharpen_image(Ewl_Widget *image)
{
	unsigned int *im_data, *im_data_new, *p1, *p2;
	int a, r, g, b, x, y, w, h;
	int mul, mul2, tot;
	int rad = 2;

        im_data = evas_object_image_data_get(EWL_IMAGE(image)->image, FALSE);
        evas_object_image_size_get(EWL_IMAGE(image)->image, &w, &h);

        im_data_new = malloc(sizeof(unsigned int) * w * h);

	mul = (rad * 4) + 1;
	mul2 = rad;
	tot = mul - (mul2 * 4);
	for (y = 1; y < (h - 1); y ++)
	{
		p1 = im_data + 1 + (y * w);
		p2 = im_data_new + 1 + (y * w);
		for (x = 1; x < (w - 1); x++)
		{
			b = (int)((p1[0]) & 0xff) * 5;
			g = (int)((p1[0] >> 8) & 0xff) * 5;
			r = (int)((p1[0] >> 16) & 0xff) * 5;
			a = (int)((p1[0] >> 24) & 0xff) * 5;
			b -= (int)((p1[-1]) & 0xff);
			g -= (int)((p1[-1] >> 8) & 0xff);
			r -= (int)((p1[-1] >> 16) & 0xff);
			a -= (int)((p1[-1] >> 24) & 0xff);
			b -= (int)((p1[1]) & 0xff);
			g -= (int)((p1[1] >> 8) & 0xff);
			r -= (int)((p1[1] >> 16) & 0xff);
			a -= (int)((p1[1] >> 24) & 0xff);
			b -= (int)((p1[-w]) & 0xff);
			g -= (int)((p1[-w] >> 8) & 0xff);
			r -= (int)((p1[-w] >> 16) & 0xff);
			a -= (int)((p1[-w] >> 24) & 0xff);
			b -= (int)((p1[-w]) & 0xff);
			g -= (int)((p1[-w] >> 8) & 0xff);
			r -= (int)((p1[-w] >> 16) & 0xff);
			a -= (int)((p1[-w] >> 24) & 0xff);

			a = (a & ((~a) >> 16));
			a = ((a | ((a & 256) - ((a & 256) >> 8))));
			r = (r & ((~r) >> 16));
			r = ((r | ((r & 256) - ((r & 256) >> 8))));
			g = (g & ((~g) >> 16));
			g = ((g | ((g & 256) - ((g & 256) >> 8))));
			b = (b & ((~b) >> 16));
			b = ((b | ((b & 256) - ((b & 256) >> 8))));

			*p2 = (a << 24) | (r << 16) | (g << 8) | b;
			p2++;
			p1++;
		}
	}
	return im_data_new;
}

unsigned int *grayscale_image(Ewl_Widget *image)
{
        unsigned int *im_data, *im_data_new;
        int gray, i, r, g, b, a, ew, eh;

        im_data = evas_object_image_data_get(EWL_IMAGE(image)->image, FALSE);
        evas_object_image_size_get(EWL_IMAGE(image)->image, &ew, &eh);

        im_data_new = malloc(sizeof(unsigned int) * ew * eh);

        for (i = 0; i < (ew * eh); i++)
        {
	        b = (int)((im_data[i]) & 0xff);
	        g = (int)((im_data[i] >> 8) & 0xff);
	        r = (int)((im_data[i] >> 16) & 0xff);
		a = (int)((im_data[i] >> 24) & 0xff);

		gray = (int)((0.3 * r) + (0.59 * g) + (0.11 * b));
	
		im_data_new[i] = (a << 24) | (gray << 16) | (gray << 8) | gray;
        }
        return im_data_new;
}

unsigned int *sepia_image(Ewl_Widget *image)
{
	unsigned int *im_data, *im_data_new;
	int i, r, g, b, a, ew, eh;
	float h, s, v;

        im_data = evas_object_image_data_get(EWL_IMAGE(image)->image, FALSE);
        evas_object_image_size_get(EWL_IMAGE(image)->image, &ew, &eh);

        im_data_new = malloc(sizeof(unsigned int) * ew * eh);

	for (i = 0; i < (ew * eh); i++)
        {
                b = (int)((im_data[i]) & 0xff);
                g = (int)((im_data[i] >> 8) & 0xff);
                r = (int)((im_data[i] >> 16) & 0xff);
                a = (int)((im_data[i] >> 24) & 0xff);

                evas_color_rgb_to_hsv(r, g, b, &h, &s, &v);
		evas_color_hsv_to_rgb(35, s, v, &r, &g, &b);

                im_data_new[i] = (a << 24) | (r << 16) | (g << 8) | b;
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
		evas_object_image_data_update_add(EWL_IMAGE(image)->image, 0, 0,
									 w, h);
	}
}

static void close_dialog(Ewl_Widget *w, void *event, void *data)
{
	ewl_widget_destroy(save_win);
}

static void close_progress(Ewl_Widget *w, void *event, void *data)
{
	ewl_widget_destroy(w);
}

static void save_image(Ewl_Widget *w, void *event, void *data)
{
	const char *file;
	char flags[PATH_MAX];
	char *ext;

	file = data;

	ext = strrchr(file, '.')+1;
	if (!strncmp(ext, "png", 3))
	{
		snprintf(flags, PATH_MAX, "compress=%i",
                 	       (int)ewl_range_value_get(EWL_RANGE(qseek)));
	}
	else
	{
		double svalue;
		float jvalue;
 
		svalue = ewl_range_value_get(EWL_RANGE(qseek));
		jvalue = (svalue / 9) * 100;

		snprintf(flags, PATH_MAX, "quality=%.0f", jvalue);
	}

	if(!file) return;

        if(VISIBLE(em->eimage))
        {
        	evas_object_image_save(EWL_IMAGE(em->eimage)->image,
                                                        file, NULL, flags);
        }
	
	ewl_widget_destroy(EWL_WIDGET(w));
}

static void save_clicked(Ewl_Widget *w, void *event, void *data)
{
	char *file;

	Ewl_Widget *pwin, *vbox, *label, *pbar;

	file = ewl_text_text_get(EWL_TEXT(data)); 
	
	if(!file) return;

	ewl_widget_destroy(save_win);

	pwin = add_window("Save Progress", 200, 75, close_progress, NULL);
	ewl_callback_append(pwin, EWL_CALLBACK_SHOW, save_image, file);

	vbox = add_box(pwin, EWL_ORIENTATION_VERTICAL, 5);

	label = add_label(vbox, "Save Progress");

	pbar = ewl_progressbar_new();
	ewl_progressbar_label_set(EWL_PROGRESSBAR(pbar), "Saving...");
	ewl_range_unknown_set(EWL_RANGE(pbar), 1);
	ewl_container_child_append(EWL_CONTAINER(vbox), pbar);
	ewl_widget_show(pbar);

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

	add_label(vbox, "Quality/Compression:");

	qseek = ewl_hseeker_new();
	ewl_range_minimum_value_set(EWL_RANGE(qseek), 1);
	ewl_range_maximum_value_set(EWL_RANGE(qseek), 9);
	ewl_range_step_set(EWL_RANGE(qseek), 1);
	ewl_range_value_set(EWL_RANGE(qseek), 7);
	ewl_container_child_append(EWL_CONTAINER(vbox), qseek);
	ewl_widget_show(qseek);

	hbox = add_box(vbox, EWL_ORIENTATION_HORIZONTAL, 5);
	ewl_object_alignment_set(EWL_OBJECT(hbox), EWL_FLAG_ALIGN_CENTER);
	ewl_object_fill_policy_set(EWL_OBJECT(hbox), EWL_FLAG_FILL_SHRINK);

	button = add_button(hbox, "Save", 
				PACKAGE_DATA_DIR "/images/stock_save.png", 
							save_clicked, entry);
	ewl_button_image_size_set(EWL_BUTTON(button), 25, 25);	

	button = add_button(hbox, "Close", 
				PACKAGE_DATA_DIR "/images/dialog-close.png", 
							close_dialog, NULL);
        ewl_button_image_size_set(EWL_BUTTON(button), 25, 25);

	return;
}
   
