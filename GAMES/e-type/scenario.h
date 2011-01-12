#ifndef _EFL_SCENARIO_H_
#define _EFL_SCENARIO_H_


#include <Evas.h>

#include "forward.h"
#include "alien.h"
#include "powerup.h"


Scenario *scenario_new(Game *g);
void      scenario_free(Scenario *s);
void      scenario_add(Scenario *s, int count, Alien_Type alien_type, Powerup_Type powerup_type, Evas_Coord x);
/* void      scenario_del(Scenario *s, Action *a); */
void      scenario_launch(Scenario *s, int count);


#endif /* _EFL_SCENARIO_H_ */
