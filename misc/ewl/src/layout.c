#include "layout.h"

EwlRect *ewl_rect_new()
{
	EwlRect *r = malloc(sizeof(EwlRect));
	FUNC_BGN("ewl_rect_new");
	if (!r)	{
		ewl_debug("ewl_rect_new", EWL_NULL_ERROR, "r");
	} else {
		r->x = 0;
		r->y = 0;
		r->w = -1;
		r->h = -1;
	}
	FUNC_END("ewl_rect_new");
	return r;
}

EwlRect *ewl_rect_new_with_values(int *x, int *y, int *w, int *h)
{
	EwlRect *r = malloc(sizeof(EwlRect));
	FUNC_BGN("ewl_rect_new_with_values");
	if (!r)	{
		ewl_debug("ewl_rect_new_with_values", EWL_NULL_ERROR, "r");
	} else {
		r->x = x?*x:0;
		r->y = y?*y:0;
		r->w = w?*w:-1;
		r->h = h?*h:-1;
	}
	FUNC_END("ewl_rect_new_with_values");
	return r;
}

EwlRect *ewl_rect_dup(EwlRect *r)
{
	EwlRect *tr = NULL;
	FUNC_BGN("ewl_rect_dup");
	if (!r)	{
		ewl_debug("ewl_rect_dup", EWL_NULL_ERROR, "r");
	} else {
		tr = ewl_rect_new_with_values(&r->x, &r->y, &r->w, &r->h);
	}
	FUNC_END("ewl_rect_dup");
	return tr;
}

void     ewl_rect_free(EwlRect *r)
{
	FUNC_BGN("ewl_rect_free");
	if (!r)	{
		ewl_debug("ewl_rect_free", EWL_NULL_ERROR, "r");
	} else {
		free(r);
		r = NULL;
	}
	FUNC_END("ewl_rect_free");
	return;
}

void     ewl_rect_set(EwlRect *r, int *x, int *y, int *w, int *h)
{
	FUNC_BGN("ewl_rect_set");
	if (!r)	{
		ewl_debug("ewl_rect_set", EWL_NULL_ERROR, "r");
	} else {
		if (x) r->x = *x;
		if (y) r->y = *y;
		if (w) r->w = *w;
		if (h) r->h = *h;
	}
	FUNC_END("ewl_rect_set");
	return;
}

void     ewl_rect_get(EwlRect *r, int *x, int *y, int *w, int *h)
{
	FUNC_BGN("ewl_rect_get");
	if (!r)	{
		ewl_debug("ewl_rect_get", EWL_NULL_ERROR, "r");
	} else {
		if (x) *x = r->x;
		if (y) *y = r->y;
		if (w) *w = r->w;
		if (h) *h = r->h;
	}
	return;
	FUNC_END("ewl_rect_get");
}

void     ewl_rect_set_rect(EwlRect *dst, EwlRect *src)
{
	FUNC_BGN("ewl_rect_set_rect");
	if (!dst)	{
		ewl_debug("ewl_rect_set_rect", EWL_NULL_ERROR, "dst");
	} else if (!src) {
		ewl_debug("ewl_rect_set_rect", EWL_NULL_ERROR, "src");
	} else {
		dst->x = src->x;
		dst->y = src->y;
		dst->w = src->w;
		dst->h = src->h;
	}
	FUNC_END("ewl_rect_set_rect");
	return;
}

void       ewl_rect_dump(EwlRect *r)
{
	FUNC_BGN("ewl_rect_dump");
	if (!r)	{
		ewl_debug("ewl_rect_dump", EWL_NULL_ERROR, "r");
	} else {
		fprintf(stderr,"x = %d, y = %d, w = %d, h = %d\n",r->x,r->y,r->w,r->h);
	}
	FUNC_END("ewl_rect_dump");
	return;
}

EwlRRect *ewl_rrect_new()
{
	EwlRRect *r = NULL;
	FUNC_BGN("ewl_rrect_new");
	r = ewl_rrect_new_with_values(NULL,NULL,NULL,NULL, NULL,NULL,NULL,NULL);
	if (!r)	{
		ewl_debug("ewl_rrect_new", EWL_NULL_ERROR, "r");
	}
	FUNC_END("ewl_rrect_new");
	return r;
}

EwlRRect *ewl_rrect_new_with_values(int *txr, int *txa, int *tyr, int *tya,
                                    int *bxr, int *bxa, int *byr, int *bya)
{
	EwlRRect *r = malloc(sizeof(EwlRRect));
	FUNC_BGN("ewl_rrect_new_with_values");
	if (!r)	{
		ewl_debug("ewl_rrect_new_with_values", EWL_NULL_ERROR, "r");
	} else {
		r->x[EWL_TOP_RELATIVE] = txr?*txr:0;
		r->x[EWL_TOP_ABSOLUTE] = txa?*txa:0;
		r->y[EWL_TOP_RELATIVE] = tyr?*tyr:0;
		r->y[EWL_TOP_ABSOLUTE] = tya?*tya:0;
		r->x[EWL_BOT_RELATIVE] = bxr?*bxr:8192;
		r->x[EWL_BOT_ABSOLUTE] = bxa?*bxa:0;
		r->y[EWL_BOT_RELATIVE] = byr?*byr:8192;
		r->y[EWL_BOT_ABSOLUTE] = bya?*bya:0;
	}
	FUNC_END("ewl_rrect_new_with_values");
	return r;
}

EwlRRect *ewl_rrect_dup(EwlRRect *r)
{
	EwlRRect *rr = NULL;
	FUNC_BGN("ewl_rrect_dup");
	if (!r)	{
		ewl_debug("ewl_rrect_dup", EWL_NULL_ERROR, "r");
	} else {
		rr = ewl_rrect_new_with_values(&(r->x[EWL_TOP_RELATIVE]),
		                               &(r->x[EWL_TOP_ABSOLUTE]),
		                               &(r->y[EWL_TOP_RELATIVE]),
		                               &(r->y[EWL_TOP_ABSOLUTE]),
		                               &(r->x[EWL_BOT_RELATIVE]),
		                               &(r->x[EWL_BOT_ABSOLUTE]),
		                               &(r->y[EWL_BOT_RELATIVE]),
		                               &(r->y[EWL_BOT_ABSOLUTE]));
		if (!rr)	{
			ewl_debug("ewl_rrect_dup", EWL_NULL_ERROR, "rr");
		}
	}
	FUNC_END("ewl_rrect_dup");
	return rr;
}

void      ewl_rrect_free(EwlRRect *r)
{
	FUNC_END("ewl_rrect_free");
	if (!r)	{
		ewl_debug("ewl_rrect_free", EWL_NULL_ERROR, "r");
	} else {
		free(r);
		r = NULL;
	}
	FUNC_END("ewl_rrect_free");
	return;
}

void      ewl_rrect_set(EwlRRect *r, int *txr, int *txa, int *tyr, int *tya,
                                     int *bxr, int *bxa, int *byr, int *bya)
{
	FUNC_END("ewl_rrect_set");
	if (!r)	{
		ewl_debug("ewl_rrect_set", EWL_NULL_ERROR, "r");
	} else {
		if (txr) r->x[EWL_TOP_RELATIVE] = *txr;
		if (txa) r->x[EWL_TOP_ABSOLUTE] = *txa;
		if (tyr) r->y[EWL_TOP_RELATIVE] = *tyr;
		if (tya) r->y[EWL_TOP_ABSOLUTE] = *tya;
		if (bxr) r->x[EWL_BOT_RELATIVE] = *bxr;
		if (bxa) r->x[EWL_BOT_ABSOLUTE] = *bxa;
		if (byr) r->y[EWL_BOT_RELATIVE] = *byr;
		if (bya) r->y[EWL_BOT_ABSOLUTE] = *bya;
	}
	FUNC_END("ewl_rrect_set");
	return;
}

void      ewl_rrect_get(EwlRRect *r, int *txr, int *txa, int *tyr, int *tya,
                                     int *bxr, int *bxa, int *byr, int *bya)
{
	FUNC_END("ewl_rrect_get");
	if (!r)	{
		ewl_debug("ewl_rrect_get", EWL_NULL_ERROR, "r");
	} else {
		if (txr) *txr = r->x[EWL_TOP_RELATIVE];
		if (txa) *txa = r->x[EWL_TOP_ABSOLUTE];
		if (tyr) *tyr = r->y[EWL_TOP_RELATIVE];
		if (tya) *tya = r->y[EWL_TOP_ABSOLUTE];
		if (bxr) *bxr = r->x[EWL_BOT_RELATIVE];
		if (bxa) *bxa = r->x[EWL_BOT_ABSOLUTE];
		if (byr) *byr = r->y[EWL_BOT_RELATIVE];
		if (bya) *bya = r->y[EWL_BOT_ABSOLUTE];
	}
	FUNC_END("ewl_rrect_get");
	return;
}

void      ewl_rrect_set_rect(EwlRRect *dst, EwlRRect *src)
{
	FUNC_END("ewl_rrect_set_rect");
	if (!src)	{
		ewl_debug("ewl_rrect_set_rect", EWL_NULL_ERROR, "src");
	} else if (!dst)  {
		ewl_debug("ewl_rrect_set_rect", EWL_NULL_ERROR, "dst");
	} else {
		dst->x[EWL_TOP_RELATIVE] = src->x[EWL_TOP_RELATIVE];
		dst->x[EWL_TOP_ABSOLUTE] = src->x[EWL_TOP_ABSOLUTE];
		dst->y[EWL_TOP_RELATIVE] = src->y[EWL_TOP_RELATIVE];
		dst->y[EWL_TOP_ABSOLUTE] = src->y[EWL_TOP_ABSOLUTE];
		dst->x[EWL_BOT_RELATIVE] = src->x[EWL_BOT_RELATIVE];
		dst->x[EWL_BOT_ABSOLUTE] = src->x[EWL_BOT_ABSOLUTE];
		dst->y[EWL_BOT_RELATIVE] = src->y[EWL_BOT_RELATIVE];
		dst->y[EWL_BOT_ABSOLUTE] = src->y[EWL_BOT_ABSOLUTE];
	}
	FUNC_END("ewl_rrect_set_rect");
	return;
}

void      ewl_rrect_dump(EwlRRect *r)
{
	FUNC_END("ewl_rrect_dump");
	if (!r)	{
		ewl_debug("ewl_rrect_dump", EWL_NULL_ERROR, "r");
	} else {
		fprintf(stderr,"ewl_rrect_dump(0x%08x):\n"
		               "\ttxr = %d, txa = %d, tyr = %d, tya = %d\n"
		               "\tbxr = %d, bxa = %d, byr = %d, bya = %d\n",
		               (unsigned int) r,
		               r->x[EWL_TOP_RELATIVE],
		               r->x[EWL_TOP_ABSOLUTE],
		               r->y[EWL_TOP_RELATIVE],
		               r->y[EWL_TOP_ABSOLUTE],
		               r->x[EWL_BOT_RELATIVE],
		               r->x[EWL_BOT_ABSOLUTE],
		               r->y[EWL_BOT_RELATIVE],
		               r->y[EWL_BOT_ABSOLUTE]);
		               
	}
	FUNC_END("ewl_rrect_dump");
	return;
}






EwlLayout *ewl_layout_new()
{
	int x=0,   y=0,   w=-1,   h=-1,
	    rx=0,  ry=0,  rw=-1,  rh=-1,
	    mix=0, miy=0, miw=-1, mih=-1,
        max=0, may=0, maw=-1, mah=-1;
	EwlLayout *l = NULL;
	FUNC_BGN("ewl_layout_new");
	l = ewl_layout_new_with_values(&x,   &y,   &w,   &h,
	                               &rx,  &ry,  &rw,  &rh,
	                               &mix, &miy, &miw, &mih,
	                               &max, &may, &maw, &mah);
	if (!l)	{
		ewl_debug("ewl_layout_new", EWL_NULL_ERROR, "l");
	}
	FUNC_END("ewl_layout_new");
	return l;
}

EwlLayout *ewl_layout_new_with_values(int *x,   int *y,   int *w,   int *h,
                                      int *rx,  int *ry,  int *rw,  int *rh,
                                      int *mix, int *miy, int *miw, int *mih,
                                      int *max, int *may, int *maw, int *mah)
{
	EwlLayout *l = NULL;
	FUNC_BGN("ewl_layout_new_with_values");
	l = malloc(sizeof(EwlLayout));
	if (!l)	{
		ewl_debug("ewl_layout_new_with_values", EWL_NULL_ERROR, "l");
	} else {
		l->rect = ewl_rect_new_with_values(x, y, w, h);
		l->req =  ewl_rect_new_with_values(rx, ry, rw, rh);
		l->min =  ewl_rect_new_with_values(mix, miy, miw, mih);
		l->max =  ewl_rect_new_with_values(max, may, maw, mah);
	}
	FUNC_END("ewl_layout_new_with_values");
	return l;
}

EwlLayout *ewl_layout_new_with_rect_values(EwlRect *rect, EwlRect *req,
                                           EwlRect *min,  EwlRect *max)
{
	int x, y, w, h, rx, ry, rw, rh, mix, miy, miw, mih, maxx, may, maw, mah;
	EwlLayout *l = NULL;
	FUNC_BGN("ewl_layout_new_with_rect_values");
	x = rect?rect->x:0; y = rect?rect->y:0;
	w = rect?rect->w:-1; h = rect?rect->h:-1;
	rx = req?req->x:0; ry = req?req->y:0;
	rw = req?req->w:-1; rh = req?req->h:-1;
	mix = min?min->x:0; miy = min?min->y:0;
	miw = min?min->w:-1; mih = min?min->h:-1;
	maxx = max?max->x:0; may = max?max->y:0;
	maw = max?max->w:-1; mah = max?max->h:-1;
	l = ewl_layout_new_with_values(&x,   &y,   &w,   &h,
	                               &rx,  &ry,  &rw,  &rh,
	                               &mix, &miy, &miw, &mih,
	                               &maxx, &may, &maw, &mah);
	if (!l)	{
		ewl_debug("ewl_layout_new_with_rect_values", EWL_NULL_ERROR, "l");
	}
	FUNC_END("ewl_layout_new_with_rect_values");
	return l;
}

EwlLayout *ewl_layout_dup(EwlLayout *l)
{
	EwlLayout *nl = NULL;
	FUNC_BGN("ewl_layout_dup");
	if (!l)	{
		ewl_debug("ewl_layout_dup", EWL_NULL_ERROR, "l");
	} else {
		nl = ewl_layout_new_with_rect_values(l->rect, l->req, l->min, l->max);
		if (!nl)	{
			ewl_debug("ewl_layout_dup", EWL_NULL_ERROR, "nl");
		}
	}
	FUNC_END("ewl_layout_dup");
	return nl;
}

void       ewl_layout_free(EwlLayout *l)
{
	FUNC_BGN("ewl_layout_free");
	if (!l)	{
		ewl_debug("ewl_layout_free", EWL_NULL_ERROR, "l");
	} else {
		free(l);
		l = 0;
	}
	FUNC_BGN("ewl_layout_free");
	return;
}

void       ewl_layout_set(EwlLayout *l, int *x,   int *y,   int *w,   int *h,
                                        int *rx,  int *ry,  int *rw,  int *rh,
                                        int *mix, int *miy, int *miw, int *mih,
                                        int *max, int *may, int *maw, int *mah)
{
	FUNC_BGN("ewl_layout_set");
	if (!l)	{
		ewl_debug("ewl_layout_set", EWL_NULL_ERROR, "l");
	} else {
		if (l->rect) {
			if (x) l->rect->x = *x; if (y) l->rect->y = *y;
			if (w) l->rect->w = *w; if (h) l->rect->h = *h;
		} else {
			ewl_debug("ewl_layout_set", EWL_NULL_ERROR, "l->rect");
		}
		if (l->req) {
			if (rx) l->req->x = *rx; if (ry) l->req->y = *ry;
			if (rw) l->req->w = *rw; if (rh) l->req->h = *rh;
		} else {
			ewl_debug("ewl_layout_set", EWL_NULL_ERROR, "l->req");
		}
		if (l->min) {
			if (mix) l->min->x = *mix; if (miy) l->min->y = *miy;
			if (miw) l->min->w = *miw; if (mih) l->min->h = *mih;
		} else {
			ewl_debug("ewl_layout_set", EWL_NULL_ERROR, "l->min");
		}
		if (l->max) {
			if (max) l->max->x = *max; if (may) l->max->y = *may;
			if (maw) l->max->w = *maw; if (mah) l->max->h = *mah;
		} else {
			ewl_debug("ewl_layout_set", EWL_NULL_ERROR, "l->max");
		}
	}
	FUNC_END("ewl_layout_set");
	return;
}

void       ewl_layout_get(EwlLayout *l, int *x,   int *y,   int *w,   int *h,
                                        int *rx,  int *ry,  int *rw,  int *rh,
                                        int *mix, int *miy, int *miw, int *mih,
                                        int *max, int *may, int *maw, int *mah)
{
	FUNC_BGN("ewl_layout_get");
	if (!l)	{
		ewl_debug("ewl_layout_get", EWL_NULL_ERROR, "l");
	} else {
		if (l->rect) {
			if (x) *x = l->rect->x; if (y) *y = l->rect->y;
			if (w) *w = l->rect->w; if (h) *h = l->rect->h;
		} else {
			ewl_debug("ewl_layout_get", EWL_NULL_ERROR, "l->rect");
		}
		if (l->req) {
			if (rx) *rx = l->req->x; if (ry) *ry = l->req->y;
			if (rw) *rw = l->req->w; if (rh) *rh = l->req->h;
		} else {
			ewl_debug("ewl_layout_get", EWL_NULL_ERROR, "l->req");
		}
		if (l->min) {
			if (mix) *mix = l->min->x; if (miy) *miy = l->min->y;
			if (miw) *miw = l->min->w; if (mih) *mih = l->min->h;
		} else {
			ewl_debug("ewl_layout_get", EWL_NULL_ERROR, "l->min");
		}
		if (l->max) {
			if (max) *max = l->max->x; if (may) *may = l->max->y;
			if (maw) *maw = l->max->w; if (mah) *mah = l->max->h;
		} else {
			ewl_debug("ewl_layout_get", EWL_NULL_ERROR, "l->max");
		}
	}
	FUNC_END("ewl_layout_get");
	return;
}

void       ewl_layout_set_rects(EwlLayout *l, EwlRect *rect, EwlRect *req,
                                              EwlRect *min,  EwlRect *max)
{
	FUNC_BGN("ewl_layout_set_rects");
	if (!l)	{
			ewl_debug("ewl_layout_set_rects", EWL_NULL_ERROR, "l");
	} else {
		if (rect)	{
			if (l->rect) ewl_rect_free(l->rect);
			l->rect = ewl_rect_dup(rect);
		}
		if (req)	{
			if (l->req) ewl_rect_free(l->req);
			l->req = ewl_rect_dup(req);
		}
		if (min)	{
			if (l->min) ewl_rect_free(l->min);
			l->min = ewl_rect_dup(min);
		}
		if (max)	{
			if (l->max) ewl_rect_free(l->max);
			l->max = ewl_rect_dup(max);
		}
	}
	FUNC_END("ewl_layout_set_rects");
	return;
}

void       ewl_layout_get_rects(EwlLayout *l, EwlRect *rect, EwlRect *req,
                                              EwlRect *min,  EwlRect *max)
{
	FUNC_BGN("ewl_layout_get_rects");
	if (!l)	{
			ewl_debug("ewl_layout_get_rects", EWL_NULL_ERROR, "l");
	} else {
		if (rect) ewl_rect_set_rect(rect, l->rect);
		if (req)  ewl_rect_set_rect(req, l->req);
		if (min)  ewl_rect_set_rect(min, l->min);
		if (max)  ewl_rect_set_rect(max, l->max);
	}
	FUNC_END("ewl_layout_get_rects");
	return;
}

void       ewl_layout_set_rect(EwlLayout *l, EwlRect *r)
{
	FUNC_BGN("ewl_layout_set_rect");
	if (!l)	{
			ewl_debug("ewl_layout_set_rect", EWL_NULL_ERROR, "l");
	} else {
		if (l->rect) ewl_rect_free(l->rect);
		l->rect = ewl_rect_dup(r);
	}
	FUNC_END("ewl_layout_set_rect");
	return;
}

void       ewl_layout_set_req_rect(EwlLayout *l, EwlRect *r)
{
	FUNC_BGN("ewl_layout_set_req_rect");
	if (!l)	{
			ewl_debug("ewl_layout_set_req_rect", EWL_NULL_ERROR, "l");
	} else {
		if (l->req) ewl_rect_free(l->req);
		l->req = ewl_rect_dup(r);
	}
	FUNC_END("ewl_layout_set_req_rect");
	return;
}

void       ewl_layout_set_min_rect(EwlLayout *l, EwlRect *r)
{
	FUNC_BGN("ewl_layout_set_min_rect");
	if (!l)	{
			ewl_debug("ewl_layout_set_min_rect", EWL_NULL_ERROR, "l");
	} else {
		if (l->min) ewl_rect_free(l->min);
		l->min = ewl_rect_dup(r);
	}
	FUNC_END("ewl_layout_set_min_rect");
	return;
}

void       ewl_layout_set_max_rect(EwlLayout *l, EwlRect *r)
{
	FUNC_BGN("ewl_layout_set_max_rect");
	if (!l)	{
			ewl_debug("ewl_layout_set_max_rect", EWL_NULL_ERROR, "l");
	} else {
		if (l->max) ewl_rect_free(l->max);
		l->max = ewl_rect_dup(r);
	}
	FUNC_END("ewl_layout_set_max_rect");
	return;
}

EwlRect   *ewl_layout_get_rect(EwlLayout *l)
{
	EwlRect *r = NULL;
	FUNC_BGN("ewl_layout_get_rect");
	if (!l)	{
			ewl_debug("ewl_layout_get_rect", EWL_NULL_ERROR, "l");
	} else {
		r = ewl_rect_dup(l->rect);
		if (!r) {
			ewl_debug("ewl_layout_get_rect", EWL_NULL_ERROR, "r");
		}
	}
	FUNC_END("ewl_layout_get_rect");
	return r;
}

EwlRect   *ewl_layout_get_req_rect(EwlLayout *l)
{
	EwlRect *r = NULL;
	FUNC_BGN("ewl_layout_get_req_rect");
	if (!l)	{
			ewl_debug("ewl_layout_get_req_rect", EWL_NULL_ERROR, "l");
	} else {
		r = ewl_rect_dup(l->req);
		if (!r) {
			ewl_debug("ewl_layout_get_req_rect", EWL_NULL_ERROR, "r");
		}
	}
	FUNC_END("ewl_layout_get_req_rect");
	return r;
}

EwlRect   *ewl_layout_get_min_rect(EwlLayout *l)
{
	EwlRect *r = NULL;
	FUNC_BGN("ewl_layout_get_min_rect");
	if (!l)	{
			ewl_debug("ewl_layout_get_min_rect", EWL_NULL_ERROR, "l");
	} else {
		r = ewl_rect_dup(l->min);
		if (!r) {
			ewl_debug("ewl_layout_get_min_rect", EWL_NULL_ERROR, "r");
		}
	}
	FUNC_END("ewl_layout_get_min_rect");
	return r;
}

EwlRect   *ewl_layout_get_max_rect(EwlLayout *l)
{
	EwlRect *r = NULL;
	FUNC_BGN("ewl_layout_get_max_rect");
	if (!l)	{
			ewl_debug("ewl_layout_get_max_rect", EWL_NULL_ERROR, "l");
	} else {
		r = ewl_rect_dup(l->max);
		if (!r) {
			ewl_debug("ewl_layout_get_max_rect", EWL_NULL_ERROR, "r");
		}
	}
	FUNC_END("ewl_layout_get_max_rect");
	return r;
}

void       ewl_layout_set_x(EwlLayout *l, int x)
{
	FUNC_BGN("ewl_layout_set_x");
	if (!l)	{
		ewl_debug("ewl_layout_set_x", EWL_NULL_ERROR, "l");
	} else if (!l->rect) {
		ewl_debug("ewl_layout_set_x", EWL_NULL_ERROR, "l->rect");
	} else {
		ewl_layout_set(l, &x,0,0,0, 0,0,0,0, 0,0,0,0, 0,0,0,0);
	}
	FUNC_END("ewl_layout_set_x");
	return;
}

void       ewl_layout_set_y(EwlLayout *l, int y)
{
	FUNC_BGN("ewl_layout_set_y");
	if (!l)	{
		ewl_debug("ewl_layout_set_y", EWL_NULL_ERROR, "l");
	} else if (!l->rect) {
		ewl_debug("ewl_layout_set_y", EWL_NULL_ERROR, "l->rect");
	} else {
		ewl_layout_set(l, 0,&y,0,0, 0,0,0,0, 0,0,0,0, 0,0,0,0);
	}
	FUNC_END("ewl_layout_set_y");
	return;
}

void       ewl_layout_set_w(EwlLayout *l, int w)
{
	FUNC_BGN("ewl_layout_set_w");
	if (!l)	{
		ewl_debug("ewl_layout_set_w", EWL_NULL_ERROR, "l");
	} else if (!l->rect) {
		ewl_debug("ewl_layout_set_w", EWL_NULL_ERROR, "l->rect");
	} else {
		ewl_layout_set(l, 0,0,&w,0, 0,0,0,0, 0,0,0,0, 0,0,0,0);
	}
	FUNC_END("ewl_layout_set_w");
	return;
}

void       ewl_layout_set_h(EwlLayout *l, int h)
{
	FUNC_BGN("ewl_layout_set_h");
	if (!l)	{
		ewl_debug("ewl_layout_set_h", EWL_NULL_ERROR, "l");
	} else if (!l->rect) {
		ewl_debug("ewl_layout_set_h", EWL_NULL_ERROR, "l->rect");
	} else {
		ewl_layout_set(l, 0,0,0,&h, 0,0,0,0, 0,0,0,0, 0,0,0,0);
	}
	FUNC_END("ewl_layout_set_h");
	return;
}

void       ewl_layout_set_req_x(EwlLayout *l, int x)
{
	FUNC_BGN("ewl_layout_set_req_x");
	if (!l)	{
		ewl_debug("ewl_layout_set_req_x", EWL_NULL_ERROR, "l");
	} else if (!l->req) {
		ewl_debug("ewl_layout_set_req_x", EWL_NULL_ERROR, "l->req");
	} else {
		ewl_layout_set(l, 0,0,0,0, &x,0,0,0, 0,0,0,0, 0,0,0,0);
	}
	FUNC_END("ewl_layout_set_req_x");
	return;
}

void       ewl_layout_set_req_y(EwlLayout *l, int y)
{
	FUNC_BGN("ewl_layout_set_req_y");
	if (!l)	{
		ewl_debug("ewl_layout_set_req_y", EWL_NULL_ERROR, "l");
	} else if (!l->req) {
		ewl_debug("ewl_layout_set_req_y", EWL_NULL_ERROR, "l->req");
	} else {
		ewl_layout_set(l, 0,0,0,0, 0,&y,0,0, 0,0,0,0, 0,0,0,0);
	}
	FUNC_END("ewl_layout_set_req_y");
	return;
}

void       ewl_layout_set_req_w(EwlLayout *l, int w)
{
	FUNC_BGN("ewl_layout_set_req_w");
	if (!l)	{
		ewl_debug("ewl_layout_set_req_w", EWL_NULL_ERROR, "l");
	} else if (!l->req) {
		ewl_debug("ewl_layout_set_req_w", EWL_NULL_ERROR, "l->req");
	} else {
		ewl_layout_set(l, 0,0,0,0, 0,0,&w,0, 0,0,0,0, 0,0,0,0);
	}
	FUNC_END("ewl_layout_set_req_w");
	return;
}

void       ewl_layout_set_req_h(EwlLayout *l, int h)
{
	FUNC_BGN("ewl_layout_set_req_h");
	if (!l)	{
		ewl_debug("ewl_layout_set_req_h", EWL_NULL_ERROR, "l");
	} else if (!l->req) {
		ewl_debug("ewl_layout_set_req_h", EWL_NULL_ERROR, "l->req");
	} else {
		ewl_layout_set(l, 0,0,0,0, 0,0,0,&h, 0,0,0,0, 0,0,0,0);
	}
	FUNC_END("ewl_layout_set_req_h");
	return;
}

void       ewl_layout_set_min_x(EwlLayout *l, int x)
{
	FUNC_BGN("ewl_layout_set_min_x");
	if (!l)	{
		ewl_debug("ewl_layout_set_min_x", EWL_NULL_ERROR, "l");
	} else if (!l->min) {
		ewl_debug("ewl_layout_set_min_x", EWL_NULL_ERROR, "l->min");
	} else {
		ewl_layout_set(l, 0,0,0,0, 0,0,0,0, &x,0,0,0, 0,0,0,0);
	}
	FUNC_END("ewl_layout_set_min_x");
	return;
}

void       ewl_layout_set_min_y(EwlLayout *l, int y)
{
	FUNC_BGN("ewl_layout_set_min_y");
	if (!l)	{
		ewl_debug("ewl_layout_set_min_y", EWL_NULL_ERROR, "l");
	} else if (!l->min) {
		ewl_debug("ewl_layout_set_min_y", EWL_NULL_ERROR, "l->min");
	} else {
		ewl_layout_set(l, 0,0,0,0, 0,0,0,0, 0,&y,0,0, 0,0,0,0);
	}
	FUNC_END("ewl_layout_set_min_y");
	return;
}

void       ewl_layout_set_min_w(EwlLayout *l, int w)
{
	FUNC_BGN("ewl_layout_set_min_w");
	if (!l)	{
		ewl_debug("ewl_layout_set_min_w", EWL_NULL_ERROR, "l");
	} else if (!l->min) {
		ewl_debug("ewl_layout_set_min_w", EWL_NULL_ERROR, "l->min");
	} else {
		ewl_layout_set(l, 0,0,0,0, 0,0,0,0, 0,0,&w,0, 0,0,0,0);
	}
	FUNC_END("ewl_layout_set_min_w");
	return;
}

void       ewl_layout_set_min_h(EwlLayout *l, int h)
{
	FUNC_BGN("ewl_layout_set_min_h");
	if (!l)	{
		ewl_debug("ewl_layout_set_min_h", EWL_NULL_ERROR, "l");
	} else if (!l->min) {
		ewl_debug("ewl_layout_set_min_h", EWL_NULL_ERROR, "l->min");
	} else {
		ewl_layout_set(l, 0,0,0,0, 0,0,0,0, 0,0,0,&h, 0,0,0,0);
	}
	FUNC_END("ewl_layout_set_min_h");
	return;
}

void       ewl_layout_set_max_x(EwlLayout *l, int x)
{
	FUNC_BGN("ewl_layout_set_max_x");
	if (!l)	{
		ewl_debug("ewl_layout_set_max_x", EWL_NULL_ERROR, "l");
	} else if (!l->max) {
		ewl_debug("ewl_layout_set_max_x", EWL_NULL_ERROR, "l->max");
	} else {
		ewl_layout_set(l, 0,0,0,0, 0,0,0,0, 0,0,0,0, &x,0,0,0);
	}
	FUNC_END("ewl_layout_set_max_x");
	return;
}

void       ewl_layout_set_max_y(EwlLayout *l, int y)
{
	FUNC_BGN("ewl_layout_set_max_y");
	if (!l)	{
		ewl_debug("ewl_layout_set_max_y", EWL_NULL_ERROR, "l");
	} else if (!l->rect) {
		ewl_debug("ewl_layout_set_max_y", EWL_NULL_ERROR, "l->rect");
	} else {
		ewl_layout_set(l, 0,0,0,0, 0,0,0,0, 0,0,0,0, 0,&y,0,0);
	}
	FUNC_END("ewl_layout_set_max_y");
	return;
}

void       ewl_layout_set_max_w(EwlLayout *l, int w)
{
	FUNC_BGN("ewl_layout_set_max_w");
	if (!l)	{
		ewl_debug("ewl_layout_set_max_w", EWL_NULL_ERROR, "l");
	} else if (!l->max) {
		ewl_debug("ewl_layout_set_max_w", EWL_NULL_ERROR, "l->max");
	} else {
		ewl_layout_set(l, 0,0,0,0, 0,0,0,0, 0,0,0,0, 0,0,&w,0);
	}
	FUNC_END("ewl_layout_set_max_w");
	return;
}

void       ewl_layout_set_max_h(EwlLayout *l, int h)
{
	FUNC_BGN("ewl_layout_set_max_h");
	if (!l)	{
		ewl_debug("ewl_layout_set_max_h", EWL_NULL_ERROR, "l");
	} else if (!l->max) {
		ewl_debug("ewl_layout_set_max_h", EWL_NULL_ERROR, "l->max");
	} else {
		ewl_layout_set(l, 0,0,0,0, 0,0,0,0, 0,0,0,0, 0,0,0,&h);
	}
	FUNC_END("ewl_layout_set_max_h");
	return;
}

int        ewl_layout_get_x(EwlLayout *l)
{
	int x;
	FUNC_BGN("ewl_layout_get_x");
	if (!l)	{
		ewl_debug("ewl_layout_get_x", EWL_NULL_ERROR, "l");
	} else if (!l->rect) {
		ewl_debug("ewl_layout_get_x", EWL_NULL_ERROR, "l->rect");
	} else {
		ewl_layout_get(l, &x,0,0,0, 0,0,0,0, 0,0,0,0, 0,0,0,0);
	}
	FUNC_END("ewl_layout_get_x");
	return x;
}

int        ewl_layout_get_y(EwlLayout *l)
{
	int y;
	FUNC_BGN("ewl_layout_get_y");
	if (!l)	{
		ewl_debug("ewl_layout_get_y", EWL_NULL_ERROR, "l");
	} else if (!l->rect) {
		ewl_debug("ewl_layout_get_y", EWL_NULL_ERROR, "l->rect");
	} else {
		ewl_layout_get(l, 0,&y,0,0, 0,0,0,0, 0,0,0,0, 0,0,0,0);
	}
	FUNC_END("ewl_layout_get_y");
	return y;
}

int        ewl_layout_get_w(EwlLayout *l)
{
	int w;
	FUNC_BGN("ewl_layout_get_w");
	if (!l)	{
		ewl_debug("ewl_layout_get_w", EWL_NULL_ERROR, "l");
	} else if (!l->rect) {
		ewl_debug("ewl_layout_get_w", EWL_NULL_ERROR, "l->rect");
	} else {
		ewl_layout_get(l, 0,0,&w,0, 0,0,0,0, 0,0,0,0, 0,0,0,0);
	}
	FUNC_END("ewl_layout_get_w");
	return w;
}

int        ewl_layout_get_h(EwlLayout *l)
{
	int h;
	FUNC_BGN("ewl_layout_get_h");
	if (!l)	{
		ewl_debug("ewl_layout_get_h", EWL_NULL_ERROR, "l");
	} else if (!l->rect) {
		ewl_debug("ewl_layout_get_h", EWL_NULL_ERROR, "l->rect");
	} else {
		ewl_layout_get(l, 0,0,0,&h, 0,0,0,0, 0,0,0,0, 0,0,0,0);
	}
	FUNC_END("ewl_layout_get_h");
	return h;
}

int        ewl_layout_get_req_x(EwlLayout *l)
{
	int x;
	FUNC_BGN("ewl_layout_get_req_x");
	if (!l)	{
		ewl_debug("ewl_layout_get_req_x", EWL_NULL_ERROR, "l");
	} else if (!l->req) {
		ewl_debug("ewl_layout_get_req_x", EWL_NULL_ERROR, "l->req");
	} else {
		ewl_layout_get(l, 0,0,0,0, &x,0,0,0, 0,0,0,0, 0,0,0,0);
	}
	FUNC_END("ewl_layout_get_req_x");
	return x;
}

int        ewl_layout_get_req_y(EwlLayout *l)
{
	int y;
	FUNC_BGN("ewl_layout_get_req_y");
	if (!l)	{
		ewl_debug("ewl_layout_get_req_y", EWL_NULL_ERROR, "l");
	} else if (!l->req) {
		ewl_debug("ewl_layout_get_req_y", EWL_NULL_ERROR, "l->req");
	} else {
		ewl_layout_get(l, 0,0,0,0, 0,&y,0,0, 0,0,0,0, 0,0,0,0);
	}
	FUNC_END("ewl_layout_get_req_y");
	return y;
}

int        ewl_layout_get_req_w(EwlLayout *l)
{
	int w;
	FUNC_BGN("ewl_layout_get_req_w");
	if (!l)	{
		ewl_debug("ewl_layout_get_req_w", EWL_NULL_ERROR, "l");
	} else if (!l->req) {
		ewl_debug("ewl_layout_get_req_w", EWL_NULL_ERROR, "l->req");
	} else {
		ewl_layout_get(l, 0,0,0,0, 0,0,&w,0, 0,0,0,0, 0,0,0,0);
	}
	FUNC_END("ewl_layout_get_req_w");
	return w;
}

int        ewl_layout_get_req_h(EwlLayout *l)
{
	int h;
	FUNC_BGN("ewl_layout_get_req_h");
	if (!l)	{
		ewl_debug("ewl_layout_get_req_h", EWL_NULL_ERROR, "l");
	} else if (!l->req) {
		ewl_debug("ewl_layout_get_req_h", EWL_NULL_ERROR, "l->req");
	} else {
		ewl_layout_get(l, 0,0,0,0, 0,0,0,&h, 0,0,0,0, 0,0,0,0);
	}
	FUNC_END("ewl_layout_get_req_h");
	return h;
}

int        ewl_layout_get_min_x(EwlLayout *l)
{
	int x;
	FUNC_BGN("ewl_layout_get_min_x");
	if (!l)	{
		ewl_debug("ewl_layout_get_min_x", EWL_NULL_ERROR, "l");
	} else if (!l->min) {
		ewl_debug("ewl_layout_get_min_x", EWL_NULL_ERROR, "l->min");
	} else {
		ewl_layout_get(l, 0,0,0,0, 0,0,0,0, &x,0,0,0, 0,0,0,0);
	}
	FUNC_END("ewl_layout_get_min_x");
	return x;
}

int        ewl_layout_get_min_y(EwlLayout *l)
{
	int y;
	FUNC_BGN("ewl_layout_get_min_y");
	if (!l)	{
		ewl_debug("ewl_layout_get_min_y", EWL_NULL_ERROR, "l");
	} else if (!l->min) {
		ewl_debug("ewl_layout_get_min_y", EWL_NULL_ERROR, "l->min");
	} else {
		ewl_layout_get(l, 0,0,0,0, 0,0,0,0, 0,&y,0,0, 0,0,0,0);
	}
	FUNC_END("ewl_layout_get_min_y");
	return y;
}

int        ewl_layout_get_min_w(EwlLayout *l)
{
	int w;
	FUNC_BGN("ewl_layout_get_min_w");
	if (!l)	{
		ewl_debug("ewl_layout_get_min_w", EWL_NULL_ERROR, "l");
	} else if (!l->min) {
		ewl_debug("ewl_layout_get_min_w", EWL_NULL_ERROR, "l->min");
	} else {
		ewl_layout_get(l, 0,0,0,0, 0,0,0,0, 0,0,&w,0, 0,0,0,0);
	}
	FUNC_END("ewl_layout_get_min_w");
	return w;
}

int        ewl_layout_get_min_h(EwlLayout *l)
{
	int h;
	FUNC_BGN("ewl_layout_get_min_h");
	if (!l)	{
		ewl_debug("ewl_layout_get_min_h", EWL_NULL_ERROR, "l");
	} else if (!l->min) {
		ewl_debug("ewl_layout_get_min_h", EWL_NULL_ERROR, "l->min");
	} else {
		ewl_layout_get(l, 0,0,0,0, 0,0,0,0, 0,0,0,&h, 0,0,0,0);
	}
	FUNC_END("ewl_layout_get_min_h");
	return h;
}

int        ewl_layout_get_max_x(EwlLayout *l)
{
	int x;
	FUNC_BGN("ewl_layout_get_max_x");
	if (!l)	{
		ewl_debug("ewl_layout_get_max_x", EWL_NULL_ERROR, "l");
	} else if (!l->max) {
		ewl_debug("ewl_layout_get_max_x", EWL_NULL_ERROR, "l->max");
	} else {
		ewl_layout_get(l, 0,0,0,0, 0,0,0,0, 0,0,0,0, &x,0,0,0);
	}
	FUNC_END("ewl_layout_get_max_x");
	return x;
}

int        ewl_layout_get_max_y(EwlLayout *l)
{
	int y;
	FUNC_BGN("ewl_layout_get_max_y");
	if (!l)	{
		ewl_debug("ewl_layout_get_max_y", EWL_NULL_ERROR, "l");
	} else if (!l->rect) {
		ewl_debug("ewl_layout_get_max_y", EWL_NULL_ERROR, "l->rect");
	} else {
		ewl_layout_get(l, 0,0,0,0, 0,0,0,0, 0,0,0,0, 0,&y,0,0);
	}
	FUNC_END("ewl_layout_get_max_y");
	return y;
}

int        ewl_layout_get_max_w(EwlLayout *l)
{
	int w;
	FUNC_BGN("ewl_layout_get_max_w");
	if (!l)	{
		ewl_debug("ewl_layout_get_max_w", EWL_NULL_ERROR, "l");
	} else if (!l->max) {
		ewl_debug("ewl_layout_get_max_w", EWL_NULL_ERROR, "l->max");
	} else {
		ewl_layout_get(l, 0,0,0,0, 0,0,0,0, 0,0,0,0, 0,0,&w,0);
	}
	FUNC_END("ewl_layout_get_max_w");
	return w;
}

int        ewl_layout_get_max_h(EwlLayout *l)
{
	int h;
	FUNC_BGN("ewl_layout_get_max_h");
	if (!l)	{
		ewl_debug("ewl_layout_get_max_h", EWL_NULL_ERROR, "l");
	} else if (!l->max) {
		ewl_debug("ewl_layout_get_max_h", EWL_NULL_ERROR, "l->max");
	} else {
		ewl_layout_get(l, 0,0,0,0, 0,0,0,0, 0,0,0,0, 0,0,0,&h);
	}
	FUNC_END("ewl_layout_get_max_h");
	return h;
}

void       ewl_layout_dump(EwlLayout *l)
{
	FUNC_BGN("ewl_layout_dump");
	if (!l)	{
		ewl_debug("ewl_layout_dump", EWL_NULL_ERROR, "l");
	} else {
		if (l->rect) {fprintf(stderr,"rect: "); ewl_rect_dump(l->rect);}
		if (l->req) {fprintf(stderr,"req:  "); ewl_rect_dump(l->req);}
		if (l->min) {fprintf(stderr,"min:  "); ewl_rect_dump(l->min);}
		if (l->max) {fprintf(stderr,"max:  "); ewl_rect_dump(l->max);}
	}
	FUNC_END("ewl_layout_dump");
	return;
}

EwlRLayout *ewl_rlayout_new()
{
	int         x = 0;
	EwlRLayout *r = NULL;
	FUNC_BGN("ewl_rlayout_new");
	r = ewl_rlayout_new_with_values(&x,&x,&x,&x, &x,&x,&x,&x, &x,&x,&x,&x,
	                                &x,&x,&x,&x, &x,&x,&x,&x, &x,&x,&x,&x,
	                                &x,&x,&x,&x, &x,&x,&x,&x );
	if (!r)	{
		ewl_debug("ewl_rlayout_new", EWL_NULL_ERROR, "r");
	}
	FUNC_END("ewl_rlayout_new");
	return r;
}

EwlRLayout *ewl_rlayout_new_with_values(
              int *txr,   int *txa,   int *tyr,   int *tya,
              int *bxr,   int *bxa,   int *byr,   int *bya,
              int *rtxr,  int *rtxa,  int *rtyr,  int *rtya,
              int *rbxr,  int *rbxa,  int *rbyr,  int *rbya,
              int *mitxr, int *mitxa, int *mityr, int *mitya,
              int *mibxr, int *mibxa, int *mibyr, int *mibya,
              int *matxr, int *matxa, int *matyr, int *matya,
              int *mabxr, int *mabxa, int *mabyr, int *mabya)
{
	EwlRLayout *l = malloc(sizeof(EwlRLayout));
	FUNC_BGN("ewl_rlayout_new_with_values");
	if (!l)	{
		ewl_debug("ewl_rlayout_new_with_values", EWL_NULL_ERROR, "l");
	} else {
		l->rect = ewl_rrect_new_with_values(txr, txa, tyr, tya,
		                                    bxr, bxa, byr, bya);
		l->req  = ewl_rrect_new_with_values(rtxr, rtxa, rtyr, rtya,
		                                    rbxr, rbxa, rbyr, rbya);
		l->min  = ewl_rrect_new_with_values(mitxr, mitxa, mityr, mitya,
		                                    mibxr, mibxa, mibyr, mibya);
		l->max  = ewl_rrect_new_with_values(matxr, matxa, matyr, matya,
		                                    mabxr, mabxa, mabyr, mabya);
	}
	FUNC_END("ewl_rlayout_new_with_values");
	return l;
}

EwlRLayout *ewl_rlayout_new_with_rect_values(EwlRRect *rect, EwlRRect *req,
                                           EwlRRect *min,  EwlRRect *max)
{
	EwlRLayout *l = malloc(sizeof(EwlRLayout));
	FUNC_BGN("ewl_rlayout_new_with_rect_values");
	if (!l)	{
		ewl_debug("ewl_rlayout_new_with_rect_values", EWL_NULL_ERROR, "l");
	} else {
		if (rect) l->rect = ewl_rrect_dup(rect);
		if (req)  l->req  = ewl_rrect_dup(req);
		if (min)  l->min  = ewl_rrect_dup(min);
		if (max)  l->max  = ewl_rrect_dup(max);
	}
	FUNC_END("ewl_rlayout_new_with_rect_values");
	return l;
}

EwlRLayout *ewl_rlayout_dup(EwlRLayout *l)
{
	EwlRLayout *ll = NULL;
	FUNC_BGN("ewl_rlayout_dup");
	if (!l)	{
		ewl_debug("ewl_rlayout_dup", EWL_NULL_ERROR, "l");
	} else {
		ll = ewl_rlayout_new_with_rect_values(l->rect, l->req,
		                                      l->min,  l->max);
	}
	FUNC_BGN("ewl_rlayout_dup");
	return ll;
}

void        ewl_rlayout_free(EwlRLayout *l)
{
	FUNC_BGN("ewl_rlayout_free");
	if (!l)	{
		ewl_debug("ewl_rlayout_dup", EWL_NULL_ERROR, "l");
	} else {
		if (l->rect) ewl_rrect_free(l->rect);
		if (l->req)  ewl_rrect_free(l->req);
		if (l->min)  ewl_rrect_free(l->min);
		if (l->max)  ewl_rrect_free(l->max);
		free(l);
		l = NULL;
	}
	FUNC_END("ewl_rlayout_free");
	return;
}

void        ewl_rlayout_set(EwlRLayout *l,
              int *txr,   int *txa,   int *tyr,   int *tya,
              int *bxr,   int *bxa,   int *byr,   int *bya,
              int *rtxr,  int *rtxa,  int *rtyr,  int *rtya,
              int *rbxr,  int *rbxa,  int *rbyr,  int *rbya,
              int *mitxr, int *mitxa, int *mityr, int *mitya,
              int *mibxr, int *mibxa, int *mibyr, int *mibya,
              int *matxr, int *matxa, int *matyr, int *matya,
              int *mabxr, int *mabxa, int *mabyr, int *mabya)
{
	FUNC_BGN("ewl_rlayout_set");
	if (!l)	{
		ewl_debug("ewl_rlayout_set", EWL_NULL_ERROR, "l");
	} else {
		ewl_rrect_set(l->rect, txr, txa, tyr, tya,
		                       bxr, bxa, byr, bya);
		ewl_rrect_set(l->req,  rtxr,  rtxa,  rtyr,  rtya,
		                       rbxr,  rbxa,  rbyr,  rbya);
		ewl_rrect_set(l->min,  mitxr, mitxa, mityr, mitya,
		                       mibxr, mibxa, mibyr, mibya);
		ewl_rrect_set(l->max,  matxr, matxa, matyr, matya,
		                       mabxr, mabxa, mabyr, mabya);
	}
	FUNC_END("ewl_rlayout_set");
	return;
}

void        ewl_rlayout_get(EwlRLayout *l,
              int *txr,   int *txa,   int *tyr,   int *tya,
              int *bxr,   int *bxa,   int *byr,   int *bya,
              int *rtxr,  int *rtxa,  int *rtyr,  int *rtya,
              int *rbxr,  int *rbxa,  int *rbyr,  int *rbya,
              int *mitxr, int *mitxa, int *mityr, int *mitya,
              int *mibxr, int *mibxa, int *mibyr, int *mibya,
              int *matxr, int *matxa, int *matyr, int *matya,
              int *mabxr, int *mabxa, int *mabyr, int *mabya)
{
	FUNC_BGN("ewl_rlayout_get");
	if (!l)	{
		ewl_debug("ewl_rlayout_get", EWL_NULL_ERROR, "l");
	} else {
		ewl_rrect_get(l->rect, txr, txa, tyr, tya,
		                       bxr, bxa, byr, bya);
		ewl_rrect_get(l->req,  rtxr,  rtxa,  rtyr,  rtya,
		                       rbxr,  rbxa,  rbyr,  rbya);
		ewl_rrect_get(l->min,  mitxr, mitxa, mityr, mitya,
		                       mibxr, mibxa, mibyr, mibya);
		ewl_rrect_get(l->max,  matxr, matxa, matyr, matya,
		                       mabxr, mabxa, mabyr, mabya);
	}
	FUNC_END("ewl_rlayout_get");
	return;
}

void        ewl_rlayout_set_rects(EwlRLayout *l, EwlRRect *rect,
                                                 EwlRRect *req,
                                                 EwlRRect *min, 
                                                 EwlRRect *max)
{
	FUNC_BGN("ewl_rlayout_set_rects");
	if (!l)	{
		ewl_debug("ewl_rlayout_set_rects", EWL_NULL_ERROR, "l");
	} else {
		if (rect) {
			if (l->rect) ewl_rrect_free(l->rect);
			l->rect = ewl_rrect_dup(rect);
		}
		if (req) {
			if (l->req)  ewl_rrect_free(l->req);
			l->req  = ewl_rrect_dup(req);
		}
		if (min) {
			if (l->min)  ewl_rrect_free(l->min);
			l->min  = ewl_rrect_dup(min);
		}
		if (max) {
			if (l->max)  ewl_rrect_free(l->max);
			l->max  = ewl_rrect_dup(max);
		}
	}
	FUNC_END("ewl_rlayout_set_rects");
	return;
}

void        ewl_rlayout_get_rects(EwlRLayout *l, EwlRRect *rect,
                                                 EwlRRect *req,
                                                 EwlRRect *min, 
                                                 EwlRRect *max)
{
	FUNC_BGN("ewl_rlayout_get_rects");
	if (!l)	{
		ewl_debug("ewl_rlayout_get_rects", EWL_NULL_ERROR, "l");
	} else {
		rect = ewl_rrect_dup(l->rect);
		req =  ewl_rrect_dup(l->req);
		min =  ewl_rrect_dup(l->min);
		max =  ewl_rrect_dup(l->max);
	}
	FUNC_END("ewl_rlayout_get_rects");
	return;
}

void        ewl_rlayout_set_rect(EwlRLayout *l, EwlRRect *r)
{
	FUNC_BGN("ewl_rlayout_set_rect");
	if (!l)	{
		ewl_debug("ewl_rlayout_set_rect", EWL_NULL_ERROR, "l");
	} else {
		ewl_rlayout_set_rects(l,r,0,0,0);
	}
	FUNC_END("ewl_rlayout_set_rect");
	return;
}

void        ewl_rlayout_set_req_rect(EwlRLayout *l, EwlRRect *r)
{
	FUNC_BGN("ewl_rlayout_set_req_rect");
	if (!l)	{
		ewl_debug("ewl_rlayout_set_req_rect", EWL_NULL_ERROR, "l");
	} else {
		ewl_rlayout_set_rects(l,0,r,0,0);
	}
	FUNC_END("ewl_rlayout_set_req_rect");
	return;
}

void        ewl_rlayout_set_min_rect(EwlRLayout *l, EwlRRect *r)
{
	FUNC_BGN("ewl_rlayout_set_min_rect");
	if (!l)	{
		ewl_debug("ewl_rlayout_set_min_rect", EWL_NULL_ERROR, "l");
	} else {
		ewl_rlayout_set_rects(l,0,0,r,0);
	}
	FUNC_END("ewl_rlayout_set_min_rect");
	return;
}

void        ewl_rlayout_set_max_rect(EwlRLayout *l, EwlRRect *r)
{
	FUNC_BGN("ewl_rlayout_set_max_rect");
	if (!l)	{
		ewl_debug("ewl_rlayout_set_max_rect", EWL_NULL_ERROR, "l");
	} else {
		ewl_rlayout_set_rects(l,0,0,0,r);
	}
	FUNC_END("ewl_rlayout_set_max_rect");
	return;
}

EwlRRect   *ewl_rlayout_get_rect(EwlRLayout *l)
{
	EwlRRect *r = NULL;
	FUNC_BGN("ewl_rlayout_get_rect");
	if (!l)	{
		ewl_debug("ewl_rlayout_get_rect", EWL_NULL_ERROR, "l");
	} else {
		r = ewl_rrect_dup(l->rect);
	}
	FUNC_END("ewl_rlayout_get_rect");
	return r;
}

EwlRRect   *ewl_rlayout_get_req_rect(EwlRLayout *l)
{
	EwlRRect *r = NULL;
	FUNC_BGN("ewl_rlayout_get_req_rect");
	if (!l)	{
		ewl_debug("ewl_rlayout_get_req_rect", EWL_NULL_ERROR, "l");
	} else {
		r = ewl_rrect_dup(l->req);
	}
	FUNC_END("ewl_rlayout_get_req_rect");
	return r;
}

EwlRRect   *ewl_rlayout_get_min_rect(EwlRLayout *l)
{
	EwlRRect *r = NULL;
	FUNC_BGN("ewl_rlayout_get_min_rect");
	if (!l)	{
		ewl_debug("ewl_rlayout_get_min_rect", EWL_NULL_ERROR, "l");
	} else {
		r = ewl_rrect_dup(l->min);
	}
	FUNC_END("ewl_rlayout_get_min_rect");
	return r;
}

EwlRRect   *ewl_rlayout_get_max_rect(EwlRLayout *l)
{
	EwlRRect *r = NULL;
	FUNC_BGN("ewl_rlayout_get_max_rect");
	if (!l)	{
		ewl_debug("ewl_rlayout_get_max_rect", EWL_NULL_ERROR, "l");
	} else {
		r = ewl_rrect_dup(l->max);
	}
	FUNC_END("ewl_rlayout_get_max_rect");
	return r;
}


void        ewl_rlayout_set_top_x_relative(EwlRLayout *l, int x)
{
	FUNC_BGN("ewl_rlayout_set_top_x_relative");
	if (!l)	{
		ewl_debug("ewl_rlayout_set_top_x_relative", EWL_NULL_ERROR, "l");
	} else {
		ewl_rlayout_set(l,&x,0,0,0, 0,0,0,0, 0,0,0,0, 0,0,0,0,
		                  0,0,0,0, 0,0,0,0, 0,0,0,0, 0,0,0,0);
	}

	FUNC_END("ewl_rlayout_set_top_x_relative");
	return;
}

void        ewl_rlayout_set_top_x_absolute(EwlRLayout *l, int x)
{
	FUNC_BGN("ewl_rlayout_set_top_x_absolute");
	if (!l)	{
		ewl_debug("ewl_rlayout_set_top_x_absolute", EWL_NULL_ERROR, "l");
	} else {
		ewl_rlayout_set(l,0,&x,0,0, 0,0,0,0, 0,0,0,0, 0,0,0,0,
		                  0,0,0,0, 0,0,0,0, 0,0,0,0, 0,0,0,0);
	}

	FUNC_END("ewl_rlayout_set_top_x_absolute");
	return;
}

void        ewl_rlayout_set_top_y_relative(EwlRLayout *l, int y)
{
	FUNC_BGN("ewl_rlayout_set_top_y_relative");
	if (!l)	{
		ewl_debug("ewl_rlayout_set_top_y_relative", EWL_NULL_ERROR, "l");
	} else {
		ewl_rlayout_set(l,0,0,&y,0, 0,0,0,0, 0,0,0,0, 0,0,0,0,
		                  0,0,0,0, 0,0,0,0, 0,0,0,0, 0,0,0,0);
	}

	FUNC_END("ewl_rlayout_set_top_y_relative");
	return;
}

void        ewl_rlayout_set_top_y_absolute(EwlRLayout *l, int y)
{
	FUNC_BGN("ewl_rlayout_set_top_y_absolute");
	if (!l)	{
		ewl_debug("ewl_rlayout_set_top_y_absolute", EWL_NULL_ERROR, "l");
	} else {
		ewl_rlayout_set(l,0,0,0,&y, 0,0,0,0, 0,0,0,0, 0,0,0,0,
		                  0,0,0,0, 0,0,0,0, 0,0,0,0, 0,0,0,0);
	}

	FUNC_END("ewl_rlayout_set_top_y_absolute");
	return;
}

void        ewl_rlayout_set_bot_x_relative(EwlRLayout *l, int x)
{
	FUNC_BGN("ewl_rlayout_set_bot_x_relative");
	if (!l)	{
		ewl_debug("ewl_rlayout_set_bot_x_relative", EWL_NULL_ERROR, "l");
	} else {
		ewl_rlayout_set(l,0,0,0,0, &x,0,0,0, 0,0,0,0, 0,0,0,0,
		                  0,0,0,0, 0,0,0,0, 0,0,0,0, 0,0,0,0);
	}

	FUNC_END("ewl_rlayout_set_bot_x_relative");
	return;
}

void        ewl_rlayout_set_bot_x_absolute(EwlRLayout *l, int x)
{
	FUNC_BGN("ewl_rlayout_set_bot_x_absolute");
	if (!l)	{
		ewl_debug("ewl_rlayout_set_bot_x_absolute", EWL_NULL_ERROR, "l");
	} else {
		ewl_rlayout_set(l,0,0,0,0, 0,&x,0,0, 0,0,0,0, 0,0,0,0,
		                  0,0,0,0, 0,0,0,0, 0,0,0,0, 0,0,0,0);
	}

	FUNC_END("ewl_rlayout_set_bot_x_absolute");
	return;
}

void        ewl_rlayout_set_bot_y_relative(EwlRLayout *l, int y)
{
	FUNC_BGN("ewl_rlayout_set_bot_y_relative");
	if (!l)	{
		ewl_debug("ewl_rlayout_set_bot_y_relative", EWL_NULL_ERROR, "l");
	} else {
		ewl_rlayout_set(l,0,0,0,0, 0,0,&y,0, 0,0,0,0, 0,0,0,0,
		                  0,0,0,0, 0,0,0,0, 0,0,0,0, 0,0,0,0);
	}

	FUNC_END("ewl_rlayout_set_bot_y_relative");
	return;
}

void        ewl_rlayout_set_bot_y_absolute(EwlRLayout *l, int y)
{
	FUNC_BGN("ewl_rlayout_set_bot_y_absolute");
	if (!l)	{
		ewl_debug("ewl_rlayout_set_bot_y_absolute", EWL_NULL_ERROR, "l");
	} else {
		ewl_rlayout_set(l,0,0,0,0, 0,0,0,&y, 0,0,0,0, 0,0,0,0,
		                  0,0,0,0, 0,0,0,0, 0,0,0,0, 0,0,0,0);
	}

	FUNC_END("ewl_rlayout_set_bot_y_absolute");
	return;
}

void        ewl_rlayout_set_req_top_x_relative(EwlRLayout *l, int x)
{
	FUNC_BGN("ewl_rlayout_set_req_top_x_relative");
	if (!l)	{
		ewl_debug("ewl_rlayout_set_req_top_x_relative", EWL_NULL_ERROR, "l");
	} else {
		ewl_rlayout_set(l,0,0,0,0, 0,0,0,0, &x,0,0,0, 0,0,0,0,
		                  0,0,0,0, 0,0,0,0, 0,0,0,0, 0,0,0,0);
	}

	FUNC_END("ewl_rlayout_set_req_top_x_relative");
	return;
}

void        ewl_rlayout_set_req_top_x_absolute(EwlRLayout *l, int x)
{
	FUNC_BGN("ewl_rlayout_set_req_top_x_absolute");
	if (!l)	{
		ewl_debug("ewl_rlayout_set_req_top_x_absolute", EWL_NULL_ERROR, "l");
	} else {
		ewl_rlayout_set(l,0,0,0,0, 0,0,0,0, 0,&x,0,0, 0,0,0,0,
		                  0,0,0,0, 0,0,0,0, 0,0,0,0, 0,0,0,0);
	}

	FUNC_END("ewl_rlayout_set_req_top_x_absolute");
	return;
}

void        ewl_rlayout_set_req_top_y_relative(EwlRLayout *l, int y)
{
	FUNC_BGN("ewl_rlayout_set_req_top_y_relative");
	if (!l)	{
		ewl_debug("ewl_rlayout_set_req_top_y_relative", EWL_NULL_ERROR, "l");
	} else {
		ewl_rlayout_set(l,0,0,0,0, 0,0,0,0, 0,0,&y,0, 0,0,0,0,
		                  0,0,0,0, 0,0,0,0, 0,0,0,0, 0,0,0,0);
	}

	FUNC_END("ewl_rlayout_set_req_top_y_relative");
	return;
}

void        ewl_rlayout_set_req_top_y_absolute(EwlRLayout *l, int y)
{
	FUNC_BGN("ewl_rlayout_set_req_top_y_absolute");
	if (!l)	{
		ewl_debug("ewl_rlayout_set_req_top_y_absolute", EWL_NULL_ERROR, "l");
	} else {
		ewl_rlayout_set(l,0,0,0,0, 0,0,0,0, 0,0,0,&y, 0,0,0,0,
		                  0,0,0,0, 0,0,0,0, 0,0,0,0, 0,0,0,0);
	}

	FUNC_END("ewl_rlayout_set_req_top_y_absolute");
	return;
}

void        ewl_rlayout_set_req_bot_x_relative(EwlRLayout *l, int x)
{
	FUNC_BGN("ewl_rlayout_set_req_bot_x_relative");
	if (!l)	{
		ewl_debug("ewl_rlayout_set_req_bot_x_relative", EWL_NULL_ERROR, "l");
	} else {
		ewl_rlayout_set(l,0,0,0,0, 0,0,0,0, 0,0,0,0, &x,0,0,0,
		                  0,0,0,0, 0,0,0,0, 0,0,0,0, 0,0,0,0);
	}

	FUNC_END("ewl_rlayout_set_req_bot_x_relative");
	return;
}

void        ewl_rlayout_set_req_bot_x_absolute(EwlRLayout *l, int x)
{
	FUNC_BGN("ewl_rlayout_set_req_bot_x_absolute");
	if (!l)	{
		ewl_debug("ewl_rlayout_set_req_bot_x_absolute", EWL_NULL_ERROR, "l");
	} else {
		ewl_rlayout_set(l,0,0,0,0, 0,0,0,0, 0,0,0,0, 0,&x,0,0,
		                  0,0,0,0, 0,0,0,0, 0,0,0,0, 0,0,0,0);
	}

	FUNC_END("ewl_rlayout_set_req_bot_x_absolute");
	return;
}

void        ewl_rlayout_set_req_bot_y_relative(EwlRLayout *l, int y)
{
	FUNC_BGN("ewl_rlayout_set_req_bot_y_relative");
	if (!l)	{
		ewl_debug("ewl_rlayout_set_req_bot_y_relative", EWL_NULL_ERROR, "l");
	} else {
		ewl_rlayout_set(l,0,0,0,0, 0,0,0,0, 0,0,0,0, 0,0,&y,0,
		                  0,0,0,0, 0,0,0,0, 0,0,0,0, 0,0,0,0);
	}

	FUNC_END("ewl_rlayout_set_req_bot_y_relative");
	return;
}

void        ewl_rlayout_set_req_bot_y_absolute(EwlRLayout *l, int y)
{
	FUNC_BGN("ewl_rlayout_set_req_bot_y_absolute");
	if (!l)	{
		ewl_debug("ewl_rlayout_set_req_bot_y_absolute", EWL_NULL_ERROR, "l");
	} else {
		ewl_rlayout_set(l,0,0,0,0, 0,0,0,0, 0,0,0,0, 0,0,0,&y,
		                  0,0,0,0, 0,0,0,0, 0,0,0,0, 0,0,0,0);
	}

	FUNC_END("ewl_rlayout_set_req_bot_y_absolute");
	return;
}

void        ewl_rlayout_set_min_top_x_relative(EwlRLayout *l, int x)
{
	FUNC_BGN("ewl_rlayout_set_min_top_x_relative");
	if (!l)	{
		ewl_debug("ewl_rlayout_set_min_top_x_relative", EWL_NULL_ERROR, "l");
	} else {
		ewl_rlayout_set(l,0,0,0,0, 0,0,0,0, 0,0,0,0, 0,0,0,0,
		                  &x,0,0,0, 0,0,0,0, 0,0,0,0, 0,0,0,0);
	}

	FUNC_END("ewl_rlayout_set_min_top_x_relative");
	return;
}

void        ewl_rlayout_set_min_top_x_absolute(EwlRLayout *l, int x)
{
	FUNC_BGN("ewl_rlayout_set_min_top_x_absolute");
	if (!l)	{
		ewl_debug("ewl_rlayout_set_min_top_x_absolute", EWL_NULL_ERROR, "l");
	} else {
		ewl_rlayout_set(l,0,0,0,0, 0,0,0,0, 0,0,0,0, 0,0,0,0,
		                  0,&x,0,0, 0,0,0,0, 0,0,0,0, 0,0,0,0);
	}

	FUNC_END("ewl_rlayout_set_min_top_x_absolute");
	return;
}

void        ewl_rlayout_set_min_top_y_relative(EwlRLayout *l, int y)
{
	FUNC_BGN("ewl_rlayout_set_min_top_y_relative");
	if (!l)	{
		ewl_debug("ewl_rlayout_set_min_top_y_relative", EWL_NULL_ERROR, "l");
	} else {
		ewl_rlayout_set(l,0,0,0,0, 0,0,0,0, 0,0,0,0, 0,0,0,0,
		                  0,0,&y,0, 0,0,0,0, 0,0,0,0, 0,0,0,0);
	}

	FUNC_END("ewl_rlayout_set_min_top_y_relative");
	return;
}

void        ewl_rlayout_set_min_top_y_absolute(EwlRLayout *l, int y)
{
	FUNC_BGN("ewl_rlayout_set_min_top_y_absolute");
	if (!l)	{
		ewl_debug("ewl_rlayout_set_min_top_y_absolute", EWL_NULL_ERROR, "l");
	} else {
		ewl_rlayout_set(l,0,0,0,0, 0,0,0,0, 0,0,0,0, 0,0,0,0,
		                  0,0,0,&y, 0,0,0,0, 0,0,0,0, 0,0,0,0);
	}

	FUNC_END("ewl_rlayout_set_min_top_y_absolute");
	return;
}

void        ewl_rlayout_set_min_bot_x_relative(EwlRLayout *l, int x)
{
	FUNC_BGN("ewl_rlayout_set_min_bot_x_relative");
	if (!l)	{
		ewl_debug("ewl_rlayout_set_min_bot_x_relative", EWL_NULL_ERROR, "l");
	} else {
		ewl_rlayout_set(l,0,0,0,0, 0,0,0,0, 0,0,0,0, 0,0,0,0,
		                  0,0,0,0, &x,0,0,0, 0,0,0,0, 0,0,0,0);
	}

	FUNC_END("ewl_rlayout_set_min_bot_x_relative");
	return;
}

void        ewl_rlayout_set_min_bot_x_absolute(EwlRLayout *l, int x)
{
	FUNC_BGN("ewl_rlayout_set_min_bot_x_absolute");
	if (!l)	{
		ewl_debug("ewl_rlayout_set_min_bot_x_absolute", EWL_NULL_ERROR, "l");
	} else {
		ewl_rlayout_set(l,0,0,0,0, 0,0,0,0, 0,0,0,0, 0,0,0,0,
		                  0,0,0,0, 0,&x,0,0, 0,0,0,0, 0,0,0,0);
	}

	FUNC_END("ewl_rlayout_set_min_bot_x_absolute");
	return;
}

void        ewl_rlayout_set_min_bot_y_relative(EwlRLayout *l, int y)
{
	FUNC_BGN("ewl_rlayout_set_min_bot_y_relative");
	if (!l)	{
		ewl_debug("ewl_rlayout_set_min_bot_y_relative", EWL_NULL_ERROR, "l");
	} else {
		ewl_rlayout_set(l,0,0,0,0, 0,0,0,0, 0,0,0,0, 0,0,0,0,
		                  0,0,0,0, 0,0,&y,0, 0,0,0,0, 0,0,0,0);
	}

	FUNC_END("ewl_rlayout_set_min_bot_y_relative");
	return;
}

void        ewl_rlayout_set_min_bot_y_absolute(EwlRLayout *l, int y)
{
	FUNC_BGN("ewl_rlayout_set_min_bot_y_absolute");
	if (!l)	{
		ewl_debug("ewl_rlayout_set_min_bot_y_absolute", EWL_NULL_ERROR, "l");
	} else {
		ewl_rlayout_set(l,0,0,0,0, 0,0,0,0, 0,0,0,0, 0,0,0,0,
		                  0,0,0,0, 0,0,0,&y, 0,0,0,0, 0,0,0,0);
	}

	FUNC_END("ewl_rlayout_set_min_bot_y_absolute");
	return;
}

void        ewl_rlayout_set_max_top_x_relative(EwlRLayout *l, int x)
{
	FUNC_BGN("ewl_rlayout_set_max_top_x_relative");
	if (!l)	{
		ewl_debug("ewl_rlayout_set_max_top_x_relative", EWL_NULL_ERROR, "l");
	} else {
		ewl_rlayout_set(l,0,0,0,0, 0,0,0,0, 0,0,0,0, 0,0,0,0,
		                  0,0,0,0, 0,0,0,0, &x,0,0,0, 0,0,0,0);
	}

	FUNC_END("ewl_rlayout_set_max_top_x_relative");
	return;
}

void        ewl_rlayout_set_max_top_x_absolute(EwlRLayout *l, int x)
{
	FUNC_BGN("ewl_rlayout_set_max_top_x_absolute");
	if (!l)	{
		ewl_debug("ewl_rlayout_set_max_top_x_absolute", EWL_NULL_ERROR, "l");
	} else {
		ewl_rlayout_set(l,0,0,0,0, 0,0,0,0, 0,0,0,0, 0,0,0,0,
		                  0,0,0,0, 0,0,0,0, 0,&x,0,0, 0,0,0,0);
	}

	FUNC_END("ewl_rlayout_set_max_top_x_absolute");
	return;
}

void        ewl_rlayout_set_max_top_y_relative(EwlRLayout *l, int y)
{
	FUNC_BGN("ewl_rlayout_set_max_top_y_relative");
	if (!l)	{
		ewl_debug("ewl_rlayout_set_max_top_y_relative", EWL_NULL_ERROR, "l");
	} else {
		ewl_rlayout_set(l,0,0,0,0, 0,0,0,0, 0,0,0,0, 0,0,0,0,
		                  0,0,0,0, 0,0,0,0, 0,0,&y,0, 0,0,0,0);
	}

	FUNC_END("ewl_rlayout_set_max_top_y_relative");
	return;
}

void        ewl_rlayout_set_max_top_y_absolute(EwlRLayout *l, int y)
{
	FUNC_BGN("ewl_rlayout_set_max_top_y_absolute");
	if (!l)	{
		ewl_debug("ewl_rlayout_set_max_top_y_absolute", EWL_NULL_ERROR, "l");
	} else {
		ewl_rlayout_set(l,0,0,0,0, 0,0,0,0, 0,0,0,0, 0,0,0,0,
		                  0,0,0,0, 0,0,0,0, 0,0,0,&y, 0,0,0,0);
	}

	FUNC_END("ewl_rlayout_set_max_top_y_absolute");
	return;
}

void        ewl_rlayout_set_max_bot_x_relative(EwlRLayout *l, int x)
{
	FUNC_BGN("ewl_rlayout_set_max_bot_x_relative");
	if (!l)	{
		ewl_debug("ewl_rlayout_set_max_bot_x_relative", EWL_NULL_ERROR, "l");
	} else {
		ewl_rlayout_set(l,0,0,0,0, 0,0,0,0, 0,0,0,0, 0,0,0,0,
		                  0,0,0,0, 0,0,0,0, 0,0,0,0, &x,0,0,0);
	}

	FUNC_END("ewl_rlayout_set_max_bot_x_relative");
	return;
}

void        ewl_rlayout_set_max_bot_x_absolute(EwlRLayout *l, int x)
{
	FUNC_BGN("ewl_rlayout_set_max_bot_x_absolute");
	if (!l)	{
		ewl_debug("ewl_rlayout_set_max_bot_x_absolute", EWL_NULL_ERROR, "l");
	} else {
		ewl_rlayout_set(l,0,0,0,0, 0,0,0,0, 0,0,0,0, 0,0,0,0,
		                  0,0,0,0, 0,0,0,0, 0,0,0,0, 0,&x,0,0);
	}

	FUNC_END("ewl_rlayout_set_max_bot_x_absolute");
	return;
}

void        ewl_rlayout_set_max_bot_y_relative(EwlRLayout *l, int y)
{
	FUNC_BGN("ewl_rlayout_set_max_bot_y_relative");
	if (!l)	{
		ewl_debug("ewl_rlayout_set_max_bot_y_relative", EWL_NULL_ERROR, "l");
	} else {
		ewl_rlayout_set(l,0,0,0,0, 0,0,0,0, 0,0,0,0, 0,0,0,0,
		                  0,0,0,0, 0,0,0,0, 0,0,0,0, 0,0,&y,0);
	}

	FUNC_END("ewl_rlayout_set_max_bot_y_relative");
	return;
}

void        ewl_rlayout_set_max_bot_y_absolute(EwlRLayout *l, int y)
{
	FUNC_BGN("ewl_rlayout_set_max_bot_y_absolute");
	if (!l)	{
		ewl_debug("ewl_rlayout_set_max_bot_y_absolute", EWL_NULL_ERROR, "l");
	} else {
		ewl_rlayout_set(l,0,0,0,0, 0,0,0,0, 0,0,0,0, 0,0,0,0,
		                  0,0,0,0, 0,0,0,0, 0,0,0,0, 0,0,0,&y);
	}

	FUNC_END("ewl_rlayout_set_max_bot_y_absolute");
	return;
}


/* get starts here */

int         ewl_rlayout_get_top_x_relative(EwlRLayout *l)
{
	int x;
	FUNC_BGN("ewl_rlayout_get_top_x_relative");
	if (!l)	{
		ewl_debug("ewl_rlayout_get_top_x_relative", EWL_NULL_ERROR, "l");
	} else {
		ewl_rlayout_get(l,&x,0,0,0, 0,0,0,0, 0,0,0,0, 0,0,0,0,
		                  0,0,0,0, 0,0,0,0, 0,0,0,0, 0,0,0,0);
	}

	FUNC_END("ewl_rlayout_get_top_x_relative");
	return x;
}

int         ewl_rlayout_get_top_x_absolute(EwlRLayout *l)
{
	int x;
	FUNC_BGN("ewl_rlayout_get_top_x_absolute");
	if (!l)	{
		ewl_debug("ewl_rlayout_get_top_x_absolute", EWL_NULL_ERROR, "l");
	} else {
		ewl_rlayout_get(l,0,&x,0,0, 0,0,0,0, 0,0,0,0, 0,0,0,0,
		                  0,0,0,0, 0,0,0,0, 0,0,0,0, 0,0,0,0);
	}

	FUNC_END("ewl_rlayout_get_top_x_absolute");
	return x;
}

int         ewl_rlayout_get_top_y_relative(EwlRLayout *l)
{
	int y;
	FUNC_BGN("ewl_rlayout_get_top_y_relative");
	if (!l)	{
		ewl_debug("ewl_rlayout_get_top_y_relative", EWL_NULL_ERROR, "l");
	} else {
		ewl_rlayout_get(l,0,0,&y,0, 0,0,0,0, 0,0,0,0, 0,0,0,0,
		                  0,0,0,0, 0,0,0,0, 0,0,0,0, 0,0,0,0);
	}

	FUNC_END("ewl_rlayout_get_top_y_relative");
	return y;
}

int         ewl_rlayout_get_top_y_absolute(EwlRLayout *l)
{
	int y;
	FUNC_BGN("ewl_rlayout_get_top_y_absolute");
	if (!l)	{
		ewl_debug("ewl_rlayout_get_top_y_absolute", EWL_NULL_ERROR, "l");
	} else {
		ewl_rlayout_get(l,0,0,0,&y, 0,0,0,0, 0,0,0,0, 0,0,0,0,
		                  0,0,0,0, 0,0,0,0, 0,0,0,0, 0,0,0,0);
	}

	FUNC_END("ewl_rlayout_get_top_y_absolute");
	return y;
}

int         ewl_rlayout_get_bot_x_relative(EwlRLayout *l)
{
	int x;
	FUNC_BGN("ewl_rlayout_get_bot_x_relative");
	if (!l)	{
		ewl_debug("ewl_rlayout_get_bot_x_relative", EWL_NULL_ERROR, "l");
	} else {
		ewl_rlayout_get(l,0,0,0,0, &x,0,0,0, 0,0,0,0, 0,0,0,0,
		                  0,0,0,0, 0,0,0,0, 0,0,0,0, 0,0,0,0);
	}

	FUNC_END("ewl_rlayout_get_bot_x_relative");
	return x;
}

int         ewl_rlayout_get_bot_x_absolute(EwlRLayout *l)
{
	int x;
	FUNC_BGN("ewl_rlayout_get_bot_x_absolute");
	if (!l)	{
		ewl_debug("ewl_rlayout_get_bot_x_absolute", EWL_NULL_ERROR, "l");
	} else {
		ewl_rlayout_get(l,0,0,0,0, 0,&x,0,0, 0,0,0,0, 0,0,0,0,
		                  0,0,0,0, 0,0,0,0, 0,0,0,0, 0,0,0,0);
	}

	FUNC_END("ewl_rlayout_get_bot_x_absolute");
	return x;
}

int         ewl_rlayout_get_bot_y_relative(EwlRLayout *l)
{
	int y;
	FUNC_BGN("ewl_rlayout_get_bot_y_relative");
	if (!l)	{
		ewl_debug("ewl_rlayout_get_bot_y_relative", EWL_NULL_ERROR, "l");
	} else {
		ewl_rlayout_get(l,0,0,0,0, 0,0,&y,0, 0,0,0,0, 0,0,0,0,
		                  0,0,0,0, 0,0,0,0, 0,0,0,0, 0,0,0,0);
	}

	FUNC_END("ewl_rlayout_get_bot_y_relative");
	return y;
}

int         ewl_rlayout_get_bot_y_absolute(EwlRLayout *l)
{
	int y;
	FUNC_BGN("ewl_rlayout_get_bot_y_absolute");
	if (!l)	{
		ewl_debug("ewl_rlayout_get_bot_y_absolute", EWL_NULL_ERROR, "l");
	} else {
		ewl_rlayout_get(l,0,0,0,0, 0,0,0,&y, 0,0,0,0, 0,0,0,0,
		                  0,0,0,0, 0,0,0,0, 0,0,0,0, 0,0,0,0);
	}

	FUNC_END("ewl_rlayout_get_bot_y_absolute");
	return y;
}

int         ewl_rlayout_get_req_top_x_relative(EwlRLayout *l)
{
	int x;
	FUNC_BGN("ewl_rlayout_get_req_top_x_relative");
	if (!l)	{
		ewl_debug("ewl_rlayout_get_req_top_x_relative", EWL_NULL_ERROR, "l");
	} else {
		ewl_rlayout_get(l,0,0,0,0, 0,0,0,0, &x,0,0,0, 0,0,0,0,
		                  0,0,0,0, 0,0,0,0, 0,0,0,0, 0,0,0,0);
	}

	FUNC_END("ewl_rlayout_get_req_top_x_relative");
	return x;
}

int         ewl_rlayout_get_req_top_x_absolute(EwlRLayout *l)
{
	int x;
	FUNC_BGN("ewl_rlayout_get_req_top_x_absolute");
	if (!l)	{
		ewl_debug("ewl_rlayout_get_req_top_x_absolute", EWL_NULL_ERROR, "l");
	} else {
		ewl_rlayout_get(l,0,0,0,0, 0,0,0,0, 0,&x,0,0, 0,0,0,0,
		                  0,0,0,0, 0,0,0,0, 0,0,0,0, 0,0,0,0);
	}

	FUNC_END("ewl_rlayout_get_req_top_x_absolute");
	return x;
}

int         ewl_rlayout_get_req_top_y_relative(EwlRLayout *l)
{
	int y;
	FUNC_BGN("ewl_rlayout_get_req_top_y_relative");
	if (!l)	{
		ewl_debug("ewl_rlayout_get_req_top_y_relative", EWL_NULL_ERROR, "l");
	} else {
		ewl_rlayout_get(l,0,0,0,0, 0,0,0,0, 0,0,&y,0, 0,0,0,0,
		                  0,0,0,0, 0,0,0,0, 0,0,0,0, 0,0,0,0);
	}

	FUNC_END("ewl_rlayout_get_req_top_y_relative");
	return y;
}

int         ewl_rlayout_get_req_top_y_absolute(EwlRLayout *l)
{
	int y;
	FUNC_BGN("ewl_rlayout_get_req_top_y_absolute");
	if (!l)	{
		ewl_debug("ewl_rlayout_get_req_top_y_absolute", EWL_NULL_ERROR, "l");
	} else {
		ewl_rlayout_get(l,0,0,0,0, 0,0,0,0, 0,0,0,&y, 0,0,0,0,
		                  0,0,0,0, 0,0,0,0, 0,0,0,0, 0,0,0,0);
	}

	FUNC_END("ewl_rlayout_get_req_top_y_absolute");
	return y;
}

int         ewl_rlayout_get_req_bot_x_relative(EwlRLayout *l)
{
	int x;
	FUNC_BGN("ewl_rlayout_get_req_bot_x_relative");
	if (!l)	{
		ewl_debug("ewl_rlayout_get_req_bot_x_relative", EWL_NULL_ERROR, "l");
	} else {
		ewl_rlayout_get(l,0,0,0,0, 0,0,0,0, 0,0,0,0, &x,0,0,0,
		                  0,0,0,0, 0,0,0,0, 0,0,0,0, 0,0,0,0);
	}

	FUNC_END("ewl_rlayout_get_req_bot_x_relative");
	return x;
}

int         ewl_rlayout_get_req_bot_x_absolute(EwlRLayout *l)
{
	int x;
	FUNC_BGN("ewl_rlayout_get_req_bot_x_absolute");
	if (!l)	{
		ewl_debug("ewl_rlayout_get_req_bot_x_absolute", EWL_NULL_ERROR, "l");
	} else {
		ewl_rlayout_get(l,0,0,0,0, 0,0,0,0, 0,0,0,0, 0,&x,0,0,
		                  0,0,0,0, 0,0,0,0, 0,0,0,0, 0,0,0,0);
	}

	FUNC_END("ewl_rlayout_get_req_bot_x_absolute");
	return x;
}

int         ewl_rlayout_get_req_bot_y_relative(EwlRLayout *l)
{
	int y;
	FUNC_BGN("ewl_rlayout_get_req_bot_y_relative");
	if (!l)	{
		ewl_debug("ewl_rlayout_get_req_bot_y_relative", EWL_NULL_ERROR, "l");
	} else {
		ewl_rlayout_get(l,0,0,0,0, 0,0,0,0, 0,0,0,0, 0,0,&y,0,
		                  0,0,0,0, 0,0,0,0, 0,0,0,0, 0,0,0,0);
	}

	FUNC_END("ewl_rlayout_get_req_bot_y_relative");
	return y;
}

int         ewl_rlayout_get_req_bot_y_absolute(EwlRLayout *l)
{
	int y;
	FUNC_BGN("ewl_rlayout_get_req_bot_y_absolute");
	if (!l)	{
		ewl_debug("ewl_rlayout_get_req_bot_y_absolute", EWL_NULL_ERROR, "l");
	} else {
		ewl_rlayout_get(l,0,0,0,0, 0,0,0,0, 0,0,0,0, 0,0,0,&y,
		                  0,0,0,0, 0,0,0,0, 0,0,0,0, 0,0,0,0);
	}

	FUNC_END("ewl_rlayout_get_req_bot_y_absolute");
	return y;
}

int         ewl_rlayout_get_min_top_x_relative(EwlRLayout *l)
{
	int x;
	FUNC_BGN("ewl_rlayout_get_min_top_x_relative");
	if (!l)	{
		ewl_debug("ewl_rlayout_get_min_top_x_relative", EWL_NULL_ERROR, "l");
	} else {
		ewl_rlayout_get(l,0,0,0,0, 0,0,0,0, 0,0,0,0, 0,0,0,0,
		                  &x,0,0,0, 0,0,0,0, 0,0,0,0, 0,0,0,0);
	}

	FUNC_END("ewl_rlayout_get_min_top_x_relative");
	return x;
}

int         ewl_rlayout_get_min_top_x_absolute(EwlRLayout *l)
{
	int x;
	FUNC_BGN("ewl_rlayout_get_min_top_x_absolute");
	if (!l)	{
		ewl_debug("ewl_rlayout_get_min_top_x_absolute", EWL_NULL_ERROR, "l");
	} else {
		ewl_rlayout_get(l,0,0,0,0, 0,0,0,0, 0,0,0,0, 0,0,0,0,
		                  0,&x,0,0, 0,0,0,0, 0,0,0,0, 0,0,0,0);
	}

	FUNC_END("ewl_rlayout_get_min_top_x_absolute");
	return x;
}

int         ewl_rlayout_get_min_top_y_relative(EwlRLayout *l)
{
	int y;
	FUNC_BGN("ewl_rlayout_get_min_top_y_relative");
	if (!l)	{
		ewl_debug("ewl_rlayout_get_min_top_y_relative", EWL_NULL_ERROR, "l");
	} else {
		ewl_rlayout_get(l,0,0,0,0, 0,0,0,0, 0,0,0,0, 0,0,0,0,
		                  0,0,&y,0, 0,0,0,0, 0,0,0,0, 0,0,0,0);
	}

	FUNC_END("ewl_rlayout_get_min_top_y_relative");
	return y;
}

int         ewl_rlayout_get_min_top_y_absolute(EwlRLayout *l)
{
	int y;
	FUNC_BGN("ewl_rlayout_get_min_top_y_absolute");
	if (!l)	{
		ewl_debug("ewl_rlayout_get_min_top_y_absolute", EWL_NULL_ERROR, "l");
	} else {
		ewl_rlayout_get(l,0,0,0,0, 0,0,0,0, 0,0,0,0, 0,0,0,0,
		                  0,0,0,&y, 0,0,0,0, 0,0,0,0, 0,0,0,0);
	}

	FUNC_END("ewl_rlayout_get_min_top_y_absolute");
	return y;
}

int         ewl_rlayout_get_min_bot_x_relative(EwlRLayout *l)
{
	int x;
	FUNC_BGN("ewl_rlayout_get_min_bot_x_relative");
	if (!l)	{
		ewl_debug("ewl_rlayout_get_min_bot_x_relative", EWL_NULL_ERROR, "l");
	} else {
		ewl_rlayout_get(l,0,0,0,0, 0,0,0,0, 0,0,0,0, 0,0,0,0,
		                  0,0,0,0, &x,0,0,0, 0,0,0,0, 0,0,0,0);
	}

	FUNC_END("ewl_rlayout_get_min_bot_x_relative");
	return x;
}

int         ewl_rlayout_get_min_bot_x_absolute(EwlRLayout *l)
{
	int x;
	FUNC_BGN("ewl_rlayout_get_min_bot_x_absolute");
	if (!l)	{
		ewl_debug("ewl_rlayout_get_min_bot_x_absolute", EWL_NULL_ERROR, "l");
	} else {
		ewl_rlayout_get(l,0,0,0,0, 0,0,0,0, 0,0,0,0, 0,0,0,0,
		                  0,0,0,0, 0,&x,0,0, 0,0,0,0, 0,0,0,0);
	}

	FUNC_END("ewl_rlayout_get_min_bot_x_absolute");
	return x;
}

int         ewl_rlayout_get_min_bot_y_relative(EwlRLayout *l)
{
	int y;
	FUNC_BGN("ewl_rlayout_get_min_bot_y_relative");
	if (!l)	{
		ewl_debug("ewl_rlayout_get_min_bot_y_relative", EWL_NULL_ERROR, "l");
	} else {
		ewl_rlayout_get(l,0,0,0,0, 0,0,0,0, 0,0,0,0, 0,0,0,0,
		                  0,0,0,0, 0,0,&y,0, 0,0,0,0, 0,0,0,0);
	}

	FUNC_END("ewl_rlayout_get_min_bot_y_relative");
	return y;
}

int         ewl_rlayout_get_min_bot_y_absolute(EwlRLayout *l)
{
	int y;
	FUNC_BGN("ewl_rlayout_get_min_bot_y_absolute");
	if (!l)	{
		ewl_debug("ewl_rlayout_get_min_bot_y_absolute", EWL_NULL_ERROR, "l");
	} else {
		ewl_rlayout_get(l,0,0,0,0, 0,0,0,0, 0,0,0,0, 0,0,0,0,
		                  0,0,0,0, 0,0,0,&y, 0,0,0,0, 0,0,0,0);
	}

	FUNC_END("ewl_rlayout_get_min_bot_y_absolute");
	return y;
}

int         ewl_rlayout_get_max_top_x_relative(EwlRLayout *l)
{
	int x;
	FUNC_BGN("ewl_rlayout_get_max_top_x_relative");
	if (!l)	{
		ewl_debug("ewl_rlayout_get_max_top_x_relative", EWL_NULL_ERROR, "l");
	} else {
		ewl_rlayout_get(l,0,0,0,0, 0,0,0,0, 0,0,0,0, 0,0,0,0,
		                  0,0,0,0, 0,0,0,0, &x,0,0,0, 0,0,0,0);
	}

	FUNC_END("ewl_rlayout_get_max_top_x_relative");
	return x;
}

int         ewl_rlayout_get_max_top_x_absolute(EwlRLayout *l)
{
	int x;
	FUNC_BGN("ewl_rlayout_get_max_top_x_absolute");
	if (!l)	{
		ewl_debug("ewl_rlayout_get_max_top_x_absolute", EWL_NULL_ERROR, "l");
	} else {
		ewl_rlayout_get(l,0,0,0,0, 0,0,0,0, 0,0,0,0, 0,0,0,0,
		                  0,0,0,0, 0,0,0,0, 0,&x,0,0, 0,0,0,0);
	}

	FUNC_END("ewl_rlayout_get_max_top_x_absolute");
	return x;
}

int         ewl_rlayout_get_max_top_y_relative(EwlRLayout *l)
{
	int y;
	FUNC_BGN("ewl_rlayout_get_max_top_y_relative");
	if (!l)	{
		ewl_debug("ewl_rlayout_get_max_top_y_relative", EWL_NULL_ERROR, "l");
	} else {
		ewl_rlayout_get(l,0,0,0,0, 0,0,0,0, 0,0,0,0, 0,0,0,0,
		                  0,0,0,0, 0,0,0,0, 0,0,&y,0, 0,0,0,0);
	}

	FUNC_END("ewl_rlayout_get_max_top_y_relative");
	return y;
}

int         ewl_rlayout_get_max_top_y_absolute(EwlRLayout *l)
{
	int y;
	FUNC_BGN("ewl_rlayout_get_max_top_y_absolute");
	if (!l)	{
		ewl_debug("ewl_rlayout_get_max_top_y_absolute", EWL_NULL_ERROR, "l");
	} else {
		ewl_rlayout_get(l,0,0,0,0, 0,0,0,0, 0,0,0,0, 0,0,0,0,
		                  0,0,0,0, 0,0,0,0, 0,0,0,&y, 0,0,0,0);
	}

	FUNC_END("ewl_rlayout_get_max_top_y_absolute");
	return y;
}

int         ewl_rlayout_get_max_bot_x_relative(EwlRLayout *l)
{
	int x;
	FUNC_BGN("ewl_rlayout_get_max_bot_x_relative");
	if (!l)	{
		ewl_debug("ewl_rlayout_get_max_bot_x_relative", EWL_NULL_ERROR, "l");
	} else {
		ewl_rlayout_get(l,0,0,0,0, 0,0,0,0, 0,0,0,0, 0,0,0,0,
		                  0,0,0,0, 0,0,0,0, 0,0,0,0, &x,0,0,0);
	}

	FUNC_END("ewl_rlayout_get_max_bot_x_relative");
	return x;
}

int         ewl_rlayout_get_max_bot_x_absolute(EwlRLayout *l)
{
	int x;
	FUNC_BGN("ewl_rlayout_get_max_bot_x_absolute");
	if (!l)	{
		ewl_debug("ewl_rlayout_get_max_bot_x_absolute", EWL_NULL_ERROR, "l");
	} else {
		ewl_rlayout_get(l,0,0,0,0, 0,0,0,0, 0,0,0,0, 0,0,0,0,
		                  0,0,0,0, 0,0,0,0, 0,0,0,0, 0,&x,0,0);
	}

	FUNC_END("ewl_rlayout_get_max_bot_x_absolute");
	return x;
}

int         ewl_rlayout_get_max_bot_y_relative(EwlRLayout *l)
{
	int y;
	FUNC_BGN("ewl_rlayout_get_max_bot_y_relative");
	if (!l)	{
		ewl_debug("ewl_rlayout_get_max_bot_y_relative", EWL_NULL_ERROR, "l");
	} else {
		ewl_rlayout_get(l,0,0,0,0, 0,0,0,0, 0,0,0,0, 0,0,0,0,
		                  0,0,0,0, 0,0,0,0, 0,0,0,0, 0,0,&y,0);
	}

	FUNC_END("ewl_rlayout_get_max_bot_y_relative");
	return y;
}

int         ewl_rlayout_get_max_bot_y_absolute(EwlRLayout *l)
{
	int y;
	FUNC_BGN("ewl_rlayout_get_max_bot_y_absolute");
	if (!l)	{
		ewl_debug("ewl_rlayout_get_max_bot_y_absolute", EWL_NULL_ERROR, "l");
	} else {
		ewl_rlayout_get(l,0,0,0,0, 0,0,0,0, 0,0,0,0, 0,0,0,0,
		                  0,0,0,0, 0,0,0,0, 0,0,0,0, 0,0,0,&y);
	}

	FUNC_END("ewl_rlayout_get_max_bot_y_absolute");
	return y;
}
/*
int         ewl_rlayout_get_top_x_relative(EwlRLayout *l);
int         ewl_rlayout_get_top_x_absolute(EwlRLayout *l);
int         ewl_rlayout_get_top_y_relative(EwlRLayout *l);
int         ewl_rlayout_get_top_y_absolute(EwlRLayout *l);
int         ewl_rlayout_get_bot_x_relative(EwlRLayout *l);
int         ewl_rlayout_get_bot_x_absolute(EwlRLayout *l);
int         ewl_rlayout_get_bot_y_relative(EwlRLayout *l);
int         ewl_rlayout_get_bot_y_absolute(EwlRLayout *l);
int         ewl_rlayout_get_req_top_x_relative(EwlRLayout *l);
int         ewl_rlayout_get_req_top_x_absolute(EwlRLayout *l);
int         ewl_rlayout_get_req_top_y_relative(EwlRLayout *l);
int         ewl_rlayout_get_req_top_y_absolute(EwlRLayout *l);
int         ewl_rlayout_get_req_bot_x_relative(EwlRLayout *l);
int         ewl_rlayout_get_req_bot_x_absolute(EwlRLayout *l);
int         ewl_rlayout_get_req_bot_y_relative(EwlRLayout *l);
int         ewl_rlayout_get_req_bot_y_absolute(EwlRLayout *l);
int         ewl_rlayout_get_min_top_x_relative(EwlRLayout *l);
int         ewl_rlayout_get_min_top_x_absolute(EwlRLayout *l);
int         ewl_rlayout_get_min_top_y_relative(EwlRLayout *l);
int         ewl_rlayout_get_min_top_y_absolute(EwlRLayout *l);
int         ewl_rlayout_get_min_bot_x_relative(EwlRLayout *l);
int         ewl_rlayout_get_min_bot_x_absolute(EwlRLayout *l);
int         ewl_rlayout_get_min_bot_y_relative(EwlRLayout *l);
int         ewl_rlayout_get_min_bot_y_absolute(EwlRLayout *l);
int         ewl_rlayout_get_max_top_x_relative(EwlRLayout *l);
int         ewl_rlayout_get_max_top_x_absolute(EwlRLayout *l);
int         ewl_rlayout_get_max_top_y_relative(EwlRLayout *l);
int         ewl_rlayout_get_max_top_y_absolute(EwlRLayout *l);
int         ewl_rlayout_get_max_bot_x_relative(EwlRLayout *l);
int         ewl_rlayout_get_max_bot_x_absolute(EwlRLayout *l);
int         ewl_rlayout_get_max_bot_y_relative(EwlRLayout *l);
int         ewl_rlayout_get_max_bot_y_absolute(EwlRLayout *l);
*/
