#ifndef ELICIT_UTIL_H
#define ELICIT_UTIL_H

void elicit_color_rgb_to_hsv(int rr, int gg, int bb, double *hh, double *ss, double *vv);
void elicit_color_hsv_to_rgb(double hh, double ss, double vv, int *rr, int *gg, int *bb);
char * elicit_color_rgb_to_hex(int rr, int gg, int bb);
int elicit_glob_match(char *str, char *glob);

void elicit_util_color_get(int *r, int *g, int *b);
void elicit_util_shoot(Evas_Object *shot, int w, int h);

#endif
