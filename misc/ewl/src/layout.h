#ifndef _LAYOUT_H_
#define _LAYOUT_H_ 1

#include "includes.h"
#include "error.h"

/* EWL_RECT */
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

/* EWL_RRECT */
enum _EwlRRectEnum	{
	EWL_TOP_RELATIVE,
	EWL_TOP_ABSOLUTE,
	EWL_BOT_RELATIVE,
	EWL_BOT_ABSOLUTE
};

typedef struct _EwlRRect EwlRRect;
struct _EwlRRect	{
	int x[4], y[4];
};

EwlRRect *ewl_rrect_new();
EwlRRect *ewl_rrect_new_with_values(int txr, int txa, int tyr, int tya,
                                    int bxr, int bxa, int byr, int bya);
EwlRRect *ewl_rrect_dup(EwlRRect *r);
void      ewl_rrect_free(EwlRRect *r);

void      ewl_rrect_set(EwlRRect *r, int *txr, int *txa, int *tyr, int *tya,
                                     int *bxr, int *bxa, int *byr, int *bya);
void      ewl_rrect_get(EwlRRect *r, int *txr, int *txa, int *tyr, int *tya,
                                     int *bxr, int *bxa, int *byr, int *bya);
void      ewl_rrect_set_rect(EwlRRect *dst, EwlRRect *src);

void      ewl_rrect_dump(EwlRRect *r);



/* EWL_LAYOUT */
typedef struct _EwlLayout EwlLayout;
struct _EwlLayout  {
	EwlRect      *rect;
	EwlRect      *req;
	EwlRect      *min;
	EwlRect      *max;
};

EwlLayout *ewl_layout_new();
EwlLayout *ewl_layout_new_with_values(int x,   int y,   int w,   int h,
                                      int rx,  int ry,  int rw,  int rh,
                                      int mix, int miy, int miw, int mih,
                                      int max, int may, int maw, int mah);
EwlLayout *ewl_layout_new_with_rect_values(EwlRect *rect, EwlRect *req,
                                           EwlRect *min,  EwlRect *max);
EwlLayout *ewl_layout_dup(EwlLayout *l);
void       ewl_layout_free(EwlLayout *l);

void       ewl_layout_set(EwlLayout *l, int *x,   int *y,   int *w,   int *h,
                                        int *rx,  int *ry,  int *rw,  int *rh,
                                        int *mix, int *miy, int *miw, int *mih,
                                        int *max, int *may, int *maw, int *mah);
void       ewl_layout_get(EwlLayout *l, int *x,   int *y,   int *w,   int *h,
                                        int *rx,  int *ry,  int *rw,  int *rh,
                                        int *mix, int *miy, int *miw, int *mih,
                                        int *max, int *may, int *maw, int *mah);

void       ewl_layout_set_rects(EwlLayout *l, EwlRect *rect, EwlRect *req,
                                              EwlRect *min,  EwlRect *max);
void       ewl_layout_get_rects(EwlLayout *l, EwlRect *rect, EwlRect *req,
                                              EwlRect *min,  EwlRect *max);

void       ewl_layout_set_rect(EwlLayout *l, EwlRect *r);
void       ewl_layout_set_req_rect(EwlLayout *l, EwlRect *r);
void       ewl_layout_set_min_rect(EwlLayout *l, EwlRect *r);
void       ewl_layout_set_max_rect(EwlLayout *l, EwlRect *r);
EwlRect   *ewl_layout_get_rect(EwlLayout *l);
EwlRect   *ewl_layout_get_req_rect(EwlLayout *l);
EwlRect   *ewl_layout_get_min_rect(EwlLayout *l);
EwlRect   *ewl_layout_get_max_rect(EwlLayout *l);

void       ewl_layout_set_x(EwlLayout *l, int x);
void       ewl_layout_set_y(EwlLayout *l, int y);
void       ewl_layout_set_w(EwlLayout *l, int w);
void       ewl_layout_set_h(EwlLayout *l, int h);
void       ewl_layout_set_req_x(EwlLayout *l, int x);
void       ewl_layout_set_req_y(EwlLayout *l, int y);
void       ewl_layout_set_req_w(EwlLayout *l, int w);
void       ewl_layout_set_req_h(EwlLayout *l, int h);
void       ewl_layout_set_min_x(EwlLayout *l, int x);
void       ewl_layout_set_min_y(EwlLayout *l, int y);
void       ewl_layout_set_min_w(EwlLayout *l, int w);
void       ewl_layout_set_min_h(EwlLayout *l, int h);
void       ewl_layout_set_max_x(EwlLayout *l, int x);
void       ewl_layout_set_max_y(EwlLayout *l, int y);
void       ewl_layout_set_max_w(EwlLayout *l, int w);
void       ewl_layout_set_max_h(EwlLayout *l, int h);

int        ewl_layout_get_x(EwlLayout *l);
int        ewl_layout_get_y(EwlLayout *l);
int        ewl_layout_get_w(EwlLayout *l);
int        ewl_layout_get_h(EwlLayout *l);
int        ewl_layout_get_req_x(EwlLayout *l);
int        ewl_layout_get_req_y(EwlLayout *l);
int        ewl_layout_get_req_w(EwlLayout *l);
int        ewl_layout_get_req_h(EwlLayout *l);
int        ewl_layout_get_min_x(EwlLayout *l);
int        ewl_layout_get_min_y(EwlLayout *l);
int        ewl_layout_get_min_w(EwlLayout *l);
int        ewl_layout_get_min_h(EwlLayout *l);
int        ewl_layout_get_max_x(EwlLayout *l);
int        ewl_layout_get_max_y(EwlLayout *l);
int        ewl_layout_get_max_w(EwlLayout *l);
int        ewl_layout_get_max_h(EwlLayout *l);

void       ewl_layout_dump(EwlLayout *l);



/* EWL_RLAYOUT */
typedef struct _EwlRLayout EwlRLayout;
struct _EwlRLayout	{
	EwlLayout     layout;
	EwlRRect      *rect;
	EwlRRect      *req;
	EwlRRect      *min;
	EwlRRect      *max;
};

EwlRLayout *ewl_rlayout_new();
EwlRLayout *ewl_rlayout_new_with_values(
              int txr,   int txa,   int tyr,   int tya,
              int bxr,   int bxa,   int byr,   int bya,
              int rtxr,  int rtxa,  int rtyr,  int rtya,
              int rbxr,  int rbxa,  int rbyr,  int rbya,
              int mitxr, int mitxa, int mityr, int mitya,
              int mibxr, int mibxa, int mibyr, int mibya,
              int matxr, int matxa, int matyr, int matya,
              int mabxr, int mabxa, int mabyr, int mabya);
EwlRLayout *ewl_rlayout_new_with_rect_values(EwlRRect *rect, EwlRRect *req,
                                           EwlRRect *min,  EwlRRect *max);
EwlRLayout *ewl_rlayout_dup(EwlRLayout *l);
void        ewl_rlayout_free(EwlRLayout *l);

void        ewl_rlayout_set(EwlRLayout *l,
              int *txr,   int *txa,   int *tyr,   int *tya,
              int *bxr,   int *bxa,   int *byr,   int *bya,
              int *rtxr,  int *rtxa,  int *rtyr,  int *rtya,
              int *rbxr,  int *rbxa,  int *rbyr,  int *rbya,
              int *mitxr, int *mitxa, int *mityr, int *mitya,
              int *mibxr, int *mibxa, int *mibyr, int *mibya,
              int *matxr, int *matxa, int *matyr, int *matya,
              int *mabxr, int *mabxa, int *mabyr, int *mabya);
void        ewl_rlayout_get(EwlRLayout *l,
              int *txr,   int *txa,   int *tyr,   int *tya,
              int *bxr,   int *bxa,   int *byr,   int *bya,
              int *rtxr,  int *rtxa,  int *rtyr,  int *rtya,
              int *rbxr,  int *rbxa,  int *rbyr,  int *rbya,
              int *mitxr, int *mitxa, int *mityr, int *mitya,
              int *mibxr, int *mibxa, int *mibyr, int *mibya,
              int *matxr, int *matxa, int *matyr, int *matya,
              int *mabxr, int *mabxa, int *mabyr, int *mabya);

void        ewl_rlayout_set_rects(EwlRLayout *l, EwlRRect *rect,
                                                 EwlRRect *req,
                                                 EwlRRect *min, 
                                                 EwlRRect *max);
void        ewl_rlayout_get_rects(EwlRLayout *l, EwlRRect *rect,
                                                 EwlRRect *req,
                                                 EwlRRect *min, 
                                                 EwlRRect *max);

void        ewl_rlayout_set_rect(EwlRLayout *l, EwlRRect *r);
void        ewl_rlayout_set_req_rect(EwlRLayout *l, EwlRRect *r);
void        ewl_rlayout_set_min_rect(EwlRLayout *l, EwlRRect *r);
void        ewl_rlayout_set_max_rect(EwlRLayout *l, EwlRRect *r);
EwlRRect   *ewl_rlayout_get_rect(EwlRLayout *l);
EwlRRect   *ewl_rlayout_get_req_rect(EwlRLayout *l);
EwlRRect   *ewl_rlayout_get_min_rect(EwlRLayout *l);
EwlRRect   *ewl_rlayout_get_max_rect(EwlRLayout *l);

void        ewl_rlayout_set_top_x_relative(EwlRLayout *l, int x);
void        ewl_rlayout_set_top_x_absolute(EwlRLayout *l, int x);
void        ewl_rlayout_set_top_y_relative(EwlRLayout *l, int y);
void        ewl_rlayout_set_top_y_absolute(EwlRLayout *l, int y);
void        ewl_rlayout_set_bot_x_relative(EwlRLayout *l, int x);
void        ewl_rlayout_set_bot_x_absolute(EwlRLayout *l, int x);
void        ewl_rlayout_set_bot_y_relative(EwlRLayout *l, int y);
void        ewl_rlayout_set_bot_y_absolute(EwlRLayout *l, int y);
void        ewl_rlayout_set_req_top_x_relative(EwlRLayout *l, int x);
void        ewl_rlayout_set_req_top_x_absolute(EwlRLayout *l, int x);
void        ewl_rlayout_set_req_top_y_relative(EwlRLayout *l, int y);
void        ewl_rlayout_set_req_top_y_absolute(EwlRLayout *l, int y);
void        ewl_rlayout_set_req_bot_x_relative(EwlRLayout *l, int x);
void        ewl_rlayout_set_req_bot_x_absolute(EwlRLayout *l, int x);
void        ewl_rlayout_set_req_bot_y_relative(EwlRLayout *l, int y);
void        ewl_rlayout_set_req_bot_y_absolute(EwlRLayout *l, int y);
void        ewl_rlayout_set_min_top_x_relative(EwlRLayout *l, int x);
void        ewl_rlayout_set_min_top_x_absolute(EwlRLayout *l, int x);
void        ewl_rlayout_set_min_top_y_relative(EwlRLayout *l, int y);
void        ewl_rlayout_set_min_top_y_absolute(EwlRLayout *l, int y);
void        ewl_rlayout_set_min_bot_x_relative(EwlRLayout *l, int x);
void        ewl_rlayout_set_min_bot_x_absolute(EwlRLayout *l, int x);
void        ewl_rlayout_set_min_bot_y_relative(EwlRLayout *l, int y);
void        ewl_rlayout_set_min_bot_y_absolute(EwlRLayout *l, int y);
void        ewl_rlayout_set_max_top_x_relative(EwlRLayout *l, int x);
void        ewl_rlayout_set_max_top_x_absolute(EwlRLayout *l, int x);
void        ewl_rlayout_set_max_top_y_relative(EwlRLayout *l, int y);
void        ewl_rlayout_set_max_top_y_absolute(EwlRLayout *l, int y);
void        ewl_rlayout_set_max_bot_x_relative(EwlRLayout *l, int x);
void        ewl_rlayout_set_max_bot_x_absolute(EwlRLayout *l, int x);
void        ewl_rlayout_set_max_bot_y_relative(EwlRLayout *l, int y);
void        ewl_rlayout_set_max_bot_y_absolute(EwlRLayout *l, int y);

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
#endif /* _LAYOUT_H_ */
