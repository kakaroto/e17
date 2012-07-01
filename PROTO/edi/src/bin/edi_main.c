#ifdef HAVE_CONFIG_H
# include "config.h"
#endif

#include <Eina.h>
#include <Ecore_Getopt.h>
#include <Elementary.h>

#include "edi_private.h"

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

static const int colors[][4] =
{
  { 0, 0, 0, 0 }, /* background */
  { 0, 128, 255, 255 }, /* language keyword */
  { 0, 255, 64, 255 }, /* type */
  { 0, 0, 255, 255 }, /* function declaration */
  { 0, 255, 64, 255 }, /* variable declaration */
  { 128, 0, 0, 255 }, /* comments */
  { 128, 128, 128, 255 }, /* default */
  { 0, 0, 255, 255 }, /* warning */
  { 255, 0, 0, 255 }, /* error */
  { 255, 128, 0, 255 } /* fatal */
};

static void
_clang_load_errors(Edi_File *ef)
{
   int tgridw = 0, tgridh = 0;
   unsigned n = clang_getNumDiagnostics(ef->tx_unit);
   unsigned i = 0;

   evas_object_textgrid_size_get(textgrid, &tgridw, &tgridh);

   for(i = 0, n = clang_getNumDiagnostics(ef->tx_unit); i != n; ++i)
     {
        CXDiagnostic diag = clang_getDiagnostic(ef->tx_unit, i);
        /* FIXME: Do something other than 0 */
        unsigned line, col;
        unsigned endline, endcol;
        Evas_Textgrid_Cell *cells;

        clang_getPresumedLocation(clang_getDiagnosticLocation(diag), NULL, &line, &col);
        endline = line;
        endcol = col;

        /* Skip/break if we are out of the textgrid's size. */
        if (endline < ef->offset)
           continue;
        else if (line >= (ef->offset + tgridh))
           continue;

        /* FIXME: Currently we just skip here, should be done better. */
        if (col > tgridw)
           continue;

#if 0
        /* Make sure we don't try to mark past the tgrid size. */
        if (endcol > tgridw)
           endcol = tgridw; /* Decreased later. */
        /* FIXME: Should also show ranges. */
        CXSourceRange dgrange = clang_getDiagnosticRange(diag, 1);
        clang_getPresumedLocation(clang_getRangeStart(dgrange), NULL, &line, &col);
        clang_getPresumedLocation(clang_getRangeEnd(dgrange), NULL, &endline, &endcol);
        endline--;
        endcol--;
#endif
        /* Dec line and col... because we start from 0 */
        line -= ef->offset;
        col--;

        cells = evas_object_textgrid_cellrow_get(textgrid, line);
        switch (clang_getDiagnosticSeverity(diag))
          {
           case CXDiagnostic_Ignored:
           case CXDiagnostic_Note:
              break;
           case CXDiagnostic_Warning:
              // FIXME: Make it an enum
              cells[col].bg = 7;
              break;
           case CXDiagnostic_Error:
              // FIXME: Make it an enum
              cells[col].bg = 8;
              break;
           case CXDiagnostic_Fatal:
              // FIXME: Make it an enum
              cells[col].bg = 9;
              break;
          }

        /* FIXME: Should mark all the relevant ranges? */

        evas_object_textgrid_cellrow_set(textgrid, line, cells);

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

   CXIndex idx = clang_createIndex(0, 0);
   ef->tx_unit = clang_parseTranslationUnit(idx, filename, clang_argv, clang_argc, NULL, 0, CXTranslationUnit_None);

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
   int s;

   l = f->current;
   evas_object_geometry_get(txtgrid, NULL, NULL, &w, &h);
   evas_object_textgrid_cell_size_get(txtgrid, &cw, &ch);
   evas_object_textgrid_size_set(txtgrid, w / cw, h / ch);

   evas_object_textgrid_size_get(txtgrid, &w, &h);

   s = f->offset;
   for (y = 0; y < h && l; y++, s++)
     {
        int idx = 0;

        el = eina_list_data_get(l);
        l = eina_list_next(l);

        while (y < h && s < el->read_only.line.index)
          {
             cells = evas_object_textgrid_cellrow_get(txtgrid, y);
             for (x = 0; x < w; x++)
               {
                  cells[x].codepoint = 0;
                  cells[x].fg = 0;
               }
             evas_object_textgrid_cellrow_set(txtgrid, y++, cells);
             s++;
          }

        if (y == h) break;

        cells = evas_object_textgrid_cellrow_get(txtgrid, y);
        for (x = 0; x < w && idx < el->read_only.length; x++)
          {
             Eina_Unicode u;

             /* FIXME: there is an overrun to fix here */
             u = eina_unicode_utf8_get_next(el->read_only.line.start, &idx);
             cells[x].codepoint = u;
             cells[x].fg = 6;
             /* cells[x].bg = 6; */
          }
        for (; x < w; x++)
          {
             cells[x].codepoint = 0;
             cells[x].fg = 0;
          }
        evas_object_textgrid_cellrow_set(txtgrid, y, cells);
     }
   for (; y < h; y++)
     {
        cells = evas_object_textgrid_cellrow_get(txtgrid, y);
        for (x = 0; x < w; x++)
          {
             cells[x].codepoint = 0;
             cells[x].fg = 0;
          }
        evas_object_textgrid_cellrow_set(txtgrid, y++, cells);
     }
   evas_object_textgrid_update_add(txtgrid, 0, 0, w, h);

   _clang_load_errors(f);
}

static void
_move_to(Evas_Object *textgrid, Edi_File *f, int step)
{
   if (step < 0)
     {
        while (f->current && step)
          {
             Edi_Line *l;

             l = eina_list_data_get(f->current);
             while (step && l->read_only.line.index != f->offset)
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
   _edi_file_fill(textgrid, f);
}

static void
_resize(void *data, Evas *e, Evas_Object *obj, void *event_info)
{
   Edi_File *f = data;

   _edi_file_fill(obj, f);
}

static void
_key_down(void *data, Evas *e, Evas_Object *obj, void *event_info)
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
   for (i = 0; i < sizeof (colors) / sizeof (colors[0]); i++)
     evas_object_textgrid_palette_set(o, EVAS_TEXTGRID_PALETTE_STANDARD, i,
                                      colors[i][0], colors[i][1], colors[i][2], colors[i][3]);

   evas_object_textgrid_font_set(o, "Terminus", 10);
   textgrid = o;

   evas_object_show(win);

   file = _edi_file_open(argv[1]);
   _edi_file_fill(textgrid, file);

   evas_object_event_callback_add(o, EVAS_CALLBACK_RESIZE, _resize, file);
   evas_object_event_callback_add(win, EVAS_CALLBACK_KEY_DOWN, _key_down, file);

   elm_run();
 end:

   _edi_file_close(file);   

   eina_log_domain_unregister(_log_domain);
   _log_domain = -1;

   elm_shutdown();
   return retval;
}
ELM_MAIN()
