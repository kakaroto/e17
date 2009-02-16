#include "ephoto.h"
#include "ephoto_imaging.h"

unsigned int *rotate_left(Ewl_Widget *image) {
	unsigned int *im_data, *im_data_new;
	int index, ind, i, j, ni, nj, ew, eh, nw, nh;

	im_data = evas_object_image_data_get(EWL_IMAGE(image)->image, FALSE);
	evas_object_image_size_get(EWL_IMAGE(image)->image, &ew, &eh);
	
	index = 0;

	im_data_new = malloc(sizeof(unsigned int) * ew * eh);

	nw = eh;
	nh = ew;

	for (i = 0; i < nh; i++) {
		for (j = 0; j < nw; j++) {
			ni = j;
			nj = nh - i - 1;

			ind = ni * nh + nj;

			im_data_new[index] = im_data[ind];

			index++;
		}
	}
	return im_data_new;
}

unsigned int *rotate_right(Ewl_Widget *image) {
	unsigned int *im_data, *im_data_new;
	int index, ind, i, j, ni, nj, ew, eh, nw, nh;

	im_data = evas_object_image_data_get(EWL_IMAGE(image)->image, FALSE);
	evas_object_image_size_get(EWL_IMAGE(image)->image, &ew, &eh);

	index = 0;

	im_data_new = malloc(sizeof(unsigned int) * ew * eh);

	nw = eh;
	nh = ew;

	for (i = 0; i < nh; i++) {
		for (j = 0; j < nw; j++) {
			ni = nw - j - 1;
			nj = i;

			ind = ni * nh + nj;

			im_data_new[index] = im_data[ind];
			
			index++;
		}
	}
	return im_data_new;
}

unsigned int *flip_horizontal(Ewl_Widget *image) {
	unsigned int *im_data, *im_data_new;
	int index, ind, i, j, ni, nj, ew, eh;

	im_data = evas_object_image_data_get(EWL_IMAGE(image)->image, FALSE);
	evas_object_image_size_get(EWL_IMAGE(image)->image, &ew, &eh);

	index = 0;

	im_data_new = malloc(sizeof(unsigned int) * ew * eh);

	for (i = 0; i < eh; i ++) {
		for (j = 0; j < ew; j++) {
			ni = i;
			nj = ew - j - 1;

			ind = ni * ew + nj;

			im_data_new[index] = im_data[ind];

			index++;
		}
	}
	return im_data_new;
}

unsigned int *flip_vertical(Ewl_Widget *image) {
	unsigned int *im_data, *im_data_new;
	int index, ind, i, j, ni, nj, ew, eh;

	im_data = evas_object_image_data_get(EWL_IMAGE(image)->image, FALSE);
	evas_object_image_size_get(EWL_IMAGE(image)->image, &ew, &eh);

	index = 0;

	im_data_new = malloc(sizeof(unsigned int) * ew * eh);

	for (i = 0; i < eh; i++) {
		for (j = 0; j < ew; j++) {
			ni = eh - i - 1;
			nj = j;

			ind = ni * ew + nj;

			im_data_new[index] = im_data[ind];

			index++;
		}
	}
	return im_data_new;
}

unsigned int *blur_image(Ewl_Widget *image) {
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

	for (y = 0; y < h; y++) {
		my = y - rad;
		mh = (rad << 1) + 1;
		if (my < 0) {
			mh += my;
			my = 0;
		}
		if ((my + mh) > h) {
			mh = h - my;
		}
		p1 = im_data_new + (y * w);
		memset(as, 0, w * sizeof(int));
		memset(rs, 0, w * sizeof(int));
		memset(gs, 0, w * sizeof(int));
		memset(bs, 0, w * sizeof(int));

		for (yy = 0; yy < mh; yy++) {
			p2 = im_data + ((yy + my) * w);
			for (x = 0; x < w; x++) {
				as[x] += (*p2 >> 24) & 0xff;
				rs[x] += (*p2 >> 16) & 0xff;
				gs[x] += (*p2 >> 8) & 0xff;
				bs[x] += *p2 & 0xff;
				p2++;
			}
		}
		if (w > ((rad << 1) + 1)) {
			for (x = 0; x < w; x++) {
				a = 0;
				r = 0;
				g = 0;
				b = 0;
				mx = x - rad;
				mw = (rad << 1) + 1;
				if (mx < 0) {
					mw += mx;
					mx = 0;
				}
				if ((mx + mw) > w) {
					mw = w - mx;
				}
				mt = mw * mh;
				for (xx = mx; xx < (mw + mx); xx++) {
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

unsigned int *sharpen_image(Ewl_Widget *image) {
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
	for (y = 1; y < (h - 1); y ++) {
		p1 = im_data + 1 + (y * w);
		p2 = im_data_new + 1 + (y * w);
		for (x = 1; x < (w - 1); x++) {
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

unsigned int *grayscale_image(Ewl_Widget *image) {
        unsigned int *im_data, *im_data_new;
        int gray, i, r, g, b, a, ew, eh;

        im_data = evas_object_image_data_get(EWL_IMAGE(image)->image, FALSE);
        evas_object_image_size_get(EWL_IMAGE(image)->image, &ew, &eh);

        im_data_new = malloc(sizeof(unsigned int) * ew * eh);

        for (i = 0; i < (ew * eh); i++) {
	        b = (int)((im_data[i]) & 0xff);
	        g = (int)((im_data[i] >> 8) & 0xff);
	        r = (int)((im_data[i] >> 16) & 0xff);
		a = (int)((im_data[i] >> 24) & 0xff);

		if (a > 0 && a < 255) {
			b = b * (255 / a);
			g = g * (255 / a);
			r = r * (255 / a);
		}

		gray = (int)((0.3 * r) + (0.59 * g) + (0.11 * b));

		if (a >= 0 && a < 255) gray = (gray * a) / 255;
	
		im_data_new[i] = (a << 24) | (gray << 16) | (gray << 8) | gray;
        }
        return im_data_new;
}

unsigned int *sepia_image(Ewl_Widget *image) {
	unsigned int *im_data, *im_data_new;
	int i, r, rr, g, gg, b, bb, a, ew, eh;

        im_data = evas_object_image_data_get(EWL_IMAGE(image)->image, FALSE); 
        evas_object_image_size_get(EWL_IMAGE(image)->image, &ew, &eh);

        im_data_new = malloc(sizeof(unsigned int) * ew * eh);

	for (i = 0; i < (ew * eh); i++) {
                b = (int)((im_data[i]) & 0xff);
                g = (int)((im_data[i] >> 8) & 0xff);
                r = (int)((im_data[i] >> 16) & 0xff);
                a = (int)((im_data[i] >> 24) & 0xff);

		if (a > 0 && a < 255) {
			b = b * (255 / a);
			g = g * (255 / a);
			r = r * (255 / a);
		}
		
		rr = (int)(((r + g + b)/3)+40);
		if (rr < 0) rr = 0;
		if (rr > 255) rr = 255;
		gg = (int)(((r + g + b)/3)+2);
		if (gg < 0) gg = 0;
		if (gg > 255) gg = 255;
		bb = (int)(((r + g + b)/3)+2);              
		if (bb < 0) bb = 0;
		if (bb > 255) bb = 255;

		if (a >= 0 && a < 255) {
			rr = (rr * a) / 255;
			gg = (gg * a) / 255;
			bb = (bb * a) / 255;
		}

                im_data_new[i] = (a << 24) | (rr << 16) | (gg << 8) | bb;
        }
	return im_data_new;
}

void update_image(Ewl_Widget *image, int w, int h, 
					unsigned int *data) {
        if (!w || !h)
		return;
	if (data) {
		evas_object_image_size_set(EWL_IMAGE(image)->image, w, h);
		evas_object_image_data_set(EWL_IMAGE(image)->image, data);
		evas_object_image_data_update_add(EWL_IMAGE(image)->image, 0, 0,
									 w, h);	
	}
        else {
		ewl_image_size_set(EWL_IMAGE(image), w, h);
                ewl_widget_configure(image);
        }
}

