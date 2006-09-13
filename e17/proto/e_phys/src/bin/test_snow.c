#include "demo.h"

static void      snow_test_constraint_wrap_around_apply(E_Phys_Constraint *con);

int
cb_wind_timer(void *data)
{
  E_Phys_Particle *wind = data;

  wind->m = rand_range_f(-1e14, 1e14);

  //printf("snow time, mass: %f\n", wind->m);
  return 1;
}

void
init_snow_test(E_Phys_World *world)
{
  E_Phys_Constraint_Boundary *bound;
  E_Phys_Particle *p, *c, *w;
  Constraint_Wrap *cw;

  int i, num = 300;

  c = e_phys_particle_add(world, 1e13, world->w / 2, 1e5, 0, 0);
  e_phys_particle_size_set(c, 0, 0);

  w = e_phys_particle_add(world, 1, 1e5, world->h / 2, 0, 0);
  for (i = 0; i < num; i++)
  {
    p = e_phys_particle_add(world, rand_range_f(3.0, 6.0), rand_range(-10, world->w + 10), rand_range(-10, 610), rand_range_f(-0.1, 0.1), rand_range_f(0.0, 0.1));
    e_phys_particle_size_set(p, p->m, p->m);
  }

  e_phys_force_gravity_add(world, .001);
  cw = snow_test_constraint_wrap_around_add(world, 10);
  snow_test_constraint_wrap_around_exclude(cw, c);
  snow_test_constraint_wrap_around_exclude(cw, w);

  printf("init snow\n");
}

Constraint_Wrap *
snow_test_constraint_wrap_around_add(E_Phys_World *world, int margin)
{
  Constraint_Wrap *con;

  con = malloc(sizeof(Constraint_Wrap));
  e_phys_constraint_init(E_PHYS_CONSTRAINT(con), world, snow_test_constraint_wrap_around_apply, NULL);

  con->margin = margin;
  return con;
}

void
snow_test_constraint_wrap_around_exclude(Constraint_Wrap *con, E_Phys_Particle *p)
{
  con->exclusions = evas_list_append(con->exclusions, p);
}

static void
snow_test_constraint_wrap_around_apply(E_Phys_Constraint *con)
{
  Evas_List *l;
  E_Phys_World *world;
  Constraint_Wrap *cw = (Constraint_Wrap *)con;

  if (!con || !con->world) return;
  world = con->world;

  for (l = world->particles; l; l = l->next)
  {
    E_Phys_Particle *p;
    p = l->data;

    if (evas_list_find(cw->exclusions, p)) continue;

    /* wrap horizontally, keeping velocity */
    if (p->cur.x < -1 * cw->margin)
    {
      float vx;
      vx = p->cur.x - p->prev.x;
      p->cur.x = world->w + cw->margin + p->cur.x;
      p->prev.x = p->cur.x - vx;
    }
    else if (p->cur.x > world->w + cw->margin)
    {
      float vx;
      vx = p->cur.x - p->prev.x;
      p->cur.x = -1 * cw->margin + (p->cur.x - (world->w + cw->margin));
      p->prev.x = p->cur.x - vx;
    }

    /* when flakes fall off the bottom, start them over at the top */
    if (p->cur.y > world->h + cw->margin)
    {
      float vy;
      vy = p->cur.y - p->prev.y;
      p->cur.y = -1 * cw->margin + (p->cur.y - (world->h + cw->margin));
      p->prev.y = p->cur.y; // - vy;
    }
  }
}
