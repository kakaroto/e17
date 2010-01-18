/*
 * =====================================================================================
 *
 *       Filename:  eyelight_compiler_common.h
 *
 *    Description:
 *
 *        Version:  1.0
 *        Created:  11/07/08 12:51:33 UTC
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  (Watchwolf), Atton Jonathan <watchwolf@watchwolf.fr>
 *        Company:
 *
 * =====================================================================================
 */

#ifndef  EYELIGHT_COMPILER_PARSER_INC
#define  EYELIGHT_COMPILER_PARSER_INC


typedef struct eyelight_compiler Eyelight_Compiler;
typedef struct Eyelight_Name_Key Eyelight_Name_Key;
typedef struct Eyelight_Prop_Value_Type Eyelight_Prop_Value_Type;
typedef enum Eyelight_Value_Type Eyelight_Value_Type;
typedef struct Eyelight_Valid_Prop_Block Eyelight_Valid_Prop_Block;
typedef struct Eyelight_Prop_Nb_Value Eyelight_Prop_Nb_Value;
typedef struct eyelight_area Eyelight_Area;
typedef struct eyelight_video Eyelight_Video;

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <Ecore_Data.h>
#include <Ecore_File.h>
#include <errno.h>
#include <limits.h>

#include "eyelight_viewer.h"

struct Eyelight_Edit
{
    Eyelight_Slide *slide;
    Eyelight_Node *node;
    Evas_Object *obj;
    void *data;
};


struct Eyelight_Name_Key
{
    const char *keyname;
    Eyelight_Node_Name name;
};

static const Eyelight_Name_Key eyelight_name_keys[] = {
    { "root", EYELIGHT_NAME_ROOT },
    { "slide", EYELIGHT_NAME_SLIDE },
    { "items", EYELIGHT_NAME_ITEMS },
    { "item", EYELIGHT_NAME_ITEM },
    { "area", EYELIGHT_NAME_AREA },
    { "edje", EYELIGHT_NAME_EDJ },
    { "video", EYELIGHT_NAME_VIDEO},
    { "title", EYELIGHT_NAME_TITLE },
    { "subtitle", EYELIGHT_NAME_SUBTITLE },
    { "header_image", EYELIGHT_NAME_HEADER_IMAGE },
    { "foot_image", EYELIGHT_NAME_FOOT_IMAGE },
    { "foot_text", EYELIGHT_NAME_FOOT_TEXT },
    { "layout", EYELIGHT_NAME_LAYOUT },
    { "name", EYELIGHT_NAME_NAME },
    { "text", EYELIGHT_NAME_TEXT },
    { "image", EYELIGHT_NAME_IMAGE },
    { "aspect", EYELIGHT_NAME_ASPECT }, 
    { "keep_aspect", EYELIGHT_NAME_KEEP_ASPECT }, 
    { "file", EYELIGHT_NAME_FILE },
    { "group", EYELIGHT_NAME_GROUP },
    { "transition", EYELIGHT_NAME_TRANSITION },
    { "transition_next", EYELIGHT_NAME_TRANSITION_NEXT },
    { "transition_previous", EYELIGHT_NAME_TRANSITION_PREVIOUS },
    { "ignore_area", EYELIGHT_NAME_IGNORE_AREA },
    { "custom_area", EYELIGHT_NAME_CUSTOM_AREA },
    { "border", EYELIGHT_NAME_BORDER},
    { "numbering", EYELIGHT_NAME_NUMBERING },
    { "relative", EYELIGHT_NAME_RELATIVE},
    { "alpha", EYELIGHT_NAME_ALPHA},
    { "autoplay", EYELIGHT_NAME_AUTOPLAY},
    { "replay", EYELIGHT_NAME_REPLAY},
    { "shadow", EYELIGHT_NAME_SHADOW},
    { "presentation", EYELIGHT_NAME_PRESENTATION},
    { "theme", EYELIGHT_NAME_THEME}
};

struct Eyelight_Valid_Prop_Block
{
    Eyelight_Node_Name block;
    Eyelight_Node_Name block_prop;
};

static const Eyelight_Valid_Prop_Block eyelight_valid_prop_block[] =
{
    { EYELIGHT_NAME_ROOT, EYELIGHT_NAME_LAYOUT },
    { EYELIGHT_NAME_ROOT, EYELIGHT_NAME_HEADER_IMAGE },
    { EYELIGHT_NAME_ROOT, EYELIGHT_NAME_FOOT_TEXT },
    { EYELIGHT_NAME_ROOT, EYELIGHT_NAME_FOOT_IMAGE },
    { EYELIGHT_NAME_ROOT, EYELIGHT_NAME_TITLE },
    { EYELIGHT_NAME_ROOT, EYELIGHT_NAME_SUBTITLE },
    { EYELIGHT_NAME_ROOT, EYELIGHT_NAME_TRANSITION },
    { EYELIGHT_NAME_ROOT, EYELIGHT_NAME_TRANSITION_NEXT },
    { EYELIGHT_NAME_ROOT, EYELIGHT_NAME_TRANSITION_PREVIOUS },
    { EYELIGHT_NAME_ROOT, EYELIGHT_NAME_TRANSITION_PREVIOUS },
    { EYELIGHT_NAME_SLIDE, EYELIGHT_NAME_TITLE },
    { EYELIGHT_NAME_SLIDE, EYELIGHT_NAME_LAYOUT },
    { EYELIGHT_NAME_SLIDE, EYELIGHT_NAME_SUBTITLE },
    { EYELIGHT_NAME_SLIDE, EYELIGHT_NAME_HEADER_IMAGE },
    { EYELIGHT_NAME_SLIDE, EYELIGHT_NAME_FOOT_TEXT },
    { EYELIGHT_NAME_SLIDE, EYELIGHT_NAME_FOOT_IMAGE },
    { EYELIGHT_NAME_SLIDE, EYELIGHT_NAME_TRANSITION },
    { EYELIGHT_NAME_SLIDE, EYELIGHT_NAME_TRANSITION_NEXT },
    { EYELIGHT_NAME_SLIDE, EYELIGHT_NAME_TRANSITION_PREVIOUS },
    { EYELIGHT_NAME_SLIDE, EYELIGHT_NAME_IGNORE_AREA },
    { EYELIGHT_NAME_SLIDE, EYELIGHT_NAME_CUSTOM_AREA },
    { EYELIGHT_NAME_AREA, EYELIGHT_NAME_NAME },
    { EYELIGHT_NAME_AREA, EYELIGHT_NAME_TEXT },
    { EYELIGHT_NAME_AREA, EYELIGHT_NAME_IMAGE },
    { EYELIGHT_NAME_AREA, EYELIGHT_NAME_VIDEO },
    { EYELIGHT_NAME_AREA, EYELIGHT_NAME_LAYOUT },
    { EYELIGHT_NAME_ITEMS, EYELIGHT_NAME_TEXT },
    { EYELIGHT_NAME_ITEMS, EYELIGHT_NAME_NUMBERING },
    { EYELIGHT_NAME_ITEM, EYELIGHT_NAME_TEXT },
    { EYELIGHT_NAME_IMAGE, EYELIGHT_NAME_IMAGE },
    { EYELIGHT_NAME_IMAGE, EYELIGHT_NAME_BORDER },
    { EYELIGHT_NAME_IMAGE, EYELIGHT_NAME_SHADOW},
    { EYELIGHT_NAME_IMAGE, EYELIGHT_NAME_ASPECT},
    { EYELIGHT_NAME_IMAGE, EYELIGHT_NAME_KEEP_ASPECT},
    { EYELIGHT_NAME_EDJ, EYELIGHT_NAME_FILE },
    { EYELIGHT_NAME_EDJ, EYELIGHT_NAME_GROUP },
    { EYELIGHT_NAME_VIDEO, EYELIGHT_NAME_VIDEO},
    { EYELIGHT_NAME_VIDEO, EYELIGHT_NAME_ALPHA},
    { EYELIGHT_NAME_VIDEO, EYELIGHT_NAME_AUTOPLAY},
    { EYELIGHT_NAME_VIDEO, EYELIGHT_NAME_REPLAY},
    { EYELIGHT_NAME_VIDEO, EYELIGHT_NAME_SHADOW},
    { EYELIGHT_NAME_VIDEO, EYELIGHT_NAME_BORDER},
    { EYELIGHT_NAME_PRESENTATION, EYELIGHT_NAME_PRESENTATION},
    { EYELIGHT_NAME_PRESENTATION, EYELIGHT_NAME_THEME},
    { EYELIGHT_NAME_PRESENTATION, EYELIGHT_NAME_BORDER},
    { EYELIGHT_NAME_PRESENTATION, EYELIGHT_NAME_SHADOW}
};


static const Eyelight_Valid_Prop_Block eyelight_valid_block_block[] =
{
    { EYELIGHT_NAME_ROOT, EYELIGHT_NAME_SLIDE },
    { EYELIGHT_NAME_SLIDE, EYELIGHT_NAME_AREA },
    { EYELIGHT_NAME_AREA, EYELIGHT_NAME_ITEMS },
    { EYELIGHT_NAME_AREA, EYELIGHT_NAME_IMAGE },
    { EYELIGHT_NAME_AREA, EYELIGHT_NAME_VIDEO },
    { EYELIGHT_NAME_AREA, EYELIGHT_NAME_EDJ },
    { EYELIGHT_NAME_ITEMS, EYELIGHT_NAME_ITEM },
    { EYELIGHT_NAME_ITEMS, EYELIGHT_NAME_ITEMS },
    { EYELIGHT_NAME_AREA, EYELIGHT_NAME_PRESENTATION}
};

enum Eyelight_Value_Type
{
    EYELIGHT_VALUE_TYPE_NONE ,
    EYELIGHT_VALUE_TYPE_INT,
    EYELIGHT_VALUE_TYPE_STRING,
    EYELIGHT_VALUE_TYPE_DOUBLE
};


struct Eyelight_Prop_Nb_Value
{
    Eyelight_Node_Name prop;
    int nb;
};


/*
 * @brief by default the number of value is 1
 */
static const Eyelight_Prop_Nb_Value eyelight_prop_nb_value[]=
{
    {EYELIGHT_NAME_CUSTOM_AREA, 5},
    {EYELIGHT_NAME_RELATIVE, 2},
    {EYELIGHT_NAME_ASPECT, 2},
};


struct Eyelight_Prop_Value_Type
{
    Eyelight_Node_Name name;
    int arg_pos;
    Eyelight_Value_Type type;
};

/*
 * @brief by default the type is String
 */
static const Eyelight_Prop_Value_Type eyelight_prop_value_type[]=
{
    {EYELIGHT_NAME_CUSTOM_AREA, 1, EYELIGHT_VALUE_TYPE_DOUBLE},
    {EYELIGHT_NAME_CUSTOM_AREA, 2, EYELIGHT_VALUE_TYPE_DOUBLE},
    {EYELIGHT_NAME_CUSTOM_AREA, 3, EYELIGHT_VALUE_TYPE_DOUBLE},
    {EYELIGHT_NAME_CUSTOM_AREA, 4, EYELIGHT_VALUE_TYPE_DOUBLE},
    {EYELIGHT_NAME_RELATIVE, 0, EYELIGHT_VALUE_TYPE_DOUBLE},
    {EYELIGHT_NAME_RELATIVE, 1, EYELIGHT_VALUE_TYPE_DOUBLE},
    {EYELIGHT_NAME_BORDER, 0, EYELIGHT_VALUE_TYPE_INT},
    {EYELIGHT_NAME_ALPHA, 0, EYELIGHT_VALUE_TYPE_INT},
    {EYELIGHT_NAME_AUTOPLAY, 0, EYELIGHT_VALUE_TYPE_INT},
    {EYELIGHT_NAME_REPLAY, 0, EYELIGHT_VALUE_TYPE_INT},
    {EYELIGHT_NAME_SHADOW, 0, EYELIGHT_VALUE_TYPE_INT},
    {EYELIGHT_NAME_ASPECT, 0, EYELIGHT_VALUE_TYPE_DOUBLE},
    {EYELIGHT_NAME_ASPECT, 1, EYELIGHT_VALUE_TYPE_DOUBLE},
    {EYELIGHT_NAME_KEEP_ASPECT, 0, EYELIGHT_VALUE_TYPE_INT}
};

/*
 * @brief the structure define a block, a property or a value.
 * A block has a list of properties or blocks
 * A property has a list of values
 * The name defines the name of the block (area, slide ...) , the name of the property (image, text ...) or the value of a value
 */
struct eyelight_node
{
    int type;

    /* This two value could be an union, but Eet doesn't support it yet, so removed it for now. */
    Eyelight_Node_Name name;
    char* value;

    Eyelight_Node * father;
    Eina_List *l;

    //additional fields
    Evas_Object *obj;
    Eyelight_Area *area;
};

struct eyelight_compiler
{
    char* input_file;

    FILE* input;
    char* mmap;
    size_t size;

    int display_areas;
    int index;

    //the line number, use to display a message
    int line;

    //stack of blocks which are open
    int open_block[EYELIGHT_BUFLEN];
    int last_open_block;

    //the node root
    Eyelight_Node *root;
};

struct eyelight_area
{
    char *name;

    Evas_Object *obj;

    Eyelight_Node *node_def;
    Eyelight_Node *node_area;
};

struct eyelight_video
{
    char *video;
    Evas_Object *o_video;
    Evas_Object *o_inter;
    int replay;
    Eyelight_Viewer *pres;
};

#define EYELIGHT_NODE_TYPE_BLOCK 0
#define EYELIGHT_NODE_TYPE_PROP 1
#define EYELIGHT_NODE_TYPE_VALUE 2

Eyelight_Compiler* eyelight_elt_load(const char *input_file, const char *dump_out);

Eyelight_Node_Name eyelight_name_get(char* p);
const char *eyelight_string_name_get(Eyelight_Node_Name name);
int eyelight_save(Eyelight_Node *root, const char *file);

Eyelight_Compiler* eyelight_compiler_new(const char* input_file, int display_areas);
void eyelight_compiler_free(Eyelight_Compiler **p_compiler);
Eyelight_Node *eyelight_node_new(int type,Eyelight_Node_Name name, Eyelight_Node* father);
void eyelight_node_free(Eyelight_Node** current, Eyelight_Node *not_free);

/**
 * Add the contents in the slide id_slide (text, image ..)
 * o_slide is the object of the slide
 */
void eyelight_compile(Eyelight_Viewer *pres, Eyelight_Slide *slide, int id_slide);

/**
 * Parse the string delimited by p and end
 * compiler will contains the tree of the presentation
 */
void eyelight_parse(Eyelight_Compiler* compiler,char *p, char* end);

char *eyelight_compile_image_path_new(Eyelight_Viewer *pres, const char *image);
void eyelight_theme_areas_create(Eyelight_Viewer *pres, Eyelight_Slide *slide);
Eina_List *eyelight_theme_areas_get(Eyelight_Slide *slide);
void eyelight_theme_area_desc_get(Eyelight_Slide *slide, const char* area_name,
        char **rel1_x, char **rel1_y, char **rel2_x, char **rel2_y);

/**
 * Remove a quote " at the start and the end of p
 */
char* eyelight_remove_quote(char* p);


char* eyelight_retrieve_value_of_prop(Eyelight_Node* node,int i);
Eyelight_Node* eyelight_retrieve_node_prop(Eyelight_Node* current, Eyelight_Node_Name p);
int eyelight_number_item_in_block(Eyelight_Node* current);
Eyelight_Node *eyelight_ignore_area_is(Eyelight_Slide *slide, const char *area);

Eyelight_Area *eyelight_retrieve_area_from_node(Eyelight_Slide *slide, Eyelight_Node *node);

int eyelight_decimal_to_roman(char *dec, char *rom);

#endif   /* ----- #ifndef EYELIGHT_COMPILER_PARSER_INC  ----- */

