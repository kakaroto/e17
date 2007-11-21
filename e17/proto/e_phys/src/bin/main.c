#include "demo.h"
#include "string.h"

#define INIT_W 600
#define INIT_H 600

static void _cb_resize(Ecore_Evas *ee);

int
main (int argc, char **argv)
{
  App *app;

  if (!evas_init())
  {
    fprintf(stderr, "Error: Can't init evas.");
    return 1;
  }

  if (!ecore_init())
  {
    fprintf(stderr, "Error: Can't init ecore.");
    return 1;
  }

  if (!ecore_evas_init())
  {
    fprintf(stderr, "Error: Can't init ecore evas.");
    return 1;
  }


  app = calloc(1, sizeof(App));
  if (!app)
  {
    fprintf(stderr, "Error: Out of memory?.");
    return 1;
  }

  app->ee = ecore_evas_software_x11_new(0, 0, 0, 0, INIT_W, INIT_H);
  if (!app->ee) 
  {
    fprintf(stderr, "Error: Can't create ecore evas.");
    return 1;
  }
  app->evas = ecore_evas_get(app->ee);
  ecore_evas_data_set(app->ee, "app", app);
  ecore_evas_callback_resize_set(app->ee, _cb_resize);

  Evas_Object *o;
  o = evas_object_rectangle_add(app->evas);
  evas_object_name_set(o, "bg");
  evas_object_resize(o, INIT_W, INIT_H);
  evas_object_show(o);
  
  setup(app, argc, argv);

  ecore_evas_show(app->ee);
  ecore_main_loop_begin();

  ecore_evas_shutdown();
  ecore_shutdown();
  evas_shutdown();

  return 0;
}

static void 
_cb_resize(Ecore_Evas *ee)
{
  App *app;
  int w, h;
  app = ecore_evas_data_get(ee, "app");

  ecore_evas_geometry_get(ee, NULL, NULL, &w, &h);
  evas_object_resize(evas_object_name_find(app->evas, "bg"), w, h);
  e_phys_world_size_set(app->world, w, h);
}

int
rand_range(int from, int to)
{
  return from + (float)rand() / RAND_MAX * (to - from);
}

float
rand_range_f(float from, float to)
{
  return from + (float)rand() / RAND_MAX * (to - from);
}

void
init_rope(E_Phys_World *world) 
{
  int i, j;
  int num_p = 100;
  E_Phys_Particle *prev_p = NULL, *first_p = NULL;
 
  int anchored = 0;
  world->friction = .01;

  for (j = 0; j < 2; j++)
  {
    prev_p = NULL;
  for (i = 0; i < num_p; i++)
  {
    E_Phys_Particle *p;
    E_Phys_Point pos;

    pos.x = rand_range(50, INIT_W - 50);
    pos.y = rand_range(50, INIT_W - 50);

    p = e_phys_particle_add(world);
    e_phys_particle_place(p, &pos);
    e_phys_particle_size_set(p, 4.0, 4.0);

    if (prev_p)
      e_phys_constraint_stick_add(prev_p, p, 5);
    else
      first_p = p;

    prev_p = p;
  }

  if (anchored)
  {
    e_phys_constraint_anchor_add(first_p, 10, 10);
    e_phys_constraint_anchor_add(prev_p, world->w - 1, world->h - 1);
  }
  }

//  e_phys_force_gravity_add(world, 1e2);
  e_phys_force_collision_add(world);
  e_phys_constraint_boundary_add(world);
  world->constraint_iter = 1000;
  world->friction = 0.01;
}

void
init_mesh(E_Phys_World *world) 
{
  int i, j;
  E_Phys_Particle **prev_row = NULL;
  E_Phys_Particle **cur_row = NULL;
  E_Phys_Particle *prev_p = NULL;

  int mesh_size = 200;
  int num_p = 20;

  world->friction = .01;

  for (j = 0; j < num_p; j++)
  {
    prev_p = NULL;
    prev_row = cur_row;
    cur_row = malloc(num_p * sizeof(E_Phys_Particle *));
    for (i = 0; i < num_p; i++)
    {
      E_Phys_Particle *p;

      E_Phys_Point pos0;


      pos0.x = 50 + mesh_size * (float)i / num_p;
      pos0.y = 50 + mesh_size * (float)j / num_p;

      p = e_phys_particle_add(world);
      e_phys_particle_place(p, &pos0);
      e_phys_particle_size_set(p, 4, 4);

      if (i > 0)
      {
        e_phys_constraint_stick_add(cur_row[i-1], p, mesh_size / num_p);
        if (prev_row)
          e_phys_constraint_stick_add(prev_row[i-1], p, sqrt(2) * mesh_size / num_p);
      }

      if (prev_row)
      {
        e_phys_constraint_stick_add(prev_row[i], p, mesh_size / num_p);
        if (i < num_p - 1)
          e_phys_constraint_stick_add(prev_row[i+1], p, sqrt(2) * mesh_size / num_p);
      }

      cur_row[i] = p;
    }

  }

//  e_phys_force_gravity_add(world, 1e2);
  //e_phys_force_collision_add(world);
  e_phys_constraint_boundary_add(world);
  world->constraint_iter = 100;
  world->friction = 0.0001;
}

void
init_gravity_test(E_Phys_World *world)
{
  E_Phys_Particle *p;
  int i, n = 25;
  float w, h;
  E_Phys_Point pos;

  w = (float)world->w / n;
  h = (float)world->h / n;

  for (i = 0; i < n*n; i++)
  {
    pos.x = w * (i % n);
    pos.y = h * (i / n);

    p = e_phys_particle_add(world);
    e_phys_particle_mass_set(p, 5.0);
    e_phys_particle_place(p, &pos);
  }

  world->constraint_iter = 1;
  e_phys_force_collision_add(world);
  e_phys_force_gravity_add(world, 1e4);
}

void
init_collision_test(E_Phys_World *world)
{
  E_Phys_Particle *p;
  E_Phys_Particle *center;
  E_Phys_Point pos;

  int i;
  int num_p = 300;

  world->friction = .01;
  world->constraint_iter = 1000;
  center = e_phys_particle_add(world);
  e_phys_particle_mass_set(center, 1000.0);
  pos.x = pos.y = 300;
  e_phys_particle_place(center, &pos);
  e_phys_particle_size_set(center, 5, 5);

  for (i = 0; i < num_p; i++)
  {
    pos.x = rand_range(100, 500);
    pos.y = rand_range(100, 500);

    p = e_phys_particle_add(world);
    e_phys_particle_place(p, &pos);
    e_phys_particle_size_set(p, 5, 5);

    e_phys_force_modified_spring_add(world, p, center, 1000, 200);
  }
  e_phys_constraint_boundary_add(world);
  e_phys_force_collision_add(world);
}

void
init_boundary_test(E_Phys_World *world)
{
  E_Phys_Constraint_Boundary *cb;
  E_Phys_Particle *p;

  int i, num = 50;

  for (i = 0; i < num; i++)
  {
    E_Phys_Point pos = {rand_range(50, 550), rand_range(50, 550)};
    E_Phys_Vector vel = {rand_range(0, 20), rand_range(0, 20)};

    p = e_phys_particle_add(world);
    e_phys_particle_move(p, &pos);
    e_phys_particle_velocity_set(p, &vel);
    e_phys_particle_size_set(p, 20, 20);
  }

  e_phys_force_collision_add(world);
  cb = e_phys_constraint_boundary_add(world);
  cb->e = .5;
}

void
init_evas_objs(App *app, int use_image)
{
  Evas_List *l;
  Evas_Object *o;
  int i, num_p;

  num_p = 100;

  if (!app->world) return;

  for (l = app->world->particles, i = 0; l; l = l->next, i++)
  {
    E_Phys_Particle *p;
    p = l->data;
    if (use_image)
    {
      o = evas_object_image_add(app->evas);
      evas_object_image_file_set(o, "flake.png", "");
      if (evas_object_image_load_error_get(o))
      {
        o = evas_object_rectangle_add(app->evas);
        evas_object_color_set(o, 0, 0, 0, 50 + ((float)i / num_p) * 205);
      } else {
        evas_object_color_set(o, 255, 255, 255, 50 + ((float)i / num_p) * 205);
      }
    }
    else
    {
      o = evas_object_rectangle_add(app->evas);
      evas_object_color_set(o, 0, 0, 0, 50 + ((float)i / num_p) * 205);
    }
    if (p->w && p->h)
    {
      evas_object_resize(o, p->w, p->h);
      evas_object_image_fill_set(o, 0, 0, p->w, p->h);
    } else 
    {
      evas_object_resize(o, p->m, p->m);
      evas_object_image_fill_set(o, 0, 0, p->m, p->m);
    }

    evas_object_data_set(o, "particle", p);
    evas_object_move(o, p->cur.x - (p->w / 2), p->cur.y - (p->h / 2));
    evas_object_show(o);
    app->objects = evas_list_append(app->objects, o);
  }
}

void
cb_mouse_down(void *data, Evas *evas, Evas_Object *obj, void *event)
{
  App *app;
  Evas_Event_Mouse_Down *ev;
  E_Phys_Particle *p = NULL;

  app = data;
  ev = event;

  p = e_phys_world_nearest_particle(app->world, ev->canvas.x, ev->canvas.y);

  if (p) 
  {
    app->mouse_anchor = e_phys_constraint_anchor_add(p, ev->canvas.x, ev->canvas.y);
  }
}

void
cb_mouse_move(void *data, Evas *evas, Evas_Object *obj, void *event)
{
  App *app;
  Evas_Event_Mouse_Move *ev;

  app = data;
  ev = event;

  if (app->mouse_anchor)
  {
    app->mouse_anchor->pos.x = ev->cur.canvas.x;
    app->mouse_anchor->pos.y = ev->cur.canvas.y;
  }
}

void
cb_mouse_up(void *data, Evas *evas, Evas_Object *obj, void *event)
{
  App *app;

  app = data;
  e_phys_constraint_del(E_PHYS_CONSTRAINT(app->mouse_anchor));
  app->mouse_anchor = NULL;
}

void
update_view(void *data, E_Phys_World *world)
{
  App *app = data;
  Evas_List *l;
  for (l = app->objects; l; l = l->next)
  {
    Evas_Object *o;
    E_Phys_Particle *p;
    Evas_Coord w, h;

    o = l->data;
    p = evas_object_data_get(o, "particle");

    if (p)
    {
      evas_object_geometry_get(o, NULL, NULL, &w, &h);
      evas_object_move(o, p->cur.x - w / 2, p->cur.y - h / 2);
    }
  }
}

void
setup(App *app, int argc, char **argv)
{
  E_Phys_World *world;
  Evas_Object *o;
  int i;

  srand(ecore_time_get());

  world = e_phys_world_add();
  e_phys_world_size_set(world, INIT_W, INIT_H);
  app->world = world;
  world->dt = 1.0 / 30.0;

  struct { 
    char *name;
    void (*init_func) (E_Phys_World *world);
    int use_image;
  } test[5] = {
    { "gravity", init_gravity_test, 1 },
    { "rope", init_rope, 0 },
    { "mesh", init_mesh, 0 },
    { "collision", init_collision_test, 0 },
    { "snow", init_snow_test, 1 }
  };


  if (argc > 1)
  {
    for (i = 0; i < 5; i++)
    {
      if (!strcmp(argv[1], test[i].name))
      {
        test[i].init_func(world);
        init_evas_objs(app, test[i].use_image);

        e_phys_world_update_func_set(world, update_view, app);

        // event object
        o = evas_object_rectangle_add(app->evas);
        evas_object_resize(o, world->w, world->h);
        evas_object_layer_set(o, 100);
        evas_object_repeat_events_set(o, 1);
        evas_object_color_set(o, 0, 0, 0, 0);
        evas_object_show(o);

        evas_object_event_callback_add(o, EVAS_CALLBACK_MOUSE_DOWN, cb_mouse_down, app);
        evas_object_event_callback_add(o, EVAS_CALLBACK_MOUSE_MOVE, cb_mouse_move, app);
        evas_object_event_callback_add(o, EVAS_CALLBACK_MOUSE_UP, cb_mouse_up, app);

        e_phys_world_go(world);
        return;
      }
    }
  }
  else
  {
    printf("Usage: ./e_phys_demo <test>\n\nTests:\n");
    for (i = 0; i < 5; i++)
    {
      printf("\t%s\n", test[i].name);
    }
    exit(1);
  }

}
