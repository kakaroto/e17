#ifndef EWL_DND_H
#define EWL_DND_H

/**
 * @defgroup Ewl_Dnd Ewl_Dnd: The files containing DND functions
 *
 * @{
 */

int 		 ewl_dnd_init(void);
void		 ewl_dnd_shutdown(void);

void 		 ewl_drag_start(Ewl_Widget *w);
void 		 ewl_dnd_drag_widget_clear(void);
Ewl_Widget	*ewl_dnd_drag_widget_get(void);

int 		 ewl_dnd_status_get(void);

void 		 ewl_dnd_position_windows_set(Ewl_Widget *w);
Ewl_Dnd_Types 	*ewl_dnd_types_for_widget_get(Ewl_Widget *widget);

void 		 ewl_dnd_disable(void);
void 		 ewl_dnd_enable(void);

/**
 * @}
 */

#endif

