#include "etk_server.h"

extern Evas_Hash *_etk_server_valid_signals;

void _etk_server_valid_signals_populate()
{
   Etk_Server_Signal *sig = NULL;
   Evas_List *siglist = NULL;
   sig = calloc(1, sizeof(Etk_Server_Signal));
   sig->name = strdup("pressed");
   sig->marshaller = strdup("etk_marshaller_VOID__VOID");	      
   if((siglist = evas_hash_find(_etk_server_valid_signals, "pressed")) != NULL)
     {
        siglist = evas_list_append(siglist, sig);
        evas_hash_modify(_etk_server_valid_signals, "pressed", siglist);
     }
   else
     {
        siglist = evas_list_append(siglist, sig);
        _etk_server_valid_signals = evas_hash_add(_etk_server_valid_signals,  "pressed", siglist);
     }
   siglist = NULL;
	    
   sig = calloc(1, sizeof(Etk_Server_Signal));
   sig->name = strdup("released");
   sig->marshaller = strdup("etk_marshaller_VOID__VOID");	      
   if((siglist = evas_hash_find(_etk_server_valid_signals, "released")) != NULL)
     {
        siglist = evas_list_append(siglist, sig);
        evas_hash_modify(_etk_server_valid_signals, "released", siglist);
     }
   else
     {
        siglist = evas_list_append(siglist, sig);
        _etk_server_valid_signals = evas_hash_add(_etk_server_valid_signals,  "released", siglist);
     }
   siglist = NULL;
	    
   sig = calloc(1, sizeof(Etk_Server_Signal));
   sig->name = strdup("clicked");
   sig->marshaller = strdup("etk_marshaller_VOID__VOID");	      
   if((siglist = evas_hash_find(_etk_server_valid_signals, "clicked")) != NULL)
     {
        siglist = evas_list_append(siglist, sig);
        evas_hash_modify(_etk_server_valid_signals, "clicked", siglist);
     }
   else
     {
        siglist = evas_list_append(siglist, sig);
        _etk_server_valid_signals = evas_hash_add(_etk_server_valid_signals,  "clicked", siglist);
     }
   siglist = NULL;
	    
   sig = calloc(1, sizeof(Etk_Server_Signal));
   sig->name = strdup("color_selected");
   sig->marshaller = strdup("etk_marshaller_VOID__VOID");	      
   if((siglist = evas_hash_find(_etk_server_valid_signals, "color_selected")) != NULL)
     {
        siglist = evas_list_append(siglist, sig);
        evas_hash_modify(_etk_server_valid_signals, "color_selected", siglist);
     }
   else
     {
        siglist = evas_list_append(siglist, sig);
        _etk_server_valid_signals = evas_hash_add(_etk_server_valid_signals,  "color_selected", siglist);
     }
   siglist = NULL;
	    
   sig = calloc(1, sizeof(Etk_Server_Signal));
   sig->name = strdup("color_selected");
   sig->marshaller = strdup("etk_marshaller_VOID__VOID");	      
   if((siglist = evas_hash_find(_etk_server_valid_signals, "color_selected")) != NULL)
     {
        siglist = evas_list_append(siglist, sig);
        evas_hash_modify(_etk_server_valid_signals, "color_selected", siglist);
     }
   else
     {
        siglist = evas_list_append(siglist, sig);
        _etk_server_valid_signals = evas_hash_add(_etk_server_valid_signals,  "color_selected", siglist);
     }
   siglist = NULL;
	    
   sig = calloc(1, sizeof(Etk_Server_Signal));
   sig->name = strdup("child_added");
   sig->marshaller = strdup("etk_marshaller_VOID__POINTER");	      
   if((siglist = evas_hash_find(_etk_server_valid_signals, "child_added")) != NULL)
     {
        siglist = evas_list_append(siglist, sig);
        evas_hash_modify(_etk_server_valid_signals, "child_added", siglist);
     }
   else
     {
        siglist = evas_list_append(siglist, sig);
        _etk_server_valid_signals = evas_hash_add(_etk_server_valid_signals,  "child_added", siglist);
     }
   siglist = NULL;
	    
   sig = calloc(1, sizeof(Etk_Server_Signal));
   sig->name = strdup("child_removed");
   sig->marshaller = strdup("etk_marshaller_VOID__POINTER");	      
   if((siglist = evas_hash_find(_etk_server_valid_signals, "child_removed")) != NULL)
     {
        siglist = evas_list_append(siglist, sig);
        evas_hash_modify(_etk_server_valid_signals, "child_removed", siglist);
     }
   else
     {
        siglist = evas_list_append(siglist, sig);
        _etk_server_valid_signals = evas_hash_add(_etk_server_valid_signals,  "child_removed", siglist);
     }
   siglist = NULL;
	    
   sig = calloc(1, sizeof(Etk_Server_Signal));
   sig->name = strdup("close");
   sig->marshaller = strdup("etk_marshaller_VOID__VOID");	      
   if((siglist = evas_hash_find(_etk_server_valid_signals, "close")) != NULL)
     {
        siglist = evas_list_append(siglist, sig);
        evas_hash_modify(_etk_server_valid_signals, "close", siglist);
     }
   else
     {
        siglist = evas_list_append(siglist, sig);
        _etk_server_valid_signals = evas_hash_add(_etk_server_valid_signals,  "close", siglist);
     }
   siglist = NULL;
	    
   sig = calloc(1, sizeof(Etk_Server_Signal));
   sig->name = strdup("response");
   sig->marshaller = strdup("etk_marshaller_VOID__INT");	      
   if((siglist = evas_hash_find(_etk_server_valid_signals, "response")) != NULL)
     {
        siglist = evas_list_append(siglist, sig);
        evas_hash_modify(_etk_server_valid_signals, "response", siglist);
     }
   else
     {
        siglist = evas_list_append(siglist, sig);
        _etk_server_valid_signals = evas_hash_add(_etk_server_valid_signals,  "response", siglist);
     }
   siglist = NULL;
	    
   sig = calloc(1, sizeof(Etk_Server_Signal));
   sig->name = strdup("text_changed");
   sig->marshaller = strdup("etk_marshaller_VOID__VOID");	      
   if((siglist = evas_hash_find(_etk_server_valid_signals, "text_changed")) != NULL)
     {
        siglist = evas_list_append(siglist, sig);
        evas_hash_modify(_etk_server_valid_signals, "text_changed", siglist);
     }
   else
     {
        siglist = evas_list_append(siglist, sig);
        _etk_server_valid_signals = evas_hash_add(_etk_server_valid_signals,  "text_changed", siglist);
     }
   siglist = NULL;
	    
   sig = calloc(1, sizeof(Etk_Server_Signal));
   sig->name = strdup("text_popped");
   sig->marshaller = strdup("etk_marshaller_VOID__INT_POINTER");	      
   if((siglist = evas_hash_find(_etk_server_valid_signals, "text_popped")) != NULL)
     {
        siglist = evas_list_append(siglist, sig);
        evas_hash_modify(_etk_server_valid_signals, "text_popped", siglist);
     }
   else
     {
        siglist = evas_list_append(siglist, sig);
        _etk_server_valid_signals = evas_hash_add(_etk_server_valid_signals,  "text_popped", siglist);
     }
   siglist = NULL;
	    
   sig = calloc(1, sizeof(Etk_Server_Signal));
   sig->name = strdup("popped_up");
   sig->marshaller = strdup("etk_marshaller_VOID__VOID");	      
   if((siglist = evas_hash_find(_etk_server_valid_signals, "popped_up")) != NULL)
     {
        siglist = evas_list_append(siglist, sig);
        evas_hash_modify(_etk_server_valid_signals, "popped_up", siglist);
     }
   else
     {
        siglist = evas_list_append(siglist, sig);
        _etk_server_valid_signals = evas_hash_add(_etk_server_valid_signals,  "popped_up", siglist);
     }
   siglist = NULL;
	    
   sig = calloc(1, sizeof(Etk_Server_Signal));
   sig->name = strdup("popped_down");
   sig->marshaller = strdup("etk_marshaller_VOID__VOID");	      
   if((siglist = evas_hash_find(_etk_server_valid_signals, "popped_down")) != NULL)
     {
        siglist = evas_list_append(siglist, sig);
        evas_hash_modify(_etk_server_valid_signals, "popped_down", siglist);
     }
   else
     {
        siglist = evas_list_append(siglist, sig);
        _etk_server_valid_signals = evas_hash_add(_etk_server_valid_signals,  "popped_down", siglist);
     }
   siglist = NULL;
	    
   sig = calloc(1, sizeof(Etk_Server_Signal));
   sig->name = strdup("selected");
   sig->marshaller = strdup("etk_marshaller_VOID__VOID");	      
   if((siglist = evas_hash_find(_etk_server_valid_signals, "selected")) != NULL)
     {
        siglist = evas_list_append(siglist, sig);
        evas_hash_modify(_etk_server_valid_signals, "selected", siglist);
     }
   else
     {
        siglist = evas_list_append(siglist, sig);
        _etk_server_valid_signals = evas_hash_add(_etk_server_valid_signals,  "selected", siglist);
     }
   siglist = NULL;
	    
   sig = calloc(1, sizeof(Etk_Server_Signal));
   sig->name = strdup("deselected");
   sig->marshaller = strdup("etk_marshaller_VOID__VOID");	      
   if((siglist = evas_hash_find(_etk_server_valid_signals, "deselected")) != NULL)
     {
        siglist = evas_list_append(siglist, sig);
        evas_hash_modify(_etk_server_valid_signals, "deselected", siglist);
     }
   else
     {
        siglist = evas_list_append(siglist, sig);
        _etk_server_valid_signals = evas_hash_add(_etk_server_valid_signals,  "deselected", siglist);
     }
   siglist = NULL;
	    
   sig = calloc(1, sizeof(Etk_Server_Signal));
   sig->name = strdup("activated");
   sig->marshaller = strdup("etk_marshaller_VOID__VOID");	      
   if((siglist = evas_hash_find(_etk_server_valid_signals, "activated")) != NULL)
     {
        siglist = evas_list_append(siglist, sig);
        evas_hash_modify(_etk_server_valid_signals, "activated", siglist);
     }
   else
     {
        siglist = evas_list_append(siglist, sig);
        _etk_server_valid_signals = evas_hash_add(_etk_server_valid_signals,  "activated", siglist);
     }
   siglist = NULL;
	    
   sig = calloc(1, sizeof(Etk_Server_Signal));
   sig->name = strdup("submenu_popped_down");
   sig->marshaller = strdup("etk_marshaller_VOID__VOID");	      
   if((siglist = evas_hash_find(_etk_server_valid_signals, "submenu_popped_down")) != NULL)
     {
        siglist = evas_list_append(siglist, sig);
        evas_hash_modify(_etk_server_valid_signals, "submenu_popped_down", siglist);
     }
   else
     {
        siglist = evas_list_append(siglist, sig);
        _etk_server_valid_signals = evas_hash_add(_etk_server_valid_signals,  "submenu_popped_down", siglist);
     }
   siglist = NULL;
	    
   sig = calloc(1, sizeof(Etk_Server_Signal));
   sig->name = strdup("toggled");
   sig->marshaller = strdup("etk_marshaller_VOID__VOID");	      
   if((siglist = evas_hash_find(_etk_server_valid_signals, "toggled")) != NULL)
     {
        siglist = evas_list_append(siglist, sig);
        evas_hash_modify(_etk_server_valid_signals, "toggled", siglist);
     }
   else
     {
        siglist = evas_list_append(siglist, sig);
        _etk_server_valid_signals = evas_hash_add(_etk_server_valid_signals,  "toggled", siglist);
     }
   siglist = NULL;
	    
   sig = calloc(1, sizeof(Etk_Server_Signal));
   sig->name = strdup("item_added");
   sig->marshaller = strdup("etk_marshaller_VOID__POINTER");	      
   if((siglist = evas_hash_find(_etk_server_valid_signals, "item_added")) != NULL)
     {
        siglist = evas_list_append(siglist, sig);
        evas_hash_modify(_etk_server_valid_signals, "item_added", siglist);
     }
   else
     {
        siglist = evas_list_append(siglist, sig);
        _etk_server_valid_signals = evas_hash_add(_etk_server_valid_signals,  "item_added", siglist);
     }
   siglist = NULL;
	    
   sig = calloc(1, sizeof(Etk_Server_Signal));
   sig->name = strdup("item_removed");
   sig->marshaller = strdup("etk_marshaller_VOID__POINTER");	      
   if((siglist = evas_hash_find(_etk_server_valid_signals, "item_removed")) != NULL)
     {
        siglist = evas_list_append(siglist, sig);
        evas_hash_modify(_etk_server_valid_signals, "item_removed", siglist);
     }
   else
     {
        siglist = evas_list_append(siglist, sig);
        _etk_server_valid_signals = evas_hash_add(_etk_server_valid_signals,  "item_removed", siglist);
     }
   siglist = NULL;
	    
   sig = calloc(1, sizeof(Etk_Server_Signal));
   sig->name = strdup("popped_up");
   sig->marshaller = strdup("etk_marshaller_VOID__VOID");	      
   if((siglist = evas_hash_find(_etk_server_valid_signals, "popped_up")) != NULL)
     {
        siglist = evas_list_append(siglist, sig);
        evas_hash_modify(_etk_server_valid_signals, "popped_up", siglist);
     }
   else
     {
        siglist = evas_list_append(siglist, sig);
        _etk_server_valid_signals = evas_hash_add(_etk_server_valid_signals,  "popped_up", siglist);
     }
   siglist = NULL;
	    
   sig = calloc(1, sizeof(Etk_Server_Signal));
   sig->name = strdup("popped_down");
   sig->marshaller = strdup("etk_marshaller_VOID__VOID");	      
   if((siglist = evas_hash_find(_etk_server_valid_signals, "popped_down")) != NULL)
     {
        siglist = evas_list_append(siglist, sig);
        evas_hash_modify(_etk_server_valid_signals, "popped_down", siglist);
     }
   else
     {
        siglist = evas_list_append(siglist, sig);
        _etk_server_valid_signals = evas_hash_add(_etk_server_valid_signals,  "popped_down", siglist);
     }
   siglist = NULL;
	    
   sig = calloc(1, sizeof(Etk_Server_Signal));
   sig->name = strdup("current_page_changed");
   sig->marshaller = strdup("etk_marshaller_VOID__VOID");	      
   if((siglist = evas_hash_find(_etk_server_valid_signals, "current_page_changed")) != NULL)
     {
        siglist = evas_list_append(siglist, sig);
        evas_hash_modify(_etk_server_valid_signals, "current_page_changed", siglist);
     }
   else
     {
        siglist = evas_list_append(siglist, sig);
        _etk_server_valid_signals = evas_hash_add(_etk_server_valid_signals,  "current_page_changed", siglist);
     }
   siglist = NULL;
	    
   sig = calloc(1, sizeof(Etk_Server_Signal));
   sig->name = strdup("destroyed");
   sig->marshaller = strdup("etk_marshaller_VOID__VOID");	      
   if((siglist = evas_hash_find(_etk_server_valid_signals, "destroyed")) != NULL)
     {
        siglist = evas_list_append(siglist, sig);
        evas_hash_modify(_etk_server_valid_signals, "destroyed", siglist);
     }
   else
     {
        siglist = evas_list_append(siglist, sig);
        _etk_server_valid_signals = evas_hash_add(_etk_server_valid_signals,  "destroyed", siglist);
     }
   siglist = NULL;
	    
   sig = calloc(1, sizeof(Etk_Server_Signal));
   sig->name = strdup("value_changed");
   sig->marshaller = strdup("etk_marshaller_VOID__DOUBLE");	      
   if((siglist = evas_hash_find(_etk_server_valid_signals, "value_changed")) != NULL)
     {
        siglist = evas_list_append(siglist, sig);
        evas_hash_modify(_etk_server_valid_signals, "value_changed", siglist);
     }
   else
     {
        siglist = evas_list_append(siglist, sig);
        _etk_server_valid_signals = evas_hash_add(_etk_server_valid_signals,  "value_changed", siglist);
     }
   siglist = NULL;
	    
   sig = calloc(1, sizeof(Etk_Server_Signal));
   sig->name = strdup("text_popped");
   sig->marshaller = strdup("etk_marshaller_VOID__INT_POINTER");	      
   if((siglist = evas_hash_find(_etk_server_valid_signals, "text_popped")) != NULL)
     {
        siglist = evas_list_append(siglist, sig);
        evas_hash_modify(_etk_server_valid_signals, "text_popped", siglist);
     }
   else
     {
        siglist = evas_list_append(siglist, sig);
        _etk_server_valid_signals = evas_hash_add(_etk_server_valid_signals,  "text_popped", siglist);
     }
   siglist = NULL;
	    
   sig = calloc(1, sizeof(Etk_Server_Signal));
   sig->name = strdup("text_pushed");
   sig->marshaller = strdup("etk_marshaller_VOID__INT_POINTER");	      
   if((siglist = evas_hash_find(_etk_server_valid_signals, "text_pushed")) != NULL)
     {
        siglist = evas_list_append(siglist, sig);
        evas_hash_modify(_etk_server_valid_signals, "text_pushed", siglist);
     }
   else
     {
        siglist = evas_list_append(siglist, sig);
        _etk_server_valid_signals = evas_hash_add(_etk_server_valid_signals,  "text_pushed", siglist);
     }
   siglist = NULL;
	    
   sig = calloc(1, sizeof(Etk_Server_Signal));
   sig->name = strdup("text_changed");
   sig->marshaller = strdup("etk_marshaller_VOID__VOID");	      
   if((siglist = evas_hash_find(_etk_server_valid_signals, "text_changed")) != NULL)
     {
        siglist = evas_list_append(siglist, sig);
        evas_hash_modify(_etk_server_valid_signals, "text_changed", siglist);
     }
   else
     {
        siglist = evas_list_append(siglist, sig);
        _etk_server_valid_signals = evas_hash_add(_etk_server_valid_signals,  "text_changed", siglist);
     }
   siglist = NULL;
	    
   sig = calloc(1, sizeof(Etk_Server_Signal));
   sig->name = strdup("toggled");
   sig->marshaller = strdup("etk_marshaller_VOID__VOID");	      
   if((siglist = evas_hash_find(_etk_server_valid_signals, "toggled")) != NULL)
     {
        siglist = evas_list_append(siglist, sig);
        evas_hash_modify(_etk_server_valid_signals, "toggled", siglist);
     }
   else
     {
        siglist = evas_list_append(siglist, sig);
        _etk_server_valid_signals = evas_hash_add(_etk_server_valid_signals,  "toggled", siglist);
     }
   siglist = NULL;
	    
   sig = calloc(1, sizeof(Etk_Server_Signal));
   sig->name = strdup("row_selected");
   sig->marshaller = strdup("etk_marshaller_VOID__POINTER");	      
   if((siglist = evas_hash_find(_etk_server_valid_signals, "row_selected")) != NULL)
     {
        siglist = evas_list_append(siglist, sig);
        evas_hash_modify(_etk_server_valid_signals, "row_selected", siglist);
     }
   else
     {
        siglist = evas_list_append(siglist, sig);
        _etk_server_valid_signals = evas_hash_add(_etk_server_valid_signals,  "row_selected", siglist);
     }
   siglist = NULL;
	    
   sig = calloc(1, sizeof(Etk_Server_Signal));
   sig->name = strdup("row_unselected");
   sig->marshaller = strdup("etk_marshaller_VOID__POINTER");	      
   if((siglist = evas_hash_find(_etk_server_valid_signals, "row_unselected")) != NULL)
     {
        siglist = evas_list_append(siglist, sig);
        evas_hash_modify(_etk_server_valid_signals, "row_unselected", siglist);
     }
   else
     {
        siglist = evas_list_append(siglist, sig);
        _etk_server_valid_signals = evas_hash_add(_etk_server_valid_signals,  "row_unselected", siglist);
     }
   siglist = NULL;
	    
   sig = calloc(1, sizeof(Etk_Server_Signal));
   sig->name = strdup("row_clicked");
   sig->marshaller = strdup("etk_marshaller_VOID__POINTER_POINTER");	      
   if((siglist = evas_hash_find(_etk_server_valid_signals, "row_clicked")) != NULL)
     {
        siglist = evas_list_append(siglist, sig);
        evas_hash_modify(_etk_server_valid_signals, "row_clicked", siglist);
     }
   else
     {
        siglist = evas_list_append(siglist, sig);
        _etk_server_valid_signals = evas_hash_add(_etk_server_valid_signals,  "row_clicked", siglist);
     }
   siglist = NULL;
	    
   sig = calloc(1, sizeof(Etk_Server_Signal));
   sig->name = strdup("row_activated");
   sig->marshaller = strdup("etk_marshaller_VOID__POINTER");	      
   if((siglist = evas_hash_find(_etk_server_valid_signals, "row_activated")) != NULL)
     {
        siglist = evas_list_append(siglist, sig);
        evas_hash_modify(_etk_server_valid_signals, "row_activated", siglist);
     }
   else
     {
        siglist = evas_list_append(siglist, sig);
        _etk_server_valid_signals = evas_hash_add(_etk_server_valid_signals,  "row_activated", siglist);
     }
   siglist = NULL;
	    
   sig = calloc(1, sizeof(Etk_Server_Signal));
   sig->name = strdup("row_expaned");
   sig->marshaller = strdup("etk_marshaller_VOID__POINTER");	      
   if((siglist = evas_hash_find(_etk_server_valid_signals, "row_expaned")) != NULL)
     {
        siglist = evas_list_append(siglist, sig);
        evas_hash_modify(_etk_server_valid_signals, "row_expaned", siglist);
     }
   else
     {
        siglist = evas_list_append(siglist, sig);
        _etk_server_valid_signals = evas_hash_add(_etk_server_valid_signals,  "row_expaned", siglist);
     }
   siglist = NULL;
	    
   sig = calloc(1, sizeof(Etk_Server_Signal));
   sig->name = strdup("row_collapsed");
   sig->marshaller = strdup("etk_marshaller_VOID__POINTER");	      
   if((siglist = evas_hash_find(_etk_server_valid_signals, "row_collapsed")) != NULL)
     {
        siglist = evas_list_append(siglist, sig);
        evas_hash_modify(_etk_server_valid_signals, "row_collapsed", siglist);
     }
   else
     {
        siglist = evas_list_append(siglist, sig);
        _etk_server_valid_signals = evas_hash_add(_etk_server_valid_signals,  "row_collapsed", siglist);
     }
   siglist = NULL;
	    
   sig = calloc(1, sizeof(Etk_Server_Signal));
   sig->name = strdup("row_mouse_in");
   sig->marshaller = strdup("etk_marshaller_VOID__POINTER");	      
   if((siglist = evas_hash_find(_etk_server_valid_signals, "row_mouse_in")) != NULL)
     {
        siglist = evas_list_append(siglist, sig);
        evas_hash_modify(_etk_server_valid_signals, "row_mouse_in", siglist);
     }
   else
     {
        siglist = evas_list_append(siglist, sig);
        _etk_server_valid_signals = evas_hash_add(_etk_server_valid_signals,  "row_mouse_in", siglist);
     }
   siglist = NULL;
	    
   sig = calloc(1, sizeof(Etk_Server_Signal));
   sig->name = strdup("row_mouse_out");
   sig->marshaller = strdup("etk_marshaller_VOID__POINTER");	      
   if((siglist = evas_hash_find(_etk_server_valid_signals, "row_mouse_out")) != NULL)
     {
        siglist = evas_list_append(siglist, sig);
        evas_hash_modify(_etk_server_valid_signals, "row_mouse_out", siglist);
     }
   else
     {
        siglist = evas_list_append(siglist, sig);
        _etk_server_valid_signals = evas_hash_add(_etk_server_valid_signals,  "row_mouse_out", siglist);
     }
   siglist = NULL;
	    
   sig = calloc(1, sizeof(Etk_Server_Signal));
   sig->name = strdup("row_mouse_move");
   sig->marshaller = strdup("etk_marshaller_VOID__POINTER");	      
   if((siglist = evas_hash_find(_etk_server_valid_signals, "row_mouse_move")) != NULL)
     {
        siglist = evas_list_append(siglist, sig);
        evas_hash_modify(_etk_server_valid_signals, "row_mouse_move", siglist);
     }
   else
     {
        siglist = evas_list_append(siglist, sig);
        _etk_server_valid_signals = evas_hash_add(_etk_server_valid_signals,  "row_mouse_move", siglist);
     }
   siglist = NULL;
	    
   sig = calloc(1, sizeof(Etk_Server_Signal));
   sig->name = strdup("select_all");
   sig->marshaller = strdup("etk_marshaller_VOID__VOID");	      
   if((siglist = evas_hash_find(_etk_server_valid_signals, "select_all")) != NULL)
     {
        siglist = evas_list_append(siglist, sig);
        evas_hash_modify(_etk_server_valid_signals, "select_all", siglist);
     }
   else
     {
        siglist = evas_list_append(siglist, sig);
        _etk_server_valid_signals = evas_hash_add(_etk_server_valid_signals,  "select_all", siglist);
     }
   siglist = NULL;
	    
   sig = calloc(1, sizeof(Etk_Server_Signal));
   sig->name = strdup("unselect_all");
   sig->marshaller = strdup("etk_marshaller_VOID__VOID");	      
   if((siglist = evas_hash_find(_etk_server_valid_signals, "unselect_all")) != NULL)
     {
        siglist = evas_list_append(siglist, sig);
        evas_hash_modify(_etk_server_valid_signals, "unselect_all", siglist);
     }
   else
     {
        siglist = evas_list_append(siglist, sig);
        _etk_server_valid_signals = evas_hash_add(_etk_server_valid_signals,  "unselect_all", siglist);
     }
   siglist = NULL;
	    
   sig = calloc(1, sizeof(Etk_Server_Signal));
   sig->name = strdup("cell_value_changed");
   sig->marshaller = strdup("etk_marshaller_VOID__POINTER");	      
   if((siglist = evas_hash_find(_etk_server_valid_signals, "cell_value_changed")) != NULL)
     {
        siglist = evas_list_append(siglist, sig);
        evas_hash_modify(_etk_server_valid_signals, "cell_value_changed", siglist);
     }
   else
     {
        siglist = evas_list_append(siglist, sig);
        _etk_server_valid_signals = evas_hash_add(_etk_server_valid_signals,  "cell_value_changed", siglist);
     }
   siglist = NULL;
	    
   sig = calloc(1, sizeof(Etk_Server_Signal));
   sig->name = strdup("show");
   sig->marshaller = strdup("etk_marshaller_VOID__VOID");	      
   if((siglist = evas_hash_find(_etk_server_valid_signals, "show")) != NULL)
     {
        siglist = evas_list_append(siglist, sig);
        evas_hash_modify(_etk_server_valid_signals, "show", siglist);
     }
   else
     {
        siglist = evas_list_append(siglist, sig);
        _etk_server_valid_signals = evas_hash_add(_etk_server_valid_signals,  "show", siglist);
     }
   siglist = NULL;
	    
   sig = calloc(1, sizeof(Etk_Server_Signal));
   sig->name = strdup("hide");
   sig->marshaller = strdup("etk_marshaller_VOID__VOID");	      
   if((siglist = evas_hash_find(_etk_server_valid_signals, "hide")) != NULL)
     {
        siglist = evas_list_append(siglist, sig);
        evas_hash_modify(_etk_server_valid_signals, "hide", siglist);
     }
   else
     {
        siglist = evas_list_append(siglist, sig);
        _etk_server_valid_signals = evas_hash_add(_etk_server_valid_signals,  "hide", siglist);
     }
   siglist = NULL;
	    
   sig = calloc(1, sizeof(Etk_Server_Signal));
   sig->name = strdup("realize");
   sig->marshaller = strdup("etk_marshaller_VOID__VOID");	      
   if((siglist = evas_hash_find(_etk_server_valid_signals, "realize")) != NULL)
     {
        siglist = evas_list_append(siglist, sig);
        evas_hash_modify(_etk_server_valid_signals, "realize", siglist);
     }
   else
     {
        siglist = evas_list_append(siglist, sig);
        _etk_server_valid_signals = evas_hash_add(_etk_server_valid_signals,  "realize", siglist);
     }
   siglist = NULL;
	    
   sig = calloc(1, sizeof(Etk_Server_Signal));
   sig->name = strdup("unrealize");
   sig->marshaller = strdup("etk_marshaller_VOID__POINTER");	      
   if((siglist = evas_hash_find(_etk_server_valid_signals, "unrealize")) != NULL)
     {
        siglist = evas_list_append(siglist, sig);
        evas_hash_modify(_etk_server_valid_signals, "unrealize", siglist);
     }
   else
     {
        siglist = evas_list_append(siglist, sig);
        _etk_server_valid_signals = evas_hash_add(_etk_server_valid_signals,  "unrealize", siglist);
     }
   siglist = NULL;
	    
   sig = calloc(1, sizeof(Etk_Server_Signal));
   sig->name = strdup("size_request");
   sig->marshaller = strdup("etk_marshaller_VOID__POINTER");	      
   if((siglist = evas_hash_find(_etk_server_valid_signals, "size_request")) != NULL)
     {
        siglist = evas_list_append(siglist, sig);
        evas_hash_modify(_etk_server_valid_signals, "size_request", siglist);
     }
   else
     {
        siglist = evas_list_append(siglist, sig);
        _etk_server_valid_signals = evas_hash_add(_etk_server_valid_signals,  "size_request", siglist);
     }
   siglist = NULL;
	    
   sig = calloc(1, sizeof(Etk_Server_Signal));
   sig->name = strdup("mouse_in");
   sig->marshaller = strdup("etk_marshaller_VOID__POINTER");	      
   if((siglist = evas_hash_find(_etk_server_valid_signals, "mouse_in")) != NULL)
     {
        siglist = evas_list_append(siglist, sig);
        evas_hash_modify(_etk_server_valid_signals, "mouse_in", siglist);
     }
   else
     {
        siglist = evas_list_append(siglist, sig);
        _etk_server_valid_signals = evas_hash_add(_etk_server_valid_signals,  "mouse_in", siglist);
     }
   siglist = NULL;
	    
   sig = calloc(1, sizeof(Etk_Server_Signal));
   sig->name = strdup("mouse_out");
   sig->marshaller = strdup("etk_marshaller_VOID__POINTER");	      
   if((siglist = evas_hash_find(_etk_server_valid_signals, "mouse_out")) != NULL)
     {
        siglist = evas_list_append(siglist, sig);
        evas_hash_modify(_etk_server_valid_signals, "mouse_out", siglist);
     }
   else
     {
        siglist = evas_list_append(siglist, sig);
        _etk_server_valid_signals = evas_hash_add(_etk_server_valid_signals,  "mouse_out", siglist);
     }
   siglist = NULL;
	    
   sig = calloc(1, sizeof(Etk_Server_Signal));
   sig->name = strdup("mouse_move");
   sig->marshaller = strdup("etk_marshaller_VOID__POINTER");	      
   if((siglist = evas_hash_find(_etk_server_valid_signals, "mouse_move")) != NULL)
     {
        siglist = evas_list_append(siglist, sig);
        evas_hash_modify(_etk_server_valid_signals, "mouse_move", siglist);
     }
   else
     {
        siglist = evas_list_append(siglist, sig);
        _etk_server_valid_signals = evas_hash_add(_etk_server_valid_signals,  "mouse_move", siglist);
     }
   siglist = NULL;
	    
   sig = calloc(1, sizeof(Etk_Server_Signal));
   sig->name = strdup("mouse_down");
   sig->marshaller = strdup("etk_marshaller_VOID__POINTER");	      
   if((siglist = evas_hash_find(_etk_server_valid_signals, "mouse_down")) != NULL)
     {
        siglist = evas_list_append(siglist, sig);
        evas_hash_modify(_etk_server_valid_signals, "mouse_down", siglist);
     }
   else
     {
        siglist = evas_list_append(siglist, sig);
        _etk_server_valid_signals = evas_hash_add(_etk_server_valid_signals,  "mouse_down", siglist);
     }
   siglist = NULL;
	    
   sig = calloc(1, sizeof(Etk_Server_Signal));
   sig->name = strdup("mouse_up");
   sig->marshaller = strdup("etk_marshaller_VOID__POINTER");	      
   if((siglist = evas_hash_find(_etk_server_valid_signals, "mouse_up")) != NULL)
     {
        siglist = evas_list_append(siglist, sig);
        evas_hash_modify(_etk_server_valid_signals, "mouse_up", siglist);
     }
   else
     {
        siglist = evas_list_append(siglist, sig);
        _etk_server_valid_signals = evas_hash_add(_etk_server_valid_signals,  "mouse_up", siglist);
     }
   siglist = NULL;
	    
   sig = calloc(1, sizeof(Etk_Server_Signal));
   sig->name = strdup("mouse_click");
   sig->marshaller = strdup("etk_marshaller_VOID__POINTER");	      
   if((siglist = evas_hash_find(_etk_server_valid_signals, "mouse_click")) != NULL)
     {
        siglist = evas_list_append(siglist, sig);
        evas_hash_modify(_etk_server_valid_signals, "mouse_click", siglist);
     }
   else
     {
        siglist = evas_list_append(siglist, sig);
        _etk_server_valid_signals = evas_hash_add(_etk_server_valid_signals,  "mouse_click", siglist);
     }
   siglist = NULL;
	    
   sig = calloc(1, sizeof(Etk_Server_Signal));
   sig->name = strdup("mouse_wheel");
   sig->marshaller = strdup("etk_marshaller_VOID__POINTER");	      
   if((siglist = evas_hash_find(_etk_server_valid_signals, "mouse_wheel")) != NULL)
     {
        siglist = evas_list_append(siglist, sig);
        evas_hash_modify(_etk_server_valid_signals, "mouse_wheel", siglist);
     }
   else
     {
        siglist = evas_list_append(siglist, sig);
        _etk_server_valid_signals = evas_hash_add(_etk_server_valid_signals,  "mouse_wheel", siglist);
     }
   siglist = NULL;
	    
   sig = calloc(1, sizeof(Etk_Server_Signal));
   sig->name = strdup("key_down");
   sig->marshaller = strdup("etk_marshaller_VOID__POINTER");	      
   if((siglist = evas_hash_find(_etk_server_valid_signals, "key_down")) != NULL)
     {
        siglist = evas_list_append(siglist, sig);
        evas_hash_modify(_etk_server_valid_signals, "key_down", siglist);
     }
   else
     {
        siglist = evas_list_append(siglist, sig);
        _etk_server_valid_signals = evas_hash_add(_etk_server_valid_signals,  "key_down", siglist);
     }
   siglist = NULL;
	    
   sig = calloc(1, sizeof(Etk_Server_Signal));
   sig->name = strdup("key_up");
   sig->marshaller = strdup("etk_marshaller_VOID__POINTER");	      
   if((siglist = evas_hash_find(_etk_server_valid_signals, "key_up")) != NULL)
     {
        siglist = evas_list_append(siglist, sig);
        evas_hash_modify(_etk_server_valid_signals, "key_up", siglist);
     }
   else
     {
        siglist = evas_list_append(siglist, sig);
        _etk_server_valid_signals = evas_hash_add(_etk_server_valid_signals,  "key_up", siglist);
     }
   siglist = NULL;
	    
   sig = calloc(1, sizeof(Etk_Server_Signal));
   sig->name = strdup("enter");
   sig->marshaller = strdup("etk_marshaller_VOID__VOID");	      
   if((siglist = evas_hash_find(_etk_server_valid_signals, "enter")) != NULL)
     {
        siglist = evas_list_append(siglist, sig);
        evas_hash_modify(_etk_server_valid_signals, "enter", siglist);
     }
   else
     {
        siglist = evas_list_append(siglist, sig);
        _etk_server_valid_signals = evas_hash_add(_etk_server_valid_signals,  "enter", siglist);
     }
   siglist = NULL;
	    
   sig = calloc(1, sizeof(Etk_Server_Signal));
   sig->name = strdup("leave");
   sig->marshaller = strdup("etk_marshaller_VOID__VOID");	      
   if((siglist = evas_hash_find(_etk_server_valid_signals, "leave")) != NULL)
     {
        siglist = evas_list_append(siglist, sig);
        evas_hash_modify(_etk_server_valid_signals, "leave", siglist);
     }
   else
     {
        siglist = evas_list_append(siglist, sig);
        _etk_server_valid_signals = evas_hash_add(_etk_server_valid_signals,  "leave", siglist);
     }
   siglist = NULL;
	    
   sig = calloc(1, sizeof(Etk_Server_Signal));
   sig->name = strdup("focus");
   sig->marshaller = strdup("etk_marshaller_VOID__VOID");	      
   if((siglist = evas_hash_find(_etk_server_valid_signals, "focus")) != NULL)
     {
        siglist = evas_list_append(siglist, sig);
        evas_hash_modify(_etk_server_valid_signals, "focus", siglist);
     }
   else
     {
        siglist = evas_list_append(siglist, sig);
        _etk_server_valid_signals = evas_hash_add(_etk_server_valid_signals,  "focus", siglist);
     }
   siglist = NULL;
	    
   sig = calloc(1, sizeof(Etk_Server_Signal));
   sig->name = strdup("unfocus");
   sig->marshaller = strdup("etk_marshaller_VOID__VOID");	      
   if((siglist = evas_hash_find(_etk_server_valid_signals, "unfocus")) != NULL)
     {
        siglist = evas_list_append(siglist, sig);
        evas_hash_modify(_etk_server_valid_signals, "unfocus", siglist);
     }
   else
     {
        siglist = evas_list_append(siglist, sig);
        _etk_server_valid_signals = evas_hash_add(_etk_server_valid_signals,  "unfocus", siglist);
     }
   siglist = NULL;
	    
   sig = calloc(1, sizeof(Etk_Server_Signal));
   sig->name = strdup("drag_drop");
   sig->marshaller = strdup("etk_marshaller_VOID__POINTER");	      
   if((siglist = evas_hash_find(_etk_server_valid_signals, "drag_drop")) != NULL)
     {
        siglist = evas_list_append(siglist, sig);
        evas_hash_modify(_etk_server_valid_signals, "drag_drop", siglist);
     }
   else
     {
        siglist = evas_list_append(siglist, sig);
        _etk_server_valid_signals = evas_hash_add(_etk_server_valid_signals,  "drag_drop", siglist);
     }
   siglist = NULL;
	    
   sig = calloc(1, sizeof(Etk_Server_Signal));
   sig->name = strdup("drag_motion");
   sig->marshaller = strdup("etk_marshaller_VOID__VOID");	      
   if((siglist = evas_hash_find(_etk_server_valid_signals, "drag_motion")) != NULL)
     {
        siglist = evas_list_append(siglist, sig);
        evas_hash_modify(_etk_server_valid_signals, "drag_motion", siglist);
     }
   else
     {
        siglist = evas_list_append(siglist, sig);
        _etk_server_valid_signals = evas_hash_add(_etk_server_valid_signals,  "drag_motion", siglist);
     }
   siglist = NULL;
	    
   sig = calloc(1, sizeof(Etk_Server_Signal));
   sig->name = strdup("drag_enter");
   sig->marshaller = strdup("etk_marshaller_VOID__VOID");	      
   if((siglist = evas_hash_find(_etk_server_valid_signals, "drag_enter")) != NULL)
     {
        siglist = evas_list_append(siglist, sig);
        evas_hash_modify(_etk_server_valid_signals, "drag_enter", siglist);
     }
   else
     {
        siglist = evas_list_append(siglist, sig);
        _etk_server_valid_signals = evas_hash_add(_etk_server_valid_signals,  "drag_enter", siglist);
     }
   siglist = NULL;
	    
   sig = calloc(1, sizeof(Etk_Server_Signal));
   sig->name = strdup("drag_leave");
   sig->marshaller = strdup("etk_marshaller_VOID__VOID");	      
   if((siglist = evas_hash_find(_etk_server_valid_signals, "drag_leave")) != NULL)
     {
        siglist = evas_list_append(siglist, sig);
        evas_hash_modify(_etk_server_valid_signals, "drag_leave", siglist);
     }
   else
     {
        siglist = evas_list_append(siglist, sig);
        _etk_server_valid_signals = evas_hash_add(_etk_server_valid_signals,  "drag_leave", siglist);
     }
   siglist = NULL;
	    
   sig = calloc(1, sizeof(Etk_Server_Signal));
   sig->name = strdup("drag_begin");
   sig->marshaller = strdup("etk_marshaller_VOID__VOID");	      
   if((siglist = evas_hash_find(_etk_server_valid_signals, "drag_begin")) != NULL)
     {
        siglist = evas_list_append(siglist, sig);
        evas_hash_modify(_etk_server_valid_signals, "drag_begin", siglist);
     }
   else
     {
        siglist = evas_list_append(siglist, sig);
        _etk_server_valid_signals = evas_hash_add(_etk_server_valid_signals,  "drag_begin", siglist);
     }
   siglist = NULL;
	    
   sig = calloc(1, sizeof(Etk_Server_Signal));
   sig->name = strdup("drag_end");
   sig->marshaller = strdup("etk_marshaller_VOID__VOID");	      
   if((siglist = evas_hash_find(_etk_server_valid_signals, "drag_end")) != NULL)
     {
        siglist = evas_list_append(siglist, sig);
        evas_hash_modify(_etk_server_valid_signals, "drag_end", siglist);
     }
   else
     {
        siglist = evas_list_append(siglist, sig);
        _etk_server_valid_signals = evas_hash_add(_etk_server_valid_signals,  "drag_end", siglist);
     }
   siglist = NULL;
	    
   sig = calloc(1, sizeof(Etk_Server_Signal));
   sig->name = strdup("selection_received");
   sig->marshaller = strdup("etk_marshaller_VOID__POINTER");	      
   if((siglist = evas_hash_find(_etk_server_valid_signals, "selection_received")) != NULL)
     {
        siglist = evas_list_append(siglist, sig);
        evas_hash_modify(_etk_server_valid_signals, "selection_received", siglist);
     }
   else
     {
        siglist = evas_list_append(siglist, sig);
        _etk_server_valid_signals = evas_hash_add(_etk_server_valid_signals,  "selection_received", siglist);
     }
   siglist = NULL;
	    
   sig = calloc(1, sizeof(Etk_Server_Signal));
   sig->name = strdup("clipboard_received");
   sig->marshaller = strdup("etk_marshaller_VOID__POINTER");	      
   if((siglist = evas_hash_find(_etk_server_valid_signals, "clipboard_received")) != NULL)
     {
        siglist = evas_list_append(siglist, sig);
        evas_hash_modify(_etk_server_valid_signals, "clipboard_received", siglist);
     }
   else
     {
        siglist = evas_list_append(siglist, sig);
        _etk_server_valid_signals = evas_hash_add(_etk_server_valid_signals,  "clipboard_received", siglist);
     }
   siglist = NULL;
	    
   sig = calloc(1, sizeof(Etk_Server_Signal));
   sig->name = strdup("scroll_size_changed");
   sig->marshaller = strdup("etk_marshaller_VOID__VOID");	      
   if((siglist = evas_hash_find(_etk_server_valid_signals, "scroll_size_changed")) != NULL)
     {
        siglist = evas_list_append(siglist, sig);
        evas_hash_modify(_etk_server_valid_signals, "scroll_size_changed", siglist);
     }
   else
     {
        siglist = evas_list_append(siglist, sig);
        _etk_server_valid_signals = evas_hash_add(_etk_server_valid_signals,  "scroll_size_changed", siglist);
     }
   siglist = NULL;
	    
   sig = calloc(1, sizeof(Etk_Server_Signal));
   sig->name = strdup("move");
   sig->marshaller = strdup("etk_marshaller_VOID__VOID");	      
   if((siglist = evas_hash_find(_etk_server_valid_signals, "move")) != NULL)
     {
        siglist = evas_list_append(siglist, sig);
        evas_hash_modify(_etk_server_valid_signals, "move", siglist);
     }
   else
     {
        siglist = evas_list_append(siglist, sig);
        _etk_server_valid_signals = evas_hash_add(_etk_server_valid_signals,  "move", siglist);
     }
   siglist = NULL;
	    
   sig = calloc(1, sizeof(Etk_Server_Signal));
   sig->name = strdup("resize");
   sig->marshaller = strdup("etk_marshaller_VOID__VOID");	      
   if((siglist = evas_hash_find(_etk_server_valid_signals, "resize")) != NULL)
     {
        siglist = evas_list_append(siglist, sig);
        evas_hash_modify(_etk_server_valid_signals, "resize", siglist);
     }
   else
     {
        siglist = evas_list_append(siglist, sig);
        _etk_server_valid_signals = evas_hash_add(_etk_server_valid_signals,  "resize", siglist);
     }
   siglist = NULL;
	    
   sig = calloc(1, sizeof(Etk_Server_Signal));
   sig->name = strdup("focus_in");
   sig->marshaller = strdup("etk_marshaller_VOID__VOID");	      
   if((siglist = evas_hash_find(_etk_server_valid_signals, "focus_in")) != NULL)
     {
        siglist = evas_list_append(siglist, sig);
        evas_hash_modify(_etk_server_valid_signals, "focus_in", siglist);
     }
   else
     {
        siglist = evas_list_append(siglist, sig);
        _etk_server_valid_signals = evas_hash_add(_etk_server_valid_signals,  "focus_in", siglist);
     }
   siglist = NULL;
	    
   sig = calloc(1, sizeof(Etk_Server_Signal));
   sig->name = strdup("focus_out");
   sig->marshaller = strdup("etk_marshaller_VOID__VOID");	      
   if((siglist = evas_hash_find(_etk_server_valid_signals, "focus_out")) != NULL)
     {
        siglist = evas_list_append(siglist, sig);
        evas_hash_modify(_etk_server_valid_signals, "focus_out", siglist);
     }
   else
     {
        siglist = evas_list_append(siglist, sig);
        _etk_server_valid_signals = evas_hash_add(_etk_server_valid_signals,  "focus_out", siglist);
     }
   siglist = NULL;
	    
   sig = calloc(1, sizeof(Etk_Server_Signal));
   sig->name = strdup("delete_event");
   sig->marshaller = strdup("etk_marshaller_BOOL__VOID");	      
   if((siglist = evas_hash_find(_etk_server_valid_signals, "delete_event")) != NULL)
     {
        siglist = evas_list_append(siglist, sig);
        evas_hash_modify(_etk_server_valid_signals, "delete_event", siglist);
     }
   else
     {
        siglist = evas_list_append(siglist, sig);
        _etk_server_valid_signals = evas_hash_add(_etk_server_valid_signals,  "delete_event", siglist);
     }
   siglist = NULL;
	    
}
