
#ifndef _MIXER_H_INCLUDED
#define _MIXER_H_INCLUDED
#include <alsa/asoundlib.h>
#include <Evas.h>

#include <Ecore.h>


#ifndef __UNUSED__
#define __UNUSED__ __attribute__((unused))
#endif

typedef struct _Mixer Mixer;
typedef struct _Mixer_Name Mixer_Name;
typedef struct _Mixer_System Mixer_System;
typedef struct _Mixer_System_Name Mixer_System_Name;
typedef struct _Mixer_Elem Mixer_Elem;

typedef void (*mixer_callback) (void * pr);

struct _Mixer
{
	/** Name of this mixer */
	Mixer_Name				*name;
	/** System for this mixer */
	Mixer_System			*system;

	/** Local data - mixer engine definded data */
	void					*local;

	/** Mixer elements */
	Evas_List				*elems;

	/** Signals if update necessary */
	int						is_changed;

	/** Callback and callback data */
	mixer_callback			callback;
	void					*callback_private;	

	/** Reference counter */
	int						ref;

	/** Pointer to function, that initializes mixer */
	Mixer*	(*open)(Mixer* mixer, Mixer_System* ms, Mixer_Name* mixer_name);
	/** Pointer to function, that frees mixer */
	int (*close)(Mixer* mixer, Mixer_System* ms);
	/** Standart operations: get and set volume */
	int (*get_volume)(Mixer_Elem* melem, int *left, int *right);
	int (*set_volume)(Mixer_Elem* melem, int left, int right);

	/** get and set mute */
	int (*get_mute)(Mixer_Elem* melem);
	int (*set_mute)(Mixer_Elem* melem, int mute);

	/*
	int			(*get_elem_id)(Mixer_Elem* melem);
	Mixer_Elem* (*find_elem)(int elem_id);
	*/
};

struct _Mixer_Elem 
{
	/** pointer to the parent */
	Mixer					*mixer;

	/** type of mixer (reserved) */
	char					type;

	/** mixer name (printable) */
	char					*name;

	/** is this mixer active? */
	char					active;
	/** local (engine specific) data */
	void					*local;

	/** Element identifier (hash) */
	int						elem_id;

	/* Previous volume */
	int						pvl, pvr;

};

struct _Mixer_Name
{
	/** card name (engine specific */
	char					*card;
	/** real (printable) name */
	char					*real;
	/** active flag */
	int						active;

	/** ID of mixer, this is typically hash */
	int						mixer_id;

	/** name of mixer system (parent)*/
	Mixer_System_Name		*system_name;
};

struct _Mixer_System
{
	/** name of mixer system (child)*/
	Mixer_System_Name *name;

	/** dl info */
	void *handle;
	int ref;

//	Evas_List* (*get_names)(Mixer_System* ms);
};

struct _Mixer_System_Name
{
	/* This is system name: name of module_%s */
	char *system;
	/* This is name of system (in nice printable form */
	char *name;

	/** system id (hash) */
	int system_id;

	
	/* List of Mixer_Name structures */
	Evas_List* mixer_names;

	/* If it is created */
	Mixer_System *ms;
};


/* Returns Mixer_Name list for that Mixer_System */
//Evas_List*	mixer_get_names(Mixer_System* );

/* Retruns Mixer_System_Name list for all accessible modules */
Evas_List*	mixer_system_get_list();

/* Loads that mixer_system in memory */
int			mixer_system_load(Mixer_System* ms);

/* Retruns Mixer list for that Mixer_System_Name (according to active flag) */
Evas_List*	mixer_system_open (Mixer_System_Name* msn);

/* Closes all mixers for that Mixer_System_Name */
int			mixer_system_close(Mixer_System_Name* msn, Evas_List* mixers);

Mixer*		mixer_open (Mixer_System** ms, Mixer_System_Name* msn, 
						int mixer_id);
int			mixer_close(Mixer* mixer);

int			mixer_ref(Mixer* mixer);
int			mixer_unref(Mixer* mixer);
int			mixer_unref_close(Mixer* mixer);

int			mixer_get_volume(Mixer_Elem *melem, int *left, int *right);
int			mixer_set_volume(Mixer_Elem *melem, int left, int right);
int			mixer_set_callback(Mixer* mixer, mixer_callback callback, 
		void *data);

int			mixer_get_mute(Mixer_Elem *melem);
int			mixer_set_mute(Mixer_Elem *melem, int mute);


#ifdef MODULE
int			mixer_system_get_id(const char *key);
int			mixer_get_id(const char *key);
int			mixer_elem_get_id(const char *key);
#endif

#define GET_ELEM_ID(melem) \
		(melem->elem_id |  \
		 melem->mixer->name->mixer_id | \
		 melem->mixer->system->name->system_id)

#endif // _MIXER_H_INCLUDED 
