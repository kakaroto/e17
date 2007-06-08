#include <math.h>
#include "Rules.h"
#include "Elitaire.h"
#include "elitaire_private.h"

#undef SQR
#define SQR(x) ((x)*(x))

#undef ABS
#define ABS(x) (((x) >= 0) ? (x) : -(x))

/*** the struct for the slides and other time based movements ***/
struct Elitaire_Card_Job
{
	union {
		Evas_Coord ec;
		int        i;
	} x, y;
	Evas_Coord       w;
	Evas_Object    * card;
	Ecore_Animator * timer;
	double           t;
};

/* chain stuff */
void          elitaire_card_chain_delete      (Evas_Object* card);
int           elitaire_card_chain_length_calc (Evas_Object* card);
void          elitaire_card_chain_next_set    (Evas_Object* o, Evas_Object* ncard);
Evas_Object * elitaire_card_chain_next_get    (Evas_Object* o);

/* callbacks */
Evas_Bool elitaire_card_job_clean_up   (Elitaire_Job_List * list, void * data);
Evas_Bool elitaire_card_job_del_cb     (Elitaire_Job_List * list, void * data);
