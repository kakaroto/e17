/** @file etk_textblock.c */

#include "etk_textblock.h"
#include <stdlib.h>
#include <string.h>

#define ETK_TB_MAX_SEGMENT_CHARS 100

/**
 * @addtogroup Etk_Textblock
 * @{
 */

typedef struct Etk_Textblock_Object_SD
{
   Etk_Textblock *tb;
   
   Etk_Textblock_Wrap wrap;
   
   Evas_Object *cursor_object;
   Evas_List *text_objects;
} Etk_Textblock_Object_SD;

static void _etk_tb_constructor(Etk_Textblock *tb);
static void _etk_tb_destructor(Etk_Textblock *tb);
static void _etk_tb_iter_constructor(Etk_Textblock_Iter *tbi);
static void _etk_tb_iter_destructor(Etk_Textblock_Iter *tbi);

static void _etk_tb_object_smart_add(Evas_Object *obj);
static void _etk_tb_object_smart_del(Evas_Object *obj);
static void _etk_tb_object_smart_move(Evas_Object *obj, Evas_Coord x, Evas_Coord y);
static void _etk_tb_object_smart_resize(Evas_Object *obj, Evas_Coord w, Evas_Coord h);
static void _etk_tb_object_smart_show(Evas_Object *obj);
static void _etk_tb_object_smart_hide(Evas_Object *obj);
static void _etk_tb_object_smart_color_set(Evas_Object *obj, int r, int g, int b, int a);
static void _etk_tb_object_smart_clip_set(Evas_Object *obj, Evas_Object *clip);
static void _etk_tb_object_smart_clip_unset(Evas_Object *obj);

static void _etk_tb_object_node_render(Evas_Object *tbo, const char *text);
static int _etk_tb_object_line_render(Evas_Object *tbo, const char *text, int lx, int ly, int lw, int *lh);

static Evas_Smart *_etk_tb_object_smart = NULL;
static int _etk_tb_object_smart_use = 0;


/**************************
 *
 * Implementation
 *
 **************************/

/**
 * @brief Gets the type of an Etk_Textblock
 * @return Returns the type of an Etk_Textblock
 */
Etk_Type *etk_textblock_type_get()
{
   static Etk_Type *textblock_type = NULL;

   if (!textblock_type)
   {
      textblock_type = etk_type_new("Etk_Textblock", ETK_OBJECT_TYPE, sizeof(Etk_Textblock),
         ETK_CONSTRUCTOR(_etk_tb_constructor), ETK_DESTRUCTOR(_etk_tb_destructor));
   }

   return textblock_type;
}

/**
 * @brief Gets the type of an Etk_Textblock_Iter
 * @return Returns the type of an Etk_Textblock_Iter
 */
Etk_Type *etk_textblock_iter_type_get()
{
   static Etk_Type *textblock_iter_type = NULL;

   if (!textblock_iter_type)
   {
      textblock_iter_type = etk_type_new("Etk_Textblock_Iter", ETK_OBJECT_TYPE, sizeof(Etk_Textblock_Iter),
         ETK_CONSTRUCTOR(_etk_tb_iter_constructor), ETK_DESTRUCTOR(_etk_tb_iter_destructor));
   }

   return textblock_iter_type;
}

/**
 * @brief Creates a new textblock
 * @return Returns the new textblock
 */
Etk_Textblock *etk_textblock_new()
{
   return ETK_TEXTBLOCK(etk_object_new(ETK_TEXTBLOCK_TYPE, NULL));
}

/**
 * @brief Creates a new evas object that will display the content of the textblock. @n
 * A textblock can have several evas objects which display its content. All the evas objects are automatically updated
 * when the textblock's content is changed.
 * @param tb the textblock whose content will be displayed by the evas object
 * @param evas the evas to which the object will be added
 * @return Returns a new evas object that display the content of the textblock
 */
Evas_Object *etk_textblock_object_add(Etk_Textblock *tb, Evas *evas)
{
   Evas_Object *obj;
   Etk_Textblock_Object_SD *tbo_sd;
   
   if (!tb || !evas)
      return NULL;
   
   if (!_etk_tb_object_smart)
   {
      _etk_tb_object_smart = evas_smart_new("Textblock_Object",
         _etk_tb_object_smart_add,
         _etk_tb_object_smart_del,
         NULL,
         NULL,
         NULL,
         NULL,
         NULL,
         _etk_tb_object_smart_move,
         _etk_tb_object_smart_resize,
         _etk_tb_object_smart_show,
         _etk_tb_object_smart_hide,
         _etk_tb_object_smart_color_set,
         _etk_tb_object_smart_clip_set,
         _etk_tb_object_smart_clip_unset,
         NULL);
   }
   _etk_tb_object_smart_use++;
   
   obj = evas_object_smart_add(evas, _etk_tb_object_smart);
   tbo_sd = evas_object_smart_data_get(obj);
   tbo_sd->tb= tb;
   
   tb->evas_objects = evas_list_append(tb->evas_objects, obj);
   
   return obj;
}

/**
 * @brief Sets how the text of the textblock object should be wrapped
 * @param tbo a textblock object
 * @param wrap the wrap mode
 */
void etk_textblock_wrap_set(Evas_Object *tbo, Etk_Textblock_Wrap wrap)
{
   Etk_Textblock_Object_SD *tbo_sd;
   
   if (!tbo || !(tbo_sd = evas_object_smart_data_get(tbo)))
      return;
   
   if (tbo_sd->wrap != wrap)
   {
      tbo_sd->wrap = wrap;
      /* TODO: etk_textblock_wrap_set: redraw */
   }
}

/**
 * @brief Gets the wrap mode of the textblock object
 * @param tbo a textblock object
 * @return Returns the wrap mode of the textblock object
 */
Etk_Textblock_Wrap etk_textblock_wrap_get(Evas_Object *tbo)
{
   Etk_Textblock_Object_SD *tbo_sd;
   
   if (!tbo || !(tbo_sd = evas_object_smart_data_get(tbo)))
      return ETK_TEXTBLOCK_WRAP_WORD;
   return tbo_sd->wrap;
}

/**************************
 *
 * Etk specific functions
 *
 **************************/

/* Initializes the textblock */
static void _etk_tb_constructor(Etk_Textblock *tb)
{
   if (!tb)
      return;
   
   tb->iters = NULL;
   tb->evas_objects = NULL;
}

/* Destroys the textblock */
static void _etk_tb_destructor(Etk_Textblock *tb)
{
   if (!tb)
      return;
   
   while (tb->evas_objects)
      evas_object_del(tb->evas_objects->data);
   while (tb->iters)
      etk_object_destroy(ETK_OBJECT(tb->iters->data));
}

/* Initializes the textblock iterator */
static void _etk_tb_iter_constructor(Etk_Textblock_Iter *tbi)
{
   if (!tbi)
      return;
   
   tbi->tb = NULL;
}

/* Destroys the textblock iterator */
static void _etk_tb_iter_destructor(Etk_Textblock_Iter *tbi)
{
   if (!tbi)
      return;
   
   if (tbi->tb)
      tbi->tb->iters = evas_list_remove(tbi->tb->iters, tbi);
}

/**************************
 *
 * Textblock object's smart object
 *
 **************************/

/* Initializes the new textblock object */
static void _etk_tb_object_smart_add(Evas_Object *obj)
{
   Etk_Textblock_Object_SD *tbo_sd;
   Evas *evas;
   
   if (!obj || !(evas = evas_object_evas_get(obj)))
      return;
   
   tbo_sd = malloc(sizeof(Etk_Textblock_Object_SD));
   tbo_sd->tb = NULL;
   tbo_sd->wrap = ETK_TEXTBLOCK_WRAP_WORD;
   tbo_sd->cursor_object = NULL;
   tbo_sd->text_objects = NULL;
   evas_object_smart_data_set(obj, tbo_sd);
}

/* Destroys the textblock object */
static void _etk_tb_object_smart_del(Evas_Object *obj)
{
   Etk_Textblock_Object_SD *tbo_sd;
   
   if (!obj || !(tbo_sd = evas_object_smart_data_get(obj)))
      return;
   
   while (tbo_sd->text_objects)
   {
      evas_object_del(tbo_sd->text_objects->data);
      tbo_sd->text_objects = evas_list_remove_list(tbo_sd->text_objects, tbo_sd->text_objects);
   }
   evas_object_del(tbo_sd->cursor_object);
   
   tbo_sd->tb->evas_objects = evas_list_remove(tbo_sd->tb->evas_objects, obj);
   
   _etk_tb_object_smart_use--;
   if (_etk_tb_object_smart_use <= 0)
   {
      evas_smart_free(_etk_tb_object_smart);
      _etk_tb_object_smart = NULL;
   }
}

/* Moves the textblock object */
static void _etk_tb_object_smart_move(Evas_Object *obj, Evas_Coord x, Evas_Coord y)
{
   Etk_Textblock_Object_SD *tbo_sd;
   
   if (!obj || !(tbo_sd = evas_object_smart_data_get(obj)))
      return;
}

/* Resizes the textblock object */
static void _etk_tb_object_smart_resize(Evas_Object *obj, Evas_Coord w, Evas_Coord h)
{
   Etk_Textblock_Object_SD *tbo_sd;
   
   if (!obj || !(tbo_sd = evas_object_smart_data_get(obj)))
      return;
   
   while (tbo_sd->text_objects)
   {
      evas_object_del(tbo_sd->text_objects->data);
      tbo_sd->text_objects = evas_list_remove_list(tbo_sd->text_objects, tbo_sd->text_objects);
   }
   
   _etk_tb_object_node_render(obj, "This is a test1. This is a test2. This is a test3. This is a test4. This is a test5. "
      "This is a test6. This is a test7. This is a test8. This is a test9. This is a test10. This is a test11. This is a test12. "
      "This is a test13. This is a test14. This is a test15. This is a test16. This is a test17. This is a test18. This is a test19. "
      "This is a test20. This is a test21. This is a test22. This is a test23. This is a test24. This is a test25. This is a test26. "
      "This is a test27. This is a test28. This is a test29. This is a test30. This is a test31. This is a test32. This is a test33. "
      "This is a test34. This is a test35. This is a test36. This is a test37. This is a test38. This is a test39. This is a test40. "
      "This is a test6. This is a test7. This is a test8. This is a test9. This is a test10. This is a test11. This is a test12. "
      "This is a test13. This is a test14. This is a test15. This is a test16. This is a test17. This is a test18. This is a test19. "
      "This is a test20. This is a test21. This is a test22. This is a test23. This is a test24. This is a test25. This is a test26. "
      "This is a test27. This is a test28. This is a test29. This is a test30. This is a test31. This is a test32. This is a test33. "
      "This is a test34. This is a test35. This is a test36. This is a test37. This is a test38. This is a test39. This is a test40. "
      "This is a test6. This is a test7. This is a test8. This is a test9. This is a test10. This is a test11. This is a test12. "
      "This is a test13. This is a test14. This is a test15. This is a test16. This is a test17. This is a test18. This is a test19. "
      "This is a test20. This is a test21. This is a test22. This is a test23. This is a test24. This is a test25. This is a test26. "
      "This is a test27. This is a test28. This is a test29. This is a test30. This is a test31. This is a test32. This is a test33. "
      "This is a test34. This is a test35. This is a test36. This is a test37. This is a test38. This is a test39. This is a test40. "
      "This is a test6. This is a test7. This is a test8. This is a test9. This is a test10. This is a test11. This is a test12. "
      "This is a test13. This is a test14. This is a test15. This is a test16. This is a test17. This is a test18. This is a test19. "
      "This is a test20. This is a test21. This is a test22. This is a test23. This is a test24. This is a test25. This is a test26. "
      "This is a test27. This is a test28. This is a test29. This is a test30. This is a test31. This is a test32. This is a test33. "
      "This is a test34. This is a test35. This is a test36. This is a test37. This is a test38. This is a test39. This is a test40. "
      "This is a test6. This is a test7. This is a test8. This is a test9. This is a test10. This is a test11. This is a test12. "
      "This is a test13. This is a test14. This is a test15. This is a test16. This is a test17. This is a test18. This is a test19. "
      "This is a test20. This is a test21. This is a test22. This is a test23. This is a test24. This is a test25. This is a test26. "
      "This is a test27. This is a test28. This is a test29. This is a test30. This is a test31. This is a test32. This is a test33. "
      "This is a test34. This is a test35. This is a test36. This is a test37. This is a test38. This is a test39. This is a test40. "
      "This is a test6. This is a test7. This is a test8. This is a test9. This is a test10. This is a test11. This is a test12. "
      "This is a test13. This is a test14. This is a test15. This is a test16. This is a test17. This is a test18. This is a test19. "
      "This is a test20. This is a test21. This is a test22. This is a test23. This is a test24. This is a test25. This is a test26. "
      "This is a test27. This is a test28. This is a test29. This is a test30. This is a test31. This is a test32. This is a test33. "
      "This is a test34. This is a test35. This is a test36. This is a test37. This is a test38. This is a test39. This is a test40. "
      "This is a test6. This is a test7. This is a test8. This is a test9. This is a test10. This is a test11. This is a test12. "
      "This is a test13. This is a test14. This is a test15. This is a test16. This is a test17. This is a test18. This is a test19. "
      "This is a test20. This is a test21. This is a test22. This is a test23. This is a test24. This is a test25. This is a test26. "
      "This is a test27. This is a test28. This is a test29. This is a test30. This is a test31. This is a test32. This is a test33. "
      "This is a test34. This is a test35. This is a test36. This is a test37. This is a test38. This is a test39. This is a test40. "
      "This is a test6. This is a test7. This is a test8. This is a test9. This is a test10. This is a test11. This is a test12. "
      "This is a test13. This is a test14. This is a test15. This is a test16. This is a test17. This is a test18. This is a test19. "
      "This is a test20. This is a test21. This is a test22. This is a test23. This is a test24. This is a test25. This is a test26. "
      "This is a test27. This is a test28. This is a test29. This is a test30. This is a test31. This is a test32. This is a test33. "
      "This is a test34. This is a test35. This is a test36. This is a test37. This is a test38. This is a test39. This is a test40. "
      "This is a test6. This is a test7. This is a test8. This is a test9. This is a test10. This is a test11. This is a test12. "
      "This is a test13. This is a test14. This is a test15. This is a test16. This is a test17. This is a test18. This is a test19. "
      "This is a test20. This is a test21. This is a test22. This is a test23. This is a test24. This is a test25. This is a test26. "
      "This is a test27. This is a test28. This is a test29. This is a test30. This is a test31. This is a test32. This is a test33. "
      "This is a test34. This is a test35. This is a test36. This is a test37. This is a test38. This is a test39. This is a test40. "
      "This is a test6. This is a test7. This is a test8. This is a test9. This is a test10. This is a test11. This is a test12. "
      "This is a test13. This is a test14. This is a test15. This is a test16. This is a test17. This is a test18. This is a test19. "
      "This is a test20. This is a test21. This is a test22. This is a test23. This is a test24. This is a test25. This is a test26. "
      "This is a test27. This is a test28. This is a test29. This is a test30. This is a test31. This is a test32. This is a test33. "
      "This is a test34. This is a test35. This is a test36. This is a test37. This is a test38. This is a test39. This is a test40. "
      "This is a test41. This is a test42. This is a test43.");
}

/* Shows the textblock object */
static void _etk_tb_object_smart_show(Evas_Object *obj)
{
   Etk_Textblock_Object_SD *tbo_sd;
   
   if (!obj || !(tbo_sd = evas_object_smart_data_get(obj)))
      return;
}

/* Hides the textblock object */
static void _etk_tb_object_smart_hide(Evas_Object *obj)
{
   Etk_Textblock_Object_SD *tbo_sd;
   
   if (!obj || !(tbo_sd = evas_object_smart_data_get(obj)))
      return;
}

/* Set the color of the textblock object */
static void _etk_tb_object_smart_color_set(Evas_Object *obj, int r, int g, int b, int a)
{
   Etk_Textblock_Object_SD *tbo_sd;
   
   if (!obj || !(tbo_sd = evas_object_smart_data_get(obj)))
      return;
}

/* Clips the textblock object */
static void _etk_tb_object_smart_clip_set(Evas_Object *obj, Evas_Object *clip)
{
   Etk_Textblock_Object_SD *tbo_sd;
   
   if (!obj || !(tbo_sd = evas_object_smart_data_get(obj)))
      return;
}

/* Unclips the textblock object */
static void _etk_tb_object_smart_clip_unset(Evas_Object *obj)
{
   Etk_Textblock_Object_SD *tbo_sd;
   
   if (!obj || !(tbo_sd = evas_object_smart_data_get(obj)))
      return;
}

/**************************
 *
 * Private functions
 *
 **************************/

/* Renders a node of the textblock in a textblock object */
static void _etk_tb_object_node_render(Evas_Object *tbo, const char *text)
{
   int ox, oy, ow, oh;
   int c = 0, res = 0;
   int lh, y = 0;
   
   if (!tbo || !text)
      return;
   
   evas_object_geometry_get(tbo, &ox, &oy, &ow, &oh);
   while (res >= 0)
   {
      res = _etk_tb_object_line_render(tbo, &text[c], 0, y, ow, &lh);
      c += res;
      y += lh;
   }
}

/* Renders the text in a line starting at (lx, ly) and with a width of "lw", in the textblock object.
 * The height of the new line will be stored in "lh".
 * Returns the index of the next character to render (-1 on failure or if everything has been rendered;
 * 0 means that nothing has been rendered) */
static int _etk_tb_object_line_render(Evas_Object *tbo, const char *text, int lx, int ly, int lw, int *lh)
{
   Evas *evas;
   Etk_Textblock_Object_SD *tbo_sd;
   Evas_Object *text_object;
   char buf[ETK_TB_MAX_SEGMENT_CHARS + 1];
   int ox, oy, ow;
   int tw, th;
   int res;
   
   if (!tbo || !text)
      return -1;
   if (!(tbo_sd = evas_object_smart_data_get(tbo)) || !(evas = evas_object_evas_get(tbo)))
      return -1;
   
   /* TODO: get correct geometry */
   evas_object_geometry_get(tbo, &ox, &oy, &ow, NULL);
   if (lx > ow)
      return 0;
   
   strncpy(buf, text, ETK_TB_MAX_SEGMENT_CHARS);
   buf[ETK_TB_MAX_SEGMENT_CHARS] = '\0';
   
   text_object = evas_object_text_add(evas);
   evas_object_text_font_set(text_object, "Vera", 10);
   evas_object_text_text_set(text_object, buf);
   evas_object_geometry_get(text_object, NULL, NULL, &tw, &th);
   
   if (tw > lw)
   {
      int wrap_pos;
      int i, c;
      char *wrapped_text;
      
      wrap_pos = evas_object_text_char_coords_get(text_object, lw, th / 2, NULL, NULL, NULL, NULL);
      if (wrap_pos <= 0)
         ;
      else
      {
         for (i = 0, c = 0; i < wrap_pos; i++)
            c = evas_string_char_next_get(buf, c, NULL);
         c = evas_string_char_next_get(buf, c, NULL);
         
         wrapped_text = malloc(c);
         snprintf(wrapped_text, c, "%s", buf);
         evas_object_text_text_set(text_object, wrapped_text);
         
         res = evas_string_char_prev_get(buf, c, NULL);
      }
   }
   else
      res = -1;
   
   evas_object_move(text_object, ox + lx, oy + ly);
   evas_object_color_set(text_object, 0, 0, 0, 255);
   evas_object_show(text_object);
   
   tbo_sd->text_objects = evas_list_append(tbo_sd->text_objects, text_object);
   
   evas_object_geometry_get(text_object, NULL, NULL, NULL, lh);
   
   return res;
}

/** @} */
