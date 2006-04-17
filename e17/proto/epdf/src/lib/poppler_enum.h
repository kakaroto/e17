#ifndef __EPDF_ENUM_H__
#define __EPDF_ENUM_H__


typedef enum
{
  EPDF_FONT_INFO_UNKNOWN,                 /**< Unknown Font Information */
  EPDF_FONT_INFO_TYPE1,                   /**< Type 1 Font Information */
  EPDF_FONT_INFO_TYPE1C,                  /**< Type 1C Font Information */
  EPDF_FONT_INFO_TYPE3,                   /**< Type 3 Font Information */
  EPDF_FONT_INFO_TRUETYPE,                /**< True Type Font Information */
  EPDF_FONT_INFO_CID_TYPE0,               /**< CID Type 0 Font Information */
  EPDF_FONT_INFO_CID_TYPE0C,              /**< CID Type 0C Font Information */
  EPDF_FONT_INFO_CID_TRUETYPE             /**< CID True Type Font Information */
}Epdf_Font_Info_Type;

typedef enum
{
  EPDF_PAGE_TRANSITION_REPLACE,           /**< Replace transition */
  EPDF_PAGE_TRANSITION_SPLIT,             /**< Split transition */
  EPDF_PAGE_TRANSITION_BLINDS,            /**< Blinds transition */
  EPDF_PAGE_TRANSITION_BOX,               /**< Box transition */
  EPDF_PAGE_TRANSITION_WIPE,              /**< Wipe transition */
  EPDF_PAGE_TRANSITION_DISSOLVE,          /**< Dissolve transition */
  EPDF_PAGE_TRANSITION_GLITTER,           /**< Glitter transition */
  EPDF_PAGE_TRANSITION_FLY,               /**< Fly transition */
  EPDF_PAGE_TRANSITION_PUSH,              /**< Push transition */
  EPDF_PAGE_TRANSITION_COVER,             /**< Cover transition */
  EPDF_PAGE_TRANSITION_UNCOVER,           /**< Uncover transition */
  EPDF_PAGE_TRANSITION_FADE               /**< Fade transition */
}Epdf_Page_Transition_Type;

typedef enum
{
  EPDF_PAGE_TRANSITION_HORIZONTAL,        /**< Horizontal transition */
  EPDF_PAGE_TRANSITION_VERTICAL           /**< Vertical transition */
}Epdf_Page_Transition_Alignment;

typedef enum
{
  EPDF_PAGE_TRANSITION_INWARD,            /**< Inward transition */
  EPDF_PAGE_TRANSITION_OUTWARD            /**< Outward transition */
}Epdf_Page_Transition_Direction;

typedef enum
{
  EPDF_PAGE_ORIENTATION_PORTRAIT,         /**< Portrait orientation */
  EPDF_PAGE_ORIENTATION_LANDSCAPE,        /**< Landscape orientation */
  EPDF_PAGE_ORIENTATION_UPSIDEDOWN,       /**< Upside-down orientation */
  EPDF_PAGE_ORIENTATION_SEASCAPE          /**< Seascape orientation */
}Epdf_Page_Orientation;

typedef enum
{
  EPDF_DOCUMENT_PAGE_MODE_USE_NONE,       /**< No page mode */
  EPDF_DOCUMENT_PAGE_MODE_USE_OUTLINES,   /**< Outlines page mode */
  EPDF_DOCUMENT_PAGE_MODE_USE_THUMBS,     /**< Thumbs page mode */
  EPDF_DOCUMENT_PAGE_MODE_FULLSCREEN,     /**< Fullscreen page mode */
  EPDF_DOCUMENT_PAGE_MODE_USE_OC          /**< OC page mode */
}Epdf_Document_Page_Mode;

typedef enum
{
  EPDF_DOCUMENT_PAGE_LAYOUT_NONE,               /**< No page layout */
  EPDF_DOCUMENT_PAGE_LAYOUT_SINGLE_PAGE,        /**< Single page layout */
  EPDF_DOCUMENT_PAGE_LAYOUT_ONE_COLUMN,         /**< One column page layout */
  EPDF_DOCUMENT_PAGE_LAYOUT_TWO_COLUMN_LEFT,    /**< Two column left page layout */
  EPDF_DOCUMENT_PAGE_LAYOUT_TWO_COLUMN_RIGHT,   /**< Two column right page layout */
  EPDF_DOCUMENT_PAGE_LAYOUT_TWO_PAGE_LEFT,      /**< Two page left page layout */
  EPDF_DOCUMENT_PAGE_LAYOUT_TWO_PAGE_RIGHT      /**< Two page right page layout */
}Epdf_Document_Page_Layout;

typedef enum
{
  EPDF_LINK_ACTION_GOTO,
  EPDF_LINK_ACTION_GOTO_NEW_FILE,
  EPDF_LINK_ACTION_LAUNCH,
  EPDF_LINK_ACTION_URI,
  EPDF_LINK_ACTION_NAMED,
  EPDF_LINK_ACTION_MOVIE,
  EPDF_LINK_ACTION_UNKNOWN
}Epdf_Link_Action_Kind;


#endif /* __EPDF_ENUM_H__ */
