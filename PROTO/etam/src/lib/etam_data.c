#include <Eina.h>
#include <Eet.h>

#include "Etam.h"

#include "etam_private.h"

Eet_Data_Descriptor *_etam_edd_etam_file = NULL;
Eet_Data_Descriptor *_etam_edd_etam_collection = NULL;

static Eet_Data_Descriptor *_etam_edd_etam_packet_keys = NULL;

// static Eet_Data_Descriptor *_etam_edd_etam_packet_strings = NULL;
static Eet_Data_Descriptor *_etam_edd_etam_packet_long_long_pointer = NULL;
static Eet_Data_Descriptor *_etam_edd_etam_packet_string_pointer = NULL;
static Eet_Data_Descriptor *_etam_edd_etam_packet_long_long = NULL;
static Eet_Data_Descriptor *_etam_edd_etam_packet_string = NULL;
static Eet_Data_Descriptor *_etam_edd_etam_rle_bool = NULL;

static Eet_Data_Descriptor *_etam_edd_etam_packets_union = NULL;
static Eet_Data_Descriptor *_etam_edd_etam_packets = NULL;

void
etam_eet_data_descriptor_init(void)
{
   Eet_Data_Descriptor_Class eddc;

   if (_etam_edd_etam_file) return ;

   /* key description */
   EET_EINA_FILE_DATA_DESCRIPTOR_CLASS_SET(&eddc, Etam_Packet_Keys);
   _etam_edd_etam_packet_keys = eet_data_descriptor_file_new(&eddc);
   EET_DATA_DESCRIPTOR_ADD_BASIC_ARRAY(_etam_edd_etam_packet_keys, Etam_Packet_Keys, "keys", keys, EET_T_STRING);
   EET_DATA_DESCRIPTOR_ADD_BASIC_ARRAY(_etam_edd_etam_packet_keys, Etam_Packet_Keys, "hash", hash, EET_T_UINT);
   EET_DATA_DESCRIPTOR_ADD_BASIC_ARRAY(_etam_edd_etam_packet_keys, Etam_Packet_Keys, "generation", generation, EET_T_UINT);

   /* row data description */
   EET_EINA_FILE_DATA_DESCRIPTOR_CLASS_SET(&eddc, Etam_Packet_String);
   _etam_edd_etam_packet_string = eet_data_descriptor_file_new(&eddc);
   EET_DATA_DESCRIPTOR_ADD_BASIC_ARRAY(_etam_edd_etam_packet_string, Etam_Packet_String, "value", value, EET_T_STRING);

   EET_EINA_FILE_DATA_DESCRIPTOR_CLASS_SET(&eddc, Etam_Packet_Long_Long);
   _etam_edd_etam_packet_long_long = eet_data_descriptor_file_new(&eddc);
   EET_DATA_DESCRIPTOR_ADD_BASIC_ARRAY(_etam_edd_etam_packet_string, Etam_Packet_String, "value", value, EET_T_LONG_LONG);

   EET_EINA_FILE_DATA_DESCRIPTOR_CLASS_SET(&eddc, Etam_RLE_Bool);
   _etam_edd_etam_rle_bool = eet_data_descriptor_file_new(&eddc);
   {
      Etam_RLE_Bool ___ett;
      eet_data_descriptor_element_add(_etam_edd_etam_rle_bool, "rle_value", EET_T_UCHAR, EET_G_VAR_ARRAY,
                                      (char *)(& (___ett.rle_value)) - (char *)(& (___ett)),
                                      (char *)(& (___ett.length)) - (char *)(& (___ett)),
                                      NULL, NULL);
   }

#define ETAM_DEFINE_POINTER_TYPE(Type, Name)                            \
   {                                                                    \
      typedef struct _##Type##_Pointer Type##_Pointer;                  \
      struct _##Type##_Pointer                                          \
      {                                                                 \
         Type *pointer;                                                 \
      };                                                                \
                                                                        \
      EET_EINA_FILE_DATA_DESCRIPTOR_CLASS_SET(&eddc, Type##_Pointer);   \
      _etam_edd_##Name##_pointer = eet_data_descriptor_file_new(&eddc); \
      EET_DATA_DESCRIPTOR_ADD_SUB(_etam_edd_##Name##_pointer,           \
                                  Type##_Pointer, "pointer", pointer,   \
                                  _etam_edd_##Name);                    \
   }

   ETAM_DEFINE_POINTER_TYPE(Etam_Packet_Long_Long, etam_packet_long_long);
   ETAM_DEFINE_POINTER_TYPE(Etam_Packet_String, etam_packet_string);

#define ETAM_ADD_ARRAY_MAPPING(Variant, Name, Subtype)  \
   {                                                    \
      Etam_Packets tmp;                                 \
                                                        \
      eet_data_descriptor_element_add(Variant, Name,                    \
                                      EET_T_UNKNOW, EET_G_VAR_ARRAY,    \
                                      0, (char*)(&tmp.packets_count) - (char*)(&tmp), \
                                      NULL, Subtype);                   \
   }

   eddc.version = EET_DATA_DESCRIPTOR_CLASS_VERSION;
   eddc.func.type_get = NULL; /* FIXME */
   eddc.func.type_set = NULL; /* FIXME */
   _etam_edd_etam_packets_union = eet_data_descriptor_file_new(&eddc);

   ETAM_ADD_ARRAY_MAPPING(_etam_edd_etam_packets_union, "boolean", _etam_edd_etam_rle_bool);
   ETAM_ADD_ARRAY_MAPPING(_etam_edd_etam_packets_union, "string", _etam_edd_etam_packet_string_pointer);
   ETAM_ADD_ARRAY_MAPPING(_etam_edd_etam_packets_union, "long_long", _etam_edd_etam_packet_long_long_pointer);

   EET_EINA_FILE_DATA_DESCRIPTOR_CLASS_SET(&eddc, Etam_Packets);
   _etam_edd_etam_packets = eet_data_descriptor_file_new(&eddc);
   EET_DATA_DESCRIPTOR_ADD_UNION(_etam_edd_etam_packets, Etam_Packets, "u", u, type, _etam_edd_etam_packets_union);
   EET_DATA_DESCRIPTOR_ADD_BASIC(_etam_edd_etam_packets, Etam_Packets, "name", name, EET_T_STRING);

   EET_EINA_FILE_DATA_DESCRIPTOR_CLASS_SET(&eddc, Etam_Collection);
   _etam_edd_etam_collection = eet_data_descriptor_file_new(&eddc);
   EET_DATA_DESCRIPTOR_ADD_VAR_ARRAY(_etam_edd_etam_collection, Etam_Collection, "keys", keys, _etam_edd_etam_packet_keys);
   EET_DATA_DESCRIPTOR_ADD_HASH(_etam_edd_etam_collection, Etam_Collection, "rows", rows, _etam_edd_etam_packets);
}

#define FREED(Eed)                              \
  if (Eed)                                      \
    {                                           \
       eet_data_descriptor_free((Eed));         \
       (Eed) = NULL;                            \
    }

void
etam_eet_data_descriptor_shutdown(void)
{
   FREED(_etam_edd_etam_collection);
   FREED(_etam_edd_etam_packet_keys);
   FREED(_etam_edd_etam_packet_long_long_pointer);
   FREED(_etam_edd_etam_packet_string_pointer);
   FREED(_etam_edd_etam_packet_long_long);
   FREED(_etam_edd_etam_packet_string);
   FREED(_etam_edd_etam_rle_bool);

   FREED(_etam_edd_etam_packets_union);
   FREED(_etam_edd_etam_packets);
}

