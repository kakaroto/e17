/**********************************************/

/*** the data struct ***/
struct Elitaire_Card {
	Evas_Object * image;
	Evas_Object * next_card;
	Evas_Object * shadow;
	playingCard * pcard;
	Elitaire    * eli;
	Elitaire_Job_List * jobs;
	char          flying:1;
	char          waiting:1;
};
/*** external API ***/

Evas_Object * elitaire_card_new               (Elitaire * eli, playingCard * pcard);
Evas_Object * elitaire_card_next_card_get     (Evas_Object * card);
void          elitaire_card_playingCard_set   (Evas_Object * card, playingCard * pcard);
playingCard * elitaire_card_playingCard_get   (Evas_Object * card);
Evas_Bool     elitaire_card_is_flying         (Evas_Object * card);
void          elitaire_card_callback_add      (Evas_Object * card, Evas_Callback_Type type, 
                              void(*func)(void *data, Evas *e, Evas_Object *obj, void *event_info));
void          elitaire_card_callback_del      (Evas_Object * card, Evas_Callback_Type type, 
                              void(*func)(void *data, Evas *e, Evas_Object *obj, void *event_info));
Evas_Bool	  elitaire_card_moveable          (Evas_Object * card);
void          elitaire_card_hint_highlight_show(Evas_Object * card);
void          elitaire_card_hint_highlight_hide(Evas_Object * card);
Elitaire    * elitaire_card_elitaire_get      (Evas_Object * card);
void          elitaire_card_chain_make        (Evas_Object * card);
void          elitaire_card_chain_del         (Evas_Object * card);
void          elitaire_card_reinit            (Evas_Object * card);
void          elitaire_card_slide             (Evas_Object * card, Evas_Coord x, Evas_Coord y);
void          elitaire_card_flip              (Evas_Object * card);
void          elitaire_card_dragged           (Evas_Object * card);
void          elitaire_card_fade              (Evas_Object * card);
void          elitaire_card_wait              (Evas_Object * card, int t);
void          elitaire_card_xy_get            (Evas_Object * card, Evas_Coord * x, Evas_Coord * y);
