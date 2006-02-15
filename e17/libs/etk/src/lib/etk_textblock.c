/** @file etk_textblock.c */
#include "etk_textblock.h"
#include <stdlib.h>
#include <string.h>
#include <Ecore.h>
#include <Evas_Engine_Buffer.h>
#include "etk_utils.h"

#define ETK_TEXTBLOCK_EVAS_SIZE 32

static void _etk_textblock_constructor(Etk_Textblock *textblock);
static void _etk_textblock_destructor(Etk_Textblock *textblock);

static Evas *_etk_textblock_evas = NULL;
static unsigned char _etk_textblock_pixel_buffer[ETK_TEXTBLOCK_EVAS_SIZE * ETK_TEXTBLOCK_EVAS_SIZE * 3];
static Evas_Textblock_Style *_etk_textblock_style = NULL;
static int _etk_textblock_count = 0;

/**************************
 *
 * Implementation
 *
 **************************/

/**
 * @brief Gets the type of an Etk_Textblock
 * @return Returns the type on an Etk_Textblock
 */
Etk_Type *etk_textblock_type_get()
{
   static Etk_Type *textblock_type = NULL;

   if (!textblock_type)
      textblock_type = etk_type_new("Etk_Textblock", ETK_OBJECT_TYPE, sizeof(Etk_Textblock), ETK_CONSTRUCTOR(_etk_textblock_constructor), ETK_DESTRUCTOR(_etk_textblock_destructor));

   return textblock_type;
}

/**
 * @brief Creates a new textblock
 * @return Returns the new textblock
 */
Etk_Textblock *etk_textblock_new()
{
   return ETK_TEXTBLOCK(etk_object_new(ETK_TEXTBLOCK_TYPE, NULL));
}

/* TODO: doc */
void etk_textblock_realize(Etk_Textblock *textblock, Evas *evas)
{
   if (!textblock || !evas)
      return;
}

/* TODO: doc */
void etk_textblock_unrealize(Etk_Textblock *textblock)
{
   if (!textblock)
      return;
   
   ecore_timer_del(textblock->cursor_timer);
   textblock->cursor_timer = NULL;
   
   //_etk_textblock_object_evas_change(textblock, textblock->textblock_object, _etk_textblock_evas);
   
   evas_object_del(textblock->cursor_object);
   textblock->cursor_object = NULL;
   evas_object_del(textblock->clip);
   textblock->clip = NULL;
   
   while (textblock->selection_rects)
   {
      evas_object_del(textblock->selection_rects->data);
      textblock->selection_rects = evas_list_remove_list(textblock->selection_rects, textblock->selection_rects);
   }
}

/**************************
 *
 * Etk specific functions
 *
 **************************/

/* Initializes the default values of the textblock */
static void _etk_textblock_constructor(Etk_Textblock *textblock)
{
   if (!textblock)
      return;
   
   textblock->textblock_object = NULL;
   textblock->cursor_object = NULL;
   textblock->clip = NULL;
   textblock->selection_rects = NULL;
   textblock->selection_start = NULL;
   textblock->iterators = NULL;
   textblock->cursor_timer = NULL;
   _etk_textblock_count++;
   
   if (!_etk_textblock_evas)
   {
      Evas_Engine_Info_Buffer *engine_info;
      int render_method;
      
      if (!(render_method = evas_render_method_lookup("buffer")))
      {
         ETK_WARNING("Unable to use the buffer engine of Evas. Unable to create the textblock");
         return;
      }
      
      _etk_textblock_evas = evas_new();
      evas_output_method_set(_etk_textblock_evas, render_method);
      evas_output_size_set(_etk_textblock_evas, ETK_TEXTBLOCK_EVAS_SIZE, ETK_TEXTBLOCK_EVAS_SIZE);
      evas_output_viewport_set(_etk_textblock_evas, 0, 0, ETK_TEXTBLOCK_EVAS_SIZE, ETK_TEXTBLOCK_EVAS_SIZE);
      
      if (!(engine_info = (Evas_Engine_Info_Buffer *)evas_engine_info_get(_etk_textblock_evas)))
      {
         ETK_WARNING("Unable to get the info of the buffer engine of Evas. Unable to create the textblock");
         evas_free(_etk_textblock_evas);
         _etk_textblock_evas = NULL;
         return;
      }
      
      engine_info->info.depth_type = EVAS_ENGINE_BUFFER_DEPTH_RGB24;
      engine_info->info.dest_buffer = _etk_textblock_pixel_buffer;
      engine_info->info.dest_buffer_row_bytes = ETK_TEXTBLOCK_EVAS_SIZE * 3;
      engine_info->info.use_color_key = 0;
      engine_info->info.alpha_threshold = 0;
      engine_info->info.func.new_update_region = NULL;
      engine_info->info.func.free_update_region = NULL;
      evas_engine_info_set(_etk_textblock_evas, (Evas_Engine_Info *)engine_info);
   }
   
   if (!_etk_textblock_style)
   {
      _etk_textblock_style = evas_textblock_style_new();
      evas_textblock_style_set(_etk_textblock_style,
         "DEFAULT='font=Vera font_size=10 align=left color=#000000 wrap=word style=shadow shadow_color=#ffffff80'"
         "center='+ align=center'"
         "/center='- \n'"
         "right='+ align=right'"
         "/right='- \n'"
         "h1='+ font_size=20'"
         "red='+ color=#ff0000'"
         "br='\n'"
         "tab='\t'");
   }
   
   textblock->textblock_object = evas_object_textblock_add(_etk_textblock_evas);
   evas_object_textblock_style_set(textblock->textblock_object, _etk_textblock_style);
}

/* Destroys the textblock */
static void _etk_textblock_destructor(Etk_Textblock *textblock)
{
}
