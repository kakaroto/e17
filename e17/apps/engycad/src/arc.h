
void                arc_create(void);
Arc                *_arc_create_copy(Arc *);

void                arc_load(int);
void                arc_redraw(Arc *);
void                arc_sync(Arc *);
void                arc_destroy(Arc *);
void                arc_free(Arc *);

void                arc_move(Arc *, double, double);
void                arc_scale(Arc *, double, double, double);
void                arc_scale_xy(Arc *, double, double, double, double);
void                arc_rotate(Arc *, double, double, double);

void                arc_clone(Arc *, double, double);
void                arc_array(Arc *, int, int, double, double);
void                arc_array_polar(Arc *, double, double, int, double);

void                arc_mirror_ab(Arc *, double, double);
void                arc_mirror_y(Arc *, double);
void                arc_mirror_x(Arc *, double);

void                arc_trim_ab(Arc *, double, double, int);
void                arc_trim_y(Arc *, double, int);
void                arc_trim_x(Arc *, double, int);

void                arc_delete(Arc *);
void                arc_cut(Arc *);
void                arc_copy(Arc *, int, double, double);
void                arc_paste(CP_Header, int, double, double);

char              **arc_get_info(Arc *);

int                 arc_select_by_click(Arc *, double, double);
void                arc_select_by_rect(Arc *, double, double, double, double);
int                 arc_deselect_by_click(Arc *, double, double);
void                arc_deselect_by_rect(Arc *, double, double, double, double);
