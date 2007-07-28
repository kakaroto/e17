#ifndef _EX_COMMAND_H
#define _EX_COMMAND_H

#define FMT_TRACK_NUM    	'n'
#define FMT_TRACK_TITLE  	't'
#define FMT_TRACK_ARTIST 	'a'
#define FMT_DISC_ARTIST  	'A'
#define FMT_DISC_TITLE   	'T'
#define FMT_DISC_YEAR    	'y'
#define FMT_DISC_GENRE_NUM   	'g'
#define FMT_DISC_GENRE_STRING   'G'
#define FMT_FILE_EXT     	'e'
#define FMT_OUTPUT_FILE  	'o'
#define FMT_INPUT_WAV    	'i'
#define FMT_DISC_DEVICE		'd'

char  **ex_command_translate(Extrackt *ex, Ex_Config_Exe_Type ext);

void    ex_command_rip(Extrackt *ex);
int     ex_command_rip_update(Extrackt *ex);
void    ex_command_rip_append(Extrackt *ex, int numtrack);
int	ex_command_rip_set(Extrackt *ex, char *name);
void    ex_command_rip_abort(Extrackt *ex);

void    ex_command_encode(Extrackt *ex);
int     ex_command_encode_update(Extrackt *ex);
int	ex_command_encode_set(Extrackt *ex, char *name);
void    ex_command_encode_abort(Extrackt *ex);
void    ex_command_encode_append(Extrackt *ex, int tracknumber);

void    ex_string_file_delete(char *file);
char   *ex_string_file_extension_get(char *file);
int     ex_string_file_exists(char *path);

#endif
