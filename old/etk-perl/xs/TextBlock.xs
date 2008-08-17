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

MODULE = Etk::TextView	PACKAGE = Etk::TextView	PREFIX = etk_text_view_

Etk_Text_View *
new(class)
	SV * class
	CODE:
	RETVAL = ETK_TEXT_VIEW(etk_text_view_new());
	OUTPUT:
	RETVAL
	

Etk_Textblock *
etk_text_view_textblock_get(text_view)
	Etk_Text_View * text_view
      ALIAS:
	TextblockGet=1

Etk_Textblock_Iter *
etk_text_view_cursor_get(text_view)
	Etk_Text_View *text_view
      ALIAS:
	CursorGet=1

Etk_Textblock_Iter *
etk_text_view_selection_bound_get(text_view)
	Etk_Text_View *text_view
      ALIAS:
	SelectionBoundGet=1



MODULE = Etk::TextBlock::Iter	PACKAGE = Etk::TextBlock::Iter	PREFIX = etk_textblock_iter_

void
etk_textblock_iter_copy(iter, dest_iter)
	Etk_Textblock_Iter *	iter
	Etk_Textblock_Iter *	dest_iter
      ALIAS:
	Copy=1

void
etk_textblock_iter_free(iter)
	Etk_Textblock_Iter *	iter
      ALIAS:
	Free=1

void 
etk_textblock_iter_gravity_set(iter, gravity)
	Etk_Textblock_Iter *iter
	Etk_Textblock_Gravity gravity
      ALIAS:
	GravitySet=1

Etk_Textblock_Gravity
etk_textblock_iter_gravity_get(iter)
	Etk_Textblock_Iter *iter
      ALIAS:
	GravityGet=1

void
etk_textblock_iter_forward_end(iter)
	Etk_Textblock_Iter *	iter
      ALIAS:
	ForwardEnd=1

void
etk_textblock_iter_backward_char(iter)
	Etk_Textblock_Iter *	iter
      ALIAS:
	BackwardChar=1

void
etk_textblock_iter_forward_char(iter)
	Etk_Textblock_Iter *	iter
      ALIAS:
	ForwardChar=1

void
etk_textblock_iter_backward_start(iter)
	Etk_Textblock_Iter *	iter
      ALIAS:
	BackwardStart=1

int
etk_textblock_iter_compare(iter1, iter2)
	Etk_Textblock_Iter *iter1
	Etk_Textblock_Iter *iter2
      ALIAS:
	Compare=1

Etk_Textblock_Iter *
new(class, textblock)
	SV * class
	Etk_Textblock *	textblock
	CODE:
	RETVAL = etk_textblock_iter_new(textblock);
	OUTPUT:
	RETVAL



MODULE = Etk::TextBlock	PACKAGE = Etk::TextBlock	PREFIX = etk_textblock_

Etk_Textblock *
new(class)
	SV * class
	CODE:
	RETVAL = etk_textblock_new();
	OUTPUT:
	RETVAL

void
etk_textblock_text_set(textblock, text, markup)
	Etk_Textblock *	textblock
	char *	text
        Etk_Bool markup
      ALIAS:
	TextSet=1
	
const char *
etk_textblock_text_get(tb, markup)
	Etk_Textblock * tb
	Etk_Bool markup
      ALIAS:
	TextGet=1
	CODE:
	RETVAL = etk_string_get(etk_textblock_text_get(tb, markup));
	OUTPUT:
	RETVAL

const char *
etk_textblock_range_text_get(tb, iter1, iter2, markup)
	Etk_Textblock *tb
	Etk_Textblock_Iter * iter1
	Etk_Textblock_Iter * iter2
	Etk_Bool markup
      ALIAS:
	RangeTextGet=1
	CODE:
	RETVAL = etk_string_get(etk_textblock_range_text_get(tb, iter1, iter2, markup));
	OUTPUT:
	RETVAL

void
etk_textblock_insert(tb, iter, txt)
	Etk_Textblock *tb
	Etk_Textblock_Iter *iter
	SV * txt
      ALIAS:
	Insert=1
	CODE:
	int length;
	const char * text;
	text = SvPV(txt, length);
	etk_textblock_insert(tb, iter, text, length);

void
etk_textblock_insert_markup(tb, iter, txt)
	Etk_Textblock *tb
	Etk_Textblock_Iter *iter
	SV * txt
      ALIAS:
	InsertMarkup=1
	CODE:
	int length;
	const char * text;
	text = SvPV(txt, length);
	etk_textblock_insert_markup(tb, iter, text, length);

void
etk_textblock_clear(tb)
	Etk_Textblock *tb
      ALIAS:
	Clear=1

void
etk_textblock_delete_before(tb, iter)
	Etk_Textblock *tb
	Etk_Textblock_Iter *iter
      ALIAS:
	DeleteBefore=1

void
etk_textblock_delete_after(tb, iter)
	Etk_Textblock *tb
	Etk_Textblock_Iter *iter
      ALIAS:
	DeleteAfter=1

void
etk_textblock_delete_range(tb, iter1, iter2)
	Etk_Textblock *tb
	Etk_Textblock_Iter *iter1
	Etk_Textblock_Iter *iter2
      ALIAS:
	DeleteRange=1

void
etk_textblock_unicode_length_get(unicode_string)
	const char *unicode_string
	ALIAS:
	UnicodeLengthGet=1

Etk_Bool etk_textblock_is_white_char(c)
	int c
	ALIAS:
	IsWhiteChar=1

void
etk_textblock_char_size_get(evas, font_face, font_size)
	Evas * evas
	const char * font_face
	int font_size
	ALIAS:
	CharSizeGet=1
	PPCODE:
	int w, h;
	etk_textblock_char_size_get(evas, font_face, font_size, &w, &h);
	EXTEND(SP, 2);
	PUSHs(sv_2mortal(newSViv(w)));
	PUSHs(sv_2mortal(newSViv(h)));


