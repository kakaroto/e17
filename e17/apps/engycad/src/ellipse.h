
void                ellipse_create(void);
Ellipse            *_ell_create_copy(Ellipse *);

void                ell_load(int);
void                ell_redraw(Ellipse *);
void                ell_sync(Ellipse *);
void                ell_destroy(Ellipse *);
void                ell_free(Ellipse *);

void                ell_move(Ellipse *, double, double);
void                ell_scale(Ellipse *, double, double, double);
void                ell_scale_xy(Ellipse *, double, double, double, double);
void                ell_rotate(Ellipse *, double, double, double);

void                ell_clone(Ellipse *, double, double);
void                ell_array(Ellipse *, int, int, double, double);
void                ell_array_polar(Ellipse *, double, double, int, double);

void                ell_mirror_ab(Ellipse *, double, double);
void                ell_mirror_y(Ellipse *, double);
void                ell_mirror_x(Ellipse *, double);

void                ell_trim_ab(Ellipse *, double, double, int);
void                ell_trim_y(Ellipse *, double, int);
void                ell_trim_x(Ellipse *, double, int);

void                ell_delete(Ellipse *);
void                ell_cut(Ellipse *);
void                ell_copy(Ellipse *, int, double, double);
void                ell_paste(CP_Header, int, double, double);

char              **ell_get_info(Ellipse *);

int                 ell_select_by_click(Ellipse *, double, double);
void                ell_select_by_rect(Ellipse *, double, double, double,
                                       double);
int                 ell_deselect_by_click(Ellipse *, double, double);
void                ell_deselect_by_rect(Ellipse *, double, double, double,
                                         double);

void                ghost_ell_create(void);
void                ghost_ell_redraw(Evas_List*, double, double);
void                ghost_ell_destroy(void);
