#ifndef _STYLE_H_
#define _STYLE_H_

typedef struct __dox_style_bg
{
   char        *file;
   Imlib_Image  im;
} DoxStyleBg;

typedef struct __dox_style_font
{
   char       *name;
   char       *colour;
   int         size;
   Imlib_Font  font;
} DoxFont;

typedef struct __dox_style_link
{
   DoxFont  font;
   char    *colour;
} DoxStyleLink;

typedef struct __dox_style_links
{
   DoxStyleLink normal;
   DoxStyleLink visited;
} DoxStyleLinks;

typedef struct __dox_style
{
   DoxFont       title;
   DoxStyleLinks link;
   DoxStyleBg    background;
} DoxStyle;

#endif
