#ifndef E_PHYS_CONSTRAINT_H
#define E_PHYS_CONSTRAINT_H

/**
 * Type definition of the general constraint
 */
typedef struct E_Phys_Constraint E_Phys_Constraint;

/**
 * @def E_PHYS_CONSTRAINT(con)
 * Typecasts a pointer to an E_Phys_Constraint pointer.
 */
#define E_PHYS_CONSTRAINT(con) ((E_Phys_Constraint *)(con))

/**
 * The structur for the general constraint
 */
struct E_Phys_Constraint
{
  E_Phys_World *world;
  void (*apply) (E_Phys_Constraint *con);
  void (*free) (E_Phys_Constraint *con);
};

void e_phys_constraint_init(E_Phys_Constraint *con, E_Phys_World *world,
       void (*apply_func) (E_Phys_Constraint *con),
       void (*free_func) (E_Phys_Constraint *con));
void e_phys_constraint_del(E_Phys_Constraint *con);
void e_phys_constraint_free(E_Phys_Constraint *con);

/**
 * Type definition of the boundary constraint
 */
typedef struct E_Phys_Constraint_Boundary E_Phys_Constraint_Boundary;

/**
 * @def E_PHYS_CONSTRAINT_BOUNDARY(c)
 * Typecasts a pointer to an E_Phys_Constraint_Boundary pointer.
 */
#define E_PHYS_CONSTRAINT_BOUNDARY(p) ((E_Phys_Constraint_Boundary *)(p))

/**
 * The structur for the boundary constraint
 */
struct E_Phys_Constraint_Boundary
{
  E_Phys_Constraint con;
  float e; // elasticity (0 fully inelastic -> 1 fully elastic)
  Evas_List *exclusions;
};

E_Phys_Constraint_Boundary *e_phys_constraint_boundary_add(E_Phys_World *world);

/**
 * Type definition of the stick constraint
 */
typedef struct E_Phys_Constraint_Stick E_Phys_Constraint_Stick;

/**
 * @def E_PHYS_CONSTRAINT_STICK(con)
 * Typecasts a pointer to an E_Phys_Constraint_Stick pointer.
 */
#define E_PHYS_CONSTRAINT_STICK(con) ((E_Phys_Constraint_Stick *)(con))

/**
 * The structur for the stick constraint
 */
struct E_Phys_Constraint_Stick
{
  E_Phys_Constraint con;
  E_Phys_Particle *p1;
  E_Phys_Particle *p2;
  float len;
};

E_Phys_Constraint_Stick    *e_phys_constraint_stick_add(
                              E_Phys_Particle *p1,
                              E_Phys_Particle *p2,
                              int len);

/**
 * Type definition of the anchor constraint
 */
typedef struct E_Phys_Constraint_Anchor E_Phys_Constraint_Anchor;

/**
 * @def E_PHYS_CONSTRAINT_ANCHOR(con)
 * Typecasts a pointer to an E_Phys_Constraint_Anchor pointer.
 */
#define E_PHYS_CONSTRAINT_ANCHOR(con) ((E_Phys_Constraint_Anchor *)(con))

/**
 * The structur for the anchor constraint
 */
struct E_Phys_Constraint_Anchor
{
  E_Phys_Constraint con;
  E_Phys_Particle *p;
  E_Phys_Point pos;
};

E_Phys_Constraint_Anchor *e_phys_constraint_anchor_add(E_Phys_Particle *p, 
                            float x, float y);

#endif
