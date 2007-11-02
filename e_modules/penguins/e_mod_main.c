#include <e.h>
#include <Ecore_Str.h>

#include "config.h"
#include "e_mod_main.h"
#include "e_mod_config.h"


#define CLIMBER_PROB 4 // 4 Means: one climber every 5 - 1 Means: all climber - !!Don't set to 0
#define FLOATER_PROB 2 
#define MAX_FALLER_HEIGHT 300

#define FLYER_PROB 1000 // every n animation cicle
#define CUSTOM_PROB 600 // every n animation cicle

//_RAND is true 1 time every prob
#define _RAND(prob) ( ( random() % prob ) == 0 )

/* module private routines */
static int _is_inside_any_win(Population *pop, int x, int y, int ret_value);
static Population    *_population_init(E_Module *m);
static void          _population_shutdown(Population *pop);
static int           _cb_animator(void *data);
static void          _population_load(Population *pop);
static void          _theme_load(Population *pop);
static void          _start_walking_at(Penguin *tux, int at_y);
static void          _start_climbing_at(Penguin *tux, int at_x);
static void          _start_falling_at(Penguin *tux, int at_x);
static void          _start_flying_at(Penguin *tux, int at_y);
static void          _start_splatting_at(Penguin *tux, int at_y);
static void          _start_custom_at(Penguin *tux, int at_y);
//static void        _win_shape_change(void *data, E_Container_Shape *es, E_Container_Shape_Change ch);
static void          _reborn(Penguin *tux);

/* public module routines. all modules must have these */
EAPI E_Module_Api e_modapi = {
   E_MODULE_API_VERSION,
   "Penguins"
};

EAPI void *
e_modapi_init(E_Module *m)
{
   Population *pop;

   /* Set up module's message catalogue */
   bindtextdomain(PACKAGE, LOCALEDIR);
   bind_textdomain_codeset(PACKAGE, "UTF-8");

   pop = _population_init(m);
   return pop;
}

EAPI int
e_modapi_shutdown(E_Module *m)
{
   Population *pop;

   pop = m->data;
   if (pop)
   {
      if (pop->config_dialog)
      {
         e_object_del(E_OBJECT(pop->config_dialog));
         pop->config_dialog = NULL;
      }
      _population_shutdown(pop);
   }
   return 1;
}

EAPI int
e_modapi_save(E_Module *m)
{
   Population *pop;

   pop = m->data;
   if (!pop)
      return 1;
   e_config_domain_save("module.penguins", pop->conf_edd, pop->conf);
   return 1;
}

/* module private routines */
static Population *
_population_init(E_Module *m)
{
   Population *pop;
   Evas_List *managers, *l, *l2, *l3;

   pop = calloc(1, sizeof(Population));
   if (!pop)
      return NULL;

   pop->module = m;
   pop->conf_edd = E_CONFIG_DD_NEW("Penguins_Config", Config);
#undef T
#undef D
#define T Config
#define D pop->conf_edd
   E_CONFIG_VAL(D, T, zoom, DOUBLE);
   E_CONFIG_VAL(D, T, penguins_count, INT);
   E_CONFIG_VAL(D, T, theme, STR);
   E_CONFIG_VAL(D, T, alpha, INT);

   pop->conf = e_config_domain_load("module.penguins", pop->conf_edd);
   if (!pop->conf)
     {
        pop->conf = E_NEW(Config, 1);

        pop->conf->zoom = 1;
        pop->conf->penguins_count = 3;
        pop->conf->alpha = 200;
        pop->conf->theme = PACKAGE_DATA_DIR"/themes/default.edj";
     }

   managers = e_manager_list();
   for (l = managers; l; l = l->next)
   {
      E_Manager *man;
         
      man = l->data;
        
      for (l2 = man->containers; l2; l2 = l2->next)
      {
         E_Container *con;
         con = l2->data;
         printf("E_container: '%s' [x:%d y:%d w:%d h:%d]\n",
                 con->name, con->x, con->y, con->w, con->h);
         pop->cons = evas_list_append(pop->cons, con);
         pop->canvas = con->bg_evas;
         //e_container_shape_change_callback_add(con, _win_shape_change, NULL);
         for (l3 = e_container_shape_list_get(con); l3; l3 = l3->next)
         {
            E_Container_Shape *es;
            int x, y, w, h;
            es = l3->data;   
            if (es->visible)
            {
               e_container_shape_geometry_get(es, &x, &y, &w, &h);
               printf("E_shape: [%d] x:%d y:%d w:%d h:%d\n", es->visible, x, y, w, h);
            }
         }
      }
   }

   evas_output_viewport_get(pop->canvas, NULL, NULL, &pop->width, &pop->height);

   printf("Get themes list\n");
   Ecore_List *files;
   char *filename;
   char *name;
   char buf[4096];
     
   files = ecore_file_ls(PACKAGE_DATA_DIR"/themes");
   while ((filename = ecore_list_next(files)))
   {
         if (ecore_str_has_suffix(filename, ".edj"))
         {
            snprintf(buf, sizeof(buf),"%s/%s", PACKAGE_DATA_DIR"/themes", filename);
            name = edje_file_data_get(buf, "PopulationName");
            if (name)
            {
               printf("THEME FILE: %s (%s)\n", filename, name);
               pop->themes = evas_list_append(pop->themes, strdup(buf));
            }
         }
   }   
   ecore_list_destroy(files);
     
   _theme_load(pop);
   _population_load(pop);

   pop->animator = ecore_animator_add(_cb_animator, pop);
   //srand(ecore_time_get());    TODO we need it?
   return pop;
}

Evas_Bool 
_action_free(Evas_Hash *hash, const char *key, void *data, void *fdata)
{
   Action *a;
   a = data;
   printf("Free Action '%s' :(\n", a->name);
   E_FREE(a->name);
   E_FREE(a);
   return 1;
}
static void
_population_free(Population *pop)
{
   while (pop->penguins)
     {
        Penguin *tux;
        printf("Free TUX :(\n");
        tux = pop->penguins->data;
        evas_object_del(tux->obj);
        pop->penguins = evas_list_remove_list(pop->penguins, pop->penguins);
        E_FREE(tux);
         tux = NULL;
     }
   while (pop->customs)
     {
        Custom_Action *cus;
        printf("Free Custom :(\n");
        cus = pop->customs->data;
        E_FREE(cus->name);
        E_FREE(cus->left_program_name);
        E_FREE(cus->right_program_name);
        
        pop->customs = evas_list_remove_list(pop->customs, pop->customs);
        E_FREE(cus);
        cus = NULL;
     } 
     
     evas_hash_foreach(pop->actions, _action_free, NULL);
     evas_hash_free(pop->actions);
     pop->actions = NULL;
}

/* static void
_real_population_shutdown(Population *pop)
{

} */

static void
_population_shutdown(Population *pop)
{
   printf("KILL 'EM ALL\n");
   
   while (pop->cons)
     {
        E_Container *con;

        con = pop->cons->data;
        pop->cons = evas_list_remove_list(pop->cons, pop->cons);
     }

   _population_free(pop);

     
   if (pop->animator)
      ecore_animator_del(pop->animator);
   
   while (pop->themes)
   {
      printf("Free Theme '%s' :(\n", (char *)pop->themes->data);
      pop->themes = evas_list_remove_list(pop->themes, pop->themes);
   }

   //E_FREE(pop->conf->theme); //TODO Why can't free??
   E_FREE(pop->conf);
   E_CONFIG_DD_FREE(pop->conf_edd);
   
   
   E_FREE(pop);
   pop = NULL;
}

void
_penguins_cb_config_updated(void *data)
{
   Population *pop;

   pop = (Population *)data;
   if (!pop)
      return;
   
   _population_free(pop);

   _theme_load(pop);
   _population_load(pop);
}
static Action*
_load_action(Population *pop, char *filename, char *name, int id)
{
   Action *act;
   char *data;
 
   data = edje_file_data_get(filename, name);
   if (!data) 
      return NULL;
   
   act = calloc(1, sizeof(Action));
   if (!act)
      return NULL;

   act->name = strdup(name);
   sscanf(data, "%d %d %d", &act->w, &act->h, &act->speed);
   
   act->w = act->w * pop->conf->zoom;
   act->h = act->h * pop->conf->zoom;
   act->id = id;
   //TODO ZOOM ALSO THE SPEED ???
   
   pop->actions = evas_hash_add(pop->actions, name, act);
   
   return act;
}
static Custom_Action*
_load_custom_action(Population *pop, char *filename, char *name)
{
   Custom_Action *c;
   char *data;
   char buf[25];
   
   data = edje_file_data_get(filename, name);
   if (!data) 
      return NULL;
   
   c = calloc(1, sizeof(Custom_Action));
   if (!c)
      return NULL;

   c->name = strdup(name);
   sscanf(data, "%d %d %d %d %d %d", 
            &c->w, &c->h, &c->h_speed, &c->v_speed, &c->r_min, &c->r_max);
   
   c->w = c->w * pop->conf->zoom;
   c->h = c->h * pop->conf->zoom;
   
   snprintf(buf, sizeof(buf), "start_custom_%d_left", pop->custom_num+1);
   c->left_program_name = strdup(buf);
   snprintf(buf, sizeof(buf), "start_custom_%d_right", pop->custom_num+1);
   c->right_program_name = strdup(buf);
   
   pop->customs = evas_list_append(pop->customs, c);
   pop->custom_num++;
   
   return c;
}
Evas_Bool hash_fn(Evas_Hash *hash, const char *key, void *data, void *fdata)
{
   Action *a = data;
   printf("ACTIONS: name:'%s' w:%d h:%d speed:%d\n", key, a->w, a->h, a->speed);
   return 1;
}

static void
_theme_load(Population *pop)
{
   char *name;
   char buf[15];
   int i;
   
   pop->actions = NULL;
   pop->customs = NULL;
   pop->custom_num = 0;
   
   if (!pop->conf->theme)
      pop->conf->theme = PACKAGE_DATA_DIR "/default.edj";
      

   name = edje_file_data_get(pop->conf->theme, "PopulationName");
   if (!name) 
      return;
   
   printf("*** LOAD THEME %s (%s)\n", name, pop->conf->theme);
   
   // load standard actions
   _load_action(pop, pop->conf->theme, "Walker", ID_WALKER);
   
   _load_action(pop, pop->conf->theme, "Faller", ID_FALLER);
   _load_action(pop, pop->conf->theme, "Climber", ID_CLIMBER);
   _load_action(pop, pop->conf->theme, "Floater", ID_FLOATER);
   //_load_action(pop, pop->conf->theme, "Bomber");
   _load_action(pop, pop->conf->theme, "Splatter", ID_SPLATTER);
   _load_action(pop, pop->conf->theme, "Flyer", ID_FLYER);
   
   // load custom actions
   i = 2;
   snprintf(buf, sizeof(buf), "Custom_1");
   while (_load_custom_action(pop, pop->conf->theme, buf))
      snprintf(buf, sizeof(buf), "Custom_%d", i++);
   
   evas_hash_foreach(pop->actions, hash_fn, NULL);
   Evas_List *l;
   for (l = pop->customs; l; l = l->next )
   {
      Custom_Action *c = l->data;
      printf("CUSTOMS: name:'%s' w:%d h:%d h_speed:%d v_speed:%d\n",
               c->name, c->w, c->h, c->h_speed, c->v_speed);
   }
}
static void
_population_load(Population *pop)
{
   Evas_Object *o;
   Evas_Coord xx, yy, ww, hh;
   int i;
   Penguin *tux;
   
   evas_output_viewport_get(pop->canvas, &xx, &yy, &ww, &hh);
  
   //Create edje
   printf("\n *** LOAD %d ANIMS ***\n", pop->conf->penguins_count);

	
   for (i = 0; i < pop->conf->penguins_count; i++)
     {
        tux = malloc(sizeof(Penguin));

        o = edje_object_add(pop->canvas);
        edje_object_file_set(o, pop->conf->theme, "anims");
        
        tux->action = evas_hash_find(pop->actions,"Faller");
              
        evas_object_image_alpha_set(o, 0.5);
        evas_object_color_set(o, pop->conf->alpha, pop->conf->alpha,
                              pop->conf->alpha, pop->conf->alpha);
        evas_object_pass_events_set(o, 1);
       
        tux->obj = o;
        tux->pop = pop;
        

        pop->penguins = evas_list_append(pop->penguins, tux);
        evas_object_show(o);
        
        _reborn(tux);
     }
}

static void
_reborn(Penguin *tux)
{
   printf("Reborn :)\n");
   tux->reverse = random() % (2);
   tux->x = random() % (tux->pop->width);
   tux->y = 0;
   tux->custom = 0;
   evas_object_move(tux->obj, (int)tux->x, (int)tux->y);
   _start_falling_at(tux, tux->x);
   evas_object_resize(tux->obj, tux->action->w, tux->action->h);
   evas_object_image_fill_set(tux->obj, 0, 0, tux->action->w, tux->action->h);
}

static int
_cb_animator(void *data)
{
   Population *pop;
   Evas_List *l;

   pop = data;
   for (l = pop->penguins; l; l = l->next)
   {
      Penguin *tux;
      int touch;
      tux = l->data;
      
      // ******  CUSTOM ACTIONS  ********
      if (tux->custom)
      {
         tux->x += ((double)tux->custom->h_speed / 100);
         tux->y += ((double)tux->custom->v_speed / 100);
      }
      // ******  FALLER  ********
      else if (tux->action->id == ID_FALLER)
      {
         tux->y += ((double)tux->action->speed / 100);
         if ((touch = _is_inside_any_win(pop,
                        (int)tux->x+(tux->action->w/2),
                        (int)tux->y + tux->action->h,
                        _RET_TOP_VALUE)))
         {
            if (( (int)tux->y - tux->faller_h)  > MAX_FALLER_HEIGHT)
               _start_splatting_at(tux, touch);
            else
               _start_walking_at(tux, touch); 
         }
         else if (( (int)tux->y + tux->action->h ) > pop->height)
         {
            if (( (int)tux->y - tux->faller_h)  > MAX_FALLER_HEIGHT)
               _start_splatting_at(tux, pop->height);
            else
               _start_walking_at(tux, pop->height);
         }
         
      }
      // ******  FLOATER ********
      else if (tux->action->id == ID_FLOATER)
      {
         tux->y += ((double)tux->action->speed / 100);
         if ((touch = _is_inside_any_win(pop,
                        (int)tux->x+(tux->action->w/2),
                        (int)tux->y + tux->action->h,
                        _RET_TOP_VALUE)
            ))
            _start_walking_at(tux, touch);
         else if (( (int)tux->y + tux->action->h ) > pop->height)
            _start_walking_at(tux, pop->height);
            
      }
      // ******  WALKER  ********
      else if (tux->action->id == ID_WALKER)
      {
         // random flyer
         if (_RAND(FLYER_PROB)){
            _start_flying_at(tux, tux->y);
         }
         // random custom
         else if (_RAND(CUSTOM_PROB)){
            _start_custom_at(tux, tux->y+tux->action->h);
         }
         // left
         else if (tux->reverse)
         {
            tux->x -= ((double)tux->action->speed / 100);
            if ((touch = _is_inside_any_win(pop, (int)tux->x , (int)tux->y, _RET_RIGHT_VALUE)) ||
                tux->x < 0)
            {
               if (_RAND(CLIMBER_PROB))
                  _start_climbing_at(tux, touch);
               else
               {
                  edje_object_signal_emit(tux->obj, "start_walking_right", "epenguins");
                  tux->reverse = FALSE;
               }
            }
            if ((tux->y + tux->action->h) < pop->height)   
               if (!_is_inside_any_win(pop, (int)tux->x+(tux->action->w/2) , (int)tux->y+tux->action->h+1, _RET_NONE_VALUE))
                  _start_falling_at(tux, (int)tux->x+(tux->action->w/2));
         }
         // right
         else
         {
            tux->x += ((double)tux->action->speed / 100);
            if ((touch = _is_inside_any_win(pop, (int)tux->x + tux->action->w, (int)tux->y, _RET_LEFT_VALUE)) ||
                (tux->x + tux->action->w) > pop->width)
            {
               if (_RAND(CLIMBER_PROB))
               {
                  if (touch)
                     _start_climbing_at(tux, touch);
                  else
                     _start_climbing_at(tux, pop->width);
               }
               else
               {
                  edje_object_signal_emit(tux->obj, "start_walking_left", "epenguins");
                  tux->reverse = TRUE;
               }
            }
            if ((tux->y + tux->action->h) < pop->height)   
               if (!_is_inside_any_win(pop, (int)tux->x+(tux->action->w/2), (int)tux->y+tux->action->h+1, _RET_NONE_VALUE))
                  _start_falling_at(tux, (int)tux->x+(tux->action->w/2));
         }

      }
       // ******  FLYER  ********
      else if (tux->action->id == ID_FLYER)
      {
         tux->y -= ((double)tux->action->speed / 100);
         tux->x += (random() % 3) - 1;
         if (tux->y < 0){
            tux->reverse = !tux->reverse;
            _start_falling_at(tux, (int)tux->x);
         }
      }
      // ******  CLIMBER  ********
      else if (tux->action->id == ID_CLIMBER)
      {
         tux->y -= ((double)tux->action->speed / 100);
         // left
         if (tux->reverse)
         {
            if (!_is_inside_any_win(pop,
                  (int)tux->x-1,
                  (int)tux->y+(tux->action->h/2),
                  _RET_NONE_VALUE))
            {
               if (tux->x > 0)
               {
                  tux->x-=(tux->action->w/2)+1;  
                  _start_walking_at(tux, (int)tux->y+(tux->action->h/2));
               }
            }
         }
         // right
         else
         {
            if (!_is_inside_any_win(pop, 
                  (int)tux->x+tux->action->w+1, 
                  (int)tux->y+(tux->action->h/2), 
                  _RET_NONE_VALUE))
            {
               if ((tux->x+tux->action->w) < pop->width)
               {
                  tux->x+=(tux->action->w/2)+1;
                  _start_walking_at(tux, (int)tux->y+(tux->action->h/2));
               }
            }
         }
         if (tux->y < 0){
            tux->reverse = !tux->reverse;
            _start_falling_at(tux, (int)tux->x);
         }
         
      }

     // printf("Place tux at x:%d y:%d w:%d h:%d\n", tux->x, tux->y, tux->action->w, tux->action->h);
      evas_object_move(tux->obj, (int)tux->x, (int)tux->y);

  }
  return 1;
}

static int
_is_inside_any_win(Population *pop, int x, int y, int ret_value)
{
   Evas_List *l;
   E_Container *con;
   
   con = e_container_current_get(e_manager_current_get());

   for (l = e_container_shape_list_get(con); l; l = l->next)
   {
      E_Container_Shape *es;
      int sx, sy, sw, sh;
      es = l->data;
      if (es->visible)
      {
         e_container_shape_geometry_get(es, &sx, &sy, &sw, &sh);
         //printf("E_shape: [%d] x:%d y:%d w:%d h:%d\n", es->visible, sx, sy, sw, sh);
         if ( ((x > sx) && (x < (sx+sw))) &&
              ((y > sy) && (y < (sy+sh))) )
         {  
            switch (ret_value)
            {
               case _RET_NONE_VALUE:
                  return 1;
                  break;
               case _RET_RIGHT_VALUE:
                  return sx+sw;
                  break;
               case _RET_BOTTOM_VALUE:
                  return sy+sh;
                  break;
               case _RET_TOP_VALUE:
                  return sy;
                  break;
               case _RET_LEFT_VALUE:
                  return sx;
                  break;
               default:
                  return 1;
            }
            
         }
      }
   }   

   return 0;
}

static void 
_start_walking_at(Penguin *tux, int at_y)
{
   printf("Start walking...at %d\n", at_y);
   tux->action = evas_hash_find(tux->pop->actions, "Walker");
   
   tux->y = at_y - tux->action->h;
   evas_object_resize(tux->obj, tux->action->w, tux->action->h);

   if (tux->reverse)
      edje_object_signal_emit(tux->obj, "start_walking_left", "epenguins");
   else
      edje_object_signal_emit(tux->obj, "start_walking_right", "epenguins");      
}
static void 
_start_climbing_at(Penguin *tux, int at_x)
{
   //printf("Start climbing...at: %d\n", at_x);
   tux->action = evas_hash_find(tux->pop->actions, "Climber");
   evas_object_resize(tux->obj, tux->action->w, tux->action->h);
   
   if (tux->reverse)
   {
      tux->x = at_x;
      edje_object_signal_emit(tux->obj, "start_climbing_left", "epenguins");
   }
   else
   {
      tux->x = at_x - tux->action->w;
      edje_object_signal_emit(tux->obj, "start_climbing_right", "epenguins");      
   }
   
}
static void 
_start_falling_at(Penguin *tux, int at_x)
{
   if (_RAND(FLOATER_PROB))
   {
      //printf("Start floating...\n");
      tux->action = evas_hash_find(tux->pop->actions, "Floater");
      evas_object_resize(tux->obj, tux->action->w, tux->action->h);

      if (tux->reverse)
      {
         tux->x = (double)(at_x - tux->action->w);
         edje_object_signal_emit(tux->obj, "start_floating_left", "epenguins");
      }
      else
      {
         tux->x = (double)at_x;
         edje_object_signal_emit(tux->obj, "start_floating_right", "epenguins");      
      }
   }
   else
   {
      //printf("Start falling...\n");
      tux->action = evas_hash_find(tux->pop->actions, "Faller");
      evas_object_resize(tux->obj, tux->action->w, tux->action->h);

      if (tux->reverse)
      {
         tux->x = (double)(at_x - tux->action->w);
         edje_object_signal_emit(tux->obj, "start_falling_left", "epenguins");
      }
      else
      {
         tux->x = (double)at_x;
         edje_object_signal_emit(tux->obj, "start_falling_right", "epenguins");      
      }
   }
   tux->faller_h = (int)tux->y;
}

static void 
_start_flying_at(Penguin *tux, int at_y)
{
   tux->action = evas_hash_find(tux->pop->actions, "Flyer");
   evas_object_resize(tux->obj, tux->action->w, tux->action->h);
   tux->y = at_y - tux->action->h;
   if (tux->reverse)
      edje_object_signal_emit(tux->obj, "start_flying_left", "epenguins");
   else
      edje_object_signal_emit(tux->obj, "start_flying_right", "epenguins");
}

static void 
_cb_splatter_end (void *data, Evas_Object *o, const char *emi, const char *src)
{
   _reborn((Penguin*)data);
   edje_object_signal_callback_del(o,"splatting_done","edje", _cb_splatter_end);
}

static void 
_start_splatting_at(Penguin *tux, int at_y)
{
  // printf("Start splatting...\n");
   evas_object_hide(tux->obj);
   tux->action = evas_hash_find(tux->pop->actions, "Splatter");
   evas_object_resize(tux->obj, tux->action->w, tux->action->h);
   evas_object_image_fill_set(tux->obj, 0, 0, tux->action->w, tux->action->h);
   tux->y = at_y - tux->action->h;
   if (tux->reverse)
      edje_object_signal_emit(tux->obj, "start_splatting_left", "epenguins");
   else
      edje_object_signal_emit(tux->obj, "start_splatting_right", "epenguins");
   
   
   edje_object_signal_callback_add(tux->obj,"splatting_done","edje", _cb_splatter_end, tux);
   evas_object_resize(tux->obj, tux->action->w, tux->action->h);
   evas_object_image_fill_set(tux->obj, 0, 0, tux->action->w, tux->action->h);
   evas_object_move(tux->obj,(int)tux->x,(int)tux->y);
   evas_object_show(tux->obj);
}


static void 
_cb_custom_end (void *data, Evas_Object *o, const char *emi, const char *src)
{
   Penguin* tux = data;
  // printf("CUSTOM END.\n");
   if (tux->r_count > 0)
   {
      if (tux->reverse)
         edje_object_signal_emit(tux->obj, tux->custom->left_program_name, "epenguins");
      else
         edje_object_signal_emit(tux->obj, tux->custom->right_program_name, "epenguins");
      tux->r_count--;
   }
   else
   {
      edje_object_signal_callback_del(o,"custom_done","edje", _cb_custom_end);
      _start_walking_at(tux, tux->y+tux->custom->h);
      tux->custom = NULL;
   }
}


static void 
_start_custom_at(Penguin *tux, int at_y)
{
   int ran;

   if (tux->pop->custom_num < 1)
      return;
   
   ran = random() % (tux->pop->custom_num);
   tux->custom = evas_list_nth(tux->pop->customs, ran);
   if (!tux->custom)
      return;
   
   evas_object_resize(tux->obj, tux->custom->w, tux->custom->h);
   tux->y = at_y - tux->custom->h;
   
   if ( tux->custom->r_min == tux->custom->r_max)
      tux->r_count = tux->custom->r_min;
   else
      tux->r_count = tux->custom->r_min + 
                     (random() % (tux->custom->r_max - tux->custom->r_min + 1));
   tux->r_count --;
   if (tux->reverse)
      edje_object_signal_emit(tux->obj, tux->custom->left_program_name, "epenguins");
   else   
      edje_object_signal_emit(tux->obj, tux->custom->right_program_name, "epenguins");
      
   
   
   printf("START Custom Action n %d (%s) repeat: %d\n", ran, tux->custom->left_program_name, tux->r_count);
   
   edje_object_signal_callback_add(tux->obj,"custom_done","edje", _cb_custom_end, tux);
   
}
/* static void
_win_shape_change(void *data, E_Container_Shape *es, E_Container_Shape_Change ch)
{
   //printf("Shape changed\n");
   int x, y, w, h;
   
   switch (ch)
     {
      case E_CONTAINER_SHAPE_ADD:
	
	break;
      case E_CONTAINER_SHAPE_DEL:
	
	
	break;
      case E_CONTAINER_SHAPE_SHOW:
	
	break;
      case E_CONTAINER_SHAPE_HIDE:
	
	break;
      case E_CONTAINER_SHAPE_MOVE:
	
	break;
      case E_CONTAINER_SHAPE_RESIZE:
	
	break;
      case E_CONTAINER_SHAPE_RECTS:
	
	break;
      default:
	break;
     }
} */
