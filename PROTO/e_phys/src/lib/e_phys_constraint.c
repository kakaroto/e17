#include "E_Phys.h"

static void e_phys_constraint_boundary_apply(E_Phys_Constraint *con);
static void e_phys_constraint_stick_apply(E_Phys_Constraint *con);
static void e_phys_constraint_anchor_apply(E_Phys_Constraint *con);

/* (( Generic constraint functions )) */
void
e_phys_constraint_init(E_Phys_Constraint *con, E_Phys_World *world, void (*apply_func) (E_Phys_Constraint *con), void (*free_func) (E_Phys_Constraint *con))
{
  con->world = world;
  if (world) world->constraints = eina_list_append(world->constraints, con);

  con->apply = apply_func;
  if (free_func)
    con->free = free_func;
  else
    con->free = e_phys_constraint_free;
}

void
e_phys_constraint_del(E_Phys_Constraint *con)
{
  if (!con) return;
  if (con->world) 
    con->world->constraints = eina_list_remove(con->world->constraints, con);
  con->free(con);
}

void
e_phys_constraint_free(E_Phys_Constraint *con)
{
  if (con) free(con);
}

/* (( Boundary Constraint )) */

E_Phys_Constraint_Boundary *
e_phys_constraint_boundary_add(E_Phys_World *world)
{
   E_Phys_Constraint_Boundary *con = calloc(1, sizeof(E_Phys_Constraint_Boundary));
  e_phys_constraint_init(E_PHYS_CONSTRAINT(con), world,
    e_phys_constraint_boundary_apply, NULL);

  return con;
}

void
e_phys_constraint_boundary_exclude(E_Phys_Constraint_Boundary *con, E_Phys_Particle *p)
{
  if (!con) return;
  con->exclusions = eina_list_append(con->exclusions, p);
}

static void
e_phys_constraint_boundary_apply(E_Phys_Constraint *con)
{
  E_Phys_Constraint_Boundary *bound = (E_Phys_Constraint_Boundary *)con;
  E_Phys_World *world = con->world;

  Eina_List *l;
  // keep particles inside world boundaries
  for (l = world->particles; l; l = l->next)
  {
    E_Phys_Particle *p;
    float hw, hh;
    E_Phys_Vector v;

    p = l->data;

    if (eina_list_data_find(bound->exclusions, p)) continue;

    hw = p->w / 2;
    hh = p->h / 2;

    v.x = p->cur.x - p->prev.x;
    v.y = p->cur.y - p->prev.y;

    if (p->cur.x < hw) {
      p->cur.x = hw + (bound->e * (hw - p->cur.x));
      p->prev.x = hw + (bound->e * (hw - p->prev.x));
    } else if (p->cur.x > world->w - hw) {
      p->cur.x = (world->w - hw) + (bound->e * ((world->w - hw) - p->cur.x));
      p->prev.x = (world->w - hw) + (bound->e * ((world->w - hw) - p->cur.x));
    }

    if (p->cur.y - hh < 0) {
      p->cur.y = hh + (bound->e * (hh - p->cur.y));
      p->prev.y = hh + (bound->e * (hh - p->prev.y));
    } else if (p->cur.y > world->h - hh) {
      p->cur.y = (world->h - hh) + (bound->e * ((world->h - hh) - p->cur.y));
      p->prev.y = (world->h - hh) + (bound->e * ((world->h - hh) - p->cur.y));
    }

  }
}

/* (( Stick Constraint )) */

E_Phys_Constraint_Stick *
e_phys_constraint_stick_add(E_Phys_Particle *p1, E_Phys_Particle *p2, int len)
{
  E_Phys_Constraint_Stick *con = malloc(sizeof(E_Phys_Constraint_Stick));
  e_phys_constraint_init(E_PHYS_CONSTRAINT(con), p1->world, e_phys_constraint_stick_apply, NULL);

  con->p1 = p1;
  con->p2 = p2;
  con->len = len;

  return con;
}

static void
e_phys_constraint_stick_apply(E_Phys_Constraint *con)
{
  E_Phys_Constraint_Stick *st = (E_Phys_Constraint_Stick *)con;
  E_Phys_Vector d, u;
  double r;

  d.x = st->p2->cur.x - st->p1->cur.x;
  d.y = st->p2->cur.y - st->p1->cur.y;
  r = sqrt(d.x*d.x + d.y*d.y);
  
  if (r == 0) r = 1;
  u.x = d.x / r * (r - st->len);
  u.y = d.y / r * (r - st->len);

  st->p1->cur.x += u.x / 2;
  st->p1->cur.y += u.y / 2;
  st->p2->cur.x -= u.x / 2;
  st->p2->cur.y -= u.y / 2;
}

/* (( Anchor Constraint )) */
static void
e_phys_constraint_anchor_apply(E_Phys_Constraint *con)
{
  E_Phys_Constraint_Anchor *a = (E_Phys_Constraint_Anchor *)con;

  if (a->p)
  {
    a->p->cur.x = a->pos.x;
    a->p->cur.y = a->pos.y;
  }
}

E_Phys_Constraint_Anchor *
e_phys_constraint_anchor_add(E_Phys_Particle *p, float x, float y)
{
  E_Phys_Constraint_Anchor *a = malloc(sizeof(E_Phys_Constraint_Anchor));
  e_phys_constraint_init(E_PHYS_CONSTRAINT(a), p->world, e_phys_constraint_anchor_apply, NULL);

  a->p = p;
  a->pos.x = x;
  a->pos.y = y;

  return a;
}


