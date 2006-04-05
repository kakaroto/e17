#include "dEvian.h"

#ifdef HAVE_FILE

static Evas_Object *_create_tb(FILE *fd);

static Ecore_List *_get_lines(char *buffer, int *size);
static int _get_file_first(Source_File *source);
static int _get_file_update(Source_File *source);
static int _set_text_tb(Source_File *source);

static void _cb_destroy_block(void *data);
static void _cb_destroy_retlines(void *data);

/* PUBLIC FUNCTIONS */

int DEVIANF(data_file_add) (Source_File *source)
{
   DDATAFILE(("Creation Data file START, file %s", source->devian->conf->file_path));

   source->blocks = ecore_list_new();
   ecore_list_set_free_cb(source->blocks, _cb_destroy_block);

   /* Open the file */
   if (!(source->fd = fopen((char *)source->devian->conf->file_path, "r")))
     {
        char buf[4096];

        snprintf(buf, sizeof(buf),
                 _("<hilight>Can't open file %s</hilight><br><br>"
                   "You can change the file in the dEvian's config panel"), source->devian->conf->file_path);
        e_module_dialog_show(_(MODULE_NAME " Module Error"), buf);
     }

   /* Create the textblock */
   if (!(source->obj_tb = _create_tb(source->fd)))
      return 0;

   /* Put the textblock in a the source's scrollframe */
   e_scrollframe_child_set(source->obj, source->obj_tb);
   evas_object_show(source->obj_tb);

   /* First read of the file */
   if (!_get_file_first(source))
      return 0;

   /* Put the text in the textblock */
   if (!_set_text_tb(source))
      return 0;

   return 1;
}

void DEVIANF(data_file_del) (Source_File *source)
{
   if (source->fd)
      fclose(source->fd);
   if (source->blocks)
      if (ecore_list_nodes(source->blocks))
         ecore_list_destroy(source->blocks);
   if (source->obj_tb)
      evas_object_del(source->obj_tb);
}

int DEVIANF(data_file_update) (Source_File *source, int option)
{
   switch (option)
     {
     case -1:
        /* Normal update */

        /* Update the source->blocks */
        if (!_get_file_update(source))
           return 0;
        /* Set the text in textblock with the content of source->blocks */
        if (!_set_text_tb(source))
           return 0;

        break;

     case 0:
        /* Full update */

        DEVIANF(source_file_update_change) (source->devian, 0, 0);
        DEVIANF(data_file_del) (source);
        if (!DEVIANF(data_file_add) (source))
           return 0;
        DEVIANF(source_file_update_change) (source->devian, 1, 0);
        DEVIANF(container_infos_text_change) (source->devian, NULL);

        break;

     case 1:
        /* Remove hilighted text */

        //... TODO

        break;
     }

   return 1;
}

/* PRIVATE FUNCTIONS */

static Evas_Object *
_create_tb(FILE *fd)
{
   Evas_Object *tb;
   Evas_Textblock_Style *tb_style;
   char style[1024];

   /*
    * snprintf(style, sizeof(style),
    * "DEFAULT='font=Vera font_size=%d align=left color=#000000ff wrap=word'"
    * "br='\n'",
    * DEVIANM->conf->sources_file_font_size);
    */

   tb = evas_object_textblock_add(DEVIANM->container->bg_evas);
   tb_style = evas_textblock_style_new();
   evas_textblock_style_set(tb_style, "DEFAULT='font=Vera font_size=10 align=left color=#000000ff wrap=word'" "br='\n'");
   /*  evas_textblock_style_set(tb_style,
    * style); */
   evas_object_textblock_style_set(tb, tb_style);
   evas_textblock_style_free(tb_style);

   return tb;
}

/**
 * Count the number of lines of a buffer
 * convert \n to <br>
 * and return a list of positions of the carrier returns
 *
 * @param buffer The buffer
 * @param *size Size of the buffer, can be modified in \n -> <br> replacement
 * @return The ecore list of carrier's returns positions
 */
static Ecore_List *
_get_lines(char *buffer, int *size)
{
   Ecore_List *lines;
   int size_left;
   int *pos;
   char *p1, *p2;

   lines = ecore_list_new();
   ecore_list_set_free_cb(lines, _cb_destroy_retlines);
   p1 = buffer;
   size_left = *size;

   while (p1 < (buffer + *size))
     {
        if (*p1 == 0xd)
           *p1 = ' ';
        if (*p1 == 0x9)
           *p1 = ' ';
        p1++;
     };

   p1 = buffer;

   while ((p2 = memchr(p1, '\n', size_left)))
     {
        /* Replace by <br> */

        *size = *size + 3;
        realloc(buffer, *size);
        memmove(p2 + 3, p2, strlen(p2) + 1);
        *p2 = '<';
        *(p2 + 1) = 'b';
        *(p2 + 2) = 'r';
        *(p2 + 3) = '>';

        /* Add the position to the list */
        pos = E_NEW(int, 1);

        *pos = p2 - buffer;
        ecore_list_append(lines, pos);
        /*
         * p1 = p2+1;
         * size_left = *size - (*pos+1);
         */

        p1 = p2 + 4;
        size_left = *size - (*pos + 4);

     };

   return lines;
}

/**
 * Reads backwards from the end, blocks of DATA_FILE_BUF_SIZE in the source->fd
 * and put them into source->blocks
 *
 * @param source The source assiciated
 * @return 1 on success, 0 if fails
 */
static int
_get_file_first(Source_File *source)
{
   Data_File_Block *block;
   int block_size;
   int moves_back;
   int beg;

   beg = 0;
   moves_back = 0;
   source->lines_tot = 0;
   source->new_blocks = 0;

   if (!source->fd)
      return 1;

   if (fseek(source->fd, 0, SEEK_END) == -1)
      return 0;

   do
     {
        if (fseek(source->fd, -(DATA_FILE_BUF_SIZE * sizeof(char)), SEEK_CUR) == -1)
          {
             DDATAFILE(("Fails to read one full block (offset %d)", ftell(source->fd)));
             if (beg)
                block_size = 0;
             else
               {
                  block_size = ftell(source->fd);
                  rewind(source->fd);
                  beg = 1;
               }
          }
        else
          {
             block_size = DATA_FILE_BUF_SIZE;
          }
        moves_back += block_size;

        if (block_size)
          {
             block = E_NEW(Data_File_Block, 1);
             block->retlines = NULL;
             block->buf = E_NEW(char, block_size);

             block->size = block_size;
             if (!fread(block->buf, sizeof(char), block->size, source->fd))
               {
                  E_FREE(block->buf);   //....CHANGE delete block
                  return 0;
               }
             block->retlines = _get_lines(block->buf, &block->size);
             DDATAFILE(("read_first (%d lines, block_size %d, offset %d):\n%s",
                        ecore_list_nodes(block->retlines), block_size, ftell(source->fd), block->buf));

             source->lines_tot += ecore_list_nodes(block->retlines);
             source->new_blocks++;

             ecore_list_prepend(source->blocks, block);
             /* Come back at the begining of this block */
             fseek(source->fd, -(block_size * sizeof(char)), SEEK_CUR);
          }
     }
   while (block_size && (source->lines_tot < DEVIANM->conf->sources_file_nb_lines_max));

   /* Repositioning to where we start reading */
   fseek(source->fd, moves_back, SEEK_CUR);
   DDATAFILE(("read_first finished, offset %d", ftell(source->fd)));

   return 1;
}

/**
 * Reads forward from current source->fd pos, blocks of DATA_FILE_BUF_SIZE
 * and put them into source->blocks
 *
 * @param source The source assiciated
 * @return 1 on success, 0 if fails
 */
static int
_get_file_update(Source_File *source)
{
   char buffer[DATA_FILE_BUF_SIZE];
   int block_size;

   if (!source->fd)
      return 1;

   fflush(source->fd);          //...REMOVE

   do
     {
        block_size = fread(buffer, sizeof(char), DATA_FILE_BUF_SIZE, source->fd);
        block_size = block_size * sizeof(char);
        if (ferror(source->fd))
           DDATAFILE(("ERROR WHEN READING"));
        DDATAFILE(("read_update (offset %d):\n%s", ftell(source->fd), buffer));
        if (block_size)
          {
             Data_File_Block *block;

             block = E_NEW(Data_File_Block, 1);
             block->retlines = NULL;
             block->buf = E_NEW(char, block_size);

             memcpy(block->buf, buffer, block_size);
             block->size = block_size;
             block->retlines = _get_lines(block->buf, &block->size);

             source->lines_tot += ecore_list_nodes(block->retlines);
             source->new_blocks++;

             ecore_list_append(source->blocks, block);
          }
     }
   while (block_size);

   return 1;
}

static int
_set_text_tb(Source_File *source)
{
   Data_File_Block *block;
   const Evas_Textblock_Cursor *cur;
   const char *buf;
   char buf2[8192];
   int w, h;

   if (!source->new_blocks)
      return 1;

   ecore_list_goto_index(source->blocks, ecore_list_nodes(source->blocks) - source->new_blocks);

   do
     {
        block = ecore_list_next(source->blocks);

        cur = evas_object_textblock_cursor_get(source->obj_tb);
        evas_textblock_cursor_node_last((Evas_Textblock_Cursor *) cur);
        //evas_textblock_cursor_text_append((Evas_Textblock_Cursor *)cur, block->buf);
        buf = evas_object_textblock_text_markup_get(source->obj_tb);
        snprintf(buf2, sizeof(buf2), "%s%s", buf, block->buf);
        evas_object_textblock_text_markup_set(source->obj_tb, buf2);

        evas_object_textblock_size_formatted_get(source->obj_tb, &w, &h);
        evas_object_resize(source->obj_tb, w, h);
        if (source->devian->conf->file_auto_scroll)
           e_scrollframe_child_pos_set(source->obj, 0, 9999);

        DDATAFILE(("add_tb (w:%d h:%d):\n%s", w, h, block->buf));
        source->new_blocks--;
     }
   while (source->new_blocks);

   return 1;
}

static void
_cb_destroy_block(void *data)
{
   Data_File_Block *block;

   block = data;

   if (block->buf)
      E_FREE(block->buf);
   if (block->retlines)
      if (ecore_list_nodes(block->retlines))
         ecore_list_destroy(block->retlines);
   E_FREE(block);
}

static void
_cb_destroy_retlines(void *data)
{
   E_FREE(data);
}

#endif
