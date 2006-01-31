#ifndef MENUS_H
#define MENUS_H

/* Desktop File Locations */
#define GNOME_DIRS "/usr/share/applications:/usr/share/gnome/apps"
#define KDE_DIRS "/usr/share/applications/kde:/usr/share/applnk"
#define DEBIAN_DIRS "/usr/share/menu:/usr/lib/menu:/etc/menu"

/* Function Prototypes */
void make_menus(void);
void check_for_dirs(char *path);
void check_for_files(char *dir);

#endif
