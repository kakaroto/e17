#include "E_Phys.h"

static void e_phys_world_accumulate_forces(E_Phys_World *world);
static void e_phys_world_verlet_integrate(E_Phys_World *world);
static void e_phys_world_satisfy_constraints(E_Phys_World *world);
static int e_phys_world_timer(void *data);

E_Phys_World *
e_phys_world_add(int w, int h)
{
  E_Phys_World *world;
  world = calloc(1, sizeof(E_Phys_World));
  world->constraint_iter = 10;
  world->w = w;
  world->h = h;
  world->dt = 1.0 / 60.0;
  return world;
}

void
e_phys_world_free(E_Phys_World *world)
{
  if (world) free(world);
  // XXX free particles, springs, etc
  while (world->particles) 
  {
    e_phys_particle_free(world->particles->data);
    world->particles = evas_list_remove_list(world->particles, world->particles);
  }
  while (world->forces) 
  {
    E_Phys_Force *f = world->forces->data;
    f->free(f);
    world->forces = evas_list_remove_list(world->forces, world->forces);
  }
  while (world->constraints) 
  {
    E_Phys_Constraint *c = world->constraints->data;
    c->free(c);
    world->constraints = evas_list_remove_list(world->constraints, world->constraints);
  }
}

static int 
e_phys_world_timer(void *data)
{
  E_Phys_World *world;
  int i;

  world = data;
  e_phys_world_accumulate_forces(world);
  e_phys_world_verlet_integrate(world);
  for (i = 0; i < world->constraint_iter; i++)
    e_phys_world_satisfy_constraints(world);

  if (world->update_func) world->update_func(world->update_data, world);
  return 1;
}

void
e_phys_world_go(E_Phys_World *world)
{
  world->timer = ecore_timer_add(world->dt, e_phys_world_timer, world);
}

void
e_phys_world_stop(E_Phys_World *world)
{
  if (world->timer) ecore_timer_del(world->timer);
  world->timer = NULL;
}

void
e_phys_world_update_func_set(E_Phys_World *world, void (*func) (void *data, E_Phys_World *world), void *data)
{
  world->update_func = func;
  world->update_data = data;
}

E_Phys_Particle *
e_phys_world_nearest_particle(E_Phys_World *world, int x, int y)
{
  Evas_List *l;
  E_Phys_Particle *nearest = NULL;
  float distance;


  for (l = world->particles; l; l = l->next)
  {
    E_Phys_Particle *p;
    E_Phys_Vector d;
    float r;
    p = l->data;

    d.x = p->cur.x - x;
    d.y = p->cur.y - y;
    r = sqrt(d.x * d.x + d.y * d.y);

    if (!nearest || r < distance)
    {
      nearest = p;
      distance = r;
    }
  }

  return nearest;
}

void
e_phys_world_size_set(E_Phys_World *world, int w, int h)
{
  world->w = w;
  world->h = h;
}
static void
e_phys_world_accumulate_forces(E_Phys_World *world)
{
  Evas_List *l;

  // clear old forces
  for (l = world->particles; l; l = l->next)
  {
    E_Phys_Particle *p;

    p = l->data;
    p->force.x = 0;
    p->force.y = 0;
  }

  // accumulate forces
  for (l = world->forces; l; l = l->next)
  {
    E_Phys_Force *force = l->data;
    if (force && force->apply) force->apply(force);
  }
}

static void
e_phys_world_verlet_integrate(E_Phys_World *world)
{
  Evas_List *l;
  for(l = world->particles; l; l = l->next)
  {
    E_Phys_Particle *p;
    E_Phys_Point tmp;

    p = l->data;

    tmp.x = p->cur.x;
    tmp.y = p->cur.y;

    //printf("force: (%0.2f, %0.2f)\n", p->force.x, p->force.y);
    p->cur.x = (2 - world->friction) * p->cur.x - (1 - world->friction) * p->prev.x + p->force.x * world->dt * world->dt / p->m;
    p->cur.y = (2 - world->friction) * p->cur.y - (1 - world->friction) *p->prev.y + p->force.y * world->dt * world->dt / p->m;

    p->prev.x = tmp.x;
    p->prev.y = tmp.y;
  }
}


static void
e_phys_world_satisfy_constraints(E_Phys_World *world)
{
  Evas_List *l;

  for (l = world->constraints; l; l = l->next)
  {
    E_Phys_Constraint *con = l->data;
    if (con && con->apply) con->apply(con);
  }
}

