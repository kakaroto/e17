#ifndef E_DB_H
#define E_DB_H 1

/* The basic db handle */
typedef void        E_DB_File;

/* Edb Core Functions .............................. */

/* Open a db for read/write - returns NULL if not possible */
E_DB_File          *e_db_open(char *file);

/* Open a db for read only - retunr NULL if not possible */ 
E_DB_File          *e_db_open_read(char *file);

/* Close a db handle - does not always close - leaves pending till a flush */
void                e_db_close(E_DB_File * db);

/* Get data from the db indexed with the key and return
   size in bytes. You need to free the returned data.
*/
void               *e_db_data_get(E_DB_File * db, char *key, int *size_ret);

/* Set the data of the stated size in the db with the stated key */
void                e_db_data_set(E_DB_File * db, char *key, void *data, int size);

/* Delete the data and key with in the db */
void                e_db_data_del(E_DB_File * db, char *key);

/* Tells you how many db's this process has open */
int                 e_db_usage(void);

/* Flush all pending closes to db's and ensure a write to disk */
void                e_db_flush(void);

/* Flush db's ONLY if db's have not been accessed in the last 0.5 seconds */
int                 e_db_runtime_flush(void);

/* High-level convenience functions ................... */

/* Put an integer into the db (handle endianess flip too) */
void                e_db_int_set(E_DB_File * db, char *key, int val);

/* Get the integer value form the bd - handle endianess too - return 1 if ok */
int                 e_db_int_get(E_DB_File * db, char *key, int *val);

/* Set a float in the db - handle endianess */
void                e_db_float_set(E_DB_File * db, char *key, float val);

/* Get the float from the db - handle endianess - return 1 if ok, 0 if not */
int                 e_db_float_get(E_DB_File * db, char *key, float *val);

/* Set a string in the db */
void                e_db_str_set(E_DB_File * db, char *key, char *str);

/* Get a string from the db - you must free it when done using free() */
char               *e_db_str_get(E_DB_File * db, char *key);

/* Set the encoding type of an entry - types defined are: */
/* "int"    - integer        */
/* "str"    - string         */
/* "float"  - floating point */
/* All other types are application specific */
/* You do not need to set the type if you use e_db_int_set(), */
/* e_db_float_set() and e_db_str_set() routines. edb does this for you. */
void                e_db_type_set(E_DB_File * db, char *key, char *type);

/* Get the ype of an entry - if type is unknown NULL is returned. */
/* You must free the string returned when you are done using free() */
char               *e_db_type_get(E_DB_File * db, char *key);

/* Dump the db as if it were a list of keys and strings as a list of strings */
/* so the first string si a key, the seocnd is its string value */
/* the next string is a key.. then value etc... so for a db like: */
/* KEY: blah      STRING VALUE: hello */
/* KEY: nym       STRING VALUE: goodbye */
/* KEY: flim      STRINg VALUE: boo */
/* Your string list would be: */
/* "blah", "hello", "nym", "goodbye", "flim", "boo:" */
char              **e_db_dump_multi_field(char *file, char *file2, int *num_ret);

/* Return a list of strings of the keys in the db. you must free the list */
/* and the strings in the list when done */
char              **e_db_dump_key_list(char *file, int *num_ret);

/* Convenience macros to make setting and getting values form a db easy */ 
#define E_DB_INT_SET(edb, key, val) \
{ E_DB_File *__edb; \
__edb = e_db_open(edb); \
if (__edb) { \
e_db_int_set(__edb, key, val); \
e_db_close(__edb); \
}}
#define E_DB_STR_SET(edb, key, str) \
{ E_DB_File *__edb; \
__edb = e_db_open(edb); \
if (__edb) { \
e_db_str_set(__edb, key, str); \
e_db_close(__edb); \
}}
#define E_DB_FLOAT_SET(edb, key, flt) \
{ E_DB_File *__edb; \
__edb = e_db_open(edb); \
if (__edb) { \
e_db_float_set(__edb, key, flt); \
e_db_close(__edb); \
}}
#define E_DB_INT_GET(edb, key, val, ok) \
{ E_DB_File *__edb; \
__edb = e_db_open_read(edb); \
if (__edb) { \
ok = e_db_int_get(__edb, key, &val); \
e_db_close(__edb); \
}}
#define E_DB_STR_GET(edb, key, str, ok) \
{ E_DB_File *__edb; \
__edb = e_db_open_read(edb); \
if (__edb) { \
str = e_db_str_get(__edb, key); \
ok = (int)str; \
e_db_close(__edb); \
}}
#define E_DB_FLOAT_GET(edb, key, flt, ok) \
{ E_DB_File *__edb; \
__edb = e_db_open_read(edb); \
if (__edb) { \
ok = e_db_float_get(__edb, key, &flt); \
e_db_close(__edb); \
}}
#define E_DB_DEL(edb, key) \
{ E_DB_File *__edb; \
__edb = e_db_open(edb); \
if (__edb) { \
e_db_data_del(__edb, key); \
e_db_close(__edb); \
}}
#define E_DB_FLUSH e_db_runtime_flush();

#endif
