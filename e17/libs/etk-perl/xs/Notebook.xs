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


MODULE = Etk::Notebook	PACKAGE = Etk::Notebook	PREFIX = etk_notebook_
	
int
etk_notebook_current_page_get(notebook)
	Etk_Notebook *	notebook
      ALIAS:
	CurrentPageGet=1

void
etk_notebook_current_page_set(notebook, page_num)
	Etk_Notebook *	notebook
	int	page_num
      ALIAS:
	CurrentPageSet=1

Etk_Notebook *
new(class)
	SV * class
	CODE:
	RETVAL = ETK_NOTEBOOK(etk_notebook_new());
	OUTPUT:
	RETVAL

int
etk_notebook_num_pages_get(notebook)
	Etk_Notebook *	notebook
      ALIAS:
	NumPagesGet=1

int
etk_notebook_page_append(notebook, tab_label, page_child)
	Etk_Notebook *	notebook
	char *	tab_label
	Etk_Widget *	page_child
      ALIAS:
	PageAppend=1

Etk_Widget *
etk_notebook_page_child_get(notebook, page_num)
	Etk_Notebook *	notebook
	int	page_num
      ALIAS:
	PageChildGet=1

void
etk_notebook_page_child_set(notebook, page_num, child)
	Etk_Notebook *	notebook
	int	page_num
	Etk_Widget *	child
      ALIAS:
	PageChildSet=1

int
etk_notebook_page_index_get(notebook, child)
	Etk_Notebook *	notebook
	Etk_Widget *	child
      ALIAS:
	PageIndexGet=1

int
etk_notebook_page_insert(notebook, tab_label, page_child, position)
	Etk_Notebook *	notebook
	char *	tab_label
	Etk_Widget *	page_child
	int	position
      ALIAS:
	PageInsert=1

int
etk_notebook_page_next(notebook)
	Etk_Notebook *	notebook
      ALIAS:
	PageNext=1

int
etk_notebook_page_prepend(notebook, tab_label, page_child)
	Etk_Notebook *	notebook
	char *	tab_label
	Etk_Widget *	page_child
      ALIAS:
	PagePrepend=1

int
etk_notebook_page_prev(notebook)
	Etk_Notebook *	notebook
      ALIAS:
	PagePrev=1

void
etk_notebook_page_remove(notebook, page_num)
	Etk_Notebook *	notebook
	int	page_num
      ALIAS:
	PageRemove=1

const char *
etk_notebook_page_tab_label_get(notebook, page_num)
	Etk_Notebook *	notebook
	int	page_num
      ALIAS:
	PageTabLabelGet=1

void
etk_notebook_page_tab_label_set(notebook, page_num, tab_label)
	Etk_Notebook *	notebook
	int	page_num
	char *	tab_label
      ALIAS:
	PageTabLabelSet=1

Etk_Widget *
etk_notebook_page_tab_widget_get(notebook, page_num)
	Etk_Notebook *	notebook
	int	page_num
      ALIAS:
	PageTabWidgetGet=1

void
etk_notebook_page_tab_widget_set(notebook, page_num, tab_widget)
	Etk_Notebook *	notebook
	int	page_num
	Etk_Widget *	tab_widget
      ALIAS:
	PageTabWidgetSet=1

void
etk_notebook_tabs_visible_set(notebook, visible)
	Etk_Notebook * notebook
	Etk_Bool visible
      ALIAS:
	TabsVisibleSet=1

Etk_Bool
etk_notebook_tabs_visible_get(notebook)
	Etk_Notebook * notebook
      ALIAS:
	TabsVisibleGet=1

void
etk_notebook_tabs_homogeneous_set(notebook, tabs_homogeneous)
	Etk_Notebook * notebook
	Etk_Bool tabs_homogeneous
	ALIAS:
	TabsHomogeneousSet=1

Etk_Bool
etk_notebook_tabs_homogeneous_get(notebook)
	Etk_Notebook * notebook
	ALIAS:
	TabsHomogeneousGet=1

