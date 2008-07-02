#ifndef _EDJE_EDITOR_CONSOLLE_H_
#define _EDJE_EDITOR_CONSOLLE_H_


Evas_Object     *Consolle;              //The lower consolle panel
Etk_Widget      *UI_SignalEmitEntry;
Etk_Widget      *UI_SourceEmitEntry;


Evas_Object *create_consolle(void);

void ConsolleLog(char *text);
void ConsolleClear(void);

Etk_Widget* create_signal_embed(void);
void PopulateSourceComboEntry(void);
void PopulateSignalComboEntry(void);
void emit_entry_item_append_ifnot(Etk_Widget *combo_entry, const char *text);
Etk_Bool on_SignalEmitEntry_activated      (Etk_Combobox_Entry *combo, void *data);


#endif
