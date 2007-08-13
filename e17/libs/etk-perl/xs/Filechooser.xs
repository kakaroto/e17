#include "EXTERN.h"
#include "perl.h"
#include "XSUB.h"

#include "../ppport.h"

#ifdef _
#undef _
#endif

#include <Etk.h>
#include <Ecore.h>
#include <Ecore_Data.h>

#include "EtkTypes.h"
#include "EtkSignals.h"


MODULE = Etk::Filechooser	PACKAGE = Etk::Filechooser	PREFIX = etk_filechooser_widget_
	
const char *
etk_filechooser_widget_current_folder_get(filechooser_widget)
	Etk_Filechooser_Widget *	filechooser_widget
      ALIAS:
	CurrentFolderGet=1

void
etk_filechooser_widget_current_folder_set(filechooser_widget, folder)
	Etk_Filechooser_Widget *	filechooser_widget
	char *	folder
      ALIAS:
	CurrentFolderSet=1

Etk_Filechooser_Widget *
new(class)
	SV * class
      ALIAS:
	New=1
	CODE:
	RETVAL = ETK_FILECHOOSER_WIDGET(etk_filechooser_widget_new());
	OUTPUT:
	RETVAL

Etk_Bool
etk_filechooser_widget_select_multiple_get(filechooser_widget)
	Etk_Filechooser_Widget *	filechooser_widget
      ALIAS:
	SelectMultipleGet=1

void
etk_filechooser_widget_select_multiple_set(filechooser_widget, select_multiple)
	Etk_Filechooser_Widget *	filechooser_widget
	Etk_Bool	select_multiple
      ALIAS:
	SelectMultipleSet=1

const char *
etk_filechooser_widget_selected_file_get(widget)
	Etk_Filechooser_Widget *	widget
      ALIAS:
	SelectedFileGet=1

void
etk_filechooser_widget_selected_files_get(widget)
	Etk_Filechooser_Widget *	widget
      ALIAS:
	SelectedFilesGet=1
	PPCODE:
	Evas_List * list;

	list = etk_filechooser_widget_selected_files_get(widget);
	XPUSHs(sv_2mortal(newSVCharEvasList(list)));

Etk_Bool
etk_filechooser_widget_show_hidden_get(filechooser_widget)
	Etk_Filechooser_Widget *	filechooser_widget
      ALIAS:
	ShowHiddenGet=1

void
etk_filechooser_widget_show_hidden_set(filechooser_widget, show_hidden)
	Etk_Filechooser_Widget *	filechooser_widget
	Etk_Bool	show_hidden
      ALIAS:
	ShowHiddenSet=1
	
void
etk_filechooser_widget_is_save_set(filechooser_widget, is_save)
	Etk_Filechooser_Widget *        filechooser_widget
	Etk_Bool		is_save
	ALIAS:
	IsSaveSet=1

Etk_Bool
etk_filechooser_widget_is_save_get(filechooser_widget)
	Etk_Filechooser_Widget *        filechooser_widget
	ALIAS:
	IsSaveGet=1


