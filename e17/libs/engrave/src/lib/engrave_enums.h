#ifndef ENGRAVE_ENUMS_H
#define ENGRAVE_ENUMS_H

/**
 * @file engrave_enums.h
 * @brief Contains all of the enumerations used in Engrave
 */

/**
 * @defgroup Engrave_Enums Enums: Various Flags and Enumerations used in Engrave.
 * @{
 */

typedef enum _Engrave_Image_Type Engrave_Image_Type;
typedef enum _Engrave_Part_Type Engrave_Part_Type;
typedef enum _Engrave_Text_Effect Engrave_Text_Effect;
typedef enum _Engrave_Action Engrave_Action;
typedef enum _Engrave_Transition Engrave_Transition;
typedef enum _Engrave_Aspect_Preference Engrave_Aspect_Preference;
typedef enum _Engrave_Parse_Section Engrave_Parse_Section;
typedef enum _Engrave_Fill_Type Engrave_Fill_Type;
typedef enum _Engrave_Pointer_Mode Engrave_Pointer_Mode;

/**
 * @enum Engrave_Image_Type
 * This defines the various types of image compression available.
 */
enum _Engrave_Image_Type
{
  ENGRAVE_IMAGE_TYPE_RAW, /**< No compression, no loss of detail */
  ENGRAVE_IMAGE_TYPE_COMP, /**< Compress image, no loss of detail */
  ENGRAVE_IMAGE_TYPE_LOSSY, /**< Lossy compression of image */
  ENGRAVE_IMAGE_TYPE_NUM
};

/**
 * @enum Engrave_Part_Type
 * This defines the different part types available.
 */
enum _Engrave_Part_Type
{
  ENGRAVE_PART_TYPE_IMAGE, /**< Part is an image */
  ENGRAVE_PART_TYPE_TEXT, /**< Part is text */
  ENGRAVE_PART_TYPE_TEXTBLOCK, /**< Part is textblock */
  ENGRAVE_PART_TYPE_RECT, /**< Part is a rectangle */
  ENGRAVE_PART_TYPE_SWALLOW, /**< Part is a swallow */
  ENGRAVE_PART_TYPE_GRADIENT, /**< Part is a gradient */
  ENGRAVE_PART_TYPE_GROUP, /**< Part is a group */
  ENGRAVE_PART_TYPE_NUM
};

/**
 * @enum Engrave_Text_Effect
 * This defines the different types of effects that can be applied to a
 * chunk of text.
 */
enum _Engrave_Text_Effect
{
  ENGRAVE_TEXT_EFFECT_NONE, /**< No text effect */
  ENGRAVE_TEXT_EFFECT_PLAIN, /**< Normal text */
  ENGRAVE_TEXT_EFFECT_OUTLINE, /**< Outlined text */
  ENGRAVE_TEXT_EFFECT_SOFT_OUTLINE, /**< Soft outlined text */
  ENGRAVE_TEXT_EFFECT_SHADOW, /**< Shadowed text */
  ENGRAVE_TEXT_EFFECT_OUTLINE_SHADOW, /**< Outlined and shadowed text */
  ENGRAVE_TEXT_EFFECT_SOFT_SHADOW, /**< Soft shadowed text */
  ENGRAVE_TEXT_EFFECT_OUTLINE_SOFT_SHADOW, /**< Outlined and soft shadowed text */
  ENGRAVE_TEXT_EFFECT_FAR_SHADOW, /**< far shadowed text */
  ENGRAVE_TEXT_EFFECT_FAR_SOFT_SHADOW, /**< far and soft shadowed text */
  ENGRAVE_TEXT_EFFECT_GLOW, /**< glowing text */
  ENGRAVE_TEXT_EFFECT_NUM
};

/**
 * @enum Engrave_Action
 * The different types of actions that can be performed in a program.
 */
enum _Engrave_Action
{
  ENGRAVE_ACTION_STATE_SET, /**< Set the state of a given part */
  ENGRAVE_ACTION_STOP, /**< Stop the given action */
  ENGRAVE_ACTION_SIGNAL_EMIT, /**< Emit the given signal */
  ENGRAVE_ACTION_DRAG_VAL_SET, /**< Set the drag value of a given part */
  ENGRAVE_ACTION_DRAG_VAL_STEP, /**< Set the drag step of a given part */
  ENGRAVE_ACTION_DRAG_VAL_PAGE, /**< Set the drag page of a given part */
  ENGRAVE_ACTION_SCRIPT, /**< Set the cript action of a given part */
  ENGRAVE_ACTION_NUM
};

/**
 * @enum Engrave_Transition
 * The different types of transitions available to a program 
 */
enum _Engrave_Transition
{
  ENGRAVE_TRANSITION_LINEAR, /**< A linear transtion */
  ENGRAVE_TRANSITION_SINUSOIDAL, /**< A sinusoidal transition */
  ENGRAVE_TRANSITION_ACCELERATE, /**< An accelerating transition */
  ENGRAVE_TRANSITION_DECELERATE, /**< A decelerating transition */
  ENGRAVE_TRANSITION_NUM
};

/**
 * @enum Engrave_Aspect_Preference
 * The different aspect preferences available 
 */
enum _Engrave_Aspect_Preference
{
  ENGRAVE_ASPECT_PREFERENCE_NONE, /**< No aspect preference */
  ENGRAVE_ASPECT_PREFERENCE_VERTICAL, /**< Vertical aspect preference */
  ENGRAVE_ASPECT_PREFERENCE_HORIZONTAL, /**< Horizontal aspect preference */
  ENGRAVE_ASPECT_PREFERENCE_BOTH, /**< Vertical and Horizontal aspect preference */
  ENGRAVE_ASPECT_PREFERENCE_NUM
};

/**
 * @enum Engrave_Fill_Type
 * The different fill types available 
 */
enum _Engrave_Fill_Type
{
  ENGRAVE_FILL_TYPE_SCALE, /**< Scale type */
  ENGRAVE_FILL_TYPE_TILE, /**< Tile type */
  ENGRAVE_FILL_TYPE_NUM
};

/**
 * @enum Engrave_Pointer_Mode
 * The different pointer gran modes available
 */
enum _Engrave_Pointer_Mode
{
  ENGRAVE_POINTER_AUTOGRAB, /**< Auto Grab */
  ENGRAVE_POINTER_NOGRAB, /**< No Grab */
  ENGRAVE_POINTER_NUM
};


/**
 * @}
 */

#endif

