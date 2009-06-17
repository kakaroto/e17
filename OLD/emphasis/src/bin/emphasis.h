#ifndef EMPHASIS_H_
#define EMPHASIS_H_

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <pthread.h>
#include "config.h"

#include <libmpd/libmpd.h>

#include <Ecore.h>
#include <Evas.h>
#include <etk/Etk.h>
#include <Enhance.h>

#include "emphasis_data_list.h"
#include "emphasis_player.h"
#include "emphasis_mpc.h"
#include "emphasis_config.h"
#include "emphasis_config_gui.h"
#include "emphasis_gui.h"
#include "emphasis_media.h"
#include "emphasis_convert.h"
#include "emphasis_misc.h"
#include "emphasis_callbacks.h"
#include "emphasis_search.h"
#include "emphasis_cover.h"
#include "emphasis_cover_db.h"

#define ETK_TREE_COL_GET(tree, num) etk_tree_nth_col_get(ETK_TREE(tree), num) 
#define UNUSED(var) (void)(var);
#define UNUSED_CLICKED_PARAM (void)(object); (void)(data);

extern MpdObj *mo;

void emphasis_pref_init(void *data);
void emphasis_init_connect(Emphasis_Gui *gui);
int emphasis_try_connect(void *data);

#endif /* EMPHASIS_H_ */
