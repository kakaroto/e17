#ifndef _LAYOUT_H_
#define _LAYOUT_H_ 1

#include "includes.h"
#include "error.h"

/* EWL_RECT */

#define EWL_RECT(a) ((EwlRect*)a)
typedef struct _EwlRect EwlRect;
struct _EwlRect	{
	int x, y, w, h;
};

EwlRect *ewl_rect_new();
EwlRect *ewl_rect_new_with_values(int x, int y, int w, int h);
EwlRect *ewl_rect_dup(EwlRect *r);
void     ewl_rect_free(EwlRect *r);

void     ewl_rect_set(EwlRect *r, int *x, int *y, int *w, int *h);
void     ewl_rect_get(EwlRect *r, int *x, int *y, int *w, int *h);
void     ewl_rect_set_rect(EwlRect *dst, EwlRect *src);


void     ewl_rect_dump(EwlRect *r);


#define IN_RECT(r,tx,ty) (r&&(((r->x)<=tx)&&((r->y)<=ty)&&(tx<=(r->x)+(r->w))&&(ty<=(r->y)+(r->h))))
#define IN_RECT_NO_OFFSET(r,tx,ty) (r&&((tx>=0)&&(ty>=0)&&(tx<=(r->w))&&(ty<=(r->h))))

#define RECT_COMPARE(a,b) ((a&&b) && (a->x==b->x) && (a->y==b->y) && (a->w==b->w) && (a->h&&b->h))


/* EWL REQUISITION */

#define EWL_REQUISITION(a) ((EwlRequisition*)a)

typedef struct _EwlRequisition EwlRequisition;

struct _EwlRequisition {
	EwlRect *requested;
	int      min_w, min_h;
	int      max_w, max_h;
};

EwlRequisition *ewl_requisition_new();
EwlRequisition *ewl_requisition_new_with_values(EwlRect *requested_rect,
                                                int      min_w,
                                                int      min_h,
                                                int      max_w,
                                                int      max_h);
EwlRequisition *ewl_requisition_dup(EwlRequisition *req);
void            ewl_requisition_free(EwlRequisition *req);

void            ewl_requisition_set_requested_rect(EwlRequisition *req,
                                                   EwlRect        *rect);
EwlRect        *ewl_requisition_get_requested_rect(EwlRequisition *req);

void            ewl_requisition_set_min_width(EwlRequisition *req, int w);
int             ewl_requisition_get_min_width(EwlRequisition *req);

void            ewl_requisition_set_min_height(EwlRequisition *req, int h);
int             ewl_requisition_get_min_height(EwlRequisition *req);

void            ewl_requisition_set_max_width(EwlRequisition *req, int w);
int             ewl_requisition_get_max_width(EwlRequisition *req);

void            ewl_requisition_set_max_height(EwlRequisition *req, int h);
int             ewl_requisition_get_max_height(EwlRequisition *req);

#endif /* _LAYOUT_H_ */
