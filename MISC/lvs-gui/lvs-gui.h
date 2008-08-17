/**********************************************************************
 * ipvs-gui.h
 * Copyright (C) 1999 Carsten Haitzler and Simon Horman
 * 
 * Permission is hereby granted, free of charge, to any person
 * obtaining a copy of this software and associated documentation
 * files (the "Software"), to deal in the Software without
 * restriction, including without limitation the rights to use, copy,
 * modify, merge, publish, distribute, sublicense, and/or sell copies
 * of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 * 
 * The above copyright notice and this permission notice shall be
 * included in all copies of the Software, its documentation and
 * marketing & publicity materials, and acknowledgment shall be given
 * in the documentation, materials and software packages that this
 * Software was used.
 * 
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
 * EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF
 * MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND
 * NONINFRINGEMENT.  IN NO EVENT SHALL THE AUTHORS BE LIABLE FOR ANY
 * CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF
 * CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION
 * WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
 **********************************************************************/

#ifndef IPVS_GUI_FLIM
#define IPVS_GUI_FLIM

#include "util.h"
#include "lvs-gui_types.h"
#include "config_file.h"
#include "options.h"

#ifdef WITH_LOGO
#include "va.xpm"
#endif

void load_config(char *file);
void save_transparent_proxy(char *host, GList *hosts);
void save_config(char *file);
void start_ipvs(char *machine);
/* void stop_ipvs(char *machine); */
void start_transparent_proxy(char *machine);
/* void stop_transparent_proxy(char *machine); */
void remote_cp(char *machine1, char *file1, char *machine2, char *file2);
void gui_save_config(void);
void options_to_disk(void);

#endif
