/** @file etk_signal.h */
#ifndef _ETK_SIGNAL_H_
#define _ETK_SIGNAL_H_

#include <stdarg.h>

#include <Evas.h>

#include "etk_marshallers.h"
#include "etk_types.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @defgroup Etk_Signal Etk_Signal
 * @brief The signal system is a flexible system to create, connect and emit
 * signals
 * @{
 */

typedef struct Etk_Signal_Connect_Desc {
   const Etk_Signal_Callback *scb;
   int signal_code;
   Etk_Callback callback;
} Etk_Signal_Connect_Desc;

#define ETK_SC_DESC_SENTINEL {NULL, -1, NULL}
#define ETK_SC_DESC(code, callback) \
   { NULL, (code), ETK_CALLBACK(callback) }

/** Gets the offset of a member of a struct (used when you create a new signal
 * with a default handler, with etk_signal_new()) */
#define ETK_MEMBER_OFFSET(type, member)   ((long)((long *)&((type *)0)->member))

typedef struct Etk_Signal_Description
{
   int *signal_code_store;
   char *name;
   long handler_offset;
   Etk_Marshaller marshaller;
} Etk_Signal_Description;

#define ETK_SIGNAL_DESCRIPTION_SENTINEL {NULL, NULL, 0, NULL}

#define ETK_SIGNAL_DESC_NO_HANDLER(signal_code, name, marshaller) \
        {&(signal_code), (name), -1, \
           (marshaller)}

#define ETK_SIGNAL_DESC_HANDLER(signal_code, name, type, handler, marshaller) \
        {&(signal_code), (name), ETK_MEMBER_OFFSET(type, handler), \
           (marshaller)}

/**
 * @brief The structure of a signal
 * @structinfo
 */
struct Etk_Signal
{
   /* private: */
   char *name;
   int code;
   long handler_offset;
   Etk_Marshaller marshaller;
};

void etk_signal_shutdown(void);

int         etk_signal_new(const char *signal_name, Etk_Type *type,
                           long handler_offset, Etk_Marshaller marshaller);
void etk_signal_new_with_desc(Etk_Type *type,
                              const Etk_Signal_Description *desc);
void        etk_signal_delete(Etk_Signal *signal);

Etk_Signal    *etk_signal_lookup(const char *signal_name, Etk_Type *type);
int            etk_signal_lookup_code(const char *signal_name, Etk_Type *type);
const char    *etk_signal_name_get(Etk_Signal *signal);
Etk_Marshaller etk_signal_marshaller_get(Etk_Signal *signal);

const Etk_Signal_Callback *etk_signal_connect_full_by_code(int signal_code,
   Etk_Object *object, Etk_Callback callback, void *data, Etk_Bool swapped,
   Etk_Bool after);
const Etk_Signal_Callback *etk_signal_connect_full_by_name(
   const char *signal_name, Etk_Object *object, Etk_Callback callback,
   void *data, Etk_Bool swapped, Etk_Bool after);

inline const Etk_Signal_Callback *etk_signal_connect_by_code(int signal_code,
   Etk_Object *object, Etk_Callback callback, void *data);
inline const Etk_Signal_Callback *etk_signal_connect(const char *signal_name,
   Etk_Object *object, Etk_Callback callback, void *data);
inline const Etk_Signal_Callback *etk_signal_connect_after_by_code(
   int signal_code, Etk_Object *object, Etk_Callback callback, void *data);
inline const Etk_Signal_Callback *etk_signal_connect_after(
   const char *signal_name, Etk_Object *object, Etk_Callback callback,
   void *data);
inline const Etk_Signal_Callback *etk_signal_connect_swapped_by_code(
   int signal_code, Etk_Object *object, Etk_Callback callback, void *data);
inline const Etk_Signal_Callback *etk_signal_connect_swapped(
   const char *signal_name, Etk_Object *object, Etk_Callback callback,
   void *data);

void etk_signal_connect_multiple(Etk_Signal_Connect_Desc *desc,
                                 Etk_Object *object, void *data);

void etk_signal_disconnect_by_code(int signal_code, Etk_Object *object,
                                   Etk_Callback callback, void *data);
void etk_signal_disconnect(const char *signal_name, Etk_Object *object,
                           Etk_Callback callback, void *data);
void etk_signal_disconnect_scb_by_code(int signal_code, Etk_Object *object,
                                       const Etk_Signal_Callback *scb);
void etk_signal_disconnect_scb(const char *signal_name, Etk_Object *object,
                               const Etk_Signal_Callback *scb);
void etk_signal_disconnect_all_by_code(int signal_code, Etk_Object *object);
void etk_signal_disconnect_all(const char *signal_name, Etk_Object *object);

void etk_signal_disconnect_multiple(Etk_Signal_Connect_Desc *desc,
                                    Etk_Object *object);

void etk_signal_block_by_code(int signal_code, Etk_Object *object,
                              Etk_Callback callback, void *data);
void etk_signal_block(const char *signal_name, Etk_Object *object,
                      Etk_Callback callback, void *data);
void etk_signal_block_scb_by_code(int signal_code, Etk_Object *object,
                                  const Etk_Signal_Callback *scb);
void etk_signal_block_scb(const char *signal_name, Etk_Object *object,
                          const Etk_Signal_Callback *scb);

void etk_signal_unblock_by_code(int signal_code, Etk_Object *object,
                                Etk_Callback callback, void *data);
void etk_signal_unblock(const char *signal_name, Etk_Object *object,
                        Etk_Callback callback, void *data);
void etk_signal_unblock_scb_by_code(int signal_code, Etk_Object *object,
                                    const Etk_Signal_Callback *scb);
void etk_signal_unblock_scb(const char *signal_name, Etk_Object *object,
                            const Etk_Signal_Callback *scb);

Etk_Bool etk_signal_emit(int signal_code, Etk_Object *object, ...);
Etk_Bool etk_signal_emit_by_name(const char *signal_name, Etk_Object *object,
                                 ...);
Etk_Bool etk_signal_emit_valist(Etk_Signal *signal, Etk_Object *object,
                                va_list args);

Evas_List * etk_signal_get_all();
const Etk_Type * etk_signal_object_type_get(Etk_Signal *signal);

/** @} */

#ifdef __cplusplus
}
#endif

#endif
