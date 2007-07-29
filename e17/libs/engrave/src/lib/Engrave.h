#ifndef ENGRAVE_H
#define ENGRAVE_H

#ifdef EAPI
#undef EAPI
#endif
#ifdef WIN32
# ifdef BUILDING_DLL
#  define EAPI __declspec(dllexport)
# else
#  define EAPI __declspec(dllimport)
# endif
#else
# ifdef __GNUC__
#  if __GNUC__ >= 4
#   define EAPI __attribute__ ((visibility("default")))
#  else
#   define EAPI
#  endif
# else
#  define EAPI
# endif
#endif

/**
 * @file Engrave.h
 * @brief The file that should be included by any project using Engrave.
 * It provides all the necessary headers and includes to work with Engrave.
 */

/**
 * @mainpage Engrave Library Documentation
 *
 * @section intro Introduction
 *
 * Engrave is a designed to allow you the ability to easily create and edit
 * Edje EDJ files.
 *
 * As an example of how easy Engrave is to work with, the following example
 * will read in either an EDC file or an EDJ file and attempt to write out
 * an EDJ and EDC version of the given file.
 *
 * @code
 * #include "Engrave.h"
 * 
 * int
 * main(int argc, char ** argv)
 * {
 *     Engrave_File *ef = NULL;
 *
 *     if (argc < 2) {
 *         printf("need file\n");
 *         return 1;
 *     }
 *
 *     if (strstr(argv[1], ".edj"))
 *         ef = engrave_load_edj(argv[1]);
 *     else {
 *         if (argc < 4) {
 *             printf("need img and font dirs with .edc file\n");
 *             return 1;
 *         }
 *         ef = engrave_load_edc(argv[1], argv[2], argv[3]);
 *     }
 * 
 *     if (!engrave_edj_output(ef, "test.edj"))
 *         printf("failed to write test.edj\n");
 * 
 *     if (!engrave_edc_output(ef, "test.out"))
 *         printf("failed to write test.out\n"); 
 *
 *     return 0;
 * }
 * @endcode
 *
 * Compiling with the Engrave library is pretty simple, assuming you've
 * named your app engrave_test.c the following command will do the trick:
 *
 * @code
 * gcc -o engrave_test `engrave-config --cflags --libs` engrave_test.c
 * @endcode
 *
 * @section Conclusion
 * Engrave has been designed to make it easy to open and maniuplate Edje
 * files, be they EDJ or EDC.
 *
 * If you have any questions or comments about Engrave please email
 * enlightenment-devel at lists dot sourceforge dot net
 *
 */

#ifdef __cplusplus
extern "C" {
#endif

#include <Evas.h>

#include <engrave_enums.h>
#include <engrave_data.h>
#include <engrave_image.h>
#include <engrave_font.h>
#include <engrave_spectrum.h>
#include <engrave_color_class.h>
#include <engrave_program.h>
#include <engrave_part_state.h>
#include <engrave_part.h>
#include <engrave_group.h>
#include <engrave_style.h>
#include <engrave_file.h>

#include <engrave_load.h>
#include <engrave_out.h>

#include <engrave_canvas.h>

#ifdef __cplusplus
}
#endif
#endif

