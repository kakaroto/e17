#ifndef E_DB_H
#define E_DB_H 1

/* the basic db handle */
typedef void        E_DB_File;

/* core functions.............................. */
/* open a db for read/write - returns NULL if not possible */
E_DB_File          *e_db_open(char *file);
/* open a db for read only - retunr NULL if not possible */ 
E_DB_File          *e_db_open_read(char *file);
/* close a db handle - does not always close - leaves pending till a flush */
void                e_db_close(E_DB_File * db);
/* get data from the db indexed with the key and return size in bytes */
void               *e_db_data_get(E_DB_File * db, char *key, int *size_ret);
/* set the data of the stated size in the db with the stated key */
void                e_db_data_set(E_DB_File * db, char *key, void *data, int size);
/* delete the data and key with in the db */
void                e_db_data_del(E_DB_File * db, char *key);
/* tells you how many db's this process has open */
int                 e_db_usage(void);
/* flush all pending closes to db's and ensure a write to disk */
void                e_db_flush(void);
/* flush db's ONLY if db's have not been accessed in the last 0.5 seconds */
int                 e_db_runtime_flush(void);

/* higher level functions.............................. */
/* put an integer into the db (handle endianess flip too */
void                e_db_int_set(E_DB_File * db, char *key, int val);
/* get the integer value form the bd - handle endianess too */
int                 e_db_int_get(E_DB_File * db, char *key, int *val);
/* set a float in the db - handle endianess */
void                e_db_float_set(E_DB_File * db, char *key, float val);
/* get the float from the db - handle endianess */
int                 e_db_float_get(E_DB_File * db, char *key, float *val);
/* set a string in the db */
void                e_db_str_set(E_DB_File * db, char *key, char *str);
/* get a string from the db */
char               *e_db_str_get(E_DB_File * db, char *key);
/* dump the db as if it were a list of keys and strings as a list of strings */
/* so the first string si a key, the seocnd is its string value */
/* the next string is a key.. then value etc... so for a db like: */
/* KEY: blah      STRING VALUE: hello */
/* KEY: nym       STRING VALUE: goodbye */
/* KEY: flim      STRINg VALUE: boo */
/* your string list would be: */
/* "blah", "hello", "nym", "goodbye", "flim", "boo:" */
char              **e_db_dump_multi_field(char *file, char *file2, int *num_ret);
/* retunr a list of strings of the kesy in the db */
char              **e_db_dump_key_list(char *file, int *num_ret);

/* conveience macros to make setting and getting values form a db easy */ 
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
#define E_DB_FLUSH e_db_runtime_flush();

#endif
