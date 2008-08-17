#ifndef E_PHYS_WORLD_H
#define E_PHYS_WORLD_H

/**
 * Type definition of the world
 */
typedef struct E_Phys_World E_Phys_World;

/**
 * @def E_PHYS_WORLD(world)
 * Typecasts a pointer to an E_Phys_World pointer.
 */
#define E_PHYS_WORLD(world) ((E_Phys_World *)(world))

/**
 * Type definition of the particle
 */
typedef struct E_Phys_Particle E_Phys_Particle;

/**
 * @def E_PHYS_PARTICLE(p)
 * Typecasts a pointer to an E_Phys_Particle pointer.
 */
#define E_PHYS_PARTICLE(p) ((E_Phys_Particle *)(p))

/**
 * The structur for the world
 */
struct E_Phys_World
{
  float w, h;

  Evas_List *particles;
  Evas_List *forces;
  Evas_List *constraints;

  Ecore_Timer *timer;

  double time;

  double dt;
  double elapsed;
  int constraint_iter;
  float friction;

  void (*update_func) (void *data, E_Phys_World *world);
  void *update_data;
};

E_Phys_World    *e_phys_world_add(void);
void             e_phys_world_free(E_Phys_World *world);
void             e_phys_world_go(E_Phys_World *world);
void             e_phys_world_stop(E_Phys_World *world);
void             e_phys_world_update_func_set(E_Phys_World *world,
                   void (*func) (void *data, E_Phys_World *world),
                   void *data);
void             e_phys_world_size_set(E_Phys_World *world, float w, float h);
E_Phys_Particle *e_phys_world_nearest_particle(E_Phys_World *world,
                   int x, int y);

#endif
