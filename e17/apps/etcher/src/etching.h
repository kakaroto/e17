#ifndef ETCHER_ETCHING_H
#define ETCHER_ETCHING_H

#include "bits.h"

typedef struct etching Etching;

Etching        *etching_new(char *filename);
void            etching_free(Etching * etching);

Etching        *etching_load(char *filename);
void            etching_save_as(Etching * e, char *filename);
void            etching_save(Etching * e);

void            etching_set_dirty(Etching * e);
int             etching_is_dirty(Etching * e);
void            etching_set_filename(Etching * e, char *filename);
char           *etching_get_filename(Etching * e);
Ebits_Object    etching_get_bits(Etching * e);
char           *etching_get_current_state(Etching * e);
void            etching_set_current_state(Etching * e, char *state);
Ebits_Object_Bit_State etching_get_selected_item(Etching * e);
void            etching_set_selected_item(Etching * e,
					  Ebits_Object_Bit_State item);
void            etching_delete_current_item(Etching * e);
#endif
