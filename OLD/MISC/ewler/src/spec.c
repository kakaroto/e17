/**
 * Widget manager for ewler
 */

#include <Ewl.h>

#include "config.h"
#include "ewler.h"

Ecore_Hash *widget_specs;

void
spec_elem_destroy( void *p )
{
	Ewler_Widget_Elem_Spec *elem = p;

	IF_FREE(elem->name);
	IF_FREE(elem->set);
	IF_FREE(elem->get);

	switch( elem->type ) {
		case EWLER_SPEC_ELEM_STRING: IF_FREE(elem->info.sdefault); break;
		case EWLER_SPEC_ELEM_STRUCT:
			ecore_hash_free_value_cb_set(elem->info.children, spec_elem_destroy);
			ecore_hash_destroy(elem->info.children);
			break;
		case EWLER_SPEC_ELEM_ENUM:
			ecore_hash_destroy(elem->info.edata.map);
			ecore_hash_free_key_cb_set(elem->info.edata.map_rev, free);
			ecore_hash_destroy(elem->info.edata.map_rev);
			break;
		default:
			/* no need to worry */
			break;
	}

	FREE(elem);
}

static void
spec_destroy( void *p )
{
	Ewler_Widget_Spec *spec = p;

	if( !spec )
		return;

	IF_FREE(spec->name);
	IF_FREE(spec->constructor);
	ecore_hash_destroy(spec->ctor_args);

	IF_FREE(spec->parent);
	ecore_hash_free_value_cb_set(spec->elems, spec_elem_destroy);
	ecore_hash_destroy(spec->elems);

	memset(spec, 0, sizeof(Ewler_Widget_Spec));
	FREE(spec);
}

int
specs_init( void )
{
	widget_specs = ecore_hash_new(ecore_str_hash, ecore_str_compare);
	ecore_hash_free_value_cb_set(widget_specs, spec_destroy);

	if( file_spec_open( INITIAL_EWLER_SPEC, widget_specs ) < 0 ) {
		ewler_error("error reading base spec file");
		return -1;
	}

	return 0;
}

Ewler_Widget_Elem_Spec *
spec_elem_get( char *widget, char *spec )
{
	Ewler_Widget_Spec *w;

	w = ecore_hash_get(widget_specs, widget);
	if( !w )
		return NULL;

	return ecore_hash_get(w->elems, spec);
}

static void
elem_destroy( void *p )
{
	Ewler_Widget_Elem *elem = p;

	if( !elem )
		return;

	if( elem->items )
		ecore_hash_destroy(elem->items);

	if( elem->spec->type == EWLER_SPEC_ELEM_STRUCT )
		ecore_hash_destroy(elem->info.children);

	FREE(elem);
}

Ewler_Widget *
spec_new( char *name )
{
	Ewler_Widget *w;
	Ewler_Widget_Spec *spec;

	w = NEW(Ewler_Widget);
	if( !w ) {
		ewler_error("spec_new: cannot create widget, out of memory!");
		return NULL;
	}

	memset(w, 0, sizeof(Ewler_Widget));

	spec = ecore_hash_get(widget_specs, name);
	if( !spec ) {
		FREE(w);
		ewler_error("cannot make widget from unknown spec");
		return NULL;
	}

	w->spec = spec;
	w->elems = ecore_hash_new(ecore_str_hash, ecore_str_compare);
	ecore_hash_free_value_cb_set(w->elems, elem_destroy);

	while( spec ) {
		Ecore_List *names, *elem_names, *name_stack;
		Ecore_Hash *spec_elems;
		Ecore_List *spec_stack;
		Ecore_Hash *hash;
		Ecore_List *elem_stack;
		Ewler_Widget_Elem *elem;
		char *name;

		elem_names = ecore_hash_keys(spec->elems);

		elem_stack = ecore_list_new();
		spec_stack = ecore_list_new();
		name_stack = ecore_list_new();

		hash = w->elems;
		names = elem_names;
		spec_elems = spec->elems;

		while( (name = ecore_list_next(names)) ) {
			elem = NEW(Ewler_Widget_Elem);
			if( !elem ) {
				ewler_error("spec_new: cannot create widget, out of memory!");
				return NULL;
			}

			elem->w = w;
			elem->spec = ecore_hash_get(spec_elems, name);

			switch( elem->spec->type ) {
				case EWLER_SPEC_ELEM_INT:
					elem->info.ivalue = elem->spec->info.idefault;
					break;
				case EWLER_SPEC_ELEM_STRING:
					if( elem->spec->info.sdefault )
						elem->info.svalue = strdup(elem->spec->info.sdefault);
					else
						elem->info.svalue = NULL;
					break;
				case EWLER_SPEC_ELEM_STRUCT:
					elem->info.children = ecore_hash_new(ecore_str_hash,
																							 ecore_str_compare);
					ecore_hash_free_value_cb_set(elem->info.children, elem_destroy);
					ecore_list_prepend(elem_stack, hash);
					ecore_list_prepend(spec_stack, spec_elems);
					ecore_list_prepend(name_stack, names);
					break;
				case EWLER_SPEC_ELEM_ENUM:
					elem->info.evalue = elem->spec->info.edata.edefault;
					break;
				default:
					break;
			}

			ecore_hash_set(hash, name, elem);

			if( elem->spec->type == EWLER_SPEC_ELEM_STRUCT ) {
				hash = elem->info.children;
				spec_elems = elem->spec->info.children;
				names = ecore_hash_keys(spec_elems);
			}

			if( !ecore_list_current(names) && names != elem_names ) {
				ecore_list_destroy(names);

				hash = ecore_list_first_remove(elem_stack);
				spec_elems = ecore_list_first_remove(spec_stack);
				names = ecore_list_first_remove(name_stack);
			}
		}

		ecore_list_destroy(elem_names);

		spec = spec->parent_spec;
	}

	if( w->spec->ctor_func ) {
		Ewler_Widget_Elem_Spec *elem_spec;
		void *args[8];
		int i;

		for( i=0;i<w->spec->nargs && i<8;i++ )
			if( !w->spec->ctor_args ||
					!(elem_spec = ecore_hash_get(w->spec->ctor_args, (void *) i)) )
				args[i] = w->spec->name;
			else {
				switch( elem_spec->type ) {
					case EWLER_SPEC_ELEM_INT:
						args[i] = (void *) elem_spec->info.idefault;
						break;
					case EWLER_SPEC_ELEM_STRING:
						args[i] = (void *) elem_spec->info.sdefault;
						break;
					case EWLER_SPEC_ELEM_ENUM:
						args[i] = (void *) elem_spec->info.edata.edefault;
						break;
					default:
						args[i] = w->spec->name;
						break;
				}
			}

		switch( w->spec->nargs ) {
			case 0: w->w = w->spec->ctor_func(); break;
			case 1: w->w = w->spec->ctor_func(args[0]); break;
			case 2: w->w = w->spec->ctor_func(args[0], args[1]); break;
			case 3: w->w = w->spec->ctor_func(args[0], args[1], args[2]); break;
			case 4:
				w->w = w->spec->ctor_func(args[0], args[1], args[2], args[3]);
				break;
			case 5:
				w->w = w->spec->ctor_func(args[0], args[1], args[2], args[3],
																	args[4]);
				break;
			case 6:
				w->w = w->spec->ctor_func(args[0], args[1], args[2], args[3],
																	args[4], args[5]);
				break;
			case 7:
				w->w = w->spec->ctor_func(args[0], args[1], args[2], args[3],
																	args[4], args[5], args[6]);
				break;
			case 8:
			default:
				w->w = w->spec->ctor_func(args[0], args[1], args[2], args[3],
																	args[4], args[5], args[6], args[7]);
				break;
		}
	}

	if( w->w ) {
		int i = 0;
		bool name_ok = false;
		char name[64];

		w->w->data = ecore_hash_new(ecore_str_hash, ecore_str_compare);
		ewl_widget_data_set(w->w, "EWLER_WIDGET", w);

		i = 0;

		while( !name_ok ) {
			name_ok = true;
			sprintf(name, "%s", w->spec->name);
			if( i > 0 )
				sprintf(name + strlen(name), "%d", i);
			if( ewl_widget_name_find(name) )
				name_ok = false;
			i++;
		}

		ewl_widget_name_set(w->w, name);
	}

	return w;
}
