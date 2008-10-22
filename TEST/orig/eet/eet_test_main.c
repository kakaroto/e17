#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <Eet.h>
#include <Evas.h>

typedef struct _blah2
{
   char *string;
}
Blah2;

typedef struct _blah3
{
   char *string3;
   char *string4;
}
Blah3;

typedef struct _blah4
{
   char *string5;
}
Blah4;

typedef struct _blah
{
   char character;
   short sixteen;
   int integer;
   long long lots;
   float floating;
   double floating_lots;
   char *string;
   Blah2 *blah2;
   Eina_List *blah3;
   Evas_Hash *blah4;
}
Blah;

void
df(void *data, const char *str)
{
   printf("%s", str);
}

void
encdectest(void)
{
   Blah blah;
   Blah2 blah2;
   Blah3 blah3;
   Blah4 blah4;
   Eet_Data_Descriptor *edd, *edd2, *edd3, *edd4;
   void *data;
   int size;
   FILE *f;
   Blah *blah_in;

   edd4 = eet_data_descriptor_new("blah4", sizeof(Blah3),
				  eina_list_next,
				  eina_list_append,
				  eina_list_data_get,
				  eina_list_free,
				  evas_hash_foreach,
				  evas_hash_add,
				  evas_hash_free);
   EET_DATA_DESCRIPTOR_ADD_BASIC(edd4, Blah4, "string5", string5, EET_T_STRING);
   edd3 = eet_data_descriptor_new("blah3", sizeof(Blah3),
				  eina_list_next,
				  eina_list_append,
				  eina_list_data_get,
				  eina_list_free,
				  evas_hash_foreach,
				  evas_hash_add,
				  evas_hash_free);
   EET_DATA_DESCRIPTOR_ADD_BASIC(edd3, Blah3, "string3", string3, EET_T_STRING);
   EET_DATA_DESCRIPTOR_ADD_BASIC(edd3, Blah3, "string4", string4, EET_T_STRING);

   edd2 = eet_data_descriptor_new("blah2", sizeof(Blah2),
				  eina_list_next,
				  eina_list_append,
				  eina_list_data_get,
				  eina_list_free,
				  evas_hash_foreach,
				  evas_hash_add,
				  evas_hash_free);
   EET_DATA_DESCRIPTOR_ADD_BASIC(edd2, Blah2, "string2", string, EET_T_STRING);

   edd = eet_data_descriptor_new("blah", sizeof(Blah),
				  eina_list_next,
				  eina_list_append,
				  eina_list_data_get,
				  eina_list_free,
				  evas_hash_foreach,
				  evas_hash_add,
				  evas_hash_free);
   EET_DATA_DESCRIPTOR_ADD_BASIC(edd, Blah, "character", character, EET_T_CHAR);
   EET_DATA_DESCRIPTOR_ADD_BASIC(edd, Blah, "sixteen", sixteen, EET_T_SHORT);
   EET_DATA_DESCRIPTOR_ADD_BASIC(edd, Blah, "integer", integer, EET_T_INT);
   EET_DATA_DESCRIPTOR_ADD_BASIC(edd, Blah, "lots", lots, EET_T_LONG_LONG);
   EET_DATA_DESCRIPTOR_ADD_BASIC(edd, Blah, "floating", floating, EET_T_FLOAT);
   EET_DATA_DESCRIPTOR_ADD_BASIC(edd, Blah, "floating_lots", floating_lots, EET_T_DOUBLE);
   EET_DATA_DESCRIPTOR_ADD_BASIC(edd, Blah, "string", string, EET_T_STRING);
   EET_DATA_DESCRIPTOR_ADD_SUB  (edd, Blah, "blah2", blah2, edd2);
   EET_DATA_DESCRIPTOR_ADD_LIST (edd, Blah, "blah3_list", blah3, edd3);
   EET_DATA_DESCRIPTOR_ADD_HASH (edd, Blah, "blah4_hash", blah4, edd4);

   blah3.string3="PANTS3";
   blah3.string4="PANTS4 \"Are Hot\"";

   blah4.string5="PANTS5";
   
   blah2.string="subtype string here!";

   memset(&blah, 0, sizeof(blah));
   blah.character='7';
   blah.sixteen=0x7777;
   blah.integer=0xc0def00d;
   blah.lots=0xdeadbeef31337777;
   blah.floating=3.141592654;
   blah.floating_lots=0.777777777777777;
   blah.string="bite me like a turnip";
   blah.blah2 = &blah2;
   blah.blah3 = eina_list_append(NULL, &blah3);
   blah.blah3 = eina_list_append(blah.blah3, &blah3);
   blah.blah3 = eina_list_append(blah.blah3, &blah3);
   blah.blah3 = eina_list_append(blah.blah3, &blah3);
   blah.blah3 = eina_list_append(blah.blah3, &blah3);
   blah.blah3 = eina_list_append(blah.blah3, &blah3);
   blah.blah3 = eina_list_append(blah.blah3, &blah3);
   blah.blah4 = evas_hash_add(blah.blah4, "key1", &blah4);
   blah.blah4 = evas_hash_add(blah.blah4, "key2", &blah4);
   blah.blah4 = evas_hash_add(blah.blah4, "key3", &blah4);

   data = eet_data_descriptor_encode(edd, &blah, &size);
   if (!data)
     {
	printf("ERROR! cannot encode!!\n");
	exit(1);
     }
   f = fopen("out", "wb");
   if (f)
     {
	fwrite(data, size, 1, f);
	fclose(f);
     }
   printf("-----DECODING\n");
   if (!eet_data_text_dump(data, size, df, NULL))
     {
	printf("CANNOT DUMP!!!\n");
     }
   blah_in = eet_data_descriptor_decode(edd, data, size);
   if (!blah_in)
     {
	printf("ERROR! cannot decode!!\n");
	exit(1);
     }
   printf("-----DECODED!\n");
   printf("%c\n", blah_in->character);
   printf("%x\n", (int)blah_in->sixteen);
   printf("%x\n", blah_in->integer);
   printf("%lx\n", blah_in->lots);
   printf("%f\n", (double)blah_in->floating);
   printf("%f\n", (double)blah_in->floating_lots);
   printf("%s\n", blah_in->string);
   printf("%p\n", blah_in->blah2);
   printf("  %s\n", blah_in->blah2->string);
     {
	Eina_List *l;

	for (l = blah_in->blah3; l; l = l->next)
	  {
	     Blah3 *blah3_in;

	     blah3_in = l->data;
	     printf("%p\n", blah3_in);
	     printf("  %s\n", blah3_in->string3);
	     printf("  %s\n", blah3_in->string4);
	  }
     }
   eet_data_descriptor_free(edd);
   eet_data_descriptor_free(edd2);
   eet_data_descriptor_free(edd3);

   exit(0);
}

int
main(int argc, char **argv)
{
   eet_init();
   encdectest();
}
