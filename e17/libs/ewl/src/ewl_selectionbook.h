
#ifndef __EWL_SELECTIONBOOK_H__
#define __EWL_SELECTIONBOOK_H__


typedef struct _ewl_sbook_page Ewl_SbookPage;

#define EWL_SBOOKPAGE(sbook_page) ((Ewl_SbookPage *) sbook_page)
struct _ewl_sbook_page {
	Ewl_Widget     *tab;
	Ewl_Widget     *page;
};

typedef struct _ewl_selectionbook Ewl_Selectionbook;

#define EWL_SELECTIONBOOK(sbook) ((Ewl_Selectionbook *) sbook)
struct _ewl_selectionbook {
	Ewl_Box         box;

	Ewl_Widget     *tab_bar;	/* selectionbar */
	Ewl_Widget     *panel;	/* vbox */

	Ewd_List       *pages;
	Ewl_SbookPage  *current_page;

	int             num_pages;	/* number of pages */
};


Ewl_Widget     *ewl_selectionbook_new();
void            ewl_selectionbook_init(Ewl_Selectionbook * s);
void            ewl_selectionbook_add_page(Ewl_Selectionbook * s,
					   Ewl_Widget * tab, Ewl_Widget * page);
void            ewl_selectionbook_rem_page(Ewl_Selectionbook * s, int num,
					   int destroy);
void            ewl_selectionbook_rem_current_page(Ewl_Selectionbook * s,
						   int destroy);


#endif /* EWL_SELECTIONBOOK_H__ */
