#include "serialisation.h"

Match_Type match_type[] = {
  { "Tshirt", CLOTHING_TYPE_TSHIRT, _serialisation_clothing_tshirt_dd },
  { "Pullover", CLOTHING_TYPE_PULLOVER, _serialisation_clothing_pullover_dd },
  { "Jean", CLOTHING_TYPE_JEAN, _serialisation_clothing_jean_dd },
  { "underwear", CLOTHING_TYPE_UNDERWEAR, _serialisation_clothing_underwear_dd },
  { NULL, 0, NULL }
};

static Eina_Bool _serialisation_type_set(const char *type, void *data, Eina_Bool unknow);
static const char *_serialisation_type_get(const void *data, Eina_Bool *unknow);

Eina_Bool serialisation_init(void)
{
   Eet_Data_Descriptor_Class eddc;
   Eet_Data_Descriptor *edd;
   Eet_Data_Descriptor *un;
   int i;

   edd = unified = NULL;

   EET_EINA_STREAM_DATA_DESCRIPTOR_CLASS_SET(&eddc, clothing);
   edd = eet_data_descriptor_stream_new(&eddc);

   eddc.version = EET_DATA_DESCRIPTOR_CLASS_VERSION;
   eddc.func.type_get = _serialisation_type_get;
   eddc.func.type_set = _serialisation_type_set;
   un = eet_data_descriptor_stream_new(&eddc);

   for (i = 0; match_type[i].name != NULL; i++)
     EET_DATA_DESCRIPTOR_ADD_MAPPING(un, match_type[i].name, match_type[i].edd());

   EET_DATA_DESCRIPTOR_ADD_UNION(edd, clothing, "clothing", data, type, un);

   unified = edd;

   return EINA_TRUE;
}

static Eina_Bool _serialisation_type_set(const char *type, void *data, Eina_Bool unknow)
{
   Clothing_Type *ev = data;
   int i;

   if (unknow) return EINA_FALSE;

   for (i = 0; match_type[i].name != NULL; ++i)
     if (!strcmp(type, match_type[i].name))
       {
          fprintf(stderr, "found '%s'\n", match_type[i].name);
          *ev = match_type[i].type;
          return EINA_TRUE;
       }

   fprintf(stderr, "not found !\n");
   *ev = CLOTHING_TYPE_UNKNOWN;
   return EINA_FALSE;
}

static const char *_serialisation_type_get(const void *data, Eina_Bool *unknow)
{
   const Clothing_Type *ev = data;
   int i;

   for (i = 0; match_type[i].name != NULL; ++i)
     if (*ev == match_type[i].type)
       {
          fprintf(stderr, "found '%s'\n", match_type[i].name);
          return match_type[i].name;
       }

   if (unknow)
     *unknow = EINA_TRUE;

   fprintf(stderr, "unknow (%i)[%p] !\n", *ev, ev);
   return NULL;
}


Eet_Data_Descriptor *_serialisation_clothing_tshirt_dd()
{
   static Eet_Data_Descriptor *edd = NULL;
   Eet_Data_Descriptor_Class eddc;

   if (edd) return edd;

   EET_EINA_STREAM_DATA_DESCRIPTOR_CLASS_SET(&eddc, tshirt);
   edd = eet_data_descriptor_stream_new(&eddc);
   EET_DATA_DESCRIPTOR_ADD_BASIC(edd, tshirt, "size", size, EET_T_STRING);
   EET_DATA_DESCRIPTOR_ADD_BASIC(edd, tshirt, "long_sleeve", size, EET_T_UCHAR);
   EET_DATA_DESCRIPTOR_ADD_BASIC(edd, tshirt, "color", color, EET_T_INT);

   return edd;
}

Eet_Data_Descriptor *_serialisation_clothing_pullover_dd()
{
   static Eet_Data_Descriptor *edd = NULL;
   Eet_Data_Descriptor_Class eddc;

   if (edd) return edd;

   EET_EINA_STREAM_DATA_DESCRIPTOR_CLASS_SET(&eddc, pullover);
   edd = eet_data_descriptor_stream_new(&eddc);
   EET_DATA_DESCRIPTOR_ADD_BASIC(edd, pullover, "size", size, EET_T_STRING);
   EET_DATA_DESCRIPTOR_ADD_BASIC(edd, pullover, "color", color, EET_T_INT);

   return edd;
}

Eet_Data_Descriptor *_serialisation_clothing_jean_dd()
{
   static Eet_Data_Descriptor *edd = NULL;
   Eet_Data_Descriptor_Class eddc;

   if (edd) return edd;

   EET_EINA_STREAM_DATA_DESCRIPTOR_CLASS_SET(&eddc, jean);
   edd = eet_data_descriptor_stream_new(&eddc);
   EET_DATA_DESCRIPTOR_ADD_BASIC(edd, jean, "size", size, EET_T_UCHAR);
   EET_DATA_DESCRIPTOR_ADD_BASIC(edd, jean, "color", color, EET_T_INT);

   return edd;
}

Eet_Data_Descriptor *_serialisation_clothing_underwear_dd()
{
   static Eet_Data_Descriptor *edd = NULL;
   Eet_Data_Descriptor_Class eddc;

   if (edd) return edd;

   EET_EINA_STREAM_DATA_DESCRIPTOR_CLASS_SET(&eddc, underwear);
   edd = eet_data_descriptor_stream_new(&eddc);
   EET_DATA_DESCRIPTOR_ADD_BASIC(edd, underwear, "size", size, EET_T_UCHAR);
   EET_DATA_DESCRIPTOR_ADD_BASIC(edd, underwear, "type", type, EET_T_INT);
   EET_DATA_DESCRIPTOR_ADD_BASIC(edd, underwear, "color", color, EET_T_INT);

   return edd;
}
