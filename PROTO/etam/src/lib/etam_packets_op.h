#ifndef ETAM_PACKET_H_
# define ETAM_PACKET_H_

Eina_Bool etam_packet_boolean_equal(Etam_Packets *packets, unsigned int block,
				    Eina_Bool value,
				    Etam_RLE_Bool *in, Etam_RLE_Bool *out);
Eina_Bool etam_packet_string_equal(Etam_Packets *packets, unsigned int block,
				   const char *value,
				   Etam_RLE_Bool *in, Etam_RLE_Bool *out);
Eina_Bool etam_packet_string_like(Etam_Packets *packets, unsigned int block,
				  const char *value, int flags,
				  Etam_RLE_Bool *in, Etam_RLE_Bool *out);
Eina_Bool etam_packet_long_long_equal(Etam_Packets *packets, unsigned int block,
				      long long value,
				      Etam_RLE_Bool *in, Etam_RLE_Bool *out);
Eina_Bool etam_packet_long_long_inf(Etam_Packets *packets, unsigned int block,
				    long long value,
				    Etam_RLE_Bool *in, Etam_RLE_Bool *out);
Eina_Bool etam_packet_long_long_sup(Etam_Packets *packets, unsigned int block,
				    long long value,
				    Etam_RLE_Bool *in, Etam_RLE_Bool *out);

Eina_Bool etam_packet_boolean_set(Etam_Packets *packets, int index, Eina_Bool value);
Eina_Bool etam_packet_string_set(Etam_Packets *packets, int index, const char *s);
Eina_Bool etam_packet_long_long_set(Etam_Packets *packets, int index, long long ll);

void etam_packet_strings_append(Etam_Packets *packets, int index, Eina_List *l); /* This add l to the current list of data */
void etam_packet_strings_modify(Etam_Packets *packets, int index, Eina_List *l); /* This set l as the current list of data */

void etam_packet_boolean_value_get(Etam_Packets *packets, int index, Eina_Value *v);
void etam_packet_boolean_data_get(Etam_Packets *packets, int index, void *b);
void etam_packets_boolean_value_get(Etam_Packets *packets, unsigned int block, Etam_RLE_Bool *map, Eina_Value *v);
void etam_packets_boolean_data_get(Etam_Packets *packets, unsigned int block, Etam_RLE_Bool *map,
				   void *a, int sizeof_struct, int offset);

void etam_packet_string_value_get(Etam_Packets *packets, int index, Eina_Value *v);
void etam_packet_string_data_get(Etam_Packets *packets, int index, void *b);
void etam_packets_string_value_get(Etam_Packets *packets, unsigned int block, Etam_RLE_Bool *map, Eina_Value *v);
void etam_packets_string_data_get(Etam_Packets *packets, unsigned int block, Etam_RLE_Bool *map,
				  void *a, int sizeof_struct, int offset);

void etam_packet_long_long_value_get(Etam_Packets *packets, int index, Eina_Value *v);
void etam_packet_long_long_data_get(Etam_Packets *packets, int index, void *b);
void etam_packets_long_long_value_get(Etam_Packets *packets, unsigned int block, Etam_RLE_Bool *map, Eina_Value *v);
void etam_packets_long_long_data_get(Etam_Packets *packets, unsigned int block, Etam_RLE_Bool *map,
				     void *a, int sizeof_struct, int offset);

void etam_packet_strings_value_get(Etam_Packets *packets, int index, Eina_Value *v);
void etam_packet_strings_data_get(Etam_Packets *packets, int index, void *b);
void etam_packets_strings_value_get(Etam_Packets *packets, int block, Etam_RLE_Bool *map, Eina_Value *v);
void etam_packets_strings_data_get(Etam_Packets *packets, int block, Etam_RLE_Bool *map,
				   void *a, int sizeof_struct, int offset);

#endif
