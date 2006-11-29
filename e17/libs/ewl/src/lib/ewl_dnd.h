#ifndef EWL_DND_H
#define EWL_DND_H

/**
 * @addtogroup Ewl_Dnd Ewl_Dnd: The files containing DND functions
 *
 * @{
 */

extern int EWL_CALLBACK_DND_POSITION;
extern int EWL_CALLBACK_DND_ENTER;
extern int EWL_CALLBACK_DND_LEAVE;
extern int EWL_CALLBACK_DND_DROP;
extern int EWL_CALLBACK_DND_DATA;

int 		 ewl_dnd_init(void);
void		 ewl_dnd_shutdown(void);

void 		 ewl_dnd_drag_start(Ewl_Widget *w);
void             ewl_dnd_drag_drop(Ewl_Widget *w);
void 		 ewl_dnd_drag_widget_clear(void);
Ewl_Widget	*ewl_dnd_drag_widget_get(void);

int 		 ewl_dnd_status_get(void);

void 		 ewl_dnd_position_windows_set(Ewl_Widget *w);

void		 ewl_dnd_provided_types_set(Ewl_Widget *w, const char **types);
int		 ewl_dnd_provided_types_contains(Ewl_Widget *w, char *type);
char	       **ewl_dnd_provided_types_get(Ewl_Widget *w);

void		 ewl_dnd_accepted_types_set(Ewl_Widget *w, const char **types);
int		 ewl_dnd_accepted_types_contains(Ewl_Widget *w, char *type);
const char	**ewl_dnd_accepted_types_get(Ewl_Widget *w);

void 		 ewl_dnd_disable(void);
void 		 ewl_dnd_enable(void);

/**
 * @}
 */

#endif

