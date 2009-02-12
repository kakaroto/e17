/*
 * =====================================================================================
 *
 *       Filename:  pnl_right.h
 *
 *    Description:
 *
 *        Version:  1.0
 *        Created:  12/02/09 17:55:20 UTC
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  (Watchwolf), Atton Jonathan <watchwolf@watchwolf.fr>
 *        Company:
 *
 * =====================================================================================
 */

#ifndef  PNL_RIGHT_INC
#define  PNL_RIGHT_INC

#include "main.h"

struct _pnl_right
{
    Evas_Object* background;
    Evas_Object* btn_wired;
    Evas_Object* btn_center;
};

Pnl_Right pnl_right;


void pnl_right_create();

#endif   /* ----- #ifndef PNL_RIGHT_INC  ----- */

