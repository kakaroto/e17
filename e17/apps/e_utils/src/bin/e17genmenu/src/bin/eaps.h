#ifndef EAPS_H
#define EAPS_H

void create_dir_eap(char *path, char *cat);
char *get_window_class(char *file);
void write_icon(char *f, char *i);
void write_eap(char *file, char *section, char *value);

#endif
