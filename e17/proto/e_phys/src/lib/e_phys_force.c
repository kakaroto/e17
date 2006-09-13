#include "E_Phys.h"

static void e_phys_force_collision_helper(E_Phys_Particle *p1,
              E_Phys_Particle *p2, int axis);
static void e_phys_force_collision_apply(E_Phys_Force *force);
static void e_phys_force_gravity_apply(E_Phys_Force *force);
static void e_phys_force_spring_apply(E_Phys_Force *force);

void
e_phys_force_init(E_Phys_Force *force, E_Phys_World *world, void (*apply_func) (E_Phys_Force *force), void (*free_func) (E_Phys_Force *force))
{
  force->world = world;
  force->world = world;
  if (world) world->forces = evas_list_append(world->forces, force);

  force->apply = apply_func;
  if (free_func)
    force->free = free_func;
  else
    force->free = e_phys_force_free;
}

void
e_phys_force_free(E_Phys_Force *force)
{
  if (force) free(force);
}

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

/**
 * Apply collisions between particles
 * XXX honor elasticity param (coll->e)
 */
static void
e_phys_force_collision_apply(E_Phys_Force *force)
{
  E_Phys_Force_Collision *coll = (E_Phys_Force_Collision *)force;
  Evas_List *l, *l2;
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

  Evas_List *l, *l2;

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
 * Add a spring between two particles <p1> and <p2> with spring constant <k>.
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
