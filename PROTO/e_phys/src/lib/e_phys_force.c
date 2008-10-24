#include "E_Phys.h"

static void e_phys_force_collision_helper(E_Phys_Particle *p1,
              E_Phys_Particle *p2, int axis);
static void e_phys_force_collision_apply(E_Phys_Force *force);
static void e_phys_force_gravity_apply(E_Phys_Force *force);
static void e_phys_force_spring_apply(E_Phys_Force *force);
static void e_phys_force_uniform_apply(E_Phys_Force *force);

/**
 * Initialize an E_Phys_Force struct
 *
 * All forces should inherit from E_Phys_Force, and then call this function
 * to set up the common parameters
 *
 * @param force - The force to initialize
 * @param world - The world the force lives in
 * @param apply_func - A function that applies the force
 * @param free_func - A function to free the force struct. If NULL is passed,
 *                    a generic free function will be used. This is sufficient
 *                    if there are no allocated fields in the inheriting
 *                    structure.
 */
void
e_phys_force_init(E_Phys_Force *force, E_Phys_World *world, void (*apply_func) (E_Phys_Force *force), void (*free_func) (E_Phys_Force *force))
{
  force->world = world;
  if (world) world->forces = eina_list_append(world->forces, force);

  force->apply = apply_func;
  if (free_func)
    force->free = free_func;
  else
    force->free = e_phys_force_free;
}

/**
 * Free an E_Phys_Force struct
 */
void
e_phys_force_free(E_Phys_Force *force)
{
  if (force) free(force);
}

/**
 * Add particle-particle collisions to the world.
 * This will cause any particles with size set to bounce off of eachother when
 * they attempt to occupy the same space.
 *
 * A few qualifications (possible fixmes):
 *  - Assumes rectangular particles with no friction on the colliding faces
 *  - Only checks for collision at end of timeslice. Quick moving particles can
 *    pass over each other.
 *  - It is possible, after all forces are applied for particles to end up
 *    overlapping.
 *   
 * @param world
 */
E_Phys_Force_Collision *
e_phys_force_collision_add(E_Phys_World *world)
{
  E_Phys_Force_Collision *f = malloc(sizeof(E_Phys_Force_Collision));

  e_phys_force_init(E_PHYS_FORCE(f), world, e_phys_force_collision_apply, NULL);
  f->e = 1; // default to perfectly elastic collisions
  return f;
}

static void
e_phys_force_collision_helper(E_Phys_Particle *p1, E_Phys_Particle *p2, int axis)
{
  E_Phys_Vector V, u1, u2, v1, v2;
  float N;
  E_Phys_Particle *tmp;

  //printf("Collide!\n");

  if ((!axis && p1->cur.y > p2->cur.y) || (axis && p1->cur.x > p2->cur.x))
  {
    tmp = p1;
    p1 = p2;
    p2 = tmp;
  }

  v1.x = p1->cur.x - p1->prev.x;
  v1.y = p1->cur.y - p1->prev.y;
  v2.x = p2->cur.x - p2->prev.x;
  v2.y = p2->cur.y - p2->prev.y;

  V.x = (p1->m * v1.x + p2->m * v2.x) / (p1->m + p2->m);
  V.y = (p1->m * v1.y + p2->m * v2.y) / (p1->m + p2->m);

  u1.x = v1.x - V.x;
  u1.y = v1.y - V.y;
  u2.x = v2.x - V.x;
  u2.y = v2.y - V.y;

  // vertical collision (sides parallel to x-axis hitting eachother
  if (axis)
  {
    N = p1->m * u1.x * u1.x + p2->m * u2.x * u2.x;

    u2.x = sqrt(p1->m * N / (double)(p2->m * (p1->m + p2->m)));
    u1.x = -((double)p2->m / p1->m) * u2.x;

  }
  // horizontal collision
  else
  {
    N = p1->m * u1.y * u1.y + p2->m * u2.y * u2.y;

    u2.y = sqrt(p1->m * N / (double)(p2->m * (p1->m + p2->m)));
    u1.y = -((double)p2->m / p1->m) * u2.y;
  }

  v1.x = u1.x + V.x;
  v1.y = u1.y + V.y;
  v2.x = u2.x + V.x;
  v2.y = u2.y + V.y;

  p1->prev.x = p1->cur.x - v1.x;
  p1->prev.y = p1->cur.y - v1.y;
  p2->prev.x = p2->cur.x - v2.x;
  p2->prev.y = p2->cur.y - v2.y;
}

/* XXX honor elasticity parameter */
static void
e_phys_force_collision_apply(E_Phys_Force *force)
{
  Eina_List *l, *l2;
  float x11, x12, x21, x22;
  float y11, y12, y21, y22;
  float dx, dy;
  for (l = force->world->particles; l; l = l->next)
  {
    E_Phys_Particle *p1;
    p1 = l->data;
    if (p1->w == 0 && p1->h == 0) continue;

    x11 = p1->cur.x - p1->w / 2;
    x12 = p1->cur.x + p1->w / 2;
    y11 = p1->cur.y - p1->w / 2;
    y12 = p1->cur.y + p1->w / 2;

    for (l2 = l->next; l2; l2 = l2->next)
    {
      E_Phys_Particle *p2;
      p2 = l2->data;
      if (p2->w == 0 && p2->h == 0) continue;

      x21 = p2->cur.x - p2->w / 2;
      x22 = p2->cur.x + p2->w / 2;
      y21 = p2->cur.y - p2->w / 2;
      y22 = p2->cur.y + p2->w / 2;

      if (x11 > x22) continue;
      if (x12 < x21) continue;

      if (y11 > y22) continue;
      if (y12 < y21) continue;

//      printf("x [%.2f - %.2f] :: [%.2f - %.2f]\n", x11, x12, x21, x22);
//      printf("y [%.2f - %.2f] :: [%.2f - %.2f]\n", y11, y12, y21, y22);
      // otherwise we have a collision. decide which axes hit based on amount of overlap.
      dx = abs((x11 > x21 ? x11 : x21) - (x12 < x22 ? x12 : x22));
      dy = abs((y11 > y21 ? y11 : y21) - (y12 < y22 ? y12 : y22));

      e_phys_force_collision_helper(p1, p2, dx < dy ? 1 : 0);
    }
  }
}

/**
 * Add particle-particle gravity to a world with gravitational constant g.
 * 
 * Applies F=-g*m1*m2/r^2 to all pairs of particles in the supplied world.
 * Here m1 and m2 are the masses of the two particles, and r is the distance
 * separating them.
 * 
 * @param world
 * @param g - Gravitational constant
 */
E_Phys_Force_Gravity *
e_phys_force_gravity_add(E_Phys_World *world, float g)
{
  E_Phys_Force_Gravity *f = malloc(sizeof(E_Phys_Force_Gravity));

  e_phys_force_init(E_PHYS_FORCE(f), world, e_phys_force_gravity_apply, NULL);
  f->g = g; // default gravitational constant XXX pick units and use proper constant here
  return f;
}

// gravitational force between all particles present
static void
e_phys_force_gravity_apply(E_Phys_Force *force)
{
  E_Phys_Force_Gravity *grav = (E_Phys_Force_Gravity *)force;

  Eina_List *l, *l2;

  if (!force || !force->world) return;

  // Gravity!
  for (l = force->world->particles; l; l = l->next)
  {
    E_Phys_Particle *p1;
    p1 = l->data;
    for (l2 = l->next; l2; l2 = l2->next)
    {
      E_Phys_Particle *p2;
      E_Phys_Vector d, u;
      float r, f;

      p2 = l2->data;
      d.x = p2->cur.x - p1->cur.x;
      d.y = p2->cur.y - p1->cur.y;
      //printf("x: %f,%f\n", p2->cur.x, p1->cur.x);
      //printf("y: %f,%f\n", p2->cur.y, p1->cur.y);
      r = sqrt(d.x*d.x + d.y*d.y);
      if (r < 10) r = 10;
      u.x = d.x / r;
      u.y = d.y / r;

      //printf("r: %f, m1: %d, m2: %d\n", r, p1->m, p2->m);

      f =  grav->g * p1->m * p2->m / (r * r);
     // printf("f: %f\n", f);
      p1->force.x += u.x * f;
      p1->force.y += u.y * f;
      p2->force.x -= u.x * f;
      p2->force.y -= u.y * f;
    }
  }
}

/**
 * Add a spring between two particles <p1> and <p2> with spring constant <k> 
 * and natural length <len>.
 *
 * F = k * (len - r)
 * positive F directed away from the center point of the particles
 * negative F is directed towards
 */
E_Phys_Force_Spring *
e_phys_force_spring_add(E_Phys_World *world, E_Phys_Particle *p1, E_Phys_Particle *p2, int k, int len)
{
  E_Phys_Force_Spring *f = malloc(sizeof(E_Phys_Force_Spring));

  e_phys_force_init(E_PHYS_FORCE(f), world, e_phys_force_spring_apply, NULL);
  f->p1 = p1;
  f->p2 = p2;
  f->k = k; 
  f->len = len;
  return f;
}

/**
 * Add a modified spring force between two particles.
 *
 * F = (k * (len - r)) / r
 * e.g. 1/r * the standard spring force
 */
E_Phys_Force_Spring *
e_phys_force_modified_spring_add(E_Phys_World *world, E_Phys_Particle *p1, E_Phys_Particle *p2, int k, int len)
{
  E_Phys_Force_Spring *f = malloc(sizeof(E_Phys_Force_Spring));

  e_phys_force_init(E_PHYS_FORCE(f), world, e_phys_force_spring_apply, NULL);
  f->p1 = p1;
  f->p2 = p2;
  f->k = k; 
  f->len = len;
  f->modified = 1;
  return f;
}

static void
e_phys_force_spring_apply(E_Phys_Force *force)
{
  E_Phys_Force_Spring *sp = (E_Phys_Force_Spring *)force;

  E_Phys_Vector d, u, f;
  float r;
  float diff;

  if (!sp->p1 || !sp->p2) return;

  d.x = sp->p2->cur.x - sp->p1->cur.x;
  d.y = sp->p2->cur.y - sp->p1->cur.y;
  r = sqrt(d.x*d.x + d.y*d.y);

  if (r == 0) r = 1;
  u.x = d.x / r;
  u.y = d.y / r;

  if (sp->modified)
    diff = (r - sp->len) / r;
  else
    diff = (r - sp->len);
  f.x = u.x * sp->k * diff;
  f.y = u.y * sp->k * diff;
  //printf("force: %f, %f\n", f.x, f.y);

  sp->p1->force.x += f.x;
  sp->p1->force.y += f.y;
  sp->p2->force.x -= f.x;
  sp->p2->force.y -= f.y;
}

/**
 * Add a constant force or acceleration to all particles in the given world
 *
 * @param force - A vector specifying the force
 * @param is_accel - If 1, treat 'force' as an acceleration instead of a force
 */
E_Phys_Force_Constant *
e_phys_force_constant_add(E_Phys_World *world, E_Phys_Vector force, int is_acceleration) 
{
  E_Phys_Force_Constant *f = malloc(sizeof(E_Phys_Force_Uniform));
  e_phys_force_init(E_PHYS_FORCE(f), world, e_phys_force_uniform_apply, NULL);
  f->const_force = force;
  f->force_func = NULL;
  f->is_acceleration = is_acceleration;

  return f;
}


/**
 * Add a uniform (but time dependant) force or acceleration to all particles in 
 * the given world
 *
 * @param force_func - A function of time returning the current force 
 * @param is_accel - If 1, treat the return value of force_func as an
 *                   acceleration instead of a force
 */
E_Phys_Force_Uniform *
e_phys_force_uniform_add(E_Phys_World *world, E_Phys_Vector (*force_func) (float t), int is_acceleration)
{
  E_Phys_Force_Uniform *f = malloc(sizeof(E_Phys_Force_Uniform));
  e_phys_force_init(E_PHYS_FORCE(f), world, e_phys_force_uniform_apply, NULL);
  f->force_func = force_func;
  f->is_acceleration = is_acceleration;

  return f;
}

static void
e_phys_force_uniform_apply(E_Phys_Force *force)
{
  E_Phys_Force_Uniform *uni = (E_Phys_Force_Uniform *)force;
  E_Phys_Vector f;

  Eina_List *l;

  if (!force || !force->world) return;

  if (uni->force_func)
    f = uni->force_func(force->world->time);
  else
    f = uni->const_force;

  for (l = force->world->particles; l; l = l->next)
  {
    E_Phys_Particle *p;
    p = l->data;
    if (uni->is_acceleration)
    {
      p->force.x += f.x * p->m;
      p->force.y += f.y * p->m;
    }
    else
    {
      p->force.x += f.x;
      p->force.y += f.y;
    }
  }
}
