#ifndef ETAM_H__
# define ETAM_H__

#include <Eina.h>
#include <Eet.h>

#ifdef EAPI
# undef EAPI
#endif

#ifdef _WIN32
# ifdef EFL_EVAS_BUILD
#  ifdef DLL_EXPORT
#   define EAPI __declspec(dllexport)
#  else
#   define EAPI
#  endif /* ! DLL_EXPORT */
# else
#  define EAPI __declspec(dllimport)
# endif /* ! EFL_EVAS_BUILD */
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
#endif /* ! _WIN32 */

#ifdef __cplusplus
extern "C" {
#endif

typedef struct _Etam_DB Etam_DB; /** The data base handler */
typedef struct _Etam_Collection Etam_Collection; /** A collection handler */
typedef struct _Etam_Request Etam_Request; /** Request about collection */
typedef struct _Etam_Process Etam_Process; /** Process of a Request */

typedef enum {
  ETAM_DB_NOOP,
  ETAM_DB_OPEN, /* Triggered once the database is really open */
  ETAM_DB_FLUSH, /* Triggered when flush is done */
  ETAM_DB_CLOSE, /* Triggered when close is really done */
  ETAM_DB_COLLECTION_OPEN,
  ETAM_DB_COLLECTION_CLOSE,
  ETAM_DB_COLLECTION_ADD,
  ETAM_DB_COLLECTION_DEL,
  ETAM_DB_ERROR
} Etam_DB_Op;

typedef enum {
  ETAM_COLLECTION_READY,
  ETAM_COLLECTION_ADD,
  ETAM_COLLECTION_APPEND,
  ETAM_COLLECTION_DEL,
  ETAM_COLLECTION_MODIFY,
  ETAM_COLLECTION_PROCESS_DONE,
  ETAM_COLLECTION_ERROR
} Etam_Collection_Op;

typedef Eina_Bool (*Etam_DB_Validate_Cb)(void *data, Etam_DB *db, Etam_DB_Op op, void *event_info);
typedef Eina_Bool (*Etam_Coll_Validate_Cb)(void *data, Etam_Collection *coll, Etam_Collection_Op op, void *event_info);

EAPI int etam_init(void);
EAPI int etam_shutdown(void);

/* General manipulation of meta database */
EAPI Etam_DB *etam_db_open(const char *uri);
EAPI void etam_db_signal_callback_add(Etam_DB *db,
				      Etam_DB_Op event,
				      Etam_DB_Validate_Cb cb,
				      const void *data);
EAPI void etam_db_flush(Etam_DB *db);
EAPI void etam_db_close(Etam_DB *db);


/* General manipulation of meta data collection */
EAPI Etam_Collection *etam_collection_open(Etam_DB *db, const char *name, ...);
EAPI void etam_collection_callback_add(Etam_Collection *coll,
				       Etam_Collection_Op event,
				       Etam_Coll_Validate_Cb cb,
				       const void *data);
EAPI void etam_collection_close(Etam_Collection *coll);

EAPI Eina_Iterator *etam_collection_keys_iterator(Etam_Collection *coll);

/* Etam data descriptor for direct copy of memory data to the right place */
typedef struct _Etam_Data_Descriptor Etam_Data_Descriptor;

typedef enum {
  ETAM_T_NONE = 0,
  ETAM_T_BOOLEAN = 1,
  ETAM_T_CHAR = 2,
  ETAM_T_UCHAR = 3,
  ETAM_T_SHORT = 4,
  ETAM_T_USHORT = 5,
  ETAM_T_INT = 6,
  ETAM_T_UINT = 7,
  ETAM_T_LONG_LONG = 8,
  ETAM_T_ULONG_LONG = 9,
  ETAM_T_FLOAT = 10,
  ETAM_T_DOUBLE = 11,
  ETAM_T_F32P32 = 12,
  ETAM_T_F16P16 = 13,
  ETAM_T_F8P24 = 14,
  ETAM_T_STRING = 15,
  ETAM_T_NULL = 16,
  ETAM_T_RECTANGLE = 17,

  ETAM_G_STRINGS = 101,
  ETAM_G_RECTANGLES = 102,
  ETAM_G_TAGGED_RECTANGLES = 103
} Etam_Data_Type;

EAPI Etam_Data_Descriptor *etam_data_descriptor_new(const char *name, int size);
EAPI void etam_data_descriptor_element_add(Etam_Data_Descriptor *edd,
                                           const char *name,
                                           Etam_Data_Type type,
                                           int offset,
                                           int count_offset);
EAPI void etam_data_descriptor_free(Etam_Data_Descriptor *edd);

typedef void (*Etam_Entry_Value_Cb)(Etam_Collection *coll, const char *key, Eina_Value *value, void *user_data);
typedef void (*Etam_Entry_Data_Cb)(Etam_Collection *coll, const char *key, void *data, void *user_data);
typedef void (*Etam_Entry_Error_Cb)(Etam_Collection *coll, Etam_Request *rq);
typedef void (*Etam_Entry_Cb)(Etam_Collection *coll, const char *key, void *user_data);
typedef enum {
  ETAM_REQUEST_NOOP = 0,
  ETAM_REQUEST_LIKE = 1,
  ETAM_REQUEST_EQUAL = 2,
  ETAM_REQUEST_INF = 3,
  ETAM_REQUEST_SUP = 4,

  ETAM_REQUEST_NOT = 128
} Etam_Request_Op;

#define ETAM_TYPECHECK(Type, X)                 \
  ({                                            \
     Type __x;                                  \
     __x = X;                                   \
     (void) __x;                                \
     (Type) X;                                  \
  })

#define ETAM_REQUEST_STRING_LIKE(Name, Value, Flags) ETAM_REQUEST_LIKE, ETAM_T_STRING, ETAM_TYPECHECK(const char *, Name), ETAM_TYPECHECK(const char *, Value), ETAM_TYPECHECK(int, Flags)
#define ETAM_REQUEST_STRING_EQUAL(Name, Value) ETAM_REQUEST_EQUAL, ETAM_T_STRING, ETAM_TYPECHECK(const char *, Name), ETAM_TYPECHECK(const char *, Value)
#define ETAM_REQUEST_BOOLEAN_EQUAL(Name, Value) ETAM_REQUEST_EQUAL, ETAM_T_BOOLEAN, ETAM_TYPECHECK(const char *, Name), ETAM_TYPECHECK(Eina_Bool, Value)
#define ETAM_REQUEST_LONG_LONG(Type, Name, Value) Type, ETAM_T_LONG_LONG, ETAM_TYPECHECK(const char *, Name), ETAM_TYPECHECK(long long, Value)

EAPI Etam_Request *etam_collection_request_new(Etam_Collection *coll, ...);
EAPI void etam_collection_request_free(Etam_Request *request);

EAPI Etam_Process *etam_request_entry_value_get(Etam_Collection *collection, const Eina_Value_Struct_Desc *desc,
						Etam_Entry_Value_Cb value_cb, Etam_Entry_Error_Cb error_cb, void *user_data,
						const char *key);
EAPI Etam_Process *etam_request_entry_data_get(Etam_Collection *collection, const Etam_Data_Descriptor *desc,
					       Etam_Entry_Data_Cb value_cb, Etam_Entry_Error_Cb error_cb, void *user_data,
					       const char *key);

EAPI Etam_Process *etam_request_entries_value_get(Etam_Request *request, const Eina_Value_Struct_Desc *desc,
						  Etam_Entry_Value_Cb value_cb, Etam_Entry_Error_Cb error_cb, void *user_data);
EAPI Etam_Process *etam_request_entries_data_get(Etam_Request *request, const Etam_Data_Descriptor *desc,
						 Etam_Entry_Value_Cb value_cb, Etam_Entry_Error_Cb error_cb, void *user_data);

EAPI Etam_Process *etam_request_entries_get(Etam_Request *request,
					    Etam_Entry_Cb key_cb, Etam_Entry_Error_Cb error_cb, void *user_data);

EAPI void etam_process_cancel(Etam_Process *process);
EAPI void etam_request_del(Etam_Request *request);

#define ETAM_TYPED_STRING(Name, Value) ETAM_T_STRING, ETAM_TYPECHECK(const char *, Name), ETAM_TYPECHECK(const char *, Value)
#define ETAM_TYPED_LONG_LONG(Name, Value) ETAM_T_LONG_LONG, ETAM_TYPECHECK(const char *, Name), ETAM_TYPECHECK(long long, Value)
#define ETAM_TYPED_BOOLEAN(Name, Value) ETAM_T_BOOLEAN, ETAM_TYPECHECK(const char *, Name), ETAM_TYPECHECK(Eina_Bool, Value)

EAPI void etam_collection_entry_value_set(Etam_Collection *coll, const char *key, Eina_Value *value);
EAPI void etam_collection_entry_data_set(Etam_Collection *coll, const char *key, const Etam_Data_Descriptor *desc, void *data);
EAPI void etam_collection_entry_typed_set(Etam_Collection *coll, const char *key, ...);
EAPI void etam_collection_entry_del(Etam_Collection *coll, const char *key);

#endif
