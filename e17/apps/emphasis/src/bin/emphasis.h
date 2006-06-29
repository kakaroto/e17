#ifndef _EMPHASIS_H_
#define _EMPHASIS_H_

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "config.h"

#include <libmpd/libmpd.h>

#include <Ecore.h>
#include <Ecore_File.h>
#include <Evas.h>
#include <Eet.h>
#include <etk/Etk.h>

#include "emphasis_mpc.h"
#include "emphasis_config.h"
#include "emphasis_gui.h"
#include "emphasis_misc.h"
#include "emphasis_callbacks.h"

extern MpdObj *mo;

void emphasis_init_connect(Emphasis_Gui *gui);
int emphasis_try_connect(void *data);

#endif /*_EMPHASIS_H_*/

