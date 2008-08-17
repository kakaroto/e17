#ifndef E_PHYS_FORCE_H
#define E_PHYS_FORCE_H

/**
 * Type definition of the general force
 */
typedef struct E_Phys_Force E_Phys_Force;

/**
 * @def E_PHYS_FORCE(force)
 * Typecasts a pointer to an E_Phys_Force pointer.
 */
#define E_PHYS_FORCE(force) ((E_Phys_Force *)(force))

/**
 * The structur for the generic force
 */
struct E_Phys_Force
{
  E_Phys_World *world;
  void (*apply) (E_Phys_Force *force);
  void (*free) (E_Phys_Force *force);
};

void e_phys_force_init(E_Phys_Force *force, E_Phys_World *world,
       void (*apply_func) (E_Phys_Force *force),
       void (*free_func) (E_Phys_Force *force));
void e_phys_force_free(E_Phys_Force *force);

/**
 * Type definition of the gravity force
 */
typedef struct E_Phys_Force_Gravity E_Phys_Force_Gravity;

/**
 * @def E_PHYS_FORCE_GRAVITY(force)
 * Typecasts a pointer to an E_Phys_Force_Gravity pointer.
 */
#define E_PHYS_FORCE_GRAVITY(force) ((E_Phys_Force_Gravity *)(force))

/**
 * The structur for the gravity force
 */
struct E_Phys_Force_Gravity
{
  E_Phys_Force force;
  float g;
};

E_Phys_Force_Gravity   *e_phys_force_gravity_add(E_Phys_World *world, float g);

/**
 * Type definition of the spring force
 */
typedef struct E_Phys_Force_Spring E_Phys_Force_Spring;

/**
 * @def E_PHYS_FORCE_SPRING(force)
 * Typecasts a pointer to an E_Phys_Force_Spring pointer.
 */
#define E_PHYS_FORCE_SPRING(force) ((E_Phys_Force_Spring *)(force))

/**
 * The structur for the spring force
 */
struct E_Phys_Force_Spring
{
  E_Phys_Force force;
  E_Phys_Particle *p1;
  E_Phys_Particle *p2;
  float len;
  float k;
  int modified;
};

E_Phys_Force_Spring    *e_phys_force_spring_add(E_Phys_World *world,
                          E_Phys_Particle *p1, E_Phys_Particle *p2, int k, int len);
E_Phys_Force_Spring    *e_phys_force_modified_spring_add(E_Phys_World *world,
                          E_Phys_Particle *p1, E_Phys_Particle *p2, int k, int len);

/**
 * Type definition of the collision force
 */
typedef struct E_Phys_Force_Collision E_Phys_Force_Collision;

/**
 * @def E_PHYS_FORCE_COLLISION(force)
 * Typecasts a pointer to an E_Phys_Force_Collision pointer.
 */
#define E_PHYS_FORCE_COLLISION(force) ((E_Phys_Force_Collision *)(force))

/**
 * The structur for the collision force
 */
struct E_Phys_Force_Collision
{
  E_Phys_Force force;
  float e; // elasticity?
};

E_Phys_Force_Collision *e_phys_force_collision_add(E_Phys_World *world);

/**
 * Type definition of the uniform force
 */
typedef struct E_Phys_Force_Uniform E_Phys_Force_Uniform;

/**
 * @def E_PHYS_FORCE_UNIFORM(force)
 * Typecasts a pointer to an E_Phys_Force_Uniform pointer.
 */
#define E_PHYS_FORCE_UNIFORM(force) ((E_Phys_Force_Uniform *)(force))

/**
 * The structur for the uniform force
 */
struct E_Phys_Force_Uniform
{
  E_Phys_Force force;
  E_Phys_Vector const_force;
  E_Phys_Vector (*force_func) (float t);
  int is_acceleration;
};

E_Phys_Force_Uniform *e_phys_force_uniform_add(E_Phys_World *world, 
                        E_Phys_Vector (*force_func) (float t), 
                        int is_acceleration);

/**
 * Type definition of the constant force
 */
typedef E_Phys_Force_Uniform E_Phys_Force_Constant;

/**
 * @def E_PHYS_FORCE_CONSTANT(force)
 * Typecasts a pointer to an E_Phys_Force_Constant pointer.
 */
#define E_PHYS_FORCE_CONSTANT(force) ((E_Phys_Force_Constant *)(force))

E_Phys_Force_Constant *e_phys_force_constant_add(E_Phys_World *world, 
                         E_Phys_Vector force, int is_acceleration);

#endif
