/******************************************************************************
**  Copyright (c) 2006-2010, Calaos. All Rights Reserved.
**
**  This is free software; you can redistribute it and/or modify
**  it under the terms of the GNU General Public License as published by
**  the Free Software Foundation; either version 3 of the License, or
**  (at your option) any later version.
**
**  This is distributed in the hope that it will be useful,
**  but WITHOUT ANY WARRANTY; without even the implied warranty of
**  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
**  GNU General Public License for more details.
**
**  You should have received a copy of the GNU General Public License
**  along with this; if not, write to the Free Software
**  Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301  USA
**
******************************************************************************/

#ifndef COMMON_INC
#define COMMON_INC

#include <stdlib.h>
#include <time.h>

#include <Evas.h>
#include <Ecore.h>
#include <Ecore_Getopt.h>
#include <Ecore_Evas.h>
#include <Eina.h>
#include <Edje.h>

#include <config.h>

#define DEFAULT_WIDTH      1024
#define DEFAULT_HEIGHT     768

#define PI 3.14159265358979323846

#define swap_int(i1, i2) \
        { int tmp = i1; i1 = i2; i2 = tmp; }

extern int _drawin_log_domain;

#ifdef ERR
# undef ERR
#endif
#define ERR(...) EINA_LOG_DOM_ERR(_drawin_log_domain, __VA_ARGS__)
#ifdef INF
# undef INF
#endif
#define INF(...) EINA_LOG_DOM_INFO(_drawin_log_domain, __VA_ARGS__)
#ifdef WARN
# undef WARN
#endif
#define WARN(...) EINA_LOG_DOM_WARN(_drawin_log_domain, __VA_ARGS__)

typedef struct _DrawPoint
{
        int x, y;

} DrawPoint;

static const DrawPoint point_zero = { 0, 0 };

static inline DrawPoint drawpoint(const int x, const int y)
{
        DrawPoint p = { x, y };
        return p;
}

#endif
