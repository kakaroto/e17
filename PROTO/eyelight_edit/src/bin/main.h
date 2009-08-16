/*
 * =====================================================================================
 *
 *       Filename:  main.h
 *
 *    Description:
 *
 *        Version:  1.0
 *        Created:  06/29/2009 02:12:03 PM EDT
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  (Watchwolf), Atton Jonathan <watchwolf@watchwolf.fr>
 *        Company:
 *
 * =====================================================================================
 */

#ifndef  MAIN_INC
#define  MAIN_INC

#include <Elementary.h>
#include <Eyelight_Smart.h>
#include <Eyelight.h>
#include <Eyelight_Edit.h>

/* we use 2 presentations,
 * one is displayed in the center
 * and the second is used to display the slides in the left list
 */
Evas_Object *pres;

Evas_Object *win;


typedef struct Evas_Object_Couple Evas_Object_Couple;

struct Evas_Object_Couple
{
    Evas_Object *left;
    Evas_Object *right;
};
#include "slides_list.h"
#include "presentation.h"

Evas_Object *toolbar_create();

Evas_Object *slideshow_create();

#endif   /* ----- #ifndef MAIN_INC  ----- */

