#ifndef __FILE_H__
#define __FILE_H__

time_t              e_file_modified_time(char *file);

char               *e_file_home(void);

int                 e_file_mkdir(char *dir);

int                 e_file_exists(char *file);

int                 e_file_is_dir(char *file);

char *		    e_file_full_name(char *file);

#endif /* __FILE_H__ */
