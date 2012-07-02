#ifdef HAVE_CONFIG_H
# include "config.h"
#endif

#include <Eina.h>
#include <Ecore_Getopt.h>
#include <Elementary.h>

#include "edi_private.h"

#include "clang_debug.h"

int _log_domain = 0;
Evas_Object *win;
Evas_Object *textgrid;
Edi_File *file;

static const Ecore_Getopt options = {
  PACKAGE_NAME,
  "%prog [options]",
  PACKAGE_VERSION,
  "(C) 2012 Tom Hacohen, Cedric Bail and others",
  "BSD 2-Clause",
  "IDE written with Enlightenment Foundation Libraries and LLVM.",
  EINA_TRUE,
  {
    ECORE_GETOPT_VERSION('V', "version"),
    ECORE_GETOPT_COPYRIGHT('C', "copyright"),
    ECORE_GETOPT_LICENSE('L', "license"),
    ECORE_GETOPT_HELP('h', "help"),
    ECORE_GETOPT_SENTINEL
  }
};

typedef enum {
     EDI_COLOR_BACKGROUND_DEFAULT = 0,
     EDI_COLOR_FOREGROUND_DEFAULT,

     /* SEVIRITY */
     EDI_COLOR_BACKGROUND_SEVIRITY_IGNORED,
     EDI_COLOR_BACKGROUND_SEVIRITY_NOTE,
     EDI_COLOR_BACKGROUND_SEVIRITY_WARNING,
     EDI_COLOR_BACKGROUND_SEVIRITY_ERROR,
     EDI_COLOR_BACKGROUND_SEVIRITY_FATAL,

     /* SYNTAX */
     EDI_COLOR_FOREGROUND_PUNCTUATION,
     EDI_COLOR_FOREGROUND_KEYWORD,
     EDI_COLOR_FOREGROUND_REF,
     EDI_COLOR_FOREGROUND_COMMENT,
     EDI_COLOR_FOREGROUND_LITERAL,
     EDI_COLOR_FOREGROUND_USER_TYPE,
     EDI_COLOR_FOREGROUND_PREPROCESSING_DIRECTIVE
} Edi_Color;

static const int colors[][4] =
{
  { 0, 0, 0, 255 }, /* background */
  { 0xDB, 0xDB, 0xDB, 255 }, /* default */

  /* SEVIRITY */
  { 0, 0, 0, 255 }, /* ignored */
  { 0, 0, 0, 255 }, /* note */
  { 0xEC, 0xF3, 0x20, 255 }, /* warning */
  { 0xEF, 0x29, 0x29, 255 }, /* error */
  { 255, 128, 0, 255 }, /* fatal */

  /* SYNTAX */
  { 0x4E, 0x9A, 0x06, 255 }, /* punctuation */
  { 0x4E, 0x9A, 0x06, 255 }, /* keyword */
  { 128, 128, 128, 255 }, /* ref */
  { 0x34, 0x65, 0xA4, 255 }, /* comment */
  { 0xCC, 0x00, 0x00, 255 }, /* literal */
  { 0x4E, 0x9A, 0x06, 255 }, /* usertype */
  { 0x75, 0x50, 0x7B, 255 } /* preprocessor */
};

typedef struct
{
   unsigned int line;
   unsigned int col;
} Edi_Location;

typedef struct
{
   Edi_Location start;
   Edi_Location end;
} Edi_Range;

void
_edi_fg_set(Evas_Textgrid_Cell *cell, Edi_Color color)
{
   cell->fg = color;
}

void
_edi_bg_set(Evas_Textgrid_Cell *cell, Edi_Color color)
{
   cell->bg = color;
}

/* FIXME: Don't use the callback hack, there are better ways to avoid
 * code duplication... */
static void
_edi_range_color_set(Edi_File *ef, Edi_Range range, Edi_Color color, void (*func)(Evas_Textgrid_Cell *cell, Edi_Color color))
{
   int tgridw = 0, tgridh = 0;
   evas_object_textgrid_size_get(textgrid, &tgridw, &tgridh);

   unsigned int i;
   for (i = range.start.line ; i <= range.end.line ; i++)
     {
        unsigned int j = 0;
        unsigned int line = i - ef->offset;
        unsigned int start_col, end_col;
        Evas_Textgrid_Cell *cells, *cell;

        if (i < ef->offset)
           continue;
        else if (i >= (ef->offset + tgridh))
           continue;

        cells = evas_object_textgrid_cellrow_get(textgrid, line);

        /* FIXME: Could be done faster, just cleaner this way... */
        start_col = (i == range.start.line) ?  range.start.col : 0;
        end_col = (i == range.end.line) ?  range.end.col : (unsigned int) tgridw;

        for (cell = cells + (start_col - 1), j = start_col ; j <= end_col ; cell++, j++)
          {
             func(cell, color);
          }

        evas_object_textgrid_cellrow_set(textgrid, line, cells);
     }
}

static void
_clang_load_highlighting(Edi_File *ef)
{
   CXToken *tokens = NULL;
   unsigned int n = 0;
   unsigned int i = 0;
   CXCursor *cursors = NULL;

     {
        CXFile cfile = clang_getFile(ef->tx_unit, eina_file_filename_get(ef->f));
        int tgridw = 0, tgridh = 0;
        evas_object_textgrid_size_get(textgrid, &tgridw, &tgridh);

#if 0
        /* FIXME: Should be used, I don't know why tokenize doesn't work in mid
         * comment cases and etc. */
        int range_start, range_end;
        range_start = ef->offset;
        range_end = ef->offset + tgridh;
        CXSourceRange range = clang_getRange(clang_getLocation(ef->tx_unit, cfile, range_start, 1), clang_getLocation(ef->tx_unit, cfile, range_end, tgridw));
#else
        CXSourceRange range = clang_getRange(
              clang_getLocationForOffset(ef->tx_unit, cfile, 0),
              clang_getLocationForOffset(ef->tx_unit, cfile, eina_file_size_get(ef->f)));
#endif
        clang_tokenize(ef->tx_unit, range, &tokens, &n);
        /* FIXME: We should use annotate tokens and then use a lot more
         * color classes. I don't know why it's broken ATM... :( */
        cursors = (CXCursor *) malloc(n * sizeof(CXCursor));
        clang_annotateTokens(ef->tx_unit, tokens, n, cursors);
     }

   for (i = 0 ; i < n ; i++)
     {
        Edi_Range range;
        Edi_Color color = EDI_COLOR_FOREGROUND_DEFAULT;

        CXSourceRange tkrange = clang_getTokenExtent(ef->tx_unit, tokens[i]);
        clang_getSpellingLocation(clang_getRangeStart(tkrange), NULL,
              &range.start.line, &range.start.col, NULL);
        clang_getSpellingLocation(clang_getRangeEnd(tkrange), NULL,
              &range.end.line, &range.end.col, NULL);
        /* FIXME: Should probably do something fancier, this is only a limited
         * number of types. */
        switch (clang_getTokenKind(tokens[i]))
          {
             case CXToken_Punctuation:
             case CXToken_Identifier:
                switch (cursors[i].kind)
                  {
                   case CXCursor_DeclRefExpr:
                      /* Handle different ref kinds */
                      color = EDI_COLOR_FOREGROUND_REF;
                      break;
                   case CXCursor_PreprocessingDirective:
                      color = EDI_COLOR_FOREGROUND_PREPROCESSING_DIRECTIVE;
                      break;
                   case CXCursor_TypeRef:
                      color = EDI_COLOR_FOREGROUND_USER_TYPE;
                      break;
                   default:
                      color = EDI_COLOR_FOREGROUND_DEFAULT;
                      break;
                  }
                break;
             case CXToken_Keyword:
                color = EDI_COLOR_FOREGROUND_KEYWORD;
                break;
             case CXToken_Literal:
                color = EDI_COLOR_FOREGROUND_LITERAL;
                break;
             case CXToken_Comment:
                color = EDI_COLOR_FOREGROUND_COMMENT;
                break;
          }

        _edi_range_color_set(ef, range, color, _edi_fg_set);

#if 0
        const char *kind = NULL;
        switch (clang_getTokenKind(tokens[i])) {
           case CXToken_Punctuation: kind = "Punctuation"; break;
           case CXToken_Keyword: kind = "Keyword"; break;
           case CXToken_Identifier: kind = "Identifier"; break;
           case CXToken_Literal: kind = "Literal"; break;
           case CXToken_Comment: kind = "Comment"; break;
        }

        printf("%s ", kind);
        PrintToken(ef->tx_unit, tokens[i]);

        if (!clang_isInvalid(cursors[i].kind)) {
             printf(" ");
             PrintCursor(cursors[i]);
        }

        printf("\n");
#endif
     }

   free(cursors);
   clang_disposeTokens(ef->tx_unit, tokens, n);
}

static void
_clang_load_errors(Edi_File *ef)
{
   unsigned n = clang_getNumDiagnostics(ef->tx_unit);
   unsigned i = 0;

   for(i = 0, n = clang_getNumDiagnostics(ef->tx_unit); i != n; ++i)
     {
        CXDiagnostic diag = clang_getDiagnostic(ef->tx_unit, i);
        Edi_Range range;

        clang_getSpellingLocation(clang_getDiagnosticLocation(diag), NULL, &range.start.line, &range.start.col, NULL);
        range.end = range.start;

        /* FIXME: Also handle ranges and fix suggestions. */

        Edi_Color color = EDI_COLOR_BACKGROUND_DEFAULT;

        switch (clang_getDiagnosticSeverity(diag))
          {
           case CXDiagnostic_Ignored:
              color = EDI_COLOR_BACKGROUND_SEVIRITY_IGNORED;
              break;
           case CXDiagnostic_Note:
              color = EDI_COLOR_BACKGROUND_SEVIRITY_NOTE;
              break;
           case CXDiagnostic_Warning:
              color = EDI_COLOR_BACKGROUND_SEVIRITY_WARNING;
              break;
           case CXDiagnostic_Error:
              color = EDI_COLOR_BACKGROUND_SEVIRITY_ERROR;
              break;
           case CXDiagnostic_Fatal:
              color = EDI_COLOR_BACKGROUND_SEVIRITY_FATAL;
              break;
          }

        _edi_range_color_set(ef, range, color, _edi_bg_set);

#if 0
        CXString str = clang_formatDiagnostic(diag, clang_defaultDiagnosticDisplayOptions());
        printf("DEBUG: Diag:%s\n", clang_getCString(str));
        clang_disposeString(str);
#endif

        clang_disposeDiagnostic(diag);
     }
}

static Edi_File *
_edi_file_open(const char *filename)
{
   Edi_File *ef;
   Eina_File *f;
   Eina_Iterator *it;
   Eina_File_Lines *l;
   void *m;

   f = eina_file_open(filename, EINA_FALSE);
   if (!f) return NULL;

   m = eina_file_map_all(f, EINA_FILE_WILLNEED);
   if (!m) goto end;

   ef = calloc(1, sizeof (Edi_File));
   if (!ef) goto end;

   ef->f = f;
   ef->m = m;

   it = eina_file_map_lines(f);
   EINA_ITERATOR_FOREACH(it, l)
     {
        Edi_Line *el;

        el = calloc(1, sizeof (Edi_Line));
        if (!el) continue ;

        el->read_only = *l;

        ef->lines = eina_list_append(ef->lines, el);
     }
   eina_iterator_free(it);

   ef->current = ef->lines;
   ef->offset = 1;

   /* Clang */
   /* FIXME: index should probably be global. */
   const char const *clang_argv[] = {"-I/usr/lib/clang/3.1/include/", "-Wall", "-Wextra"};
   int clang_argc = sizeof(clang_argv) / sizeof(*clang_argv);

   ef->idx = clang_createIndex(0, 0);
   ef->tx_unit = clang_parseTranslationUnit(ef->idx, eina_file_filename_get(ef->f), clang_argv, clang_argc, NULL, 0, CXTranslationUnit_None);

   return ef;

 end:
   if (m) eina_file_map_free(f, m);
   eina_file_close(f);
   return NULL;
}

static void
_edi_file_close(Edi_File *f)
{
   Edi_Line *l;

   EINA_LIST_FREE(f->lines, l)
     free(l);
   eina_file_map_free(f->f, f->m);
   eina_file_close(f->f);

   clang_disposeTranslationUnit(f->tx_unit);
   clang_disposeIndex(f->idx);

   free(f);
}

static void
_edi_file_fill(Evas_Object *txtgrid, Edi_File *f)
{
   Evas_Textgrid_Cell *cells;
   Edi_Line *el;
   Eina_List *l;
   int cw, ch;
   int w, h;
   int x, y;
   unsigned int s;

   l = f->current;
   evas_object_geometry_get(txtgrid, NULL, NULL, &w, &h);
   evas_object_textgrid_cell_size_get(txtgrid, &cw, &ch);
   evas_object_textgrid_size_set(txtgrid, ceil(((double) w) / cw),
         ceil(((double) h) / ch));

   evas_object_textgrid_size_get(txtgrid, &w, &h);

   s = f->offset;
   for (y = 0; y < h && l; y++, s++)
     {
        int idx = 0;

        el = eina_list_data_get(l);
        l = eina_list_next(l);

        while ((y < h) && (s < el->read_only.line.index))
          {
             cells = evas_object_textgrid_cellrow_get(txtgrid, y);
             for (x = 0; x < w; x++)
               {
                  cells[x].codepoint = 0;
                  cells[x].bg = EDI_COLOR_BACKGROUND_DEFAULT;
                  cells[x].fg = EDI_COLOR_FOREGROUND_DEFAULT;
               }
             evas_object_textgrid_cellrow_set(txtgrid, y++, cells);
             s++;
          }

        if (y == h) break;

        cells = evas_object_textgrid_cellrow_get(txtgrid, y);
        for (x = 0 ; (x < w) && (idx < (int) el->read_only.length) ; x++)
          {
             Eina_Unicode u;

             /* FIXME: there is an overrun to fix here */
             u = eina_unicode_utf8_get_next(el->read_only.line.start, &idx);
             cells[x].codepoint = u;
             cells[x].bg = EDI_COLOR_BACKGROUND_DEFAULT;
             cells[x].fg = EDI_COLOR_FOREGROUND_DEFAULT;
          }
        for (; x < w; x++)
          {
             cells[x].codepoint = 0;
             cells[x].bg = EDI_COLOR_BACKGROUND_DEFAULT;
             cells[x].fg = EDI_COLOR_FOREGROUND_DEFAULT;
          }
        evas_object_textgrid_cellrow_set(txtgrid, y, cells);
     }
   for (; y < h; y++)
     {
        cells = evas_object_textgrid_cellrow_get(txtgrid, y);
        for (x = 0; x < w; x++)
          {
             cells[x].codepoint = 0;
             cells[x].bg = EDI_COLOR_BACKGROUND_DEFAULT;
             cells[x].fg = EDI_COLOR_FOREGROUND_DEFAULT;
          }
        evas_object_textgrid_cellrow_set(txtgrid, y, cells);
     }
   evas_object_textgrid_update_add(txtgrid, 0, 0, w, h);

   _clang_load_errors(f);
   _clang_load_highlighting(f);
}

static void
_move_to(Evas_Object *tgrid, Edi_File *f, int step)
{
   if (step < 0)
     {
        while (f->current && step)
          {
             Edi_Line *l;

             l = eina_list_data_get(f->current);
             while (step && (l->read_only.line.index != f->offset))
               {
                  step++;
                  f->offset++;
               }

             if (step)
               {
                  step++;
                  f->offset++;
                  f->current = eina_list_next(f->current);
               }
          }

        if (!f->current)
          {
             f->current = eina_list_last(f->lines);
             f->offset--;
          }
     }
   else if (step > 0)
     {
        while (f->current && step)
          {
             Edi_Line *l;

             l = eina_list_data_get(f->current);
             while (step && l->read_only.line.index != f->offset)
               {
                  step--;
                  f->offset--;
               }

             if (step)
               {
                  step--;
                  f->offset--;
                  f->current = eina_list_prev(f->current);
               }
          }

        if (!f->current)
          {
             f->current = f->lines;
             f->offset++;
          }
     }
   _edi_file_fill(tgrid, f);
}

static void
_resize(void *data, Evas *e EINA_UNUSED, Evas_Object *obj, void *event_info EINA_UNUSED)
{
   Edi_File *f = data;

   _edi_file_fill(obj, f);
}

static void
_mouse_down(void *data, Evas *e EINA_UNUSED, Evas_Object *obj, void *event_info)
{
   Edi_File *ef = data;
   Evas_Coord x, y, cw, ch;
   int line, col;
   Evas_Event_Mouse_Down *ev = event_info;
   CXFile cfile = clang_getFile(ef->tx_unit, eina_file_filename_get(ef->f));
   evas_object_geometry_get(obj, &x, &y, NULL, NULL);
   evas_object_textgrid_cell_size_get(obj, &cw, &ch);

   line = (ev->canvas.y - y) / ch;
   col = (ev->canvas.x - x) / cw;

   CXCursor ref = clang_getCursor(ef->tx_unit, clang_getLocation(ef->tx_unit, cfile, line + 1, col + 1));
     {
        CXCursor dest = clang_getCursorReferenced(ref);
        if (!clang_Cursor_isNull(dest))
          {
             PrintCursor(dest);
             printf("\n");
          }
     }
}

static void
_key_down(void *data, Evas *e EINA_UNUSED, Evas_Object *obj EINA_UNUSED, void *event_info)
{
   Evas_Event_Key_Down *ev = event_info;

   if (evas_key_modifier_is_set(ev->modifiers, "Shift"))
     {
        if (!strcmp("Prior", ev->keyname))
          {
             int h;

             evas_object_textgrid_size_get(textgrid, NULL, &h);
             _move_to(textgrid, data, h);
          }
        else if (!strcmp("Next", ev->keyname))
          {
             int h;

             evas_object_textgrid_size_get(textgrid, NULL, &h);
             _move_to(textgrid, data, -h);
          }
        else if (!strcmp("Up", ev->keyname))
          {
             _move_to(textgrid, data, 1);
          }
        else if (!strcmp("Down", ev->keyname))
          {
             _move_to(textgrid, data, -1);
          }
     }
}

EAPI_MAIN int
elm_main(int argc, char **argv)
{
   Eina_Bool quit_option = EINA_FALSE;
   Ecore_Getopt_Value values[] = {
     ECORE_GETOPT_VALUE_BOOL(quit_option),
     ECORE_GETOPT_VALUE_BOOL(quit_option),
     ECORE_GETOPT_VALUE_BOOL(quit_option),
     ECORE_GETOPT_VALUE_BOOL(quit_option),
     ECORE_GETOPT_VALUE_NONE
   };
   char buf[4096];
   int args, retval = EXIT_SUCCESS;
   Evas_Object *o;
   int i;

   if (argc != 2) return -1;

   _log_domain = eina_log_domain_register("edi", NULL);

   elm_policy_set(ELM_POLICY_QUIT, ELM_POLICY_QUIT_LAST_WINDOW_CLOSED);
   elm_app_compile_bin_dir_set(PACKAGE_BIN_DIR);
   elm_app_compile_data_dir_set(PACKAGE_DATA_DIR);
   elm_app_info_set(elm_main, "edi", "themes/default.edj");

   args = ecore_getopt_parse(&options, values, argc, argv);
   if (args < 1)
     {
        ERR("Could not parse command line options.");
        retval = EXIT_FAILURE; goto end;
     }

   /* Create the window */
   win = elm_win_add(NULL, "main", ELM_WIN_BASIC);
   elm_win_autodel_set(win, EINA_TRUE);
   elm_win_title_set(win, "EDI");
   elm_win_icon_name_set(win, "EDI");
   evas_object_resize(win, 800, 600);

   /* Setup window icon */
   o = evas_object_image_add(evas_object_evas_get(win));
   snprintf(buf, sizeof(buf), "%s/images/edi.png",
            elm_app_data_dir_get());
   evas_object_image_file_set(o, buf, NULL);
   elm_win_icon_object_set(win, o);

   /* Black background for the moment */
   o = evas_object_rectangle_add(evas_object_evas_get(win));
   evas_object_color_set(o, 0, 0, 0, 255);
   evas_object_size_hint_weight_set(o, EVAS_HINT_EXPAND, EVAS_HINT_EXPAND);
   evas_object_size_hint_fill_set(o, EVAS_HINT_FILL, EVAS_HINT_FILL);
   elm_win_resize_object_add(win, o);
   evas_object_show(o);

   /* And now the Textgrid */
   o = evas_object_textgrid_add(evas_object_evas_get(win));
   evas_object_size_hint_weight_set(o, EVAS_HINT_EXPAND, EVAS_HINT_EXPAND);
   evas_object_size_hint_fill_set(o, EVAS_HINT_FILL, EVAS_HINT_FILL);
   elm_win_resize_object_add(win, o);
   evas_object_show(o);

   /* Basic setup of the Textgrid */
   for (i = 0; i < (int) (sizeof (colors) / sizeof (colors[0])); i++)
     evas_object_textgrid_palette_set(o, EVAS_TEXTGRID_PALETTE_STANDARD, i,
                                      colors[i][0], colors[i][1], colors[i][2], colors[i][3]);

   evas_object_textgrid_font_set(o, "Mono", 10);
   textgrid = o;

   evas_object_show(win);

   file = _edi_file_open(argv[1]);
   _edi_file_fill(textgrid, file);

   evas_object_event_callback_add(o, EVAS_CALLBACK_RESIZE, _resize, file);
   evas_object_event_callback_add(win, EVAS_CALLBACK_KEY_DOWN, _key_down, file);
   evas_object_event_callback_add(textgrid, EVAS_CALLBACK_MOUSE_DOWN, _mouse_down, file);

   elm_run();
 end:

   _edi_file_close(file);   

   eina_log_domain_unregister(_log_domain);
   _log_domain = -1;

   elm_shutdown();
   return retval;
}
ELM_MAIN()
