/* elitaire_private.h */
#include"elitaire_job.h"

struct Elitaire{
	
	Evas      * evas;
	cardGame  * game;
	char      * file;
	int         velocity;
	float       spacing;
	float       margin_h;
	float       margin_v;
	Evas_Coord	x, y, w, h;   /* the coords of the "real" card 
	                             field without the clip offset*/
	struct {
		Evas_Coord t, l, r, b;
	} offset;                 /* offset of the card field to the clip */
	int         max_cols;     /* highest number of cards in a row */ 
	int         num_rows;
	int         num_stacks;
	int         frame_rate;	  /* pics per sec */
	int         num_card_jobs;/* the number of cards currently flying, waiting etc. */
	Evas_Bool   deal;
	Evas_Bool   animate;
	Evas_Bool   shadow;
	Evas_Bool   hints_on;
	
	Evas_Coord  card_w, card_h;
	Evas_Coord  card_max_w,card_max_h;
	Evas_Coord  card_min_w,card_min_h;
	Evas_Object *  elitaire;
	Evas_Object ** stacks;
	Evas_Object *  clip;
	
	int      (* no_job) (void * data);
	void      * no_job_data;
	Elitaire_Job_List * jobs;
};

void         elitaire_stack_layer_reinit (Elitaire * eli, playingCard * pcard);
int          elitaire_clicked_stack_get  (Elitaire * eli, Evas_Coord x, Evas_Coord y);
void         elitaire_stacks_reinit      (Elitaire * eli);
void         elitaire_hints_hide         (Elitaire * eli);
Evas_Coord   elitaire_card_offset_get    (Elitaire * eli);
Evas_Coord   elitaire_card_offset_calc   (Elitaire * eli,Evas_Coord h);
Evas_Coord   elitaire_spacing_abs_calc   (Elitaire * eli, int row);
void         elitaire_reinit             (Elitaire * eli);
void         elitaire_card_job_reg       (Elitaire * eli);
void         elitaire_card_job_unreg     (Elitaire * eli);
int          elitaire_cols_max_calc      (Elitaire * eli);
int          elitaire_num_of_rows_calc   (Elitaire * eli); /* calculate  the muber of rows*/
Evas_Coord   elitaire_spacing_abs_calc   (Elitaire * eli, int row);
void         elitaire_cards_del          (Elitaire * eli);
void         elitaire_stacks_del         (Elitaire * eli);
void         elitaire_force_resize       (Elitaire * eli);

void         elitaire_mouse_up_cb(void * data, Evas * e, Evas_Object * obj, void * event_info);
void         elitaire_mouse_down_cb(void * data, Evas * e, Evas_Object * obj, void * event_info);

#include"elitaire_card.h"
