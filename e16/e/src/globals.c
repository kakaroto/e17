/*
 * Copyright (C) 2000-2004 Carsten Haitzler, Geoff Harrison and various contributors
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to
 * deal in the Software without restriction, including without limitation the
 * rights to use, copy, modify, merge, publish, distribute, sublicense, and/or
 * sell copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in
 * all copies of the Software, its documentation and marketing & publicity
 * materials, and acknowledgment shall be given in the documentation, materials
 * and software packages that this Software was used.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL
 * THE AUTHORS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER
 * IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN
 * CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
 */
#include "E.h"

const char          e_wm_name[] = "Enlightenment";
const char          e_wm_version[] =
#ifdef ENLIGHTENMENT_RELEASE
   "enlightenment-" ENLIGHTENMENT_VERSION "-" ENLIGHTENMENT_RELEASE;
#else
   "enlightenment-" ENLIGHTENMENT_VERSION;
#endif
Display            *disp;

#if !USE_IMLIB2
ImlibData          *pImlib_Context;
ImlibData          *prImlib_Context;
ImlibData          *pI1Ctx;
ImlibImage         *pIcImg;
Drawable            vIcDrw;
#endif
#if USE_FNLIB
FnlibData          *pFnlibData;
#endif
List               *lists;
Root                root;
EConf               Conf;
EMode               Mode;
Desktops            desks;
Window              init_win1 = 0;
Window              init_win2 = 0;
Window              init_win_ext = 0;
char                themepath[FILEPATH_LEN_MAX];

char                queue_up;
char                no_overwrite = 0;
char                clickmenu = 0;
int                 child_count = 0;
pid_t              *e_children = NULL;
int                 numlock_mask = 0;
int                 scrollock_mask = 0;
int                 mask_mod_combos[8];
Group              *current_group;
const char         *dstr = NULL;
char               *e_machine_name = NULL;

#ifdef DEBUG
int                 call_level = 0;
char               *call_stack[1024];
#endif
