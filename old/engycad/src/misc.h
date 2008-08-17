
/* logo */
void                logo_init(void);

/* iconv stuff */
//char               *my_iconv(iconv_t, char *);

/* name generator */
char               *generate_name(void);

/* evas_text replacement */
Evas_Object        *my_evas_add_text(Evas*, char *, int, char *);
void                my_evas_text_get_max_ascent_descent(Evas*, Evas_Object*,
                                                        double *, double *);
int                 my_evas_get_text_width(Evas*, Evas_Object*);
void                my_evas_set_text(Evas*, Evas_Object*, char *);

/* a couple of string funcs */
char               *get_rid_of_right_spaces(char *s);
char               *get_rid_of_left_spaces(char *s);
char               *get_rid_of_extra_spaces(char *s);

/* coords parsing */
int                 get_values(char *, float x1, float y1, float *x, float *y);

/* screen-to-world transformation */
double              s2w_x(int);
double              s2w_y(int);
double              w2s_x(double);
double              w2s_y(double);

/* ipc */
void                my_run(char *);
void timer(void);

/* usefull aliasing */
void alias_init(void);
void alias_shutdown(void);
char               *_alias(char *);


Evas_Object        *evas_image_load(char *file);

#define MB_DISMISS 1
#define MB_APPLY_CANCEL 2
#define MB_YES_NO 3
#define MB_YES_NO_CANCEL 4
