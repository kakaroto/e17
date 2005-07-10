#ifndef _ECLAIR_TYPES_H_
#define _ECLAIR_TYPES_H_

typedef enum _Eclair_State Eclair_State;
typedef enum _Eclair_Engine Eclair_Engine;
typedef enum _Eclair_Drop_Object Eclair_Drop_Object;
typedef enum _Eclair_Add_File_State Eclair_Add_File_State;
typedef enum _Eclair_Dialog_File_Chooser_Type Eclair_Dialog_File_Chooser_Type;

typedef struct _Eclair_Color Eclair_Color;
typedef struct _Eclair_Media_File Eclair_Media_File;
typedef struct _Eclair_Playlist Eclair_Playlist;
typedef struct _Eclair_Playlist_Container Eclair_Playlist_Container; 
typedef struct _Eclair_Playlist_Container_Object Eclair_Playlist_Container_Object;
typedef struct _Eclair_Subtitle Eclair_Subtitle;
typedef struct _Eclair_Subtitles Eclair_Subtitles;
typedef struct _Eclair_Meta_Tag_Manager Eclair_Meta_Tag_Manager;
typedef struct _Eclair_Cover_Manager Eclair_Cover_Manager;
typedef struct _Eclair_Config Eclair_Config;
typedef struct _Eclair_Dialogs_Manager Eclair_Dialogs_Manager;
typedef struct _Eclair_Database Eclair_Database;
typedef struct _Eclair_Window Eclair_Window;
typedef struct _Eclair_Video Eclair_Video;
typedef struct _Eclair Eclair;

enum _Eclair_State
{
   ECLAIR_PLAYING = 0,
   ECLAIR_PAUSE,
   ECLAIR_STOP
};

enum _Eclair_Engine
{
   ECLAIR_SOFTWARE = 0,
   ECLAIR_GL
};

enum _Eclair_Drop_Object
{
   ECLAIR_DROP_NONE = 0,
   ECLAIR_DROP_COVER,
   ECLAIR_DROP_PLAYLIST
};

enum _Eclair_Add_File_State
{
   ECLAIR_IDLE = 0,
   ECLAIR_ADDING_FILE_TO_ADD,
   ECLAIR_ADDING_FILE_TO_TREAT
};

enum _Eclair_Dialog_File_Chooser_Type
{
   ECLAIR_FC_NONE = 0,
   ECLAIR_FC_ADD_FILES,
   ECLAIR_FC_LOAD_PLAYLIST,
   ECLAIR_FC_SAVE_PLAYLIST
};

struct _Eclair_Color
{
   int r, g, b, a;
};

#endif
