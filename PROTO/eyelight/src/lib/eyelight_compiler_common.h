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

#ifndef  EYELIGHT_COMPILER_COMMON_INC
#define  EYELIGHT_COMPILER_COMMON_INC


typedef struct eyelight_node Eyelight_Node;
typedef struct eyelight_compiler Eyelight_Compiler;
typedef enum eyelight_node_name Eyelight_Node_Name;
typedef struct Eyelight_Name_Key Eyelight_Name_Key;
typedef struct Eyelight_Prop_Value_Type Eyelight_Prop_Value_Type;
typedef enum Eyelight_Value_Type Eyelight_Value_Type;
typedef struct Eyelight_Valid_Prop_Block Eyelight_Valid_Prop_Block;
typedef struct Eyelight_Prop_Nb_Value Eyelight_Prop_Nb_Value;

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <Ecore_Data.h>
#include <Ecore_File.h>
#include <errno.h>
#include <limits.h>

#include "eyelight_parser.h"
#include "eyelight_compiler.h"
#include "Eyelight.h"

enum eyelight_node_name
{
    EYELIGHT_NAME_NONE,
    EYELIGHT_NAME_ROOT,
    EYELIGHT_NAME_SLIDE,
    EYELIGHT_NAME_ITEMS,
    EYELIGHT_NAME_ITEM,
    EYELIGHT_NAME_AREA,
    EYELIGHT_NAME_EDC,
    EYELIGHT_NAME_TITLE,
    EYELIGHT_NAME_SUBTITLE,
    EYELIGHT_NAME_HEADER_IMAGE,
    EYELIGHT_NAME_FOOT_IMAGE,
    EYELIGHT_NAME_FOOT_TEXT,
    EYELIGHT_NAME_LAYOUT,
    EYELIGHT_NAME_NAME,
    EYELIGHT_NAME_TEXT,
    EYELIGHT_NAME_IMAGE,
    EYELIGHT_NAME_FILE,
    EYELIGHT_NAME_MACRO,
    EYELIGHT_NAME_TRANSITION,
    EYELIGHT_NAME_TRANSITION_NEXT,
    EYELIGHT_NAME_TRANSITION_PREVIOUS,
    EYELIGHT_NAME_CUSTOM_AREA,
    EYELIGHT_NAME_SIZE,
    EYELIGHT_NAME_SCALE,
    EYELIGHT_NAME_BORDER,
    EYELIGHT_NAME_NUMBERING,
    EYELIGHT_NAME_RELATIVE
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
    { "edc", EYELIGHT_NAME_EDC },
    { "title", EYELIGHT_NAME_TITLE },
    { "subtitle", EYELIGHT_NAME_SUBTITLE },
    { "header_image", EYELIGHT_NAME_HEADER_IMAGE },
    { "foot_image", EYELIGHT_NAME_FOOT_IMAGE },
    { "foot_text", EYELIGHT_NAME_FOOT_TEXT },
    { "layout", EYELIGHT_NAME_LAYOUT },
    { "name", EYELIGHT_NAME_NAME },
    { "text", EYELIGHT_NAME_TEXT },
    { "image", EYELIGHT_NAME_IMAGE },
    { "file", EYELIGHT_NAME_FILE },
    { "macro", EYELIGHT_NAME_MACRO },
    { "transition", EYELIGHT_NAME_TRANSITION },
    { "transition_next", EYELIGHT_NAME_TRANSITION_NEXT },
    { "transition_previous", EYELIGHT_NAME_TRANSITION_PREVIOUS },
    { "custom_area", EYELIGHT_NAME_CUSTOM_AREA },
    { "size", EYELIGHT_NAME_SIZE },
    { "scale", EYELIGHT_NAME_SCALE },
    { "border", EYELIGHT_NAME_BORDER},
    { "numbering", EYELIGHT_NAME_NUMBERING },
    { "relative", EYELIGHT_NAME_RELATIVE}
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
    { EYELIGHT_NAME_SLIDE, EYELIGHT_NAME_CUSTOM_AREA },
    { EYELIGHT_NAME_AREA, EYELIGHT_NAME_NAME },
    { EYELIGHT_NAME_AREA, EYELIGHT_NAME_TEXT },
    { EYELIGHT_NAME_AREA, EYELIGHT_NAME_IMAGE },
    { EYELIGHT_NAME_AREA, EYELIGHT_NAME_LAYOUT },
    { EYELIGHT_NAME_ITEMS, EYELIGHT_NAME_TEXT },
    { EYELIGHT_NAME_ITEMS, EYELIGHT_NAME_NUMBERING },
    { EYELIGHT_NAME_ITEM, EYELIGHT_NAME_TEXT },
    { EYELIGHT_NAME_IMAGE, EYELIGHT_NAME_IMAGE },
    { EYELIGHT_NAME_IMAGE, EYELIGHT_NAME_SIZE },
    { EYELIGHT_NAME_IMAGE, EYELIGHT_NAME_SCALE },
    { EYELIGHT_NAME_IMAGE, EYELIGHT_NAME_BORDER },
    { EYELIGHT_NAME_IMAGE, EYELIGHT_NAME_RELATIVE},
    { EYELIGHT_NAME_EDC, EYELIGHT_NAME_IMAGE },
    { EYELIGHT_NAME_EDC, EYELIGHT_NAME_FILE },
    { EYELIGHT_NAME_EDC, EYELIGHT_NAME_MACRO },
    { EYELIGHT_NAME_EDC, EYELIGHT_NAME_RELATIVE}
};


static const Eyelight_Valid_Prop_Block eyelight_valid_block_block[] =
{
    { EYELIGHT_NAME_ROOT, EYELIGHT_NAME_SLIDE },
    { EYELIGHT_NAME_SLIDE, EYELIGHT_NAME_AREA },
    { EYELIGHT_NAME_AREA, EYELIGHT_NAME_ITEMS },
    { EYELIGHT_NAME_AREA, EYELIGHT_NAME_IMAGE },
    { EYELIGHT_NAME_AREA, EYELIGHT_NAME_EDC },
    { EYELIGHT_NAME_ITEMS, EYELIGHT_NAME_ITEM },
    { EYELIGHT_NAME_ITEMS, EYELIGHT_NAME_ITEMS },
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
    {EYELIGHT_NAME_SCALE, 2},
    {EYELIGHT_NAME_SIZE, 2},
    {EYELIGHT_NAME_RELATIVE, 2}
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
    {EYELIGHT_NAME_SCALE, 0, EYELIGHT_VALUE_TYPE_DOUBLE},
    {EYELIGHT_NAME_SCALE, 1, EYELIGHT_VALUE_TYPE_DOUBLE},
    {EYELIGHT_NAME_RELATIVE, 0, EYELIGHT_VALUE_TYPE_DOUBLE},
    {EYELIGHT_NAME_RELATIVE, 1, EYELIGHT_VALUE_TYPE_DOUBLE},
    {EYELIGHT_NAME_SIZE, 0, EYELIGHT_VALUE_TYPE_INT},
    {EYELIGHT_NAME_SIZE, 1, EYELIGHT_VALUE_TYPE_INT},
    {EYELIGHT_NAME_BORDER, 0, EYELIGHT_VALUE_TYPE_INT}
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
    union
    {
        Eyelight_Node_Name name;
        char* value;
    };
    Eyelight_Node * father;
    Ecore_List *l;
};

struct eyelight_compiler
{
    char* input_file;
    char* output_file;

    int display_areas;

    //all the images are list here, this list is add in the part images{} of edje
    Ecore_List* image_list;

    //all the edc files include in a block "edc"
    Ecore_List* edc_files;

    //the line number, use to display a message
    int line;

    //stack of blocks which are open
    int open_block[EYELIGHT_BUFLEN];
    int last_open_block;

    //the node root
    Eyelight_Node *root;

    //defaults values
    char* default_layout;
    char* default_foot_text;
    char* default_foot_image;
    char* default_header_image;
    char* default_title;
    char* default_subtitle;
    char* default_transition;
    char* default_transition_next;
    char* default_transition_previous;
};

#define EYELIGHT_NODE_TYPE_BLOCK 0
#define EYELIGHT_NODE_TYPE_PROP 1
#define EYELIGHT_NODE_TYPE_VALUE 2



char* eyelight_create_edc_from_elt(char* input_file, int display_areas);
char* eyelight_create_edj_from_edc(char* input_file, char* theme_file);

Eyelight_Node_Name eyelight_name_get(char* p);

Eyelight_Compiler* eyelight_compiler_new(char* input_file, int display_areas);
void eyelight_compiler_free(Eyelight_Compiler **p_compiler);
Eyelight_Node *eyelight_node_new(int type,Eyelight_Node_Name name, Eyelight_Node* father);
void eyelight_node_free(Eyelight_Node** current);

char* eyelight_remove_quote(char* p);
char* eyelight_source_fetch(char* file, char** p_end);
int eyelight_image_add(Eyelight_Compiler* compiler,char* image);
int eyelight_edc_file_add(Eyelight_Compiler* compiler,char* edc_file);

char* eyelight_retrieve_value_of_prop(Eyelight_Node* node,int i);
Eyelight_Node* eyelight_retrieve_node_prop(Eyelight_Node* current, Eyelight_Node_Name p);
int eyelight_number_item_in_block(Eyelight_Node* current);

int eyelight_decimal_to_roman(char *dec, char *rom);

#endif   /* ----- #ifndef EYELIGHT_COMPILER_COMMON_INC  ----- */

