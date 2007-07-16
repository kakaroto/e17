
void                text_create(void);
Text               *_text_create_copy(Text *);
void                text_load(int);
void                text_redraw(Text *);
void                text_sync(Text *);
void                text_destroy(Text *);
void                text_free(Text *);

void                text_move(Text *, double, double);
void                text_scale(Text *, double, double, double);
void                text_scale_xy(Text *, double, double, double, double);
void                text_rotate(Text *, double, double, double);

void                text_clone(Text *, double, double);
void                text_array(Text *, int, int, double, double);
void                text_array_polar(Text *, double, double, int, double);

void                text_mirror_ab(Text *, double, double);
void                text_mirror_y(Text *, double);
void                text_mirror_x(Text *, double);

void                text_trim_ab(Text *, double, double, int);
void                text_trim_y(Text *, double, int);
void                text_trim_x(Text *, double, int);

void                text_delete(Text *);
void                text_cut(Text *);
void                text_copy(Text *, int, double, double);
void                text_paste(CP_Header, int, double, double);

char              **text_get_info(Text *);

void                text_gravity(Text *, int);
void                text_text(Text *, char *);
void                text_text_height(Text *, double);
void                text_text_style(Text *, char *);

int                 text_select_by_click(Text *, double, double);
void                text_select_by_rect(Text *, double, double, double, double);
int                 text_deselect_by_click(Text *, double, double);
void                text_deselect_by_rect(Text *, double, double, double,
                                          double);
