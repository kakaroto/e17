

/* Build async events support */
#define BUILD_ASYNC_EVENTS 1

/* Build async image preload support */
/* #undef BUILD_ASYNC_PRELOAD */

/* Build plain C code */
#define BUILD_C 1

/* 16bpp BGR 565 Converter Support */
#define BUILD_CONVERT_16_BGR_565 1

/* 16bpp RGB 444 Converter Support */
#define BUILD_CONVERT_16_RGB_444 1

/* 16bpp 565 (444 ipaq) Converter Support */
#define BUILD_CONVERT_16_RGB_454645 1

/* 16bpp RGB 555 Converter Support */
#define BUILD_CONVERT_16_RGB_555 1

/* 16bpp RGB 565 Converter Support */
#define BUILD_CONVERT_16_RGB_565 1

/* 16bpp RGB Rotation 0 Converter Support */
#define BUILD_CONVERT_16_RGB_ROT0 1

/* 16bpp RGB Rotation 180 Converter Support */
#define BUILD_CONVERT_16_RGB_ROT180 1

/* 16bpp RGB Rotation 270 Converter Support */
#define BUILD_CONVERT_16_RGB_ROT270 1

/* 16bpp RGB Rotation 90 Converter Support */
#define BUILD_CONVERT_16_RGB_ROT90 1

/* 24bpp BGR 888 Converter Support */
#define BUILD_CONVERT_24_BGR_888 1

/* 24bpp 666 (666 ezx) Converter Support */
#define BUILD_CONVERT_24_RGB_666 1

/* 24bpp RGB 888 Converter Support */
#define BUILD_CONVERT_24_RGB_888 1

/* 32bpp BGRX 8888 Converter Support */
#define BUILD_CONVERT_32_BGRX_8888 1

/* 32bpp BGR 8888 Converter Support */
#define BUILD_CONVERT_32_BGR_8888 1

/* 32bpp RGBX 8888 Converter Support */
#define BUILD_CONVERT_32_RGBX_8888 1

/* 32bpp 666 (666 ezx) Converter Support */
#define BUILD_CONVERT_32_RGB_666 1

/* 32bpp RGB 8888 Converter Support */
#define BUILD_CONVERT_32_RGB_8888 1

/* 32bpp RGB Rotation 0 Converter Support */
#define BUILD_CONVERT_32_RGB_ROT0 1

/* 32bpp RGB Rotation 180 Converter Support */
#define BUILD_CONVERT_32_RGB_ROT180 1

/* 32bpp RGB Rotation 270 Converter Support */
#define BUILD_CONVERT_32_RGB_ROT270 1

/* 32bpp RGB Rotation 90 Converter Support */
#define BUILD_CONVERT_32_RGB_ROT90 1

/* 32bpp Grayscale 64-palette Converter Support */
#define BUILD_CONVERT_8_GRAYSCALE_64 1

/* 8bpp GRY 1 Converter Support */
#define BUILD_CONVERT_8_GRY_1 1

/* 8bpp GRY 16 Converter Support */
#define BUILD_CONVERT_8_GRY_16 1

/* 8bpp RGB 111 Converter Support */
#define BUILD_CONVERT_8_RGB_111 1

/* 8bpp RGB 121 Converter Support */
#define BUILD_CONVERT_8_RGB_121 1

/* 8bpp RGB 221 Converter Support */
#define BUILD_CONVERT_8_RGB_221 1

/* 8bpp RGB 222 Converter Support */
#define BUILD_CONVERT_8_RGB_222 1

/* 8bpp RGB 232 Converter Support */
#define BUILD_CONVERT_8_RGB_232 1

/* 8bpp RGB 332 Converter Support */
#define BUILD_CONVERT_8_RGB_332 1

/* 8bpp RGB 666 Converter Support */
#define BUILD_CONVERT_8_RGB_666 1

/* YUV Converter Support */
#define BUILD_CONVERT_YUV 1

/* Buffer rendering backend */
#define BUILD_ENGINE_BUFFER 1

/* Direct3D rendering backend */
/* #undef BUILD_ENGINE_DIRECT3D */

/* Generic OpenGL Rendering Support */
/* #undef BUILD_ENGINE_GL_COMMON */

/* OpenGL Glew rendering backend */
/* #undef BUILD_ENGINE_GL_GLEW */

/* Software DirectDraw 16 bits rendering backend */
#define BUILD_ENGINE_SOFTWARE_16_DDRAW 1

/* Software DirectDraw rendering backend */
#define BUILD_ENGINE_SOFTWARE_DDRAW 1

/* Software GDI rendering backend */
#define BUILD_ENGINE_SOFTWARE_GDI 1

/* EET Font Loader Support */
#define BUILD_FONT_LOADER_EET 1

/* define to 1 if you have the line dither mask support */
/* #undef BUILD_LINE_DITHER_MASK */

/* UP Image Loader Support */
#define BUILD_LOADER_BMP 1

/* UP Image Loader Support */
/* #undef BUILD_LOADER_EDB */

/* UP Image Loader Support */
#define BUILD_LOADER_EET 1

/* UP Image Loader Support */
#define BUILD_LOADER_GIF 1

/* UP Image Loader Support */
#define BUILD_LOADER_JPEG 1

/* JPEG Region Decode Support */
/* #undef BUILD_LOADER_JPEG_REGION */

/* UP Image Loader Support */
#define BUILD_LOADER_PMAPS 1

/* UP Image Loader Support */
#define BUILD_LOADER_PNG 1

/* UP Image Loader Support */
/* #undef BUILD_LOADER_SVG */

/* UP Image Loader Support */
#define BUILD_LOADER_TGA 1

/* UP Image Loader Support */
#define BUILD_LOADER_TIFF 1

/* UP Image Loader Support */
#define BUILD_LOADER_XPM 1

/* define to 1 if you have the conversion to 16bpp without dither mask support
   */
/* #undef BUILD_NO_DITHER_MASK */

/* Build pipe render support */
/* #undef BUILD_PIPE_RENDER */

/* Build Threaded Rendering */
/* #undef BUILD_PTHREAD */

/* define to 1 if you have the sampling scaler support */
#define BUILD_SCALE_SAMPLE 1

/* define to 1 if you have the smooth scaler support */
#define BUILD_SCALE_SMOOTH 1

/* define to 1 if you have the small dither mask support */
/* #undef BUILD_SMALL_DITHER_MASK */

/* Define to one of `_getb67', `GETB67', `getb67' for Cray-2 and Cray-YMP
   systems. This function is required for `alloca.c' support on those systems.
   */
/* #undef CRAY_STACKSEG_END */

/* Define to 1 if using `alloca.c'. */
/* #undef C_ALLOCA */

/* Define to mention that evas is built */
#define EFL_EVAS_BUILD 1

/* Shared caceh server. */
/* #undef EVAS_CSERVE */

/* Build async render support */
/* #undef EVAS_FRAME_QUEUING */

/* complain when people pass in wrong object types etc. */
#define EVAS_MAGIC_DEBUG 1

/* Define to 1 if you have `alloca', as a function or macro. */
#define HAVE_ALLOCA 1

/* Define to 1 if you have <alloca.h> and it should be used (not on Ultrix).
   */
/* #undef HAVE_ALLOCA_H */

/* Have altivec.h header file */
/* #undef HAVE_ALTIVEC_H */

/* Define to 1 if you have the <d3d9.h> header file. */
#define HAVE_D3D9_H 1

/* Define to 1 if you have the <d3dx9.h> header file. */
/* #undef HAVE_D3DX9_H */

/* Define to 1 if you have the `dladdr' function. */
/* #undef HAVE_DLADDR */

/* Define to 1 if you have the <dlfcn.h> header file. */
#define HAVE_DLFCN_H 1

/* Define to 1 if you have the `dlopen' function. */
/* #undef HAVE_DLOPEN */

/* Define to 1 if you have the <EGL/egl.h> header file. */
/* #undef HAVE_EGL_EGL_H */

/* Set to 1 if evil package is installed */
#define HAVE_EVIL 1

/* have fontconfig searching capabilities */
#define HAVE_FONTCONFIG 1

/* have fribidi support */
/* #undef HAVE_FRIBIDI */

/* Define to 1 if you have the <GL/glew.h> header file. */
/* #undef HAVE_GL_GLEW_H */

/* Define to 1 if you have the <GL/gl.h> header file. */
/* #undef HAVE_GL_GL_H */

/* Define to 1 if you have the <inttypes.h> header file. */
#define HAVE_INTTYPES_H 1

/* Define to 1 if you have the <memory.h> header file. */
#define HAVE_MEMORY_H 1

/* Define to 1 if you have the <pthread.h> header file. */
#define HAVE_PTHREAD_H 1

/* Define to 1 if you have the <sched.h> header file. */
#define HAVE_SCHED_H 1

/* Define to 1 if you have the <stdint.h> header file. */
#define HAVE_STDINT_H 1

/* Define to 1 if you have the <stdlib.h> header file. */
#define HAVE_STDLIB_H 1

/* Define to 1 if you have the <strings.h> header file. */
#define HAVE_STRINGS_H 1

/* Define to 1 if you have the <string.h> header file. */
#define HAVE_STRING_H 1

/* Define to 1 if you have the <sys/stat.h> header file. */
#define HAVE_SYS_STAT_H 1

/* Define to 1 if you have the <sys/types.h> header file. */
#define HAVE_SYS_TYPES_H 1

/* Define to 1 if you have the <unistd.h> header file. */
#define HAVE_UNISTD_H 1

/* Experimental metric caching to speed up text rendering. */
/* #undef METRIC_CACHE */

/* "Module architecture" */
#define MODULE_ARCH "win32-msc-x86"

/* Name of package */
#define PACKAGE "evas"

/* Define to the address where bug reports for this package should be sent. */
#define PACKAGE_BUGREPORT "enlightenment-devel@lists.sourceforge.net"

/* Define to the full name of this package. */
#define PACKAGE_NAME "evas"

/* Define to the full name and version of this package. */
#define PACKAGE_STRING "evas 1.0.0"

/* Define to the one symbol short name of this package. */
#define PACKAGE_TARNAME "evas"

/* Define to the home page for this package. */
#define PACKAGE_URL ""

/* Define to the version of this package. */
#define PACKAGE_VERSION "1.0.0"

/* Define to 1 if you have the ANSI C header files. */
#define STDC_HEADERS 1

/* Version number of package */
#define VERSION "1.0.0"

/* Macro declaring a function argument to be unused */
#define __UNUSED__


#include "config_version.h"
