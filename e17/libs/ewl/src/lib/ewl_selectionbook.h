#ifndef __EWL_SELECTIONBOOK_H__
#define __EWL_SELECTIONBOOK_H__

/**
 * @file ewl_selectionbook.h
 * @defgroup Ewl_Selectionbook The selection book group
 * @{
 */

/**
 * @themekey /selectionbook/file
 * @themekey /selectionbook/group
 */

typedef struct Ewl_Selectionbook_Page Ewl_Selectionbook_Page;

#define EWL_SELECTIONBOOK_PAGE(sbook_page) ((Ewl_Selectionbook_Page *) sbook_page)
struct Ewl_Selectionbook_Page
{
	Ewl_Widget     *tab;
	Ewl_Widget     *page;
};

typedef struct Ewl_Selectionbook Ewl_Selectionbook;

#define EWL_SELECTIONBOOK(sbook) ((Ewl_Selectionbook *) sbook)
struct Ewl_Selectionbook
{
	Ewl_Box         box;

	Ewl_Widget     *tab_bar;	/* selectionbar */
	Ewl_Widget     *panel;	/* vbox */

	Ecore_List     *pages;
	Ewl_Selectionbook_Page  *current_page;

	int             num_pages;	/* number of pages */
};

Ewl_Widget     *ewl_selectionbook_new(void);
int             ewl_selectionbook_init(Ewl_Selectionbook *s);
void            ewl_selectionbook_page_add(Ewl_Selectionbook *s,
					   Ewl_Widget *tab, Ewl_Widget * page);
void            ewl_selectionbook_page_rem(Ewl_Selectionbook *s, int num,
					   int destroy);
void            ewl_selectionbook_current_page_rem(Ewl_Selectionbook *s,
						   int destroy);

/*
 * Internally used callbacks, override at your own risk.
 */
void ewl_selectionbook_configure_cb(Ewl_Widget *w, void *ev_data,
					       void *user_data);
void ewl_selectionbook_realize_cb(Ewl_Widget *w, void *ev_data,
					     void *user_data);
void ewl_selectionbook_destroy_cb(Ewl_Widget *w, void *ev_data,
					     void *user_data);
void ewl_selectionbook_page_switch_cb(Ewl_Widget *w, void *ev_data,
						 void *user_data);

/**
 * @}
 */

#endif				/* EWL_SELECTIONBOOK_H__ */
