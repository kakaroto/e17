
void                common_move(double, double);
void                common_clone(double, double);
void                common_rotate(double, double, double);

void                common_array(int, int, double, double);
void                common_polar(double, double, int, double);

void                common_scale(double, double, double);
void                common_scale_xy(double, double, double, double);

void                common_mirror_ab(double, double);
void                common_mirror_h(double);
void                common_mirror_v(double);

void                common_trim_ab(double, double, int);
void                common_trim_h(double, int);
void                common_trim_v(double, int);

void                common_all_sel(void);
void                common_all_desel(void);
void                common_inv_sel(void);

void                common_delete(void);

void                common_all_sel(void);
void                common_all_desel(void);
void                common_inv_sel(void);

void                common_cut(double, double);
void                common_copy(double, double);
void                common_paste(double, double);

void                common_linestyle(char *);
void                common_color(int, int, int, int);
void                common_thickness(float);
void                common_linescale(float);

void                common_gravity(int);
void                common_text_height(double);
void                common_text(char *);
void                common_image_scale(double);

char              **common_get_info(void);
Color               common_get_color(void);

void                common_entity_load(int, int);
