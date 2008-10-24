
void                circle_create(void);
Circle             *_circle_create_copy(Circle *);

void                ci_load(int);
void                ci_redraw(Circle *);
void                ci_sync(Circle *);
void                ci_destroy(Circle *);
void                ci_free(Circle *);

void                ci_move(Circle *, double, double);
void                ci_scale(Circle *, double, double, double);
void                ci_scale_xy(Circle *, double, double, double, double);
void                ci_rotate(Circle *, double, double, double);

void                ci_clone(Circle *, double, double);
void                ci_array(Circle *, int, int, double, double);
void                ci_array_polar(Circle *, double, double, int, double);

void                ci_mirror_ab(Circle *, double, double);
void                ci_mirror_y(Circle *, double);
void                ci_mirror_x(Circle *, double);

void                ci_trim_ab(Circle *, double, double, int);
void                ci_trim_y(Circle *, double, int);
void                ci_trim_x(Circle *, double, int);

void                ci_delete(Circle *);
void                ci_cut(Circle *);
void                ci_copy(Circle *, int, double, double);
void                ci_paste(CP_Header, int, double, double);

char              **ci_get_info(Circle *);

int                 ci_select_by_click(Circle *, double, double);
void                ci_select_by_rect(Circle *, double, double, double, double);
int                 ci_deselect_by_click(Circle *, double, double);
void                ci_deselect_by_rect(Circle *, double, double, double,
                                        double);

void                ghost_ci_create(void);
void                ghost_ci_redraw(Eina_List*, double, double);
void                ghost_ci_destroy(void);
