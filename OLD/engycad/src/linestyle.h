
void                linestyle_set(const char *);

int                 linestyle_get_tiled(void);
int                 linestyle_get_odd(void);
double              linestyle_get_tilelength(void);

Eina_List          *linestyle_get_points(double, double);
Eina_List          *linestyle_get_dx_points(double, double, double);
