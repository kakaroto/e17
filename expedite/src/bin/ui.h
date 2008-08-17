#ifndef UI_H
#define UI_H

void ui_args(int argc, char **argv);
void ui_loop(void);
void ui_menu(void);
void ui_func_set(void (*kfunc) (char *key), void (*lfunc) (double t, int f));
void ui_fps(double fps);

#endif
