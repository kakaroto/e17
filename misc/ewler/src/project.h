#ifndef PROJECT_H
#define PROJECT_H

typedef struct Ewler_Project Ewler_Project;

struct Ewler_Project {
	char *filename; /* filename of the project description */
	char *path; /* project dir */
	Ecore_List *members;
	
	int dirty : 1;
	int filename_set : 1;
};

void project_open( void );
void projects_toggle( void );
void ewler_projects_init( void );
void project_new( void );
void project_add_file( char *filename );
char *project_set_file( char *old, char *filename );
void project_remove_file( char *filename );
char *project_get_path( void );
void project_options_dialog( void );
void project_save( void );

#endif
