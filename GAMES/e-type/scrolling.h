#ifndef _EFL_SCROLLING_H_
#define _EFL_SCROLLING_H_


#include "forward.h"
#include "game.h"


typedef enum
{
  SCROLLING_LEFT,
  SCROLLING_RIGHT,
  SCROLLING_CENTER
} Scrolling_Pos;


Scrolling *scrolling_new(Game *g, int nbr);
void       scrolling_free(Scrolling *s);
void       scrolling_start(Scrolling *s);


#endif /* _EFL_SCROLLING_H_ */
