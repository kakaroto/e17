#ifndef __MISC_H__
#define __MISC_H__

/* general functions */
double get_time (void);
void setup(void);

void e_mkdirs(char *path);
void e_child(Ecore_Event * ev);
void e_toggle_fullscreen(void);
void e_fix_icons(void);

#endif /* __MISC_H__ */
