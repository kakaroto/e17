#ifndef __POPPLER_ENUM_H__
#define __POPPLER_ENUM_H__


typedef enum
{
  EVAS_POPPLER_FONT_INFO_UNKNOWN,                 /**< Unknown Font Information */
  EVAS_POPPLER_FONT_INFO_TYPE1,                   /**< Type 1 Font Information */
  EVAS_POPPLER_FONT_INFO_TYPE1C,                  /**< Type 1C Font Information */
  EVAS_POPPLER_FONT_INFO_TYPE3,                   /**< Type 3 Font Information */
  EVAS_POPPLER_FONT_INFO_TRUETYPE,                /**< True Type Font Information */
  EVAS_POPPLER_FONT_INFO_CID_TYPE0,               /**< CID Type 0 Font Information */
  EVAS_POPPLER_FONT_INFO_CID_TYPE0C,              /**< CID Type 0C Font Information */
  EVAS_POPPLER_FONT_INFO_CID_TRUETYPE             /**< CID True Type Font Information */
}Evas_Poppler_Font_Info_Type;

typedef enum
{
  EVAS_POPPLER_PAGE_TRANSITION_REPLACE,           /**< Replace transition */
  EVAS_POPPLER_PAGE_TRANSITION_SPLIT,             /**< Split transition */
  EVAS_POPPLER_PAGE_TRANSITION_BLINDS,            /**< Blinds transition */
  EVAS_POPPLER_PAGE_TRANSITION_BOX,               /**< Box transition */
  EVAS_POPPLER_PAGE_TRANSITION_WIPE,              /**< Wipe transition */
  EVAS_POPPLER_PAGE_TRANSITION_DISSOLVE,          /**< Dissolve transition */
  EVAS_POPPLER_PAGE_TRANSITION_GLITTER,           /**< Glitter transition */
  EVAS_POPPLER_PAGE_TRANSITION_FLY,               /**< Fly transition */
  EVAS_POPPLER_PAGE_TRANSITION_PUSH,              /**< Push transition */
  EVAS_POPPLER_PAGE_TRANSITION_COVER,             /**< Cover transition */
  EVAS_POPPLER_PAGE_TRANSITION_UNCOVER,           /**< Uncover transition */
  EVAS_POPPLER_PAGE_TRANSITION_FADE               /**< Fade transition */
}Evas_Poppler_Page_Transition_Type;

typedef enum
{
  EVAS_POPPLER_PAGE_TRANSITION_HORIZONTAL,        /**< Horizontal transition */
  EVAS_POPPLER_PAGE_TRANSITION_VERTICAL           /**< Vertical transition */
}Evas_Poppler_Page_Transition_Alignment;

typedef enum
{
  EVAS_POPPLER_PAGE_TRANSITION_INWARD,            /**< Inward transition */
  EVAS_POPPLER_PAGE_TRANSITION_OUTWARD            /**< Outward transition */
}Evas_Poppler_Page_Transition_Direction;

typedef enum
{
  EVAS_POPPLER_PAGE_ORIENTATION_LANDSCAPE,        /**< Landscape orientation */
  EVAS_POPPLER_PAGE_ORIENTATION_PORTRAIT,         /**< Portrait orientation */
  EVAS_POPPLER_PAGE_ORIENTATION_SEASCAPE,         /**< Seascape orientation */
  EVAS_POPPLER_PAGE_ORIENTATION_UPSIDEDOWN        /**< Upside-down orientation */
}Evas_Poppler_Page_Orientation;

typedef enum
{
  EVAS_POPPLER_DOCUMENT_PAGE_MODE_USE_NONE,       /**< No page mode */
  EVAS_POPPLER_DOCUMENT_PAGE_MODE_USE_OUTLINES,   /**< Outlines page mode */
  EVAS_POPPLER_DOCUMENT_PAGE_MODE_USE_THUMBS,     /**< Thumbs page mode */
  EVAS_POPPLER_DOCUMENT_PAGE_MODE_FULLSCREEN,     /**< Fullscreen page mode */
  EVAS_POPPLER_DOCUMENT_PAGE_MODE_USE_OC          /**< OC page mode */
}Evas_Poppler_Document_Page_Mode;

typedef enum
{
  EVAS_POPPLER_DOCUMENT_PAGE_LAYOUT_NONE,               /**< No page layout */
  EVAS_POPPLER_DOCUMENT_PAGE_LAYOUT_SINGLE_PAGE,        /**< Single page layout */
  EVAS_POPPLER_DOCUMENT_PAGE_LAYOUT_ONE_COLUMN,         /**< One column page layout */
  EVAS_POPPLER_DOCUMENT_PAGE_LAYOUT_TWO_COLUMN_LEFT,    /**< Two column left page layout */
  EVAS_POPPLER_DOCUMENT_PAGE_LAYOUT_TWO_COLUMN_RIGHT,   /**< Two column right page layout */
  EVAS_POPPLER_DOCUMENT_PAGE_LAYOUT_TWO_PAGE_LEFT,      /**< Two page left page layout */
  EVAS_POPPLER_DOCUMENT_PAGE_LAYOUT_TWO_PAGE_RIGHT      /**< Two page right page layout */
}Evas_Poppler_Document_Page_Layout;

typedef enum
{
  EVAS_POPPLER_LINK_ACTION_GOTO,
  EVAS_POPPLER_LINK_ACTION_GOTO_NEW_FILE,
  EVAS_POPPLER_LINK_ACTION_LAUNCH,
  EVAS_POPPLER_LINK_ACTION_URI,
  EVAS_POPPLER_LINK_ACTION_NAMED,
  EVAS_POPPLER_LINK_ACTION_MOVIE,
  EVAS_POPPLER_LINK_ACTION_UNKNOWN
}Evas_Poppler_Link_Action_Kind;


#endif /* __POPPLER_ENUM_H__ */
