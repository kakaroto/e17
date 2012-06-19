#ifndef E_PHYS_PARTICLE_H
#define E_PHYS_PARTICLE_H

/**
 * Type definition of the particle direction enumeration
 */
typedef enum E_Phys_Particle_Direction E_Phys_Particle_Direction;

/**
 * The direction enumeration
 */
enum E_Phys_Particle_Direction
{
        E_PHYS_PARTICLE_DIRECTION_LEFT,
        E_PHYS_PARTICLE_DIRECTION_RIGHT,
        E_PHYS_PARTICLE_DIRECTION_ABOVE,
        E_PHYS_PARTICLE_DIRECTION_BELOW,
        E_PHYS_PARTICLE_DIRECTION_MAX
};

/**
 * The structur for the particle
 */
struct E_Phys_Particle
{
  E_Phys_World *world;
  float m;
  float w, h; // extended size
  E_Phys_Point cur;
  E_Phys_Point prev;
  E_Phys_Vector force;
  unsigned int steps;
  E_Phys_Particle *neighbor[E_PHYS_PARTICLE_DIRECTION_MAX];
};

EAPI E_Phys_Particle *e_phys_particle_add(E_Phys_World *world);
EAPI void             e_phys_particle_del(E_Phys_Particle *p);
EAPI void             e_phys_particle_free(E_Phys_Particle *p);

EAPI void             e_phys_particle_mass_set(E_Phys_Particle *p, float m);
EAPI float            e_phys_particle_mass_get(E_Phys_Particle *p);

EAPI void             e_phys_particle_move(E_Phys_Particle *p, 
                        const E_Phys_Point *pos);
EAPI void             e_phys_particle_place(E_Phys_Particle *p,
                        const E_Phys_Point *pos);
EAPI E_Phys_Point     e_phys_particle_current_position_get(E_Phys_Particle *p);
EAPI E_Phys_Point     e_phys_particle_previous_position_get(E_Phys_Particle *p);


EAPI void             e_phys_particle_velocity_set(E_Phys_Particle *p, 
                        const E_Phys_Vector *v);
EAPI E_Phys_Vector    e_phys_partivle_velocity_get(E_Phys_Particle *p);

EAPI void             e_phys_particle_force_add(E_Phys_Particle *p, 
                        const E_Phys_Vector *f);
EAPI void             e_phys_particle_force_set(E_Phys_Particle *p,
                        const E_Phys_Vector *f);
EAPI E_Phys_Vector    e_phys_particle_force_get(E_Phys_Particle *p);

EAPI void             e_phys_particle_size_set(E_Phys_Particle *p, float w, float h);
EAPI void             e_phys_particle_size_get(E_Phys_Particle *p, 
                        float *w, float *h);

EAPI E_Phys_Particle *e_phys_particle_map_insert(E_Phys_Particle *head,
                       E_Phys_Particle *p);

#endif
