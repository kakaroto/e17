#ifndef E_DB_H
#define E_DB_H 1

typedef void        E_DB_File;

E_DB_File          *e_db_open(char *file);
E_DB_File          *e_db_open_read(char *file);
void                e_db_close(E_DB_File * db);
void                e_db_flush(void);
int                 e_db_runtime_flush(void);
void               *e_db_data_get(E_DB_File * db, char *key, int *size_ret);
void                e_db_data_set(E_DB_File * db, char *key, void *data,

				  int size);
void                e_db_data_del(E_DB_File * db, char *key);

void                e_db_int_set(E_DB_File * db, char *key, int val);
int                 e_db_int_get(E_DB_File * db, char *key, int *val);
void                e_db_float_set(E_DB_File * db, char *key, float val);
int                 e_db_float_get(E_DB_File * db, char *key, float *val);
void                e_db_str_set(E_DB_File * db, char *key, char *str);
void                e_db_str_set(E_DB_File * db, char *key, char *str);
char               *e_db_str_get(E_DB_File * db, char *key);
char              **e_db_dump_multi_field(char *file, char *file2,

					  int *num_ret);
char              **e_db_dump_key_list(char *file, int *num_ret);

#define E_DB_INT_SET(db, key, val) \
{ E_DB_File *__db; \
__db = e_db_open(db); \
if (__db) { \
e_db_int_set(__db, key, val); \
e_db_close(__db); \
}}
#define E_DB_STR_SET(db, key, str) \
{ E_DB_File *__db; \
__db = e_db_open(db); \
if (__db) { \
e_db_str_set(__db, key, str); \
e_db_close(__db); \
}}
#define E_DB_FLOAT_SET(db, key, flt) \
{ E_DB_File *__db; \
__db = e_db_open(db); \
if (__db) { \
e_db_float_set(__db, key, flt); \
e_db_close(__db); \
}}
#define E_DB_INT_GET(db, key, val, ok) \
{ E_DB_File *__db; \
__db = e_db_open_read(db); \
if (__db) { \
ok = e_db_int_get(__db, key, &val); \
e_db_close(__db); \
}}
#define E_DB_STR_GET(db, key, str, ok) \
{ E_DB_File *__db; \
__db = e_db_open_read(db); \
if (__db) { \
str = e_db_str_get(__db, key); \
ok = (int)str; \
e_db_close(__db); \
}}
#define E_DB_FLOAT_GET(db, key, flt, ok) \
{ E_DB_File *__db; \
__db = e_db_open_read(db); \
if (__db) { \
ok = e_db_float_get(__db, key, &flt); \
e_db_close(__db); \
}}
#define E_DB_DEL(db, key) \
{ E_DB_File *__db; \
__db = e_db_open(db); \
if (__db) { \
e_db_data_del(__db, key); \
e_db_close(__db); \
}}

#endif
