
void                point_create(void);
void                point_load(int);
void                point_redraw(Point *);
void                point_sync(Point *);
void                point_destroy(Point *);
void                point_free(Point *);

void                point_move(Point *, double, double);
void                point_scale(Point *, double, double, double);
void                point_scale_xy(Point *, double, double, double, double);
void                point_rotate(Point *, double, double, double);

void                point_clone(Point *, double, double);
void                point_array(Point *, int, int, double, double);
void                point_array_polar(Point *, double, double, int, double);

void                point_mirror_ab(Point *, double, double);
void                point_mirror_y(Point *, double);
void                point_mirror_x(Point *, double);

void                point_trim_ab(Point *, double, double, int);
void                point_trim_y(Point *, double, int);
void                point_trim_x(Point *, double, int);

void                point_delete(Point *);
void                point_cut(Point *);
void                point_copy(Point *, int, double, double);
void                point_paste(CP_Header, int, double, double);

char              **point_get_info(Point *);

int                 point_select_by_click(Point *, double, double);
void                point_select_by_rect(Point *, double, double, double,
                                         double);
int                 point_deselect_by_click(Point *, double, double);
void                point_deselect_by_rect(Point *, double, double, double,
                                           double);
