#ifndef _EDJE_EDITOR_TOOLBAR_H_
#define _EDJE_EDITOR_TOOLBAR_H_


/* toolbar objects */

Etk_Widget *UI_Toolbar;
Etk_Widget *UI_AddMenu;
Etk_Widget *UI_RemoveMenu;
Etk_Widget *UI_OptionsMenu;

Etk_Widget *UI_PlayButton;
Etk_Widget *UI_PlayImage;
Etk_Widget *UI_PauseImage;
Etk_Widget *UI_AddStateButton;
Etk_Widget *UI_RemoveStateButton;
Etk_Widget *UI_RemovePartButton;
Etk_Widget *UI_RemoveProgramButton;


Etk_Widget* create_toolbar(Etk_Toolbar_Orientation o);
void TogglePlayButton(int set);

Etk_Bool on_AddMenu_item_activated    (Etk_Object *object, void *data);
Etk_Bool on_RemoveMenu_item_activated (Etk_Object *object, void *data);



#endif
