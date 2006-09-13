#include "E_Phys.h"

E_Phys_Particle *
e_phys_particle_add(E_Phys_World *world, float mass, float x, float y, float v_x, float v_y)
{
  E_Phys_Particle *p;
  p = calloc(1, sizeof(E_Phys_Particle));
  p->world = world;

  p->m = mass;
  p->cur.x = x;
  p->cur.y = y;
  p->prev.x = x - v_x;
  p->prev.y = y - v_y;

  world->particles = evas_list_append(world->particles, p);
  return p;
}

void
e_phys_particle_size_set(E_Phys_Particle *p, float w, float h)
{
  p->w = w;
  p->h = h;
}

void
e_phys_particle_del(E_Phys_Particle *p)
{
  if (p)
  {
    if (p->world)
      p->world->particles = evas_list_remove(p->world->particles, p);
    e_phys_particle_free(p);
  }
}

void
e_phys_particle_free(E_Phys_Particle *p)
{
  if (p) free(p);
}
