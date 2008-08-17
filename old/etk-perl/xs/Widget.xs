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


MODULE = Etk::Widget	PACKAGE = Etk::Widget	PREFIX = etk_widget_

void
etk_widget_color_set(widget, r, g, b, a)
	Etk_Widget * 	widget
	int	r
	int	g
	int	b
	int	a
	ALIAS:
	ColorSet=1

void
etk_widget_disabled_set(widget, disabled)
	Etk_Widget *	widget
	Etk_Bool	disabled
	ALIAS:
	DisabledSet=1

Etk_Bool
etk_widget_disabled_get(widget)
	Etk_Widget *    widget
	ALIAS:
	DisabledGet=1

Evas_Object *
etk_widget_clip_get(widget)
	Etk_Widget *	widget
      ALIAS:
	ClipGet=1

void
etk_widget_clip_set(widget, clip)
	Etk_Widget *	widget
	Evas_Object *	clip
      ALIAS:
	ClipSet=1

void
etk_widget_clip_unset(widget)
	Etk_Widget *	widget
      ALIAS:
	ClipUnset=1

Etk_Bool
etk_widget_dnd_dest_get(widget)
	Etk_Widget *	widget
      ALIAS:
	DndDestGet=1

void
etk_widget_dnd_dest_set(widget, on)
	Etk_Widget *	widget
	Etk_Bool	on
      ALIAS:
	DndDestSet=1

Evas_List *
etk_widget_dnd_dest_widgets_get()
      ALIAS:
	DndDestWidgetsGet=1

void
etk_widget_dnd_drag_data_set(widget, types, num_types, data, data_size)
	Etk_Widget *	widget
	const char **	types
	int	num_types
	void *	data
	int	data_size
      ALIAS:
	DndDragDataSet=1

Etk_Widget *
etk_widget_dnd_drag_widget_get(widget)
	Etk_Widget *	widget
      ALIAS:
	DndDragWidgetGet=1

void
etk_widget_dnd_drag_widget_set(widget, drag_widget)
	Etk_Widget *	widget
	Etk_Widget *	drag_widget
      ALIAS:
	DndDragWidgetSet=1

void
etk_widget_dnd_files_get(e)
	Etk_Widget *	e
      ALIAS:
	DndFilesGet=1
	PPCODE:
	const char ** files;
	int 	* num_files;
	int 	i;

	files = etk_widget_dnd_files_get(e, num_files);
	for (i=0; i<*num_files; i++)
		XPUSHs(sv_2mortal(newSVpv(files[i], strlen(files[i]))));
	
void
etk_widget_focusable_set(widget, focusable)
	Etk_Widget * widget
	Etk_Bool focusable
	ALIAS:
	FocusableSet=1

Etk_Bool
etk_widget_focusable_get(widget)
	Etk_Widget * widget
	ALIAS:
	FocusableGet=1

Etk_Bool
etk_widget_dnd_internal_get(widget)
	Etk_Widget *	widget
      ALIAS:
	DndInternalGet=1

void
etk_widget_dnd_internal_set(widget, on)
	Etk_Widget *	widget
	Etk_Bool	on
      ALIAS:
	DndInternalSet=1

Etk_Bool
etk_widget_dnd_source_get(widget)
	Etk_Widget *	widget
      ALIAS:
	DndSourceGet=1

void
etk_widget_dnd_source_set(widget, on)
	Etk_Widget *	widget
	Etk_Bool	on
      ALIAS:
	DndSourceSet=1

void
etk_widget_dnd_types_get(widget)
	Etk_Widget *	widget
      ALIAS:
	DndTypesGet=1
	PPCODE:
	const char ** types;
	int 	* num;
	int 	i;

	types = etk_widget_dnd_types_get(widget, num);
	for (i=0; i<*num; i++)
		XPUSHs(sv_2mortal(newSVpv(types[i], strlen(types[i]))));

void
etk_widget_dnd_types_set(widget, perl_types)
	Etk_Widget *	widget
	AV * perl_types
      ALIAS:
	DndTypesSet=1
	CODE:
	const char **	types;
	int	num;
	int	i;
	
	num = (int) av_len(perl_types) + 1;
	types = calloc(num, sizeof(char *));
	for (i=0; i<num; i++) 
	{
		SV ** val;
		val = av_fetch(perl_types, i, 0);
		if (val)
			types[i] = (char *) SvIV(*val);
		else
			types[i] = 0;

	}
	etk_widget_dnd_types_set(widget, types, num);


void
etk_widget_enter(widget)
	Etk_Widget *	widget
      ALIAS:
	Enter=1

void
etk_widget_focus(widget)
	Etk_Widget *	widget
      ALIAS:
	Focus=1

void
etk_widget_geometry_get(widget)
	Etk_Widget *	widget
      ALIAS:
	GeometryGet=1
	PPCODE:
	int 	x;
	int 	y;
	int 	w;
	int 	h;

	etk_widget_geometry_get(widget, &x, &y, &w, &h);
	EXTEND(SP, 4);
	PUSHs(sv_2mortal(newSViv(x)));
	PUSHs(sv_2mortal(newSViv(y)));
	PUSHs(sv_2mortal(newSViv(w)));
	PUSHs(sv_2mortal(newSViv(h)));

void
etk_widget_inner_geometry_get(widget)
	Etk_Widget *	widget
      ALIAS:
	InnerGeometryGet=1
	PPCODE:
	int 	x;
	int 	y;
	int 	w;
	int 	h;
	etk_widget_inner_geometry_get(widget, &x, &y, &w, &h);
	EXTEND(SP, 4);
	PUSHs(sv_2mortal(newSViv(x)));
	PUSHs(sv_2mortal(newSViv(y)));
	PUSHs(sv_2mortal(newSViv(w)));
	PUSHs(sv_2mortal(newSViv(h)));

Etk_Bool
etk_widget_has_event_object_get(widget)
	Etk_Widget *	widget
      ALIAS:
	HasEventObjectGet=1

void
etk_widget_has_event_object_set(widget, has_event_object)
	Etk_Widget *	widget
	Etk_Bool	has_event_object
      ALIAS:
	HasEventObjectSet=1

void
etk_widget_hide(widget)
	Etk_Widget *	widget
      ALIAS:
	Hide=1

void
etk_widget_hide_all(widget)
	Etk_Widget *	widget
      ALIAS:
	HideAll=1

Etk_Bool
etk_widget_is_swallowed(widget)
	Etk_Widget *	widget
      ALIAS:
	IsSwallowed=1

Etk_Bool
etk_widget_is_visible(widget)
	Etk_Widget *	widget
      ALIAS:
	IsVisible=1

void
etk_widget_leave(widget)
	Etk_Widget *	widget
      ALIAS:
	Leave=1

void
etk_widget_lower(widget)
	Etk_Widget *	widget
      ALIAS:
	Lower=1

Etk_Bool
etk_widget_member_object_add(widget, object)
	Etk_Widget *	widget
	Evas_Object *	object
      ALIAS:
	MemberObjectAdd=1

void
etk_widget_member_object_del(widget, object)
	Etk_Widget *	widget
	Evas_Object *	object
      ALIAS:
	MemberObjectDel=1

void
etk_widget_member_object_lower(widget, object)
	Etk_Widget *	widget
	Evas_Object *	object
      ALIAS:
	MemberObjectLower=1

void
etk_widget_member_object_raise(widget, object)
	Etk_Widget *	widget
	Evas_Object *	object
      ALIAS:
	MemberObjectRaise=1

void
etk_widget_member_object_stack_above(widget, object, above)
	Etk_Widget *	widget
	Evas_Object *	object
	Evas_Object *	above
      ALIAS:
	MemberObjectStackAbove=1

void
etk_widget_member_object_stack_below(widget, object, below)
	Etk_Widget *	widget
	Evas_Object *	object
	Evas_Object *	below
      ALIAS:
	MemberObjectStackBelow=1

Etk_Widget *
etk_widget_parent_get(widget)
	Etk_Widget *	widget
      ALIAS:
	ParentGet=1

void
etk_widget_parent_set(widget, parent)
	Etk_Widget *	widget
	Etk_Widget *	parent
      ALIAS:
	ParentSet=1

void
etk_widget_parent_set_full(widget, parent, remove_from_container)
	Etk_Widget *	widget
	Etk_Widget *	parent
	Etk_Bool	remove_from_container
      ALIAS:
	ParentSetFull=1

Etk_Bool
etk_widget_pass_mouse_events_get(widget)
	Etk_Widget *	widget
      ALIAS:
	PassMouseEventsGet=1

void
etk_widget_pass_mouse_events_set(widget, pass_mouse_events)
	Etk_Widget *	widget
	Etk_Bool	pass_mouse_events
      ALIAS:
	PassMouseEventsSet=1

void
etk_widget_raise(widget)
	Etk_Widget *	widget
      ALIAS:
	Raise=1

void
etk_widget_redraw_queue(widget)
	Etk_Widget *	widget
      ALIAS:
	RedrawQueue=1

Etk_Bool
etk_widget_repeat_mouse_events_get(widget)
	Etk_Widget *	widget
      ALIAS:
	RepeatMouseEventsGet=1

void
etk_widget_repeat_mouse_events_set(widget, repeat_mouse_events)
	Etk_Widget *	widget
	Etk_Bool	repeat_mouse_events
      ALIAS:
	RepeatMouseEventsSet=1

void
etk_widget_show(widget)
	Etk_Widget *	widget
      ALIAS:
	Show=1

void
etk_widget_show_all(widget)
	Etk_Widget *	widget
      ALIAS:
	ShowAll=1

void
etk_widget_size_allocate(widget, geometry)
	Etk_Widget *	widget
	Etk_Geometry 	geometry
      ALIAS:
	SizeAllocate=1

void
etk_widget_size_recalc_queue(widget)
	Etk_Widget *	widget
      ALIAS:
	SizeRecalcQueue=1

void
etk_widget_size_request(widget, size_requisition)
	Etk_Widget *	widget
	Etk_Size *	size_requisition
      ALIAS:
	SizeRequest=1

void
etk_widget_size_request_full(widget, size_requisition, hidden_has_no_size)
	Etk_Widget *	widget
	Etk_Size *	size_requisition
	Etk_Bool	hidden_has_no_size
      ALIAS:
	SizeRequestFull=1

void
etk_widget_size_request_set(widget, w, h)
	Etk_Widget *	widget
	int	w
	int	h
      ALIAS:
	SizeRequestSet=1

Etk_Bool
etk_widget_swallow_widget(swallowing_widget, part, widget_to_swallow)
	Etk_Widget *	swallowing_widget
	char *	part
	Etk_Widget *	widget_to_swallow
      ALIAS:
	SwallowWidget=1

const char *
etk_widget_theme_file_get(widget)
	Etk_Widget *	widget
      ALIAS:
	ThemeFileGet=1

void
etk_widget_theme_file_set(widget, theme_file)
	Etk_Widget *	widget
	char *	theme_file
      ALIAS:
	ThemeFileSet=1

const char *
etk_widget_theme_group_get(widget)
	Etk_Widget *	widget
      ALIAS:
	ThemeGroupGet=1

void
etk_widget_theme_group_set(widget, theme_group)
	Etk_Widget *	widget
	char *	theme_group
      ALIAS:
	ThemeGroupSet=1

Etk_Widget *
etk_widget_theme_parent_get(widget)
	Etk_Widget *	widget
      ALIAS:
	ThemeParentGet=1

void
etk_widget_theme_parent_set(widget, theme_parent)
	Etk_Widget *	widget
	Etk_Widget *	theme_parent
      ALIAS:
	ThemeParentSet=1

Evas *
etk_widget_toplevel_evas_get(widget)
	Etk_Widget *	widget
      ALIAS:
	ToplevelEvasGet=1

Etk_Toplevel *
etk_widget_toplevel_parent_get(widget)
	Etk_Widget *	widget
      ALIAS:
	ToplevelParentGet=1

void
etk_widget_unfocus(widget)
	Etk_Widget *	widget
      ALIAS:
	Unfocus=1

Etk_Bool
etk_widget_is_focused(widget)
	Etk_Widget *	widget
      ALIAS:
	IsFocused=1

void
etk_widget_unswallow_widget(swallowing_widget, widget)
	Etk_Widget *	swallowing_widget
	Etk_Widget *	widget
      ALIAS:
	UnswallowWidget=1

Etk_Bool
etk_widget_internal_get(widget)
	Etk_Widget *	widget
      ALIAS:
	InternalGet=1

void
etk_widget_internal_set(widget, internal)
	Etk_Widget *	widget
	Etk_Bool	internal
      ALIAS:
	InternalSet=1

void
etk_widget_padding_set(widget, left, right, top, bottom)
	Etk_Widget *    widget
	int	left
	int	right
	int	top
	int	bottom
	ALIAS:
	PaddingSet=1

void
etk_widget_padding_get(widget)
	Etk_Widget *    widget
	ALIAS:
	PaddingGet=1
	PPCODE:
	int left, right, top, bottom;
	etk_widget_padding_get(widget, &left, &right, &top, &bottom);
	EXTEND(SP, 4);
	PUSHs(sv_2mortal(newSViv(left)));
	PUSHs(sv_2mortal(newSViv(right)));
	PUSHs(sv_2mortal(newSViv(top)));
	PUSHs(sv_2mortal(newSViv(bottom)));

void
etk_widget_theme_set(widget, theme_file, theme_group)
	Etk_Widget *    widget
	const char * 	theme_file
	const char * 	theme_group
	ALIAS:
	ThemeSet=1
	
void
etk_widget_theme_signal_emit(widget, signal_name, size_recalc)
	Etk_Widget *    widget
	const char * 	signal_name
	Etk_Bool	size_recalc
	ALIAS:
	ThemeSignalEmit=1

void
etk_widget_theme_part_text_set(widget, part_name, text)
	Etk_Widget *    widget
	const char *    part_name
	char *		text
	ALIAS:
	ThemePartTextSet=1
	
Etk_Bool
etk_widget_swallow_object(swallower, part, object)
	Etk_Widget *    swallower
	const char * 	part
	Evas_Object *	object
	ALIAS:
	SwallowObject=1

void
etk_widget_unswallow_object(swallower, object)
	Etk_Widget *    swallower
	Evas_Object *	object
	ALIAS:
	UnswallowObject=1

Etk_Widget_Swallow_Error
etk_widget_swallow_error_get()
	ALIAS:
	SwallowErrorGet=1


