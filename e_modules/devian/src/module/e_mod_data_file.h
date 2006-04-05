#ifdef E_MOD_DEVIAN_TYPEDEFS

typedef struct _Data_File_Block Data_File_Block;

#else

#ifndef E_MOD_DATA_FILE_H_INCLUDED
#define E_MOD_DATA_FILE_H_INCLUDED

#define DATA_FILE_PATH_DEFAULT "/home/kiwi/tmp/messages"
#define DATA_FILE_BUF_SIZE 128
#define DATA_FILE_AUTO_SCROLL_DEFAULT 1
#define DATA_FILE_FONT_MIN 10
#define DATA_FILE_FONT_MAX 15
#define DATA_FILE_NB_LINES_INI_MIN 5
#define DATA_FILE_NB_LINES_INI_DEFAULT 10
#define DATA_FILE_NB_LINES_INI_MAX 20
#define DATA_FILE_NB_LINES_MAX_MIN 10
#define DATA_FILE_NB_LINES_MAX_DEFAULT 30
#define DATA_FILE_NB_LINES_MAX_MAX 200

struct _Data_File_Block
{
   char *buf;
   int size;
   Ecore_List *retlines;/** <Position of the retlines in the buffer */
};

int DEVIANF(data_file_add) (Source_File *source);
void DEVIANF(data_file_del) (Source_File *source);

int DEVIANF(data_file_update) (Source_File *source, int option);

#endif
#endif
