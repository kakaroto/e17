#ifndef ELM_IND_WIN_H
# define ELM_IND_WIN_H

typedef struct _Elm_Ind_Win Elm_Ind_Win;

struct _Elm_Ind_Win 
{
   Evas_Object *win, *mode, *kbd;
   Eina_List *handlers;
   unsigned char dragging : 1;
};

EAPI Elm_Ind_Win *elm_ind_win_new(Ecore_X_Window zone);

#endif
