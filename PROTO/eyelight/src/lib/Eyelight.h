/*
 * =====================================================================================
 *
 *       Filename:  eyelight.h
 *
 *    Description:
 *
 *        Version:  1.0
 *        Created:  25/06/08 12:06:42 UTC
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  (Watchwolf), Atton Jonathan <watchwolf@watchwolf.fr>
 *        Company:
 *
 * =====================================================================================
 */

#ifndef  EYELIGHT_INC
#define  EYELIGHT_INC

#include <Evas.h>
#include <Ecore.h>
#include <Ecore_Evas.h>
#include <Ecore_File.h>
#include <stdlib.h>
#include <stdio.h>
#include <Edje.h>
#include <string.h>
#include <sys/wait.h>
#define EYELIGHT_BUFLEN 2048

#define EYELIGHT_FREE(a) do{if(a) {free(a); a=NULL;}}while(0)
#define EYELIGHT_TMP_FILE "/tmp/"
#define EYELIGHT_TMP_RELATIVE_ROOT "../"



#include "eyelight_compiler_common.h"
#include "eyelight_viewer.h"

#endif   /* ----- #ifndef EYELIGHT_INC  ----- */

