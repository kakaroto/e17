#include <Eina.h>

#ifndef GLOBALVARS
# define GLOBALVARS

typedef enum _Color
{
  COLOR_BLACK,
  COLOR_WHITE,
  COLOR_PINK,
  COLOR_BROWN,
  COLOR_BLUE
} Color;

typedef enum _Underwear
{
  UNDERWEAR_BOXER,
  UNDERWEAR_STRING
} Underwear;

// We define the tshirt cloth
typedef struct _tshirt
{
   const char *size;
   Eina_Bool long_sleeve;
   Color color;
} tshirt;

// We define the pullover cloth
typedef struct _pullover
{
   const char *size;
   Color color;
} pullover;

// We define the jean cloth
typedef struct _jean
{
   unsigned char size;
   Color color;
} jean;

// We define the underwear cloth
typedef struct _underwear
{
   unsigned char size;
   Underwear type;
   Color color;
} underwear;


// We define different types of clothes
// That we can use
typedef enum _Clothing_Type
{
  CLOTHING_TYPE_TSHIRT,
  CLOTHING_TYPE_PULLOVER,
  CLOTHING_TYPE_JEAN,
  CLOTHING_TYPE_UNDERWEAR,
  CLOTHING_TYPE_UNKNOWN
} Clothing_Type;

// We have different specs depending on the cloth type
union specs
{
   tshirt stshirt;
   pullover spullover;
   jean sjean;
   underwear sunderwear;
};

typedef struct _clothing
{
   Clothing_Type type;
   union specs data;
} clothing;

#endif
