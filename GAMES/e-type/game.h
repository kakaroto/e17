#ifndef _EFL_GAME_H_
#define _EFL_GAME_H_


#include <Eina.h>
#include <Evas.h>

#include "forward.h"
#include "alien.h"
#include "powerup.h"
#include "scenario.h"
#include "ship.h"
#include "shoot.h"

Game      *game_new(Evas *evas);
void       game_free(Game *g);
Evas      *game_evas_get(Game *g);
void       game_size_get(Game *g, Evas_Coord *w, Evas_Coord *h);
void       game_alien_append(Game *g, Alien *a);
void       game_alien_remove(Game *g, Alien *a);
void       game_shoot_append(Game *g, Shoot *s);
void       game_shoot_remove(Game *g, Shoot *s);
void       game_powerup_append(Game *g, Powerup *p);
void       game_powerup_remove(Game *g, Powerup *p);
Eina_List *game_aliens_get(Game *g);
Eina_List *game_shoots_get(Game *g);
Scenario  *game_scenario_get(Game *g);
Ship      *game_ship_get(Game *g);
Eina_List *game_powerups_get(Game *g);


#endif /* _EFL_GAME_H_ */
