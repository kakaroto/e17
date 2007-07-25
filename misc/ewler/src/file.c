/**
 * File manager for ewler/ewl_uic
 * Deals heavily in EXML<->module structures
 */
#include <Ewl.h>
#include <EXML.h>
#include <dlfcn.h>

#include "ewler.h"
#include "config.h"

static char *callback_names[] = {
	"EWL_CALLBACK_EXPOSE", 
	"EWL_CALLBACK_REALIZE", 
	"EWL_CALLBACK_UNREALIZE", 
	"EWL_CALLBACK_SHOW", 
	"EWL_CALLBACK_HIDE", 
	"EWL_CALLBACK_DESTROY", 
	"EWL_CALLBACK_DELETE_WINDOW", 
	"EWL_CALLBACK_CONFIGURE", 
	"EWL_CALLBACK_REPARENT", 
	"EWL_CALLBACK_KEY_DOWN", 
	"EWL_CALLBACK_KEY_UP", 
	"EWL_CALLBACK_MOUSE_DOWN", 
	"EWL_CALLBACK_MOUSE_UP", 
	"EWL_CALLBACK_MOUSE_MOVE", 
	"EWL_CALLBACK_MOUSE_WHEEL", 
	"EWL_CALLBACK_FOCUS_IN", 
	"EWL_CALLBACK_FOCUS_OUT", 
	"EWL_CALLBACK_SELECT", 
	"EWL_CALLBACK_DESELECT", 
	"EWL_CALLBACK_CLICKED", 
	"EWL_CALLBACK_DOUBLE_CLICKED", 
	"EWL_CALLBACK_HILITED", 
	"EWL_CALLBACK_VALUE_CHANGED", 
	"EWL_CALLBACK_STATE_CHANGED", 
	"EWL_CALLBACK_APPEARANCE_CHANGED", 
	"EWL_CALLBACK_WIDGET_ENABLE", 
	"EWL_CALLBACK_WIDGET_DISABLE", 
	"EWL_CALLBACK_PASTE"
};

void
file_project_save( Ewler_Project *p )
{
	EXML *xml;
	Ecore_List *names;
	char *name;

	xml = exml_new();

	exml_start(xml);
	exml_tag_set(xml, "project");
	exml_attribute_set(xml, "source", "ewler");
	exml_attribute_set(xml, "version", VERSION);
	
	exml_start(xml);
	exml_tag_set(xml, "name");
	exml_value_set(xml, p->name);
	exml_end(xml);

	exml_start(xml);
	exml_tag_set(xml, "path");
	exml_value_set(xml, p->path);
	exml_end(xml);

	exml_start(xml);
	exml_tag_set(xml, "files");

	names = ecore_hash_keys(p->files);

	while( (name = ecore_list_next(names)) ) {
		exml_start(xml);
		exml_tag_set(xml, "file");
		exml_attribute_set(xml, "name", name);
		exml_value_set(xml, ecore_hash_get(p->files, name));
		exml_end(xml);
	}

	ecore_list_destroy(names);

	exml_end(xml);
	exml_end(xml);

	/* maybe prepend path to this */
	exml_file_write(xml, p->filename);

	exml_destroy(xml);
}

int
file_project_open( Ewler_Project *p )
{
	EXML *xml;
	char *tag;

	xml = exml_new();

	exml_file_read(xml, p->filename);

	if( !(tag = exml_tag_get(xml)) || strcmp(tag, "project") ) {
		EWLER_ERROR("not a ewler project file\n");
		return -1;
	}

	/* check version and source */

	if( !exml_down(xml) ) {
		EWLER_ERROR("no project information\n");
		return -1;
	}

	do {
		tag = exml_tag_get(xml);

		if( !strcmp(tag, "path") ) {
			p->path = exml_value_get(xml);
			if( p->path )
				p->path = strdup(p->path);
			else
				p->path = strdup("");
		} else if( !strcmp(tag, "name") ) {
			p->name = exml_value_get(xml);
			if( p->name )
				p->name = strdup(p->name);
			else
				p->name = strdup("");
		} else if( !strcmp(tag, "files") ) {
			exml_down(xml);

			do {
				char *name, *file;

				tag = exml_tag_get(xml);

				if( strcmp( tag, "file" ) )
					continue;

				name = exml_attribute_get(xml, "name");
				name = strdup(name);
				file = exml_value_get(xml);
				file = strdup(file);
				ecore_hash_set(p->files, name, file);
			} while(exml_next_nomove(xml));
		}
	} while(exml_next_nomove(xml));

	exml_destroy(xml);

	return 0;
}

static Ewler_Widget_Elem_Type
type_id( char *typename )
{
	if( !typename )
		return EWLER_SPEC_ELEM_INVALID;

	if( !strcmp(typename, "int") || !strcmp(typename, "unsigned int") )
		return EWLER_SPEC_ELEM_INT;

	if( !strcmp(typename, "string") )
		return EWLER_SPEC_ELEM_STRING;

	if( !strcmp(typename, "struct") )
		return EWLER_SPEC_ELEM_STRUCT;

	if( !strcmp(typename, "enum") )
		return EWLER_SPEC_ELEM_ENUM;

	if( !strcmp(typename, "enum_val") )
		return EWLER_SPEC_ELEM_ENUM_VAL;

	if( typename[strlen(typename)-1] == '*' )
		return EWLER_SPEC_ELEM_POINTER;

	return EWLER_SPEC_ELEM_INVALID;
}

static void
set_default( Ewler_Widget_Elem_Spec *elem, char *value )
{
	switch( elem->type ) {
		case EWLER_SPEC_ELEM_INT: elem->info.idefault = atoi(value); break;
		case EWLER_SPEC_ELEM_STRING: elem->info.sdefault = strdup(value); break;
															 /* right now we don't support enum-ed vals in
																* default */
		case EWLER_SPEC_ELEM_ENUM: elem->info.edata.edefault = atoi(value); break;
		default:
			/* err regarding trying to assign default value to invalid type */
			break;
	}
}

int
file_spec_open( char *filename, Ecore_Hash *hash )
{
	EXML *xml;
	Ecore_List *new_specs;
	Ewler_Widget_Spec *spec;
	char *tag, *name;
	void *handle;

	xml = exml_new();

	exml_file_read(xml, filename);

	if( !(tag = exml_tag_get(xml)) || strcmp(tag, "classes") ) {
		EWLER_ERROR("not a ewler spec file\n");
		return -1;
	}

	if( !exml_down(xml) ) {
		EWLER_ERROR("no spec information\n");
		return -1;
	}

	new_specs = ecore_list_new();

	handle = dlopen(NULL, RTLD_LAZY);

	do {
		char *ctor, *nargs, *parent, *visible, *group;

		spec = NEW(Ewler_Widget_Spec);
		memset(spec, 0, sizeof(Ewler_Widget_Spec));

		tag = exml_tag_get(xml); /* class */
		name = exml_attribute_get(xml, "name");
		ctor = exml_attribute_get(xml, "ctor");
		nargs = exml_attribute_get(xml, "nargs");
		parent = exml_attribute_get(xml, "super");
		visible = exml_attribute_get(xml, "visible");
		group = exml_attribute_get(xml, "group");

		spec->name = strdup(name);
		if( ctor ) {
			spec->constructor = strdup(ctor);
			spec->ctor_func = dlsym(handle, spec->constructor);
		}

		if( nargs )
			spec->nargs = strtol(nargs, NULL, 0);
		if( parent )
			spec->parent = strdup(parent);

		if( visible && !strcmp(visible, "no") )
			spec->visible = false;
		else
			spec->visible = true;

		if( group && !strcmp(group, "true") )
			spec->group = true;
		else
			spec->group = false;

		spec->elems = ecore_hash_new(ecore_str_hash, ecore_str_compare);

		if( exml_down(xml) ) {
			bool done;
			Ecore_Hash *elem_hash, *enum_map, *enum_map_rev;
			Ecore_List *hash_stack;

			hash_stack = ecore_list_new();

			elem_hash = spec->elems;
			ecore_list_prepend(hash_stack, elem_hash);

			done = false;

			enum_map = NULL;
			enum_map_rev = NULL;

			do {
				char *type, *get, *set, *id;
				char *index, *ctor_arg, *mod, *default_val, *value;
				char *next;

				Ewler_Widget_Elem_Spec *elem;

				tag = exml_tag_get(xml);

				elem = NEW(Ewler_Widget_Elem_Spec);
				memset(elem, 0, sizeof(Ewler_Widget_Elem_Spec));

				name = exml_value_get(xml);
				type = exml_attribute_get(xml, "type");
				id = exml_attribute_get(xml, "id");
				get = exml_attribute_get(xml, "get");
				set = exml_attribute_get(xml, "set");
				index = exml_attribute_get(xml, "index");
				ctor_arg = exml_attribute_get(xml, "ctor_arg");
				mod = exml_attribute_get(xml, "mod");
				default_val = exml_attribute_get(xml, "default");
				value = exml_attribute_get(xml, "value");

				if( name ) {
					char *s;

					elem->name = strdup(name);
					while( (s = strchr(elem->name, '\n')) ||
								 (s = strchr(elem->name, '\t')) )
						strcpy(s, s+1);
				}

				elem->type = type_id(type);
				if( set ) {
					elem->set = strdup(set);
					elem->set_func = dlsym(handle, set);
				}

				if( get ) {
					elem->get = strdup(get);
					elem->get_func = dlsym(handle, get);
				}

				if( mod && !strcmp(mod, "no") )
					elem->modifiable = false;
				else
					elem->modifiable = true;

				if( index )
					elem->index = atoi(index);
				else
					elem->index = -1;

				if( default_val )
					set_default(elem, default_val);

				if( ctor_arg ) {
					if( !spec->ctor_args )
						spec->ctor_args = ecore_hash_new(ecore_direct_hash,
																						 ecore_direct_compare);
					ecore_hash_set(spec->ctor_args, (void *) atoi(ctor_arg), elem);
				}

				if( elem->type == EWLER_SPEC_ELEM_ENUM_VAL ) {
					char *enum_name;
					int enum_value;

					enum_name = strdup(id);
					enum_value = strtol(value, NULL, 0);

					ecore_hash_set(enum_map, (void *) enum_value, enum_name);
					ecore_hash_set(enum_map_rev, enum_name, (void *) enum_value);

					spec_elem_destroy(elem);
				} else
					ecore_hash_set(elem_hash, elem->name, elem);

				switch(elem->type) {
					case EWLER_SPEC_ELEM_STRUCT:
						elem->info.children =
							ecore_hash_new(ecore_str_hash, ecore_str_compare);

						exml_down(xml);
						ecore_list_prepend(hash_stack, elem_hash);
						elem_hash = elem->info.children;
						break;
					case EWLER_SPEC_ELEM_ENUM:
						elem->info.edata.map = ecore_hash_new(ecore_direct_hash,
																									ecore_direct_compare);
						elem->info.edata.map_rev = ecore_hash_new(ecore_str_hash,
																											ecore_str_compare);

						enum_map = elem->info.edata.map;
						enum_map_rev = elem->info.edata.map_rev;
						ecore_list_prepend(hash_stack, elem_hash);
						elem_hash = NULL;

						exml_down(xml);
						break;
					default:
						while( !(next = exml_next_nomove(xml)) &&
									 elem_hash != spec->elems ) {
							if( elem_hash == spec->elems )
								done = true;

							elem_hash = ecore_list_first_remove(hash_stack);
							exml_up(xml);
						}

						if( !next ) {
							done = true;
							exml_up(xml);
						}

						break;
				}
			} while( !done );

			ecore_list_destroy(hash_stack);
		}

		ecore_hash_set(hash, spec->name, spec);
		ecore_list_append(new_specs, spec);
	} while( exml_next_nomove(xml) );

	ecore_list_first_goto(new_specs);

	/* 2nd pass, set parents */
	while( (spec = ecore_list_next(new_specs)) ) {
		Ewler_Widget_Spec *parent;

		if( spec->parent &&
				(parent = ecore_hash_get(hash, spec->parent)) )
			spec->parent_spec = parent;
	}

	dlclose(handle);

	ecore_list_destroy(new_specs);
	exml_destroy(xml);

	return 0;
}

static Ecore_List *
find_by_parent( Ecore_List *widgets, const char *parent )
{
	Ecore_List *children;
	Ewler_Widget *w;

	children = ecore_list_new();

	ecore_list_first_goto(widgets);
	while( (w = ecore_list_next(widgets)) ) {
		if( !parent && !w->parent )
			ecore_list_append(children, w);

		if( parent && w->parent && !strcmp(w->parent, parent) )
			ecore_list_append(children, w);
	}

	ecore_list_first_goto(children);

	return children;
}

bool
has_changed_children( Ewler_Widget_Elem *elem )
{
	Ecore_List *names, *names_stack;
	Ecore_List *elems_stack;
	Ecore_Hash *elems;
	char *name;

	if( elem->spec->type != EWLER_SPEC_ELEM_STRUCT )
		return false;

	names_stack = ecore_list_new();
	elems_stack = ecore_list_new();

	elems = elem->info.children;
	names = ecore_hash_keys(elems);

	while( (name = ecore_list_next(names)) ) {
		Ewler_Widget_Elem *c_elem;

		c_elem = ecore_hash_get(elems, name);

		if( c_elem->changed ) {
			do {
				ecore_list_destroy(names);
			} while( (names = ecore_list_first_remove(names_stack)) );
			ecore_list_destroy(names_stack);
			ecore_list_destroy(elems_stack);
			return true;
		}

		if( c_elem->spec->type == EWLER_SPEC_ELEM_STRUCT ) {
			ecore_list_prepend(names_stack, names);
			ecore_list_prepend(elems_stack, elems);

			elems = c_elem->info.children;
			names = ecore_hash_keys(elems);
		}

		if( !ecore_list_current(names) &&
				elems != elem->info.children ) {
			ecore_list_destroy(names);

			elems = ecore_list_first_remove(elems_stack);
			names = ecore_list_first_remove(names_stack);
		}
	}

	ecore_list_destroy(names);
	ecore_list_destroy(elems_stack);
	ecore_list_destroy(names_stack);

	return false;
}

int
file_form_save( Ewler_Form *f )
{
	EXML *xml;
	Ecore_List *top_level, *children, *widget_stack;
	Ewler_Widget *w;

	top_level = find_by_parent(f->widgets, NULL);
	if( ecore_list_count(top_level) != 1 ) {
		EWLER_ERROR("ewler form does not have a single top level widget\n");
		return -1;
	}

	widget_stack = ecore_list_new();

	xml = exml_new();

	exml_start(xml);
	exml_tag_set(xml, "form");
	exml_attribute_set(xml, "source", "ewler");
	exml_attribute_set(xml, "version", VERSION);

	children = top_level;

	while( (w = ecore_list_next(children)) ) {
		Ecore_Hash *elems;
		Ecore_List *elems_stack;
		Ecore_List *names, *names_stack;
		char *widget_name;
		char *name;
		int i;

		exml_start(xml);
		exml_tag_set(xml, "widget");
		exml_attribute_set(xml, "type", w->spec->name);

		elems = w->elems;
		names = ecore_hash_keys(elems);

		elems_stack = ecore_list_new();
		names_stack = ecore_list_new();

		while( (name = ecore_list_next(names)) ) {
			Ewler_Widget_Elem *elem;

			elem = ecore_hash_get(elems, name);

			if( elem->spec->type == EWLER_SPEC_ELEM_STRUCT &&
					has_changed_children(elem) ) {
				exml_start(xml);
				exml_tag_set(xml, "attr");
				exml_attribute_set(xml, "name", elem->spec->name);

				ecore_list_prepend(elems_stack, elems);
				ecore_list_prepend(names_stack, names);
				
				elems = elem->info.children;
				names = ecore_hash_keys(elems);
			} else if( elem->spec->type != EWLER_SPEC_ELEM_STRUCT &&
								 elem->changed ) {
				exml_start(xml);
				exml_tag_set(xml, "attr");
				exml_attribute_set(xml, "name", elem->spec->name);
				exml_value_set(xml, elem_to_s(elem));
				exml_end(xml);
			}

			if( !ecore_list_current(names) &&
					elems != w->elems ) {
				ecore_list_destroy(names);

				exml_end(xml);

				elems = ecore_list_first_remove(elems_stack);
				names = ecore_list_first_remove(names_stack);
			}
		}

		ecore_list_destroy(names);
		ecore_list_destroy(names_stack);
		ecore_list_destroy(elems_stack);

		for( i=0;i<EWL_CALLBACK_MAX;i++ ) {
			char *handler;

			if( !w->callbacks[i] )
				continue;

			ecore_list_first_goto(w->callbacks[i]);
			while( (handler = ecore_list_next(w->callbacks[i])) ) {
				exml_start(xml);
				exml_tag_set(xml, "callback");
				exml_attribute_set(xml, "name", callback_names[i]);
				exml_value_set(xml, handler);
				exml_end(xml);
			}
		}

		ecore_list_prepend(widget_stack, children);

		widget_name = (char *) ewl_widget_name_get(w->w);
		children = find_by_parent(f->widgets, widget_name);
		FREE(widget_name);

		while( !ecore_list_current(children) &&
					 children != top_level ) {
			ecore_list_destroy(children);

			exml_end(xml);

			children = ecore_list_first_remove(widget_stack);
		}
	}

	exml_end(xml);

	exml_file_write(xml, f->filename);
	exml_destroy(xml);

	return 0;
}

int
file_form_open( Ewler_Form *f )
{
	EXML *xml;
	Ewl_Widget *w, *top_fg;
	Ewler_Widget *ewler_w;
	Ecore_Hash *elems;
	Ecore_List *elems_stack, *w_stack;
	char *tag, *type;
	bool done;

	xml = exml_new();

	exml_file_read(xml, f->filename);

	if( !(tag = exml_tag_get(xml)) || strcmp(tag, "form") ) {
		EWLER_ERROR("not a form file\n");
		return -1;
	}

	if( !exml_down(xml) ) {
		EWLER_ERROR("no form information\n");
		return -1;
	}

	if( !(tag = exml_tag_get(xml)) ||
			strcmp(tag, "widget") ) {
		EWLER_ERROR("form information incorrect\n");
		return -1;
	}

	type = exml_attribute_get(xml, "type");

	top_fg = widget_new(type);
	f->overlay = ewl_widget_data_get(top_fg, "EWLER_WIDGET");
	ecore_list_append(f->widgets, f->overlay);

	exml_down(xml);

	w = f->overlay->w;
	elems = f->overlay->elems;

	w_stack = ecore_list_new();
	elems_stack = ecore_list_new();

	done = false;

	do {
		char *next;

		tag = exml_tag_get(xml);

		if( !strcmp(tag, "widget") ) {
			Ewl_Widget *parent = w;

			type = exml_attribute_get(xml, "type");

			exml_down(xml);

			ecore_list_prepend(elems_stack, elems);
			ecore_list_prepend(w_stack, parent);
			w = widget_new(type);
			ewler_w = ewl_widget_data_get(w, "EWLER_WIDGET");
			ewl_container_child_append(EWL_CONTAINER(parent), w);
			ecore_list_append(f->widgets, ewler_w);
		} else if( !strcmp(tag, "callback") ) {
			int callback;
			char *handler;
			char *name;

			name = exml_attribute_get(xml, "name");
			handler = exml_value_get(xml);

			if( name ) {
				for( callback=0;callback<EWL_CALLBACK_MAX;callback++ )
					if( !strcmp(name, callback_names[callback]) )
						break;
				
				if( callback < EWL_CALLBACK_MAX && handler ) {
					if( !ewler_w->callbacks[callback] ) {
						ewler_w->callbacks[callback] = ecore_list_new();
						ecore_list_free_cb_set(ewler_w->callbacks[callback], free);
					}

					ecore_list_append(ewler_w->callbacks[callback], strdup(handler));
				}
			}

			goto next;
		} else if( !strcmp(tag, "attr") ) {
			char *name;
			Ewler_Widget_Elem *elem;

			name = exml_attribute_get(xml, "name");

			elem = ecore_hash_get(elems, name);

			if( !elem ) {
				EWLER_ERROR("attribute does not exist: %s\n", name);
			} else {
				char *value;

				value = exml_value_get(xml);

				switch( elem->spec->type ) {
					case EWLER_SPEC_ELEM_INT:
						elem->info.ivalue = strtol(value, NULL, 0);
						if( elem->spec->set_func )
							elem->spec->set_func(w, elem->info.ivalue);
						break;
					case EWLER_SPEC_ELEM_STRING:
						elem->info.svalue = strdup(value);
						if( elem->spec->set_func )
							elem->spec->set_func(w, elem->info.svalue);
						break;
					case EWLER_SPEC_ELEM_ENUM:
						elem->info.evalue =
							(int) ecore_hash_get(elem->spec->info.edata.map_rev, value);
						if( elem->spec->set_func )
							elem->spec->set_func(w, elem->info.evalue);
						break;
					default:
						break;
				}
			}

			if( elem->spec->type == EWLER_SPEC_ELEM_STRUCT ) {
				ecore_list_prepend(elems_stack, elems);
				elems = elem->info.children;

				next = exml_down(xml);
			} else {
next:
				while( !(next = exml_next_nomove(xml)) &&
							 elems != f->overlay->elems ) {
					if( ecore_list_count(elems_stack) == ecore_list_count(w_stack) )
						w = ecore_list_first_remove(w_stack);
					elems = ecore_list_first_remove(elems_stack);
					exml_up(xml);
				}
				if( !next ) {
					done = true;
					exml_up(xml);
				}
			}
		}

	} while( !done );

	ecore_list_destroy(elems_stack);
	ecore_list_destroy(w_stack);
	
	exml_destroy(xml);

	/* 2nd pass, set up parent strings */
	ecore_list_first_goto(f->widgets);
	while( (ewler_w = ecore_list_next(f->widgets)) )
		if( ewler_w == f->overlay )
			ewler_w->parent = NULL;
		else
			ewler_w->parent = (char *) ewl_widget_name_get(ewler_w->fg->parent);

	return 0;
}
