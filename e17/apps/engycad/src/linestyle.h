
void                linestyle_set(const char *);

int                 linestyle_get_tiled(void);
int                 linestyle_get_odd(void);
double              linestyle_get_tilelength(void);

Evas_List          *linestyle_get_points(double, double);
Evas_List          *linestyle_get_dx_points(double, double, double);
