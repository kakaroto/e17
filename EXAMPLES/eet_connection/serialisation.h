#include <Eina.h>
#include <Eet.h>

#include "global.h"

Eet_Data_Descriptor *_serialisation_clothing_tshirt_dd(void);
Eet_Data_Descriptor *_serialisation_clothing_pullover_dd(void);
Eet_Data_Descriptor *_serialisation_clothing_jean_dd(void);
Eet_Data_Descriptor *_serialisation_clothing_underwear_dd(void);

typedef struct _Match_Type Match_Type;

struct _Match_Type {
   const char *name;
   Clothing_Type type;
   Eet_Data_Descriptor *(*edd)(void);
};

extern Match_Type match_type[];

Eet_Data_Descriptor *unified;

Eina_Bool serialisation_init(void);
