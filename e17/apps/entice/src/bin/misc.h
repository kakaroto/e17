#ifndef __MISC_H__
#define __MISC_H__

/* general functions */
double              get_time(void);
void                setup(void);

void                e_mkdirs(char *path);
int                e_child(void* data, int ev_type, Ecore_Event * ev);
// void                e_toggle_fullscreen(void);
void                e_fix_icons(void);

/* drag and drop functions - might need to move these */
/* void                e_dnd_drop_request(Ecore_Event * ev);
void                e_dnd_drop(Ecore_Event * ev);
void                e_dnd_drop_request_free(void);
*/
void                e_size_match(void);
    
#endif /* __MISC_H__ */
