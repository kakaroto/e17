#ifndef E_PHYS_DEMO_H
#define E_PHYS_DEMO_H

#include <E_Phys.h>

#include <Ecore.h>
#include <Evas.h>
#include <Ecore_Evas.h>

typedef struct _App App;
struct _App
{
  Ecore_Evas *ee;
  Evas *evas;

  E_Phys_World *world;
  E_Phys_Constraint_Anchor *mouse_anchor;
  Evas_List *objects;
};

void setup(App *app, int argc, char **argv);

int rand_range(int from, int to);
float rand_range_f(float from, float to);


typedef struct _Constraint_Wrap Constraint_Wrap;
struct _Constraint_Wrap
{
  E_Phys_Constraint con;
  int margin;
  Evas_List *exclusions;
};

Constraint_Wrap *snow_test_constraint_wrap_around_add(E_Phys_World *world,
                   int margin);
void             snow_test_constraint_wrap_around_exclude(Constraint_Wrap *con,
                   E_Phys_Particle *p);
void             init_snow_test(E_Phys_World *world);

#endif
