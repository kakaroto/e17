
void                image_create(void);
Image              *_image_create_copy(Image *);
void                image_load(int);
void                image_redraw(Image *);
void                image_sync(Image *);
void                image_destroy(Image *);
void                image_free(Image *);

void                image_move(Image *, double, double);
void                image_scale(Image *, double, double, double);
void                image_scale_xy(Image *, double, double, double, double);
void                image_rotate(Image *, double, double, double);

void                image_clone(Image *, double, double);
void                image_array(Image *, int, int, double, double);
void                image_array_polar(Image *, double, double, int, double);

void                image_mirror_ab(Image *, double, double);
void                image_mirror_y(Image *, double);
void                image_mirror_x(Image *, double);

void                image_trim_ab(Image *, double, double, int);
void                image_trim_y(Image *, double, int);
void                image_trim_x(Image *, double, int);

void                image_delete(Image *);
void                image_cut(Image *);
void                image_copy(Image *, int, double, double);
void                image_paste(CP_Header, int, double, double);

char              **image_get_info(Image *);

void                image_gravity(Image *, int);
void                image_image_scale(Image *, double);

int                 image_select_by_click(Image *, double, double);
void                image_select_by_rect(Image *, double, double, double,
                                         double);
int                 image_deselect_by_click(Image *, double, double);
void                image_deselect_by_rect(Image *, double, double, double,
                                           double);
