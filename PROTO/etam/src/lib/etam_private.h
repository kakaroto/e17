#ifndef ETAM_PRIVATE_H_
# define ETAM_PRIVATE_H_

#include <Eina.h>
#include <Eet.h>
#include <Ecore.h>

#include "etam_bool.h"

typedef struct _Etam_DB Etam_DB;
typedef struct _Etam_DB_Local Etam_DB_Local;

typedef struct _Etam_Packets Etam_Packets;
typedef struct _Etam_Packet Etam_Packet;
typedef struct _Etam_Packet_Keys Etam_Packet_Keys;
typedef struct _Etam_Packet_SubStrings Etam_Packet_SubStrings;
typedef struct _Etam_Packet_Strings Etam_Packet_Strings;
typedef struct _Etam_Packet_String Etam_Packet_String;
typedef struct _Etam_Packet_Long_Long Etam_Packet_Long_Long;

struct _Etam_DB
{
   Etam_DB_Local *local;
   Eina_List *callbacks;
};

struct _Etam_DB_Local
{
   EINA_REFCOUNT;
   EINA_MAGIC;
   Eet_File *ef;

   const char *filename;
   Eina_RWLock lock;

   Eina_Hash *collections;
};

struct _Etam_Collection
{
   EINA_REFCOUNT;
   EINA_MAGIC;

   Etam_DB_Local *db;

   unsigned int keys_count;
   Etam_Packet_Keys *keys;

   Eina_Hash *rows;

   Eina_RWLock lock;
};

struct _Etam_Packets
{
   EINA_REFCOUNT;
   Etam_Data_Type type;

   const char *name;

   Eina_RWLock lock;

   unsigned int packets_count;

   union {
/* Boolean are RLE encoded with 7 bits for the length, 1 for the value [meaning a full packet with only true or false will take 8 bytes to encode] */
      Etam_RLE_Bool *boolean;
      Etam_Packet_Long_Long **long_long;
      Etam_Packet_String **string;
      Etam_Packet_Strings **strings;
   } u;
};
struct _Etam_Packet_Keys
{
   const char *keys[1024];
   unsigned int hash[1024];
   unsigned int generation[1024];
};

/* The idea is that an Etam_Request should not walk over more than 1024 data at the same time when processing */
struct _Etam_Packet_Long_Long
{
   long long value[1024];
};

struct _Etam_Packet_String
{
   const char *value[1024];
};

struct _Etam_Packet_Strings
{
   Eina_List *values[1024];
};

#ifdef ETAM_DEFAULT_LOG_COLOR
# undef ETAM_DEFAULT_LOG_COLOR
#endif
#define ETAM_DEFAULT_LOG_COLOR EINA_COLOR_CYAN
#ifdef ERR
# undef ERR
#endif
#define ERR(...) EINA_LOG_DOM_ERR(_etam_default_log_dom, __VA_ARGS__)
#ifdef INF
# undef INF
#endif
#define INF(...) EINA_LOG_DOM_INFO(_etam_default_log_dom, __VA_ARGS__)
#ifdef WRN
# undef WRN
#endif
#define WRN(...) EINA_LOG_DOM_WARN(_etam_default_log_dom, __VA_ARGS__)
#ifdef CRIT
# undef CRIT
#endif
#define CRIT(...) EINA_LOG_DOM_CRIT(_etam_default_log_dom, __VA_ARGS__)
#ifdef DBG
# undef DBG
#endif
#define DBG(...) EINA_LOG_DOM_DBG(_etam_default_log_dom, __VA_ARGS__)

void etam_db_local_init(void);
void etam_db_local_shutdown(void);
void etam_eet_data_descriptor_init(void);
void etam_eet_data_descriptor_shutdown(void);

Etam_DB_Local *etam_db_local_open(const char *filename,
				  Etam_DB_Validate_Cb cb,
				  const void *data);
void etam_db_local_close(Etam_DB_Local *db);
void etam_db_local_flush(Etam_DB_Local *db);
void etam_db_local_free(Etam_DB_Local *db);

Etam_Packets *etam_packet_rlock(Etam_Collection *coll, const char *name, Etam_Data_Type type);
Etam_Packets *etam_packet_wlock(Etam_Collection *coll, const char *name, Etam_Data_Type type);
void etam_packet_release(Etam_Collection *coll, const char *name, Etam_Data_Type type);
void etam_packets_free(Etam_Packets *packets); /* Only use it with EINA_REFCOUNT_UNREF will be triggered only if etam_packets_del has been called on it before */
void etam_packets_del(Etam_Collection *coll, const char *name);

#include "etam_packets_op.h"

#endif
