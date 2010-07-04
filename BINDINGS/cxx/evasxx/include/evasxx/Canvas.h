#ifndef EVAS_CANVAS_H
#define EVAS_CANVAS_H

/* STL */
#include <string>

/* EFLxx */
#include <eflxx/Eflxx.h>

/* EFL */
#include <Evas.h>
#include <Eina.h>

/* forward declaration */
class Object;

using std::string;

namespace Evasxx {

/**
 * An Evas Canvas Wrapper
 */
class Canvas : public Eflxx::Trackable
{
public:
  Canvas( const Eflxx::Size &size );
  Canvas();
  ~Canvas();

  Evas* obj() const {
    return o;
  };

  /* Output Methods */
  int lookupRenderMethod( const std::string &method );

  /** @brief Set the Evas output method.
   *  This does include a call to lookupRenderMethod().
   *  @param method Name of the output method. (See C API docs).
   */
  bool setOutputMethod( const std::string &method );

  void resize( const Eflxx::Size &size );
  Eflxx::Size getSize() const;
  Eflxx::Rect getGeometry() const;
  void setViewport( const Eflxx::Rect &rect );
  Eflxx::Rect getViewport() const;

  /* Font Path */
  void appendFontPath( const std::string &path );
  void prependFontPath( const std::string &path );
  void clearFontPath();
  const Eina_List* getFontPath();

  /* Font Cache */
  void setFontCache( unsigned int size );
  void flushFontCache();
  int getFontCache() const;

  /* Image Cache */
  void setImageCache( unsigned int size );
  void flushImageCache();
  void reloadImageCache();
  int getImageCache() const;

  /* Object queries */
  Object* focusedObject() const;
  Object* objectAtTop() const;
  Object* objectAtBottom() const;

  /*!
   * @brief C object wrapper factory method.
   *
   * For internal usage only! This return a new allocated Object that holds
   * the wrapped Evas_Object variable. With a delete on this object the wrapped
   * C type won't be freed.
   *
   * @param o The C to to be wrapped.
   * @return The wrapped C++ type.
   */
  static Canvas *wrap (Evas_Object *o);
  
  /*!
   * @brief C object wrapper factory method.
   *
   * For internal usage only! This return a new allocated Object that holds
   * the wrapped Evas_Object variable. With a delete on this object the wrapped
   * C type won't be freed.
   *
   * @param o The C to to be wrapped.
   * @return The wrapped C++ type.
   */   
  static Canvas *wrap (Evas *evas);
  
private:
  Canvas( Evas* evas );
  
  Evas* o;
  bool mFree;
};

inline ostream& operator<<( ostream& s, const Canvas& canvas )
{
  return s << "(Canvas)";
}

} // end namespace efl

/*
TODO:
   EAPI void              evas_font_path_clear              (Evas *e) EINA_ARG_NONNULL(1);
   EAPI void              evas_font_path_append             (Evas *e, const char *path) EINA_ARG_NONNULL(1, 2);
   EAPI void              evas_font_path_prepend            (Evas *e, const char *path) EINA_ARG_NONNULL(1, 2);
   EAPI const Eina_List  *evas_font_path_list               (const Evas *e) EINA_WARN_UNUSED_RESULT EINA_ARG_NONNULL(1) EINA_PURE;
   
   EAPI void              evas_font_hinting_set             (Evas *e, Evas_Font_Hinting_Flags hinting) EINA_ARG_NONNULL(1);
   EAPI Evas_Font_Hinting_Flags evas_font_hinting_get       (const Evas *e) EINA_WARN_UNUSED_RESULT EINA_ARG_NONNULL(1) EINA_PURE;
   EAPI Eina_Bool         evas_font_hinting_can_hint        (const Evas *e, Evas_Font_Hinting_Flags hinting) EINA_WARN_UNUSED_RESULT EINA_ARG_NONNULL(1) EINA_PURE;

   EAPI void              evas_font_cache_flush             (Evas *e) EINA_ARG_NONNULL(1);
   EAPI void              evas_font_cache_set               (Evas *e, int size) EINA_ARG_NONNULL(1);
   EAPI int               evas_font_cache_get               (const Evas *e) EINA_WARN_UNUSED_RESULT EINA_ARG_NONNULL(1) EINA_PURE;

   EAPI Eina_List	 *evas_font_available_list	    (const Evas *e) EINA_WARN_UNUSED_RESULT EINA_ARG_NONNULL(1) EINA_PURE;
   EAPI void		  evas_font_available_list_free	    (Evas *e, Eina_List *available) EINA_ARG_NONNULL(1);
 */

#endif // EVAS_CANVAS_H

