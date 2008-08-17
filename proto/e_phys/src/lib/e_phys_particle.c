#include "E_Phys.h"

/**
 * @param world - the world to work with
 * @return Returns the new created particle
 * @brief Creates a new particle.
 */
E_Phys_Particle *
e_phys_particle_add(E_Phys_World *world)
{
  E_Phys_Particle *p;
  p = calloc(1, sizeof(E_Phys_Particle));
  p->world = world;
  p->m = 1.0;

  world->particles = evas_list_append(world->particles, p);
  return p;
}

/**
 * @param p - the particles to work with
 * @param mass - The mass of the new particle
 * @return Returns no value.
 * @brief Set the mass of the particle.
 */
void
e_phys_particle_mass_set(E_Phys_Particle *p, float mass)
{
  p->m = mass;
}

/**
 * @param p - the particles to work with
 * @return Returns the mass of the particle.
 * @brief Retrieve the mass of the particle.
 */
float
e_phys_particle_mass_get(E_Phys_Particle *p)
{
  return p->m;
}

/**
 * @param p - the particle to move
 * @param pos - The position to move the particle
 * @return Returns no value.
 * @brief Move the particle to the new postion
 */
void
e_phys_particle_move(E_Phys_Particle *p, const E_Phys_Point *pos)
{
  p->cur = *pos;
}

/**
 * @param p - the particle to place
 * @param pos - The position to place the particle
 * @return Returns no value.
 * @brief Place the particle to the new postion
 */
void
e_phys_particle_place(E_Phys_Particle *p, const E_Phys_Point *pos)
{
  p->cur = *pos;
  p->prev = *pos;
}

/**
 * @param p - the particle to work with
 * @return Returns the current position of the particle
 * @brief Retrieve the current position of the particle
 */
E_Phys_Point
e_phys_particle_current_position_get(E_Phys_Particle *p)
{
  return p->cur;
}

/**
 * @param p - the particle to work with
 * @return Returns the previous position of the particle
 * @brief Retrieve the previous position of the particle
 */
E_Phys_Point
e_phys_particle_previous_position_get(E_Phys_Particle *p)
{
  return p->prev;
}

/**
 * @param p - the particle to work with
 * @param vel - The new velocity of the particle (FIXME: what is the unit?)
 * @return Returns no value.
 * @brief Set a new velocity of the particle
 */
void
e_phys_particle_velocity_set(E_Phys_Particle *p, const E_Phys_Vector *vel)
{
  p->prev.x = p->cur.x - vel->x;
  p->prev.y = p->cur.y - vel->y;
}

/**
 * @param p - the particle to work with
 * @return Returns the velocity of the particle
 * @brief Retrieve the velocity of the particle
 */
E_Phys_Vector
e_phys_particle_velocity_get(E_Phys_Particle *p)
{
  E_Phys_Vector v = {p->cur.x - p->prev.x, p->cur.y - p->prev.y};
  return v;
}

/**
 * @param p - the particle to work with
 * @param f - The force to add to the particle
 * @return Returns no value.
 * @brief Add a force to the particle
 */
void
e_phys_particle_force_add(E_Phys_Particle *p, const E_Phys_Vector *f)
{
  p->force.x += f->x;
  p->force.y += f->y;
}

/**
 * @param p - the particle to work with
 * @param f - The new force of the particle
 * @return Returns no value.
 * @brief Set the new force of the particle
 */
void
e_phys_particle_force_set(E_Phys_Particle *p, const E_Phys_Vector *f)
{
  p->force = *f;
}

/**
 * @param p - the particle to work with
 * @return Returns the force that is applied on the particle.
 * @brief Retrieve the force
 */
E_Phys_Vector
e_phys_particle_force_get(E_Phys_Particle *p)
{
  return p->force;
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
 * @param p - the particle to work with
 * @param w - The pointer to save the width of the particle
 * @param h - The pointer to save the height of the particle
 * @return Returns no value.
 * @brief Get the size of the particle
 */
void
e_phys_particle_size_get(E_Phys_Particle *p, float *w, float *h)
{
  if (w) *w = p->w;
  if (h) *h = p->h;
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

