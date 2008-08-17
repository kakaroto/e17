#ifndef __EDVI_ENUM_H__
#define __EDVI_ENUM_H__


/**
 * @file edvi_enum.h
 * @defgroup Edvi_Enum Edvi Enumerations
 * @brief Enumerations used by Edvi
 * @ingroup Edvi
 *
 * Enumerations used by Edvi
 */


#define EDVI_DEFAULT_DPI      300
#define EDVI_DEFAULT_SHRINK   5
#define EDVI_DEFAULT_AA       4
#define EDVI_DEFAULT_OFFSET_X 1.00  /*inch*/
#define EDVI_DEFAULT_OFFSET_Y 1.00  /*inch*/


/**
 * @enum Edvi_Page_Orientation
 * @brief Page orientations.
 *
 * @ingroup Edvi_Enum
 */
typedef enum
{
  EDVI_PAGE_ORIENTATION_PORTRAIT,  /**< Portrait orientation */
  EDVI_PAGE_ORIENTATION_LANDSCAPE  /**< Landscape orientation */
} Edvi_Page_Orientation;


/**
 * @enum Edvi_Page_Format
 * @brief Page formats.
 *
 * @ingroup Edvi_Enum
 */
typedef enum
{
  EDVI_PAGE_FORMAT_A1,      /**< A1 format */
  EDVI_PAGE_FORMAT_A2,      /**< A2 format */
  EDVI_PAGE_FORMAT_A3,      /**< A3 format */
  EDVI_PAGE_FORMAT_A4,      /**< A4 format */
  EDVI_PAGE_FORMAT_A5,      /**< A5 format */
  EDVI_PAGE_FORMAT_A6,      /**< A6 format */
  EDVI_PAGE_FORMAT_A7,      /**< A7 format */
  EDVI_PAGE_FORMAT_B1,      /**< B1 format */
  EDVI_PAGE_FORMAT_B2,      /**< B2 format */
  EDVI_PAGE_FORMAT_B3,      /**< B3 format */
  EDVI_PAGE_FORMAT_B4,      /**< B4 format */
  EDVI_PAGE_FORMAT_B5,      /**< B5 format */
  EDVI_PAGE_FORMAT_B6,      /**< B6 format */
  EDVI_PAGE_FORMAT_B7,      /**< B7 format */
  EDVI_PAGE_FORMAT_LETTER,  /**< Letter format */
  EDVI_PAGE_FORMAT_US,      /**< US format */
  EDVI_PAGE_FORMAT_LEGAL    /**< Legal format */
} Edvi_Page_Format;


/**
 * @enum Edvi_Property_Id
 * @brief Propery identifiers.
 *
 * @ingroup Edvi_Enum
 */
typedef enum
{
  EDVI_PROPERTY_PRINT_FONTS,              /**< Print font names. */
  EDVI_PROPERTY_PRINT_MISSING_FONTS,      /**< Print font names that are not found. */
  EDVI_PROPERTY_ASCII_JTEX,               /**< DVI extension for Japanese TeX by ASCII Coop. */
  EDVI_PROPERTY_SCALE_FONT_IF_NOT_EXIST,  /**< Enlarge/shrink font if not a font of correct size is not found. */
  EDVI_PROPERTY_LATEX2E_GRAPHICS_STY,     /**< LaTeX 2.09 epsf.sty or graphic[sx].sty for EPS figures. */
  EDVI_PROPERTY_LATEX209_EPSF_STY,        /**< LaTeX 2e epsf.sty or graphic[sx].sty for EPS figures. It is the default property. */
  EDVI_PROPERTY_SKIP_RENDERING,           /**< Skip rendering characters on output devices. */
  EDVI_PROPERTY_SKIP_SPECIALS,            /**< Skip processing special DVI instructions (e.g., rendering EPS files). */
  EDVI_PROPERTY_LIST_FONTS,               /**< Report fonts. */
  EDVI_PROPERTY_LIST_MISSING_FONTS,       /**< Report missing fonts. */
  EDVI_PROPERTY_DELAYED_FONT_OPEN,        /**< Defer opening fonts until it is necessary. */
  EDVI_PROPERTY_ASYNC_GS_INVOCATION,      /**< Asynchronous Ghostscript invocation. */
  EDVI_PROPERTY_INCREMENTAL_EPS_DISPLAY   /**< Incremental EPS display. */
} Edvi_Property_Id;


#endif /* __EDVI_ENUM_H__ */
