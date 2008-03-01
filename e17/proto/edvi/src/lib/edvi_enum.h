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
 * @enum Edvi_Page_Size
 * @brief Page sizes.
 *
 * @ingroup Edvi_Enum
 */
typedef enum
{
  EDVI_PAGE_SIZE_A1,      /**< A1 size */
  EDVI_PAGE_SIZE_A2,      /**< A2 size */
  EDVI_PAGE_SIZE_A3,      /**< A3 size */
  EDVI_PAGE_SIZE_A4,      /**< A4 size */
  EDVI_PAGE_SIZE_A5,      /**< A5 size */
  EDVI_PAGE_SIZE_A6,      /**< A6 size */
  EDVI_PAGE_SIZE_A7,      /**< A7 size */
  EDVI_PAGE_SIZE_B1,      /**< B1 size */
  EDVI_PAGE_SIZE_B2,      /**< B2 size */
  EDVI_PAGE_SIZE_B3,      /**< B3 size */
  EDVI_PAGE_SIZE_B4,      /**< B4 size */
  EDVI_PAGE_SIZE_B5,      /**< B5 size */
  EDVI_PAGE_SIZE_B6,      /**< B6 size */
  EDVI_PAGE_SIZE_B7,      /**< B7 size */
  EDVI_PAGE_SIZE_LETTER,  /**< Letter size */
  EDVI_PAGE_SIZE_US,      /**< US size */
  EDVI_PAGE_SIZE_LEGAL    /**< Legal size */
} Edvi_Page_Size;


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
