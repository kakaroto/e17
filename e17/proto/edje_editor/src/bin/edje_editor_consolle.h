#ifndef _EDJE_EDITOR_CONSOLLE_H_
#define _EDJE_EDITOR_CONSOLLE_H_


Evas_Object     *EV_Consolle;              //The lower consolle panel



Evas_Object* consolle_create(void);
void         consolle_log   (char *text);
void         consolle_clear (void);


void PopulateSourceComboEntry(void);
void PopulateSignalComboEntry(void);



#endif
