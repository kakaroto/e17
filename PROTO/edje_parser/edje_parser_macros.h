#ifndef EDJE_PARSER_MACROS_H
#define EDJE_PARSER_MACROS_H

#define COLLECTION  ep->collection
#define COLOR_CLASS ep->color_class
#define IMAGES      ep->images
#define SET         ep->set
#define SET_IMAGE   ep->set_image
#define GROUP       ep->group
#define PARTS       ep->parts
#define PART        ep->part
#define DESCRIPTION ep->description
#define REL         ep->rel
#define FONT        ep->font
#define FONTS       ep->fonts
#define PROGRAM     ep->program
#define PROGRAMS    ep->programs
#define EXTERNAL    ep->external
#define DATA        ep->data
#define STYLE       ep->style

#define EDJE        ep->edje
#define EDJE_CREATE if (!EDJE) EDJE = edje_new()
/* retrieve current TYPE from NAME in parser struct */
#define PARSER_CURRENT(NAME, TYPE)                                                    \
  (NAME) ? EINA_INLIST_CONTAINER_GET(                                                 \
    EINA_INLIST_GET(NAME)->last ? EINA_INLIST_GET(NAME)->last : EINA_INLIST_GET(NAME) \
    , TYPE) : NULL

/* stringshares "string" without quotes */
#define STRINGSHARE_REPLACE_NOQUOTES(VAR, TEXT)                           \
  (TEXT[0] == '"') ?                                                      \
  eina_stringshare_replace_length(&VAR, TEXT + 1, strlen(TEXT + 1) - 1) : \
  eina_stringshare_replace(&VAR, TEXT)
#define STRINGSHARE_ADD_NOQUOTES(TEXT)                          \
  (TEXT[0] == '"') ?                                            \
  eina_stringshare_add_length(TEXT + 1, strlen(TEXT + 1) - 1) : \
  eina_stringshare_add(TEXT)
#define STRDUPA_NOQUOTES(TEXT) \
        (TEXT[0] == '"') ?                                            \
  strndupa(TEXT + 1, strlen(TEXT + 1) - 1) : \
  strdupa(TEXT)
/*************************** APPEND MACROS ********************************/
/* append to parser for temp storage */
#define PARSER_APPEND(NAME, APPEND, TYPE)                                    \
  if (NAME)                                                                  \
    {                                                                        \
       Eina_Inlist *l;                                                       \
       l = edje_inlist_join(EINA_INLIST_GET(NAME), EINA_INLIST_GET(APPEND)); \
       NAME = EINA_INLIST_CONTAINER_GET(l, TYPE);                            \
    }                                                                        \
  else                                                                       \
    NAME = APPEND
/* append to edje struct */
#define EDJE_APPEND(NAME, APPEND, TYPE)                                            \
  if (EDJE->NAME)                                                                  \
    {                                                                              \
       Eina_Inlist *l;                                                             \
       l = edje_inlist_join(EINA_INLIST_GET(EDJE->NAME), EINA_INLIST_GET(APPEND)); \
       EDJE->NAME = EINA_INLIST_CONTAINER_GET(l, TYPE);                            \
    }                                                                              \
  else                                                                             \
    EDJE->NAME = APPEND
/* append to current (struct->name) */
#define EDJE_STRUCT_APPEND(STRUCT, NAME, APPEND, TYPE)                               \
  if (STRUCT->NAME)                                                                  \
    {                                                                                \
       Eina_Inlist *l;                                                               \
       l = edje_inlist_join(EINA_INLIST_GET(STRUCT->NAME), EINA_INLIST_GET(APPEND)); \
       STRUCT->NAME = EINA_INLIST_CONTAINER_GET(l, TYPE);                            \
    }                                                                                \
  else                                                                               \
    STRUCT->NAME = APPEND
/**************************** ERROR MACROS **********************************/
#define ERROR_SYNTAX(TOKEN) do {                                                                                   \
       if ((!TOKEN) || (!TOKEN->type))                                                                             \
         eina_stringshare_replace(&ep->error, "Unexpected EOF!\n");                                                \
       else                                                                                                        \
         {                                                                                                         \
            eina_stringshare_del(ep->error);                                                                       \
            ep->error = eina_stringshare_printf(                                                                   \
                "Syntax error around line %d column %d: unexpected property (or mismatched arguments) %s: '%s'\n", \
                TOKEN->sline + 1, TOKEN->scol + 1, yyTokenName[TOKEN->type], TOKEN->text);                         \
         }                                                                                                         \
       ERR("%s", ep->error);                                                                                       \
  } while (0)
#define ERROR_RANGE(TOKEN)  do {                                               \
       eina_stringshare_del(ep->error);                                        \
       ep->error = eina_stringshare_printf(                                    \
           "Syntax error around line %d column %d: Out of range value '%s'\n", \
           TOKEN->sline + 1, TOKEN->scol + 1, TOKEN->text);                    \
       ERR("%s", ep->error);                                                   \
  } while (0)
#define ERROR_TYPE(TOKEN)   do {                                                                \
       eina_stringshare_del(ep->error);                                                         \
       ep->error = eina_stringshare_printf(                                                     \
           "Syntax error around line %d column %d: Block type '%s' does not match part type\n", \
           TOKEN->sline + 1, TOKEN->scol + 1, TOKEN->text);                                     \
       ERR("%s", ep->error);                                                                    \
  } while (0)

#endif
