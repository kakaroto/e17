#ifndef EVASTEXTBLOCK_H
#define EVASTEXTBLOCK_H

#include "EvasObject.h"

namespace efl {

/* Forward declarations */
class EvasCanvas;
class EvasTextblockStyle;

/*
 * Wraps an Evas Textblock Object
 */
class EvasTextblock : public EvasObject
{
public:
  EvasTextblock( EvasCanvas& canvas );
  EvasTextblock( EvasCanvas& canvas, const Point &pos, const std::string &text );
  virtual ~EvasTextblock();

  void setStyle( const EvasTextblockStyle &style );
  const EvasTextblockStyle getStyle () const;
  
  void setText( const std::string &text );
  void clear();
};

} // end namespace efl

/*
 TODO:

   EAPI const char                  *evas_textblock_escape_string_get(const char *escape) EINA_WARN_UNUSED_RESULT EINA_ARG_NONNULL(1) EINA_PURE;
   EAPI const char                  *evas_textblock_string_escape_get(const char *string, int *len_ret) EINA_WARN_UNUSED_RESULT EINA_ARG_NONNULL(1) EINA_PURE;


   EAPI void                         evas_object_textblock_replace_char_set(Evas_Object *obj, const char *ch) EINA_ARG_NONNULL(1);
   EAPI const char                  *evas_object_textblock_replace_char_get(Evas_Object *obj) EINA_WARN_UNUSED_RESULT EINA_ARG_NONNULL(1) EINA_PURE;
   
   EAPI void                         evas_object_textblock_text_markup_set(Evas_Object *obj, const char *text) EINA_ARG_NONNULL(1);
   EAPI void                         evas_object_textblock_text_markup_prepend(Evas_Textblock_Cursor *cur, const char *text) EINA_ARG_NONNULL(1, 2);
   EAPI const char                  *evas_object_textblock_text_markup_get(const Evas_Object *obj) EINA_WARN_UNUSED_RESULT EINA_ARG_NONNULL(1) EINA_PURE;
   
   EAPI const Evas_Textblock_Cursor *evas_object_textblock_cursor_get(const Evas_Object *obj) EINA_WARN_UNUSED_RESULT EINA_ARG_NONNULL(1) EINA_PURE;
   EAPI Evas_Textblock_Cursor       *evas_object_textblock_cursor_new(Evas_Object *obj) EINA_WARN_UNUSED_RESULT EINA_ARG_NONNULL(1) EINA_MALLOC;
   
   EAPI void                         evas_textblock_cursor_free(Evas_Textblock_Cursor *cur) EINA_ARG_NONNULL(1);
   
   EAPI void                         evas_textblock_cursor_node_first(Evas_Textblock_Cursor *cur) EINA_ARG_NONNULL(1);
   EAPI void                         evas_textblock_cursor_node_last(Evas_Textblock_Cursor *cur) EINA_ARG_NONNULL(1);
   EAPI Eina_Bool                    evas_textblock_cursor_node_next(Evas_Textblock_Cursor *cur) EINA_ARG_NONNULL(1);
   EAPI Eina_Bool                    evas_textblock_cursor_node_prev(Evas_Textblock_Cursor *cur) EINA_ARG_NONNULL(1);
   EAPI Eina_Bool                    evas_textblock_cursor_char_next(Evas_Textblock_Cursor *cur) EINA_ARG_NONNULL(1);
   EAPI Eina_Bool                    evas_textblock_cursor_char_prev(Evas_Textblock_Cursor *cur) EINA_ARG_NONNULL(1);
   EAPI void                         evas_textblock_cursor_char_first(Evas_Textblock_Cursor *cur) EINA_ARG_NONNULL(1);
   EAPI void                         evas_textblock_cursor_char_last(Evas_Textblock_Cursor *cur) EINA_ARG_NONNULL(1);
   EAPI void                         evas_textblock_cursor_line_first(Evas_Textblock_Cursor *cur) EINA_ARG_NONNULL(1);
   EAPI void                         evas_textblock_cursor_line_last(Evas_Textblock_Cursor *cur) EINA_ARG_NONNULL(1);
   EAPI int                          evas_textblock_cursor_pos_get(const Evas_Textblock_Cursor *cur) EINA_WARN_UNUSED_RESULT EINA_ARG_NONNULL(1) EINA_PURE;
   EAPI void                         evas_textblock_cursor_pos_set(Evas_Textblock_Cursor *cur, int pos) EINA_ARG_NONNULL(1);
   EAPI Eina_Bool                    evas_textblock_cursor_line_set(Evas_Textblock_Cursor *cur, int line) EINA_ARG_NONNULL(1);
   EAPI int                          evas_textblock_cursor_compare(const Evas_Textblock_Cursor *cur1, const Evas_Textblock_Cursor *cur2) EINA_WARN_UNUSED_RESULT EINA_ARG_NONNULL(1, 2) EINA_PURE;
   EAPI void                         evas_textblock_cursor_copy(const Evas_Textblock_Cursor *cur, Evas_Textblock_Cursor *cur_dest) EINA_ARG_NONNULL(1, 2);
   
   EAPI void                         evas_textblock_cursor_text_append(Evas_Textblock_Cursor *cur, const char *text) EINA_ARG_NONNULL(1, 2);
   EAPI void                         evas_textblock_cursor_text_prepend(Evas_Textblock_Cursor *cur, const char *text) EINA_ARG_NONNULL(1, 2);

   EAPI void                         evas_textblock_cursor_format_append(Evas_Textblock_Cursor *cur, const char *format) EINA_ARG_NONNULL(1, 2);
   EAPI void                         evas_textblock_cursor_format_prepend(Evas_Textblock_Cursor *cur, const char *format) EINA_ARG_NONNULL(1, 2);
   EAPI void                         evas_textblock_cursor_node_delete(Evas_Textblock_Cursor *cur) EINA_ARG_NONNULL(1);
   EAPI void                         evas_textblock_cursor_char_delete(Evas_Textblock_Cursor *cur) EINA_ARG_NONNULL(1);
   EAPI void                         evas_textblock_cursor_range_delete(Evas_Textblock_Cursor *cur1, Evas_Textblock_Cursor *cur2) EINA_ARG_NONNULL(1, 2);
   
   EAPI const char                  *evas_textblock_cursor_node_text_get(const Evas_Textblock_Cursor *cur) EINA_WARN_UNUSED_RESULT EINA_ARG_NONNULL(1) EINA_PURE;
   EAPI int                          evas_textblock_cursor_node_text_length_get(const Evas_Textblock_Cursor *cur) EINA_WARN_UNUSED_RESULT EINA_ARG_NONNULL(1) EINA_PURE;
   EAPI const char                  *evas_textblock_cursor_node_format_get(const Evas_Textblock_Cursor *cur) EINA_WARN_UNUSED_RESULT EINA_ARG_NONNULL(1) EINA_PURE;
   EAPI Eina_Bool                    evas_textblock_cursor_node_format_is_visible_get(const Evas_Textblock_Cursor *cur) EINA_WARN_UNUSED_RESULT EINA_ARG_NONNULL(1) EINA_PURE;
   EAPI char                        *evas_textblock_cursor_range_text_get(const Evas_Textblock_Cursor *cur1, const Evas_Textblock_Cursor *cur2, Evas_Textblock_Text_Type format) EINA_WARN_UNUSED_RESULT EINA_ARG_NONNULL(1, 2) EINA_PURE;
       
   EAPI int                          evas_textblock_cursor_char_geometry_get(const Evas_Textblock_Cursor *cur, Evas_Coord *cx, Evas_Coord *cy, Evas_Coord *cw, Evas_Coord *ch) EINA_ARG_NONNULL(1);
   EAPI int                          evas_textblock_cursor_line_geometry_get(const Evas_Textblock_Cursor *cur, Evas_Coord *cx, Evas_Coord *cy, Evas_Coord *cw, Evas_Coord *ch) EINA_ARG_NONNULL(1);
   EAPI Eina_Bool                    evas_textblock_cursor_char_coord_set(Evas_Textblock_Cursor *cur, Evas_Coord x, Evas_Coord y) EINA_ARG_NONNULL(1);
   EAPI int                          evas_textblock_cursor_line_coord_set(Evas_Textblock_Cursor *cur, Evas_Coord y) EINA_ARG_NONNULL(1);
   EAPI Eina_List                   *evas_textblock_cursor_range_geometry_get(const Evas_Textblock_Cursor *cur1, const Evas_Textblock_Cursor *cur2) EINA_WARN_UNUSED_RESULT EINA_ARG_NONNULL(1, 2) EINA_PURE;
   
   EAPI Eina_Bool                    evas_object_textblock_line_number_geometry_get(const Evas_Object *obj, int line, Evas_Coord *cx, Evas_Coord *cy, Evas_Coord *cw, Evas_Coord *ch) EINA_ARG_NONNULL(1);
   EAPI void                         evas_object_textblock_clear(Evas_Object *obj) EINA_ARG_NONNULL(1);
   EAPI void                         evas_object_textblock_size_formatted_get(const Evas_Object *obj, Evas_Coord *w, Evas_Coord *h) EINA_ARG_NONNULL(1);
   EAPI void                         evas_object_textblock_size_native_get(const Evas_Object *obj, Evas_Coord *w, Evas_Coord *h) EINA_ARG_NONNULL(1);
   EAPI void                         evas_object_textblock_style_insets_get(const Evas_Object *obj, Evas_Coord *l, Evas_Coord *r, Evas_Coord *t, Evas_Coord *b) EINA_ARG_NONNULL(1);
   
*/

#endif // EVASTEXTBLOCK_H
