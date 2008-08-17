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


MODULE = Etk::Editable	PACKAGE = Etk::Editable	PREFIX = etk_editable_

Evas_Object *
etk_editable_add(Evas * evas)
	ALIAS:
	Add=1

void
etk_editable_disabled_set(editable, disabled)
	Evas_Object *	editable
	Etk_Bool	disabled
	ALIAS:
	DisabledSet=1

Etk_Bool
etk_editable_disabled_get(editable)
	Evas_Object *   editable
	ALIAS:
	DisabledGet=1

void
etk_editable_theme_set(editable, file, group)
	Evas_Object *editable
	const char *file
	const char *group
	ALIAS:
	ThemeSet=1

void
etk_editable_align_set(editable, align)
	Evas_Object *editable
	float	align
	ALIAS:
	AlignSet=1

float
etk_editable_align_get(editable)
	Evas_Object *editable
	ALIAS:
	AlignGet=1

void
etk_editable_password_mode_set(editable, password_mode)
	Evas_Object *editable
	Etk_Bool	password_mode
	ALIAS:
	PasswordModeSet=1

Etk_Bool
etk_editable_password_mode_get(editable)
	Evas_Object *editable
	ALIAS:
	PasswordModeGet=1

void
etk_editable_text_set(editable, text)
	Evas_Object *editable
	const char * text
	ALIAS:
	TextSet=1

const char *
etk_editable_text_get(editable)
	Evas_Object *editable
	ALIAS:
	TextGet=1

char *
etk_editable_text_range_get(editable, start, end)
	Evas_Object *editable
	int start
	int end
	ALIAS:
	TextRangeGet=1

int
etk_editable_text_length_get(editable)
	Evas_Object *editable
	ALIAS:
	TextLengthGet=1

Etk_Bool
etk_editable_insert(editable, pos, text)
	Evas_Object *editable
	int pos
	const char * text
	ALIAS:
	Insert=1

Etk_Bool
etk_editable_delete(editable, start, end)
	Evas_Object *editable
	int start
	int end
	ALIAS:
	Delete=1

void
etk_editable_cursor_pos_set(editable, pos)
	Evas_Object *editable
	int pos
	ALIAS:
	CursorPosSet=1

int
etk_editable_cursor_pos_get(editable)
	Evas_Object *editable
	ALIAS:
	CursorPosGet=1

void 
etk_editable_cursor_move_to_start(editable)
	Evas_Object *editable
	ALIAS:
	CursorMoveToStart=1

void
etk_editable_cursor_move_to_end(editable)
	Evas_Object *editable
	ALIAS:
	CursorMoveToEnd=1

void
etk_editable_cursor_move_left(editable)
	Evas_Object *editable
	ALIAS:
	CursorMoveLeft=1

void
etk_editable_cursor_move_right(editable)
	Evas_Object *editable
	ALIAS:
	CursorMoveRight=1

void
etk_editable_cursor_show(editable)
	Evas_Object *editable
	ALIAS:
	CursorShow=1

void
etk_editable_cursor_hide(editable)
	Evas_Object *editable
	ALIAS:
	CursorHide=1

void
etk_editable_selection_pos_set(editable, pos)
	Evas_Object *editable
	int pos
	ALIAS:
	SelectionPosSet=1

int 
etk_editable_selection_pos_get(editable)
	Evas_Object *editable
	ALIAS:
	SelectionPosGet=1

void
etk_editable_selection_move_to_start(editable)
	Evas_Object *editable
	ALIAS:
	SelectionMoveToStart=1

void
etk_editable_selection_move_to_end(editable)
	Evas_Object *editable
	ALIAS:
	SelectionMoveToEnd=1

void
etk_editable_selection_move_left(editable)
	Evas_Object *editable
	ALIAS:
	SelectionMoveLeft=1

void 
etk_editable_selection_move_right(editable)
	Evas_Object *editable
	ALIAS:
	SelectionMoveRight=1

void 
etk_editable_select_all(editable)
	Evas_Object *editable
	ALIAS:
	SelectAll=1

void 
etk_editable_unselect_all(editable)
	Evas_Object *editable
	ALIAS:
	UnselectAll=1

void
etk_editable_selection_show(editable)
	Evas_Object *editable
	ALIAS:
	SelectionShow=1

void
etk_editable_selection_hide(editable)
	Evas_Object *editable
	ALIAS:
	SelectionHide=1

int
etk_editable_pos_get_from_coords(editable, x, y)
	Evas_Object *editable
	int x
	int y
	ALIAS:
	PosGetFromCoods=1

void
etk_editable_char_size_get(editable)
	Evas_Object *editable
	ALIAS:
	CharSizeGet=1
	PPCODE:
	int w, h;
	etk_editable_char_size_get(editable, &w, &h);
	EXTEND(SP, 2);
	PUSHs(sv_2mortal(newSViv(w)));
	PUSHs(sv_2mortal(newSViv(h)));
	

