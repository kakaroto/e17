#include "layout.h"

EwlRect *ewl_rect_new()
{
	EwlRect *r = malloc(sizeof(EwlRect));
	FUNC_BGN("ewl_rect_new");
	if (!r)	{
		/* FIXME */
	} else {
		r->x = 0;
		r->y = 0;
		r->w = -1;
		r->h = -1;
	}
	FUNC_END("ewl_rect_new");
	return r;
}

EwlRect *ewl_rect_new_with_values(int x, int y, int w, int h)
{
	EwlRect *r = malloc(sizeof(EwlRect));
	FUNC_BGN("ewl_rect_new_with_values");
	if (!r)	{
		ewl_debug("ewl_rect_new_with_values", EWL_NULL_ERROR, "r");
	} else {
		r->x = x;
		r->y = y;
		r->w = w;
		r->h = h;
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
		tr = ewl_rect_new_with_values(r->x, r->y, r->w, r->h);
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

EwlRequisition *ewl_requisition_new()
{
	return ewl_requisition_new_with_values(0, 0, 0, 0, 0);
}

EwlRequisition *ewl_requisition_new_with_values(EwlRect *requested_rect,
                                                int      min_w,
                                                int      min_h,
                                                int      max_w,
                                                int      max_h)
{
	EwlRequisition *req = malloc(sizeof(EwlRequisition));
	
	req->requested = requested_rect?ewl_rect_dup(requested_rect):ewl_rect_new_with_values(0,0,0,0);
	req->min_w = min_w;
	req->min_h = min_h;
	req->max_w = max_w;
	req->max_h = max_h;

	return req;
}

EwlRequisition *ewl_requisition_dup(EwlRequisition *req)
{
	return ewl_requisition_new_with_values(ewl_rect_dup(req->requested), req->min_w, req->min_h, req->max_w, req->max_h);
}
void            ewl_requisition_free(EwlRequisition *req)
{
	if (req) free(req);
	return;
}

void            ewl_requisition_set_requested_rect(EwlRequisition *req,
                                                   EwlRect        *rect)
{
	if (req->requested) ewl_rect_free(req->requested);
	req->requested = ewl_rect_dup(rect);
	return;
}

EwlRect        *ewl_requisition_get_requested_rect(EwlRequisition *req)
{
	return ewl_rect_dup(req->requested);
}

void            ewl_requisition_set_min_width(EwlRequisition *req, int w)
{
	req->min_w = w;
	return;
}

int             ewl_requisition_get_min_width(EwlRequisition *req)
{
	return req->min_w;
}

void            ewl_requisition_set_min_height(EwlRequisition *req, int h)
{
	req->min_h = h;
	return;
}

int             ewl_requisition_get_min_height(EwlRequisition *req)
{
	return req->min_h;
}

void            ewl_requisition_set_max_width(EwlRequisition *req, int w)
{
	req->max_w = w;
	return;
}

int             ewl_requisition_get_max_width(EwlRequisition *req)
{
	return req->max_w;
}

void            ewl_requisition_set_max_height(EwlRequisition *req, int h)
{
	req->max_h = h;
	return;
}

int             ewl_requisition_get_max_height(EwlRequisition *req)
{
	return req->max_h;
}
