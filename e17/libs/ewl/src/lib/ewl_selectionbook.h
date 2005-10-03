
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

typedef struct _ewl_sbook_page Ewl_SbookPage;

#define EWL_SBOOKPAGE(sbook_page) ((Ewl_SbookPage *) sbook_page)
struct _ewl_sbook_page
{
	Ewl_Widget     *tab;
	Ewl_Widget     *page;
};

typedef struct _ewl_selectionbook Ewl_Selectionbook;

#define EWL_SELECTIONBOOK(sbook) ((Ewl_Selectionbook *) sbook)
struct _ewl_selectionbook
{
	Ewl_Box         box;

	Ewl_Widget     *tab_bar;	/* selectionbar */
	Ewl_Widget     *panel;	/* vbox */

	Ecore_List     *pages;
	Ewl_SbookPage  *current_page;

	int             num_pages;	/* number of pages */
};


Ewl_Widget     *ewl_selectionbook_new(void);
int             ewl_selectionbook_init(Ewl_Selectionbook * s);
void            ewl_selectionbook_page_add(Ewl_Selectionbook * s,
					   Ewl_Widget * tab, Ewl_Widget * page);
void            ewl_selectionbook_page_rem(Ewl_Selectionbook * s, int num,
					   int destroy);
void            ewl_selectionbook_current_page_rem(Ewl_Selectionbook * s,
						   int destroy);

/*
 * Internally used callbacks, override at your own risk.
 */
void            ewl_selectionbook_configure_cb(Ewl_Widget * w, void *ev_data,
					       void *user_data);
void            ewl_selectionbook_realize_cb(Ewl_Widget * w, void *ev_data,
					     void *user_data);
void            ewl_selectionbook_destroy_cb(Ewl_Widget * w, void *ev_data,
					     void *user_data);
void            ewl_selectionbook_page_switch_cb(Ewl_Widget * w, void *ev_data,
						 void *user_data);

/**
 * @}
 */

#endif				/* EWL_SELECTIONBOOK_H__ */
