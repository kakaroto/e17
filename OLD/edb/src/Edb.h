#ifndef EDB_H
#define EDB_H 1

#ifdef EAPI
#undef EAPI
#endif
#ifdef WIN32
# ifdef BUILDING_DLL
#  define EAPI __declspec(dllexport)
# else
#  define EAPI __declspec(dllimport)
# endif
#else
# ifdef __GNUC__
#  if __GNUC__ >= 4
#   define EAPI __attribute__ ((visibility("default")))
#  else
#   define EAPI
#  endif
# else
#  define EAPI
# endif
#endif

#ifdef __cplusplus
extern "C" {
#endif  


/* The basic db handle */
typedef void        E_DB_File;

/* Edb Core Functions .............................. */

/**
 * e_db_open - Opens a db for read/write access
 * @file: name of the db file.
 *
 * This function opens an edb database file for read
 * and write access. It returns NULL if not possible,
 * a pointer to an E_DB_File otherwise.
 */
EAPI E_DB_File          *e_db_open(const char *file);

/**
 * e_db_open_mode - Opens a db with a user-specified mode
 * @file: name of the db file.
 * @flags: bitmask in the format specified by open(2)
 *
 * This function opens an edb database file for access as specified by
 * flags. It returns NULL if not possible, a pointer to an E_DB_File
 * otherwise.
 */
EAPI E_DB_File          *e_db_open_mode(const char *file, int flags);

/**
 * e_db_property_set - Sets the value of a Property Field.
 * @edb: A database handle.
 * @property: The name of the Property Field to edit.
 * @value: The value to assign the the field.
 *
 * The property fields are special database fields that cannot be accessed
 * by the normal get/set methods. They are used to store meta-data about the
 * database (e.g. Type and Version Information). This function will set the
 * value of a specified property field.
 */
EAPI void                e_db_property_set(E_DB_File *edb, const char *property, const char *value);


/**
 * e_db_property_get - Gets the value of a Property Field.
 * @edb: A database handle.
 * @property: The name of the Property Field to edit.
 *
 * This function will retrieve the data stored in the specified property
 * field.
 */
EAPI char                *e_db_property_get(E_DB_File *edb, const char *property);

/**
 * e_db_set_type - Convenience function for setting the value
 * of the Type Property Field.
 * @edb: A database handle.
 * @value: The value to append to the current Type Property.
 *
 * This function sets the value of the Type Property for the current database.
 * If there is already a value in the type field this value will be appended
 * to the end. Each type is seperated by a '/' character with the entire
 * string having leading and trailing seperating characters. The function adds
 * the seperation characters itself.
 */
EAPI void                e_db_set_type(E_DB_File * edb, const char *value);

/**
 * e_db_is_type - Convenience function for determining if a database is
 * of a certain type.
 * @edb: A database handle.
 * @type: The type string to check for.
 *
 * This function will compare the specified type string against the type
 * data stored in the database. If a match is found (case-insensitive) then 1
 * is returned, else 0.
 */
EAPI int                 e_db_is_type(E_DB_File * edb, const char *type);

/**
 * e_db_open_read - Opens a database for read access only
 * @file: name of the database file
 *
 * This function opens an edb database file for read access.
 * It returns NULL if that's not possible, a pointer to an
 * E_DB_File otherwise.
 */ 
EAPI E_DB_File          *e_db_open_read(const char *file);


/** 
 * e_db_close - Closes a db handle
 * @db: Edb database handle
 *
 * This function closes a database. It does not guarantee a
 * write to disk, it just decrements the use count on the
 * database. When a database has been closed by all users,
 * calling e_db_flush() writes the database to disk.
 */
EAPI void                e_db_close(E_DB_File * db);

/**
 * e_db_data_get - Retrieves generic data from the db for a given key
 * @db: Edb database handle
 * @key: key for the data item
 * @size_ret: result parameter that returns the size of the data item
 *
 * This function is the generic way to retrieve data from
 * an Edb database. Use the @size_ret value to find out the
 * size of the retrieved data. Returns the retrieved data,
 * which you need to free when you don't need it any longer.
 */
EAPI void               *e_db_data_get(E_DB_File * db, const char *key, int *size_ret);

/**
 * e_db_data_set - Writes generic data to a database
 * @db: Edb database handle
 * @key: Key through which the data can be retrieved
 * @data: the data item itself
 * @size: size of the data item @data
 *
 * This function is the generic way to write items of data to a
 * database. 
 */
EAPI void                e_db_data_set(E_DB_File * db, const char *key, const void *data, int size);

/**
 * e_db_data_del - Removes a key-data pair in a database
 * @db: Edb database handle
 * @key: Key that determines the data item that is removed
 *
 * This function removed the key and data in a database that
 * the given key points to.
 */
EAPI void                e_db_data_del(E_DB_File * db, const char *key);

/**
 * e_db_usage - returns the number of database files a process uses
 *
 * This function returns the number of database files the
 * current process is using at the moment.
 */
EAPI int                 e_db_usage(void);

/**
 * e_db_flush - flushes the contents of unused databases to disk
 * 
 * This function checks for which databases the use count has
 * dropped to zero and writes their contents out to disk.
 */
EAPI void                e_db_flush(void);

/**
 * e_db_runtime_flush - time-sensitive version of @efsd_db_flush
 *
 * This function is like @efsd_db_flush, but flushes databases
 * only if they have not been accessed in the last 0.5 seconds.
 * It returns 1 if the databases have been flushed, 0 otherwise.
*/
EAPI int                 e_db_runtime_flush(void);

/* High-level convenience functions ................... */


/**
 * e_db_int_set - convenience function for writing integers to a database
 * @db: Edb database handle
 * @key: key through which the data can be retrieved
 * @val: the integer value that is to be written to the database
 *
 * This function is a convenience wrapper around e_db_data_set() to
 * set integer values in a database. It also handles endianness
 * byteorder flips.
 */
EAPI void                e_db_int_set(E_DB_File * db, const char *key, int val);

/**
 * e_db_int_get - convenience function for retrieving integers from a database
 * @db: Edb database handle
 * @key: key for the integer value
 * @val: result value that points to an integer where the result is stored
 *
 * This function retrieves an integer for the given key from a database,
 * handling endianness flips. It returns 1 if the lookup succeeded, 0 otherwise.
 */
EAPI int                 e_db_int_get(E_DB_File * db, const char *key, int *val);

/**
 * e_db_float_set - convenience function for writing floats to a database
 * @db: Edb database handle
 * @key: key through which the data can be retrieved
 * @val: the float value that is to be written to the database
 *
 * This function is a convenience wrapper around e_db_data_set() to
 * set float values in a database. It also handles endianness
 * byteorder flips.
 */
EAPI void                e_db_float_set(E_DB_File * db, const char *key, float val);

/**
 * e_db_float_get - convenience function for retrieving floats from a database
 * @db: Edb database handle
 * @key: key for the integer value
 * @val: result value that points to a float where the result is stored
 *
 * This function retrieves a float for the given key from a database,
 * handling endianness flips. It returns 1 if the lookup succeeded, 0 otherwise.
 */
EAPI int                 e_db_float_get(E_DB_File * db, const char *key, float *val);

/**
 * e_db_str_set - convenience function for writing strings to a database
 * @db: Edb database handle
 * @key: key through which the data can be retrieved
 * @str: the string that is to be written to the database
 *
 * This function is a convenience wrapper around e_db_data_set() to
 * set character strings in a database.
 */
EAPI void                e_db_str_set(E_DB_File * db, const char *key, const char *str);

/**
 * e_db_str_get - convenience function for retrieving strings from a database
 * @db: Edb database handle
 * @key: key for the integer value
 *
 * This function retrieves a string for the given key from a database
 * and returns it. When an error occurs, NULL is returned. You must free
 * the string when you're finished using free().
 */
EAPI char               *e_db_str_get(E_DB_File * db, const char *key);

/**
 * e_db_type_set - sets the encoding of a database item
 * @db: Edb database handle
 * @key: key for the data item whose type is to be set
 * @type: string containing the type of the data item
 *
 * This function sets the type of a database entry. The types currently
 * used are "int" for integers, "str" for strings and "float" for
 * floats. All other types are application-specific. You do not need to
 * set the types manually when using e_db_int_set(), e_db_float_set()
 * and e_db_str_set().
 */
EAPI void                e_db_type_set(E_DB_File * db, const char *key, const char *type);
  
/**
 * e_db_type_get - returns type of a data item
 * @db: Edb database handle
 * @key: key for the data item whose type is retrieved
 *
 * This function returns the type of a data item in a database. If
 * the type is unknown, NULL is returned. Otherwise, you need to 
 * free the result when you're finished, using free().
 */
EAPI char               *e_db_type_get(E_DB_File * db, const char *key);

/* Dump the db as if it were a list of keys and strings as a list of strings */
/* so the first string si a key, the seocnd is its string value */
/* the next string is a key.. then value etc... so for a db like: */
/* KEY: blah      STRING VALUE: hello */
/* KEY: nym       STRING VALUE: goodbye */
/* KEY: flim      STRINg VALUE: boo */
/* Your string list would be: */
/* "blah", "hello", "nym", "goodbye", "flim", "boo:" */
EAPI char              **e_db_dump_multi_field(const char *file, const char *file2, int *num_ret);

/**
 * e_db_dump_key_list - returns list of keys in a database
 * @file: name of an Edb database file
 * @num_ret: result pointer that returns the number of keys in the database
 *
 * This function returns a list of strings of the keys in the database.
 * You must free the list and the strings in the list when done.
 */
EAPI char              **e_db_dump_key_list(const char *file, int *num_ret);

/**
 * e_db_match_keys - returns list of keys matching a pattern
 * @db: Edb database handle
 * @pattern: key pattern to match
 * @num_ret: result pointer that returns the number of keys in the database
 *
 * This function returns a list of strings of the keys in the database
 * that match the given pattern. The pattern is a shell wildcard pattern.
 * You must free the list and the strings in the list when done.
 */
EAPI char              **e_db_match_keys(E_DB_File *db, const char *pattern, int *num_ret);

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
ok = str != NULL; \
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


#ifdef __cplusplus
}
#endif                                                  /* __cplusplus */

#endif
