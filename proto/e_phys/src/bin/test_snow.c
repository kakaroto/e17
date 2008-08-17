#include "demo.h"

static void      snow_test_constraint_wrap_around_apply(E_Phys_Constraint *con);
static E_Phys_Vector snow_test_wind(float time);

void
init_snow_test(E_Phys_World *world)
{
  E_Phys_Particle *p;
  Constraint_Wrap *cw;
  E_Phys_Vector g;

  int i, num = 300;

  int padding = 10;
  
  g.x = 0;
  g.y = 1;

  for (i = 0; i < num; i++)
  {
    E_Phys_Vector pos, v0;

    pos.x = rand_range(-1 * padding, world->w + padding);
    pos.y = rand_range(-1 * padding, world->h + padding);

    v0.x = 0;
    v0.y = sqrt(2 * g.y * pos.y) * world->dt;

    p = e_phys_particle_add(world);
    e_phys_particle_mass_set(p, rand_range(3, 5));
    e_phys_particle_move(p, &pos);
    e_phys_particle_velocity_set(p, &v0);
    e_phys_particle_size_set(p, p->m, p->m);
  }


  cw = snow_test_constraint_wrap_around_add(world, 10);
  e_phys_force_constant_add(world, g, 1);
  e_phys_force_uniform_add(world, snow_test_wind, 0);

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
    if (p->cur.y > world->h)
    {
      p->cur.x = rand_range(-1 * cw->margin, world->w + cw->margin);
      p->cur.y = -1 * cw->margin;
      p->prev.x = p->cur.x; 
      p->prev.y = p->cur.y; 
    }
  }
}

static E_Phys_Vector
snow_test_wind(float time)
{
  E_Phys_Vector f;


  f.x = rand_range(-50, 50);
  f.y = 0;
  return f;
}
