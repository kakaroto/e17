/*
 * =====================================================================================
 *
 *       Filename:  eyelight_common.h
 *
 *    Description:
 *
 *        Version:  1.0
 *        Created:  08/14/2009 01:55:47 PM EDT
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  (Watchwolf), Atton Jonathan <watchwolf@watchwolf.fr>
 *        Company:
 *
 * =====================================================================================
 */

#ifndef  EYELIGHT_COMMON_INC
#define  EYELIGHT_COMMON_INC

#define EYELIGHT_BUFLEN 2048

#define EYELIGHT_FREE(a) do{if(a) {free(a); a=NULL;}}while(0)
#define EYELIGHT_TMP_FILE "/tmp/"
#define EYELIGHT_TMP_RELATIVE_ROOT "../"

typedef enum Eyelight_Viewer_State Eyelight_Viewer_State;

enum Eyelight_Viewer_State
{
    EYELIGHT_VIEWER_STATE_DEFAULT,
    EYELIGHT_VIEWER_STATE_EXPOSE,
    EYELIGHT_VIEWER_STATE_SLIDESHOW,
    EYELIGHT_VIEWER_STATE_GOTOSLIDE,
    EYELIGHT_VIEWER_STATE_TABLEOFCONTENTS
};



#endif   /* ----- #ifndef EYELIGHT_COMMON_INC  ----- */

