#include <stdio.h>
#include <stdlib.h>
#include <Ecore.h>
#include <Ecore_Data.h>
#include <Evas.h>
#include <Edje.h>
#include "intl.h"

struct Elitaire;

/* external API */
const char ** elitaire_available_games_get         (void);
Evas_Object * elitaire_object_new                  (Evas* evas);
Evas_Bool     elitaire_object_game_set             (Evas_Object * elitaire, const char *game);
int           elitaire_object_file_set             (Evas_Object * elitaire, const char *file);
int           elitaire_object_file_get             (Evas_Object * elitaire, const char **file);
void          elitaire_object_deal                 (Evas_Object * elitaire);
int           elitaire_object_velocity_get         (Evas_Object * elitaire);
void          elitaire_object_velocity_set         (Evas_Object * elitaire, int v);
void          elitaire_object_frame_rate_set       (Evas_Object * elitaire, int fr);
int           elitaire_object_frame_rate_get       (Evas_Object * elitaire);
void          elitaire_object_animations_set       (Evas_Object * elitaire, Evas_Bool on);
Evas_Bool     elitaire_object_animations_get       (Evas_Object * elitaire);
void          elitaire_object_shadows_set          (Evas_Object * elitaire, Evas_Bool on);
Evas_Bool     elitaire_object_shadows_get          (Evas_Object * elitaire);
void          elitaire_object_lazy_mode_set        (Evas_Object * elitaire, int lm);
int           elitaire_object_lazy_mode_get        (Evas_Object * elitaire);
void          elitaire_object_size_min_get         (Evas_Object * elitaire, Evas_Coord *minw, Evas_Coord *minh);
void          elitaire_object_size_max_get         (Evas_Object * elitaire, Evas_Coord *maxw, Evas_Coord *maxh);
void          elitaire_object_callback_win_add     (Evas_Object * elitaire, 
                                                    void (*func)(int end, float points, 
                                                                 pointsType type, void* data),
                                                    void* data);
void          elitaire_object_callback_points_add  (Evas_Object * elitaire,
                                                    void (*func)(float points, pointsType type, void* data), 
                                                    void* data);
Evas_Bool     elitaire_object_wait_for_end_of_jobs (Evas_Object * elitaire, int (*func)(void * data), void * data);
void          elitaire_object_undo                 (Evas_Object * elitaire);
void          elitaire_object_restart              (Evas_Object * elitaire);
Evas_Bool     elitaire_object_giveup               (Evas_Object * elitaire);
void          elitaire_object_pause                (Evas_Object * elitaire);
void          elitaire_object_hints_show           (Evas_Object * elitaire);
void          elitaire_object_hints_hide           (Evas_Object * elitaire);
Evas_Bool     elitaire_object_hints_visible        (Evas_Object * elitaire);
void          elitaire_object_offset_set           (Evas_Object * elitaire, Evas_Coord l, Evas_Coord r, Evas_Coord t, Evas_Coord b);
void          elitaire_object_offset_get           (Evas_Object * elitaire, Evas_Coord * l, Evas_Coord * r, Evas_Coord * t, Evas_Coord * b);

/* other useful stuff */
char * itoa(int val, char * buf, int length);
