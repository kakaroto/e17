/**************************************************************************
 * ebindings.h
 * Project: Ebindings
 * Programmer: Corey Donohoe<atmos@atmos.org>
 * October 10, 2001
 *************************************************************************/
#ifndef __EBINDINGS_H_
#define __EBINDINGS_H_

#include <Evas.h>
#include <stdio.h>
#include "config.h"
#include <string.h>
#include <stdlib.h>
#include <gtk/gtk.h>
#include "callbacks.h"

#define UN(_ptr) _ptr = 0
#define MOD_STR_MAX 16
#define IF_FREE(_ptr) if(_ptr) free(_ptr)
#endif
