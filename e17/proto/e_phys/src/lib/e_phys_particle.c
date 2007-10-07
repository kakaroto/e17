#include "E_Phys.h"

/**
 * @param world - the world to work with
 * @param mass - The mass of the new particle
 * @param x - The x coordinate of the location
 * @param y - The y coordinate of the location
 * @param v_x - The x component of the velocity (FIXME: what is the unit?)
 * @param v_y - The y component of the velocity
 * @return Returns the new created particle
 * @brief Creates a new particle.
 */
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

/**
 * @param p - the particle to work with
 * @param w - The new width of the particle
 * @param h - The new height of the particle
 * @return Returns no value.
 * @brief Set the new size of the particle
 */
void
e_phys_particle_size_set(E_Phys_Particle *p, float w, float h)
{
  p->w = w;
  p->h = h;
}

/**
 * @param p - the particle to delete
 * @return Returns no value.
 * @brief Remove the given particle from its world and free it
 */
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

/**
 * Do not use this function, as long you really know what you are doning.
 * Use e_phys_particle_del() instead.
 *
 * @param p - the particle to free
 * @return Returns no value.
 * @brief Free the given particle.
 */
void
e_phys_particle_free(E_Phys_Particle *p)
{
  if (p) free(p);
}
