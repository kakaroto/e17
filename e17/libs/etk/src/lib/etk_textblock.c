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
static void _etk_textblock_object_evas_set(Etk_Textblock *textblock, Evas *new_evas);

static void _etk_textblock_smart_move_cb(Evas_Object *obj, Evas_Coord x, Evas_Coord y);
static void _etk_textblock_smart_resize_cb(Evas_Object *obj, Evas_Coord w, Evas_Coord h);
static void _etk_textblock_smart_show_cb(Evas_Object *obj);
static void _etk_textblock_smart_hide_cb(Evas_Object *obj);
static void _etk_textblock_smart_clip_set_cb(Evas_Object *obj, Evas_Object *clip);
static void _etk_textblock_smart_clip_unset_cb(Evas_Object *obj);

static Evas *_etk_textblock_evas = NULL;
static unsigned char _etk_textblock_pixel_buffer[ETK_TEXTBLOCK_EVAS_SIZE * ETK_TEXTBLOCK_EVAS_SIZE * 3];
static Evas_Textblock_Style *_etk_textblock_style = NULL;
static Evas_Smart *_etk_textblock_smart = NULL;
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
   
   _etk_textblock_object_evas_set(textblock, evas);
   
   if (!_etk_textblock_smart)
   {
      _etk_textblock_smart = evas_smart_new(
         "etk_textblock_smart_object",
         NULL, /* add */
         NULL, /* del */
         NULL, /* layer_set */
         NULL, /* raise */
         NULL, /* lower */
         NULL, /* stack_above */
         NULL, /* stack_below */
         _etk_textblock_smart_move_cb, /* move */
         _etk_textblock_smart_resize_cb, /* resize */
         _etk_textblock_smart_show_cb, /* show */
         _etk_textblock_smart_hide_cb, /* hide */
         NULL, /* color_set */
         _etk_textblock_smart_clip_set_cb, /* clip_set */
         _etk_textblock_smart_clip_unset_cb, /* clip_unset */
         NULL); /* data*/
   }
   textblock->smart_object = evas_object_smart_add(evas, _etk_textblock_smart);
   evas_object_smart_data_set(textblock->smart_object, textblock);
   evas_object_smart_member_add(textblock->textblock_object, textblock->smart_object);
   
   /*textblock->clip = evas_object_rectangle_add(evas);
   evas_object_color_set(textblock->clip, 255, 128, 0, 80);
   evas_object_smart_member_add(textblock->clip, textblock->smart_object);
   evas_object_lower(textblock->clip);*/
   /* TODO: realize: creates selection rects, clip, cursor, timer  */
}

/* TODO: doc */
void etk_textblock_unrealize(Etk_Textblock *textblock)
{
   if (!textblock)
      return;
   
   if (textblock->cursor_timer)
   {
      ecore_timer_del(textblock->cursor_timer);
      textblock->cursor_timer = NULL;
   }
   
   while (textblock->selection_rects)
   {
      evas_object_del(textblock->selection_rects->data);
      textblock->selection_rects = evas_list_remove_list(textblock->selection_rects, textblock->selection_rects);
   }
   
   if (textblock->cursor_object)
   {
      evas_object_del(textblock->cursor_object);
      textblock->cursor_object = NULL;
   }
   if (textblock->clip)
   {
      evas_object_del(textblock->clip);
      textblock->clip = NULL;
   }
   if (textblock->smart_object)
   {
      if (textblock->textblock_object)
         evas_object_smart_member_del(textblock->textblock_object);
      evas_object_del(textblock->smart_object);
      textblock->smart_object = NULL;
   }
   
   _etk_textblock_object_evas_set(textblock, _etk_textblock_evas);
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
   
   textblock->smart_object = NULL;
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
   /* TODO; does it need to be shown */
   evas_object_show(textblock->textblock_object);
   
   /* TODO: testing */
   evas_object_textblock_text_markup_set
     (textblock->textblock_object,
      "<center><h1>Title</h1></center><br>"
      "<p><tab>A pragraph here <red>red text</red> and stuff.</p>"
      "<p>And escaping &lt; and &gt; as well as &amp; as <h1>normal.</h1></p>"
      "<p>If you want a newline use &lt;br&gt;<br>woo a new line!</p>"
      "<right>Right "
      "<style=outline color=#fff outline_color=#000>aligned</> "
      "<style=shadow shadow_color=#fff8>text</> "
      "<style=soft_shadow shadow_color=#0002>should</> "
      "<style=glow color=#fff glow2_color=#fe87 glow_color=#f214 >go here</> "
      "<style=far_shadow shadow_color=#0005>as it is</> "
      "<style=outline_shadow color=#fff outline_color=#8228 shadow_color=#005>within</> "
      "<style=outline_soft_shadow color=#fff outline_color=#8228 shadow_color=#0002>right tags</> "
      "<style=far_soft_shadow color=#fff shadow_color=#0002>to make it align to the</> "
      "<underline=on underline_color=#00f>right hand</> "
      "<backing=on backing_color=#fff8>side </><backing_color=#ff08>of</><backing_color=#0f08> </>"
      "<strikethrough=on strikethrough_color=#f0f8>the textblock</>.</right>"
      "<p>And "
      "<underline=double underline_color=#f00 underline2_color=#00f>now we need</> "
      "to test some <color=#f00 font_size=8>C</><color=#0f0 font_size=10>O</>"
      "<color=#00f font_size=12>L</><color=#fff font_size=14>O</>"
      "<color=#ff0 font_size=16>R</><color=#0ff font_size=18> Bla Rai</>"
      "<color=#f0f font_size=20> Stuff</>.</p>"
      "<blockquote>"
      "(日本語 カタカナ ひらがな) "
      "Heizölrückstoßabdämpfung fløde pingüino kilómetros cœur déçu l'âme "
      "plutôt naïve Louÿs rêva crapaüter Íosa Úrmhac Óighe pór Éava Ádhaim"
      "</blockquote>"
      );
}

/* Destroys the textblock */
static void _etk_textblock_destructor(Etk_Textblock *textblock)
{
   if (!textblock)
      return;
   
   if (textblock->cursor_timer)
      ecore_timer_del(textblock->cursor_timer);
   
   while (textblock->selection_rects)
   {
      evas_object_del(textblock->selection_rects->data);
      textblock->selection_rects = evas_list_remove_list(textblock->selection_rects, textblock->selection_rects);
   }
   
   if (textblock->cursor_object)
      evas_object_del(textblock->cursor_object);
   if (textblock->clip)
      evas_object_del(textblock->clip);
   if (textblock->textblock_object)
      evas_object_del(textblock->textblock_object);
   if (textblock->smart_object)
      evas_object_del(textblock->smart_object);
   
   _etk_textblock_count--;
   if (_etk_textblock_count <= 0)
   {
      if (_etk_textblock_style)
      {
         evas_textblock_style_free(_etk_textblock_style);
         _etk_textblock_style = NULL;
      }
      if (_etk_textblock_evas)
      {
         evas_free(_etk_textblock_evas);
         _etk_textblock_evas = NULL;
      }
   }
}

/**************************
 *
 * Private functions
 *
 **************************/

/* Changes the evas used by the textblock object */
static void _etk_textblock_object_evas_set(Etk_Textblock *textblock, Evas *new_evas)
{
   Evas_Object *new_tbo;
   
   if (!textblock || !new_evas)
      return;
   if (new_evas == evas_object_evas_get(textblock->textblock_object))
      return;
   
   new_tbo = evas_object_textblock_add(new_evas);
   evas_object_textblock_style_set(new_tbo, _etk_textblock_style);
   
   /* TODO */
   evas_object_textblock_text_markup_set(new_tbo, "This is a test");
   
   evas_object_del(textblock->textblock_object);
   textblock->textblock_object = new_tbo;
}

/* Called when the smart object of the textblock is moved */
static void _etk_textblock_smart_move_cb(Evas_Object *obj, Evas_Coord x, Evas_Coord y)
{
   Etk_Textblock *textblock;
   
   if (!obj || !(textblock = evas_object_smart_data_get(obj)))
      return;
   
   /* TODO: smart_move: update selection_rects, cursor_object  */
   if (textblock->textblock_object)
      evas_object_move(textblock->textblock_object, x, y);
   if (textblock->clip)
      evas_object_move(textblock->clip, x, y);
}

/* Called when the smart object of the textblock is resized */
static void _etk_textblock_smart_resize_cb(Evas_Object *obj, Evas_Coord w, Evas_Coord h)
{
   Etk_Textblock *textblock;
   
   if (!obj || !(textblock = evas_object_smart_data_get(obj)))
      return;
   
   /* TODO: smart_resize: update selection_rects, cursor_object  */
   if (textblock->textblock_object)
      evas_object_resize(textblock->textblock_object, w, h);
   if (textblock->clip)
      evas_object_resize(textblock->clip, w, h);
}

/* Called when the smart object of the textblock is shown */
static void _etk_textblock_smart_show_cb(Evas_Object *obj)
{
   Evas_List *l;
   Etk_Textblock *textblock;
   
   if (!obj || !(textblock = evas_object_smart_data_get(obj)))
      return;
   
   if (textblock->textblock_object)
      evas_object_show(textblock->textblock_object);
   /* TODO: smart_show: restart timer */
   if (textblock->cursor_object)
      evas_object_show(textblock->cursor_object);
   if (textblock->clip)
      evas_object_show(textblock->clip);
   
   for (l = textblock->selection_rects; l; l = l->next)
      evas_object_show(l->data);
}

/* Called when the smart object of the textblock is hidden */
static void _etk_textblock_smart_hide_cb(Evas_Object *obj)
{
   Evas_List *l;
   Etk_Textblock *textblock;
   
   if (!obj || !(textblock = evas_object_smart_data_get(obj)))
      return;
   
   if (textblock->textblock_object)
      evas_object_hide(textblock->textblock_object);
   /* TODO: smart_hide: stop timer */
   if (textblock->cursor_object)
      evas_object_hide(textblock->cursor_object);
   if (textblock->clip)
      evas_object_hide(textblock->clip);
   
   for (l = textblock->selection_rects; l; l = l->next)
      evas_object_hide(l->data);
}

/* Called when the smart object of the textblock is clipped */
static void _etk_textblock_smart_clip_set_cb(Evas_Object *obj, Evas_Object *clip)
{
   Etk_Textblock *textblock;
   
   if (!obj || !clip || !(textblock = evas_object_smart_data_get(obj)) || !textblock->clip)
      return;
   evas_object_clip_set(textblock->clip, clip);
}

/* Called when the smart object of the textblock is unclipped */
static void _etk_textblock_smart_clip_unset_cb(Evas_Object *obj)
{
   Etk_Textblock *textblock;
   
   if (!obj || !(textblock = evas_object_smart_data_get(obj)) || !textblock->clip)
      return;
   evas_object_clip_unset(textblock->clip);
}
