/*
 * =====================================================================================
 *
 *       Filename:  compiler.c
 *
 *    Description: from an eyelight_tree, create and edc file
 *
 *        Version:  1.0
 *        Created:  11/07/08 12:46:30 UTC
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  (Watchwolf), Atton Jonathan <watchwolf@watchwolf.fr>
 *        Company:
 *
 * =====================================================================================
 */

#include "eyelight_compiler_parser.h"
#include "eyelight_object.h"


void eyelight_compile_block_image(Eyelight_Viewer *pres, Eyelight_Slide *slide, int id_slide, Eyelight_Node *node_image, const char *area);
void eyelight_compile_block_area(Eyelight_Viewer *pres, Eyelight_Slide *slide, int id_slide, Eyelight_Node *node_area, int id_summary);
int eyelight_compile_block_items(Eyelight_Viewer *pres, Eyelight_Slide *slide, int id_slide, Eyelight_Node *node_items, int id_summary, int id_item, const char *area, int depth);
void eyelight_compile_block_item(Eyelight_Viewer *pres, Eyelight_Slide *slide, int id_slide, Eyelight_Node *node_item, const char *area, int depth, char *numbering, int numbering_id);

void eyelight_compile_block_edj(Eyelight_Viewer *pres, Eyelight_Slide *slide, int id_slide, Eyelight_Node *node_edj, const char *area);
void eyelight_compile_block_video(Eyelight_Viewer *pres, Eyelight_Slide *slide, int id_slide, Eyelight_Node *node_edj, const char *area);
void eyelight_compile_block_presentation(Eyelight_Viewer *pres, Eyelight_Slide *slide, int id_slide, Eyelight_Node *node_pres, const char *area);

void eyelight_compile_block_slide(Eyelight_Viewer *pres, Eyelight_Node* node_slide, Eyelight_Slide *slide,
        int slide_number, int nb_slides, int id_summary,
        char *default_title, char *default_subtitle,
        char *default_header_image,
        char *default_foot_text, char *default_foot_image
        );
int eyelight_nb_slides_get(Eyelight_Compiler* compiler);

void eyelight_slide_transitions_get(Eyelight_Viewer* pres,int id_slide, const char** previous, const char** next);


/*
 * @brief Create a tree from a presentation file
 */
Eyelight_Compiler* eyelight_elt_load(char *input_file)
{
    FILE* output;
    char* end;
    char* p = NULL;
    char buf[EYELIGHT_BUFLEN];
    char* output_file;

    Eyelight_Compiler* compiler = eyelight_compiler_new(input_file, 0);

    if(input_file)
    {
        p = eyelight_source_fetch(compiler->input_file,&end);
        eyelight_parse(compiler,p,end);
    }

    EYELIGHT_FREE(p);

    return compiler;
}


/*
 * @brief create a new compiler
 */
Eyelight_Compiler* eyelight_compiler_new(char* input_file, int display_areas)
{
    Eyelight_Compiler* compiler;

    compiler = calloc(1,sizeof(Eyelight_Compiler));
    compiler->line = 1;
    compiler->last_open_block = -1;
    compiler->root = eyelight_node_new(EYELIGHT_NODE_TYPE_BLOCK,EYELIGHT_NAME_ROOT,NULL);

    //special summary slide
    compiler->node_summary = eyelight_node_new(EYELIGHT_NODE_TYPE_BLOCK,EYELIGHT_NAME_SLIDE,NULL);
    Eyelight_Node *node = eyelight_node_new(EYELIGHT_NODE_TYPE_PROP,EYELIGHT_NAME_LAYOUT,
            compiler->node_summary);
    node = eyelight_node_new(EYELIGHT_NODE_TYPE_VALUE,EYELIGHT_NAME_NONE,node);
    node->value = strdup("summary");

    node = eyelight_node_new(EYELIGHT_NODE_TYPE_PROP,EYELIGHT_NAME_TITLE,
            compiler->node_summary);
    node = eyelight_node_new(EYELIGHT_NODE_TYPE_VALUE,EYELIGHT_NAME_NONE,node);
    node->value = strdup(" Summary ");

    node = eyelight_node_new(EYELIGHT_NODE_TYPE_PROP,EYELIGHT_NAME_SUBTITLE,
            compiler->node_summary);
    node = eyelight_node_new(EYELIGHT_NODE_TYPE_VALUE,EYELIGHT_NAME_NONE,node);
    node->value = strdup("");

    Eyelight_Node *node_area = eyelight_node_new(EYELIGHT_NODE_TYPE_BLOCK,EYELIGHT_NAME_AREA,
            compiler->node_summary);
    node = eyelight_node_new(EYELIGHT_NODE_TYPE_PROP,EYELIGHT_NAME_NAME,
            node_area);
    node = eyelight_node_new(EYELIGHT_NODE_TYPE_VALUE,EYELIGHT_NAME_NONE,node);
    node->value = strdup("summary");

    compiler->node_summary_items = eyelight_node_new(EYELIGHT_NODE_TYPE_BLOCK,EYELIGHT_NAME_ITEMS,
            node_area);
    //

    if(input_file)
        compiler->input_file = strdup(input_file);
    compiler-> display_areas = display_areas;

    return compiler;
}

/*
 * @brief free a compiler
 */
void eyelight_compiler_free(Eyelight_Compiler **p_compiler)
{
    Eyelight_Compiler* compiler = *p_compiler;
    char* str;

    if(!p_compiler || !(*p_compiler)) return ;

    eyelight_node_free(&(compiler->root),compiler->node_summary);
    eyelight_node_free(&(compiler->node_summary),NULL);

    EYELIGHT_FREE(compiler->input_file);

    EYELIGHT_FREE(compiler);
}

/*
 * @brief create a new node
 */
Eyelight_Node *eyelight_node_new(int type,Eyelight_Node_Name name, Eyelight_Node* father)
{
    Eyelight_Node* node = calloc(1,sizeof(Eyelight_Node));
    node->type = type;
    node->father = father;
    node->name = name;
    if(father)
    {
        father->l = eina_list_append(father->l,node);
    }

    return node;
}

/*
 * @brief free a node
 */
void eyelight_node_free(Eyelight_Node** current, Eyelight_Node *not_free)
{
    Eyelight_Node* node;
    Eina_List *l;

    if((*current)->type==EYELIGHT_NODE_TYPE_VALUE)
        EYELIGHT_FREE((*current)->value);

    EINA_LIST_FOREACH( (*current)->l ,l ,node)
        if(node != not_free)
            eyelight_node_free(&node, not_free);

    eina_list_free( (*current)->l );
    (*current)-> l = NULL;
    EYELIGHT_FREE(*current);
}



/*
 * return the ith value of the property "node"
 */
char* eyelight_retrieve_value_of_prop(Eyelight_Node* node,int i)
{
    Eyelight_Node *data;
    data = eina_list_nth(node->l,i);
    return data->value;
}

/*
 * @brief return node of the property "prop" of the block node "current"
 */
Eyelight_Node* eyelight_retrieve_node_prop(Eyelight_Node* current, Eyelight_Node_Name prop)
{
    Eyelight_Node* node = NULL;
    int find = 0;
    Eina_List *l;

    l = current->l;
    while(!find && l)
    {
        node = eina_list_data_get(l);
        if(node->type == EYELIGHT_NODE_TYPE_PROP && node->name == prop)
            find = 1;
        l=eina_list_next(l);
    }
    if(!find)
        return NULL;
    else
        return node;
}

/*
 * @brief return the number of items in a block and his sub blocks
 * this function is used to know how many items will be add in an area
 */
int eyelight_number_item_in_block(Eyelight_Node* current)
{
    int number = 0;
    Eyelight_Node* node;
    Eina_List *l;

    EINA_LIST_FOREACH(current->l, l, node)
    {
        if(node->type == EYELIGHT_NODE_TYPE_BLOCK && node->name == EYELIGHT_NAME_ITEMS)
            number+=eyelight_number_item_in_block(node);
        else if(node->type==EYELIGHT_NODE_TYPE_BLOCK &&
                  ( node->name == EYELIGHT_NAME_ITEM
                  || node->name == EYELIGHT_NAME_IMAGE
                  || node->name == EYELIGHT_NAME_EDJ))
            number++;
        else if(node->type == EYELIGHT_NODE_TYPE_PROP &&
                  ( node->name == EYELIGHT_NAME_TEXT
                  || node->name == EYELIGHT_NAME_IMAGE))
            number++;
    }
    return number;
}




/**
 * create an image path from the path of the presentation and the image file
 */
char *eyelight_compile_image_path_new(Eyelight_Viewer *pres, const char *image)
{
    if(image[0] == '/')
        return strdup(image);


    char *path_pres = ecore_file_dir_get(pres->elt_file);
    char *path_image = calloc(strlen(path_pres)+1+strlen(image)+1,sizeof(char));
    path_image[0] = '\0';
    strcat(path_image, path_pres);
    strcat(path_image,"/");
    strcat(path_image,image);
    EYELIGHT_FREE(path_pres);

    return path_image;
}


/*
 * @brief compile a block item (text ...)
 */
void eyelight_compile_block_item(Eyelight_Viewer *pres, Eyelight_Slide *slide, int id_slide, Eyelight_Node *node_item, const char *area, int depth, char *numbering, int numbering_id)
{
    Eyelight_Node* node;
    Eina_List *l;
    EINA_LIST_FOREACH(node_item->l, l, node)
    {
        if(node->type == EYELIGHT_NODE_TYPE_PROP
                && node->name == EYELIGHT_NAME_TEXT)
        {
            if(!numbering)
                eyelight_object_item_simple_text_add(
                        pres,slide,id_slide,node_item, area,depth,
                        eyelight_retrieve_value_of_prop(node,0));
            else if(strcmp(numbering,"normal")==0)
            {
                char dec[EYELIGHT_BUFLEN];
                snprintf(dec,EYELIGHT_BUFLEN,"%d",
                        numbering_id);

                eyelight_object_item_numbering_text_add(
                        pres,slide, id_slide, node_item, area,dec,depth,
                        eyelight_retrieve_value_of_prop(node,0));
            }
            else //roman
            {
                char dec[EYELIGHT_BUFLEN],
                     roman[EYELIGHT_BUFLEN];
                snprintf(dec,EYELIGHT_BUFLEN,"%d",
                        numbering_id);
                eyelight_decimal_to_roman(dec,roman);
                eyelight_object_item_numbering_text_add(
                        pres,slide,id_slide,node_item,area,roman,depth,
                        eyelight_retrieve_value_of_prop(node,0));

            }
        }
    }
}

/*
 * @brief compile a block image (scale, size, image)
 */
void eyelight_compile_block_image(Eyelight_Viewer *pres, Eyelight_Slide *slide,int id_slide, Eyelight_Node *node_image, const char *area)
{
    Eyelight_Node *node_image_file,*node_size, *node_scale, *node_border, *node_relative, *node_shadow;
    int border =  0;
    int shadow = 0;

    node_image_file = eyelight_retrieve_node_prop(node_image,
            EYELIGHT_NAME_IMAGE);
    node_size = eyelight_retrieve_node_prop(node_image,
            EYELIGHT_NAME_SIZE);
    node_scale = eyelight_retrieve_node_prop(node_image,
            EYELIGHT_NAME_SCALE);
    node_border = eyelight_retrieve_node_prop(node_image,
            EYELIGHT_NAME_BORDER);
    node_relative = eyelight_retrieve_node_prop(node_image,
            EYELIGHT_NAME_RELATIVE);
    node_shadow = eyelight_retrieve_node_prop(node_image,
            EYELIGHT_NAME_SHADOW);

    if(node_border)
        border = atoi(eyelight_retrieve_value_of_prop(node_border,0));

    if(node_shadow)
        shadow = atoi(eyelight_retrieve_value_of_prop(node_shadow,0));


    eyelight_object_item_image_add(pres,slide,id_slide,node_image, area,
            eyelight_retrieve_value_of_prop(node_image_file,0),
            border,shadow);
}

/*
 * @brief compile a block video (file, alpha)
 */
void eyelight_compile_block_video(Eyelight_Viewer *pres, Eyelight_Slide *slide, int id_slide, Eyelight_Node *node_edj, const char *area)
{
    Eyelight_Node* node_file, *node_alpha, *node_autoplay, *node_shadow, *node_border, *node_replay;

    node_file = eyelight_retrieve_node_prop(node_edj,EYELIGHT_NAME_VIDEO);
    node_alpha = eyelight_retrieve_node_prop(node_edj,EYELIGHT_NAME_ALPHA);
    node_autoplay = eyelight_retrieve_node_prop(node_edj,EYELIGHT_NAME_AUTOPLAY);
    node_shadow = eyelight_retrieve_node_prop(node_edj,EYELIGHT_NAME_SHADOW);
    node_border = eyelight_retrieve_node_prop(node_edj,EYELIGHT_NAME_BORDER);
    node_replay = eyelight_retrieve_node_prop(node_edj,EYELIGHT_NAME_REPLAY);

    int alpha = 255;
    if(node_alpha)
        alpha = atoi(eyelight_retrieve_value_of_prop(node_alpha,0));

    int autoplay = 0;
    if(node_autoplay)
        autoplay = atoi(eyelight_retrieve_value_of_prop(node_autoplay,0));

    int shadow = 0;
    if(node_shadow)
        shadow = atoi(eyelight_retrieve_value_of_prop(node_shadow,0));

    int border = 0;
    if(node_border)
        border = atoi(eyelight_retrieve_value_of_prop(node_border,0));

    int replay = 0;
    if(node_replay)
        replay = atoi(eyelight_retrieve_value_of_prop(node_replay,0));


    if(node_file)
    {
        eyelight_object_item_video_add(pres, slide, id_slide, node_edj, area,
                                eyelight_retrieve_value_of_prop(node_file,0),alpha,autoplay,replay,border,shadow);
    }
}


/*
 * @brief compile a block presentation (presentation, theme)
 */
void eyelight_compile_block_presentation(Eyelight_Viewer *pres, Eyelight_Slide *slide, int id_slide, Eyelight_Node *node_pres, const char *area)
{
    Eyelight_Node* node_presentation, *node_theme, *node_shadow, *node_border;

    node_presentation = eyelight_retrieve_node_prop(node_pres,EYELIGHT_NAME_PRESENTATION);
    node_theme = eyelight_retrieve_node_prop(node_pres,EYELIGHT_NAME_THEME);
    node_shadow = eyelight_retrieve_node_prop(node_pres,EYELIGHT_NAME_SHADOW);
    node_border = eyelight_retrieve_node_prop(node_pres,EYELIGHT_NAME_BORDER);

    char *presentation = strdup("");
    if(node_presentation)
    {
        EYELIGHT_FREE(presentation);
        presentation = eyelight_retrieve_value_of_prop(node_presentation,0);
        presentation = eyelight_compile_image_path_new(pres,presentation);
    }

    char *theme = strdup(PACKAGE_DATA_DIR"/themes/default/theme.edj");
    if(node_theme)
    {
        EYELIGHT_FREE(theme);
        theme = eyelight_retrieve_value_of_prop(node_theme,0);
        theme = eyelight_compile_image_path_new(pres,theme);
    }

    int shadow = 0;
    if(node_shadow)
        shadow = atoi(eyelight_retrieve_value_of_prop(node_shadow,0));

    int border = 0;
    if(node_border)
        border = atoi(eyelight_retrieve_value_of_prop(node_border,0));


    eyelight_object_item_presentation_add(pres, slide, id_slide, node_pres, area, presentation, theme, border, shadow);

    EYELIGHT_FREE(presentation);
    EYELIGHT_FREE(theme);
}


/*
 * @brief compile a block video (file, macro)
 */
void eyelight_compile_block_edj(Eyelight_Viewer *pres, Eyelight_Slide *slide, int id_slide, Eyelight_Node *node_edj, const char *area)
{
    Eyelight_Node* node_file, *node_group;

    node_file = eyelight_retrieve_node_prop(node_edj,EYELIGHT_NAME_FILE);
    node_group = eyelight_retrieve_node_prop(node_edj,EYELIGHT_NAME_GROUP);
    if(node_group && node_file)
    {
        eyelight_object_item_edje_add(pres,slide,id_slide,node_edj, area,
                eyelight_retrieve_value_of_prop(node_file,0),
                eyelight_retrieve_value_of_prop(node_group,0));
    }
}



/*
 * @brief compile a block items (text, numbering, item, items)
 */
int eyelight_compile_block_items(Eyelight_Viewer *pres, Eyelight_Slide *slide, int id_slide, Eyelight_Node *node_items, int id_summary, int id_item, const char *area, int depth)
{
    Eyelight_Node* node,*node_numbering;
    char *numbering = NULL;
    int numbering_id = 1;
    Eina_List *l;

    node_numbering = eyelight_retrieve_node_prop(node_items
            ,EYELIGHT_NAME_NUMBERING);
    if(node_numbering)
        numbering = eyelight_retrieve_value_of_prop(node_numbering,0);

    if(numbering && strcmp(numbering,"none")==0)
        numbering = NULL;

    EINA_LIST_FOREACH(node_items->l, l, node)
    {
        switch(node->type)
        {
            case EYELIGHT_NODE_TYPE_BLOCK:
                switch(node->name)
                {
                    case EYELIGHT_NAME_ITEMS:
                        id_item = eyelight_compile_block_items(pres, slide, id_slide, node,
                                id_summary, id_item, area, depth+1);
                        break;
                    case EYELIGHT_NAME_ITEM:
                        eyelight_compile_block_item(pres, slide, id_slide, node,
                                area, depth+1, numbering, numbering_id);
                        numbering_id++;
                        id_item++;
                        break;
                }
                break;
            case EYELIGHT_NODE_TYPE_PROP:
                switch(node->name)
                {
                    case EYELIGHT_NAME_TEXT:
                        if(!numbering)
                            eyelight_object_item_simple_text_add(
                                    pres,slide,id_slide,node, area,depth,
                                    eyelight_retrieve_value_of_prop(node,0));
                        else if(strcmp(numbering,"normal")==0)
                        {
                            char dec[EYELIGHT_BUFLEN];
                            snprintf(dec,EYELIGHT_BUFLEN,"%d",
                                    numbering_id);

                            eyelight_object_item_numbering_text_add(
                                    pres,slide,id_slide,node, area,dec,depth,
                                    eyelight_retrieve_value_of_prop(node,0));
                        }
                        else //roman
                        {
                            char dec[EYELIGHT_BUFLEN],
                                 roman[EYELIGHT_BUFLEN];
                            snprintf(dec,EYELIGHT_BUFLEN,"%d",
                                    numbering_id);
                            eyelight_decimal_to_roman(dec,roman);
                            eyelight_object_item_numbering_text_add(
                                    pres,slide,id_slide,node,area,roman,depth,
                                    eyelight_retrieve_value_of_prop(node,0));
                        }
                        numbering_id++;
                        id_item++;
                        break;
                    case EYELIGHT_NAME_TEXT_SUMMARY:
                        eyelight_object_item_summary_text_add(
                                pres,slide,id_slide,id_summary, id_item, area,depth,
                                eyelight_retrieve_value_of_prop(node,0));
                        id_item++;
                        break;
                }
                break;
        }
    }
    return id_item;
}

/*
 * @brief compile a block area (name, layout, items ...)
 */
void eyelight_compile_block_area(Eyelight_Viewer *pres, Eyelight_Slide *slide, int id_slide, Eyelight_Node *node_area, int id_summary)
{
    Eyelight_Node* node;
    char* area;
    int number_item = 0;
    int item_number = 0;
    char* layout;
    char* value;
    Eina_List *l;

    node = eyelight_retrieve_node_prop(node_area, EYELIGHT_NAME_NAME);
    if(!node)
    {
        fprintf(stderr,"An area doesn't have a name\n");
        return;
    }
    area = eyelight_retrieve_value_of_prop(node,0);

    Eyelight_Node *node_layout = eyelight_retrieve_node_prop(node_area, EYELIGHT_NAME_LAYOUT);
    if(node_layout)
    {
        char buf[EYELIGHT_BUFLEN];
        char *layout = eyelight_retrieve_value_of_prop(node_layout, 0);

        Evas_Object *o_area = eyelight_object_area_obj_get(pres,slide,id_slide,
                area, buf);

        if(o_area == slide->obj)
            snprintf(buf,EYELIGHT_BUFLEN, "area,%s,layout,%s", area, layout);
        else
            snprintf(buf,EYELIGHT_BUFLEN, "area,custom,layout,%s", layout);

        edje_object_signal_emit(o_area, buf, "eyelight");
    }

    EINA_LIST_FOREACH(node_area->l, l, node)
    {
        switch(node->type)
        {
            case EYELIGHT_NODE_TYPE_BLOCK:
                switch(node->name)
                {
                    case EYELIGHT_NAME_ITEMS:
                        eyelight_compile_block_items(pres, slide, id_slide, node, id_summary, 1, area, 1);
                        break;
                    case EYELIGHT_NAME_IMAGE:
                        eyelight_compile_block_image(pres, slide, id_slide, node, area);
                        break;
                    case EYELIGHT_NAME_EDJ:
                        eyelight_compile_block_edj(pres, slide, id_slide, node, area);
                        break;
                    case EYELIGHT_NAME_VIDEO:
                        eyelight_compile_block_video(pres, slide, id_slide, node, area);
                        break;
                    case EYELIGHT_NAME_PRESENTATION:
                        eyelight_compile_block_presentation(pres, slide, id_slide, node, area);
                        break;
                    default : break;
                }
                break;
            case EYELIGHT_NODE_TYPE_PROP:
                switch(node->name)
                {
                    case EYELIGHT_NAME_TEXT:
                        eyelight_object_item_text_add(pres, slide, id_slide, node, area,
                                eyelight_retrieve_value_of_prop(node,0));
                        break;
                    case EYELIGHT_NAME_IMAGE:
                        eyelight_object_item_image_add(pres,slide,id_slide, node, area,
                                eyelight_retrieve_value_of_prop(node,0), 0,0);
                        break;
                    case EYELIGHT_NAME_VIDEO:
                        eyelight_object_item_video_add(pres,slide, id_slide, node, area,
                                eyelight_retrieve_value_of_prop(node,0),255,0,1,0,0);
                }
                break;
        }
    }
}

/*
 * @brief compile a block slide (area, title, subtitle ...)
 */
void eyelight_compile_block_slide(Eyelight_Viewer *pres, Eyelight_Node* node_slide, Eyelight_Slide *slide, int slide_number, int nb_slides,int id_summary,
        char *default_title, char *default_subtitle,
        char *default_header_image,
        char *default_foot_text, char *default_foot_image
        )
{
    Eyelight_Node * node;
    Eina_List *l;

    eyelight_object_title_add(pres,slide,slide_number,node_slide,default_title);
    eyelight_object_subtitle_add(pres,slide,slide_number,node_slide,default_subtitle);
    eyelight_object_header_image_add(pres,slide,slide_number,node_slide,default_header_image);
    eyelight_object_foot_text_add(pres,slide,slide_number,node_slide,default_foot_text);
    eyelight_object_foot_image_add(pres,slide,slide_number,node_slide,default_foot_image);

    eyelight_object_pages_add(pres,slide,slide_number,slide_number, nb_slides);

    //first we compile all custom area
    EINA_LIST_FOREACH(node_slide->l, l, node)
    {
        switch(node->type)
        {
            case EYELIGHT_NODE_TYPE_PROP:
                switch(node->name)
                {
                    case EYELIGHT_NAME_CUSTOM_AREA:
                        {
                            char *name = eyelight_retrieve_value_of_prop(node,0);
                            double rel1_x = atof(eyelight_retrieve_value_of_prop(node,1));
                            double rel1_y = atof(eyelight_retrieve_value_of_prop(node,2));
                            double rel2_x = atof(eyelight_retrieve_value_of_prop(node,3));
                            double rel2_y = atof(eyelight_retrieve_value_of_prop(node,4));

                            Evas_Object *o = eyelight_object_custom_area_add(pres, slide, slide_number,node, name,rel1_x,rel1_y,rel2_x,rel2_y);
                        }
                        break;
                }
                break;
            default: ;
        }
    }


    EINA_LIST_FOREACH(node_slide->l, l, node)
    {
        switch(node->type)
        {
            case EYELIGHT_NODE_TYPE_BLOCK:
                switch(node->name)
                {
                    case EYELIGHT_NAME_AREA:
                        eyelight_compile_block_area(pres,slide, slide_number, node, id_summary);
                        break;
                }
                break;
            default: ;
        }
    }
}

/*
 * @brief main compile function
 */
void eyelight_compile(Eyelight_Viewer *pres, Eyelight_Slide *slide, int id_slide)
{
    Eyelight_Node* node;
    int slide_number = 1;
    Eyelight_Compiler *compiler = pres->compiler;
    char* image,*edc_file;
    int nb_slides;
    Eyelight_Node *node_slide;
    Eina_List *l;
    int id_summary = 0;
    int old_summary = id_summary;

    char *default_foot_text = NULL;
    char *default_foot_image = NULL;

    char *default_header_image = NULL;

    char *default_title = NULL;
    char *default_subtitle = NULL;

    nb_slides = eyelight_nb_slides_get(compiler);

    int i_slide = -1;
    l = compiler->root->l;
    while( l && i_slide<id_slide)
    {
        node = eina_list_data_get(l);
        switch(node->type)
        {
            case EYELIGHT_NODE_TYPE_PROP:
                switch(node->name)
                {
                    case EYELIGHT_NAME_FOOT_TEXT:
                            default_foot_text = eyelight_retrieve_value_of_prop(node,0);
                        break;
                    case EYELIGHT_NAME_FOOT_IMAGE:
                            default_foot_image = eyelight_retrieve_value_of_prop(node,0);
                            break;
                    case EYELIGHT_NAME_HEADER_IMAGE:
                            default_header_image = eyelight_retrieve_value_of_prop(node,0);
                            break;
                    case EYELIGHT_NAME_TITLE:
                            default_title = eyelight_retrieve_value_of_prop(node,0);
                            break;
                    case EYELIGHT_NAME_SUBTITLE:
                            default_subtitle = eyelight_retrieve_value_of_prop(node,0);
                            break;
                    case EYELIGHT_NAME_CHAPTER:
                    case EYELIGHT_NAME_SECTION:
                        id_summary = ++old_summary;
                        break;
                    case EYELIGHT_NAME_SUMMARY:
                        if( strcmp( "complete",
                                    eyelight_retrieve_value_of_prop(node,0)) == 0)
                            // -1 we display all items as a previous item
                            id_summary= -1;
                        break;
                }
                break;
            case EYELIGHT_NODE_TYPE_BLOCK:
                switch(node->name)
                {
                    case EYELIGHT_NAME_SLIDE:
                        node_slide = node;
                        i_slide++;
                        break;
                }
                break;
        }
        l = eina_list_next(l);
    }
    eyelight_compile_block_slide(pres, node_slide, slide, id_slide, nb_slides, id_summary,
            default_title, default_subtitle,
            default_header_image,
            default_foot_text, default_foot_image
            );
}

int eyelight_nb_slides_get(Eyelight_Compiler* compiler)
{
    Eyelight_Node* node;
    int number = 0;
    Eina_List *l;

    if(!compiler)
        return 0;

    EINA_LIST_FOREACH(compiler->root->l, l, node)
    {
        if(node->name==EYELIGHT_NAME_SLIDE)
            number++;
    }
    return number;
}

void eyelight_slide_transitions_get(Eyelight_Viewer* pres,int id_slide, const char** previous, const char** next)
{
    int i_slide = -1;
    Eyelight_Node *node_slide, *node;
    Eyelight_Compiler *compiler = pres->compiler;
    Eina_List *l;

    l = compiler->root->l;
    while( l && i_slide<id_slide)
    {
        node = eina_list_data_get(l);
        switch(node->type)
        {
            case EYELIGHT_NODE_TYPE_BLOCK:
                switch(node->name)
                {
                    case EYELIGHT_NAME_SLIDE:
                        node_slide = node;
                        i_slide++;
                        break;
                }
                break;
            default : break;
        }
        l = eina_list_next(l);
    }

    *next = "none";
    *previous = "none";

    node = eyelight_retrieve_node_prop(pres->compiler->root, EYELIGHT_NAME_TRANSITION);
    if(node)
    {
        *next = eyelight_retrieve_value_of_prop(node, 0);
        *previous = eyelight_retrieve_value_of_prop(node, 0);
    }

    node = eyelight_retrieve_node_prop(node_slide, EYELIGHT_NAME_TRANSITION_NEXT);
    if(node)
        *next = eyelight_retrieve_value_of_prop(node, 0);

    node = eyelight_retrieve_node_prop(node_slide, EYELIGHT_NAME_TRANSITION_PREVIOUS);
    if(node)
        *previous = eyelight_retrieve_value_of_prop(node, 0);
}


/*
 * @brief Originally written by magma
 * translate a decimal value into a roman value
 * source: http://files.codes-sources.com/fichier.aspx?id=31354&f=Romain.c
 */
int eyelight_decimal_to_roman(char *dec, char *rom)
{
    int len, i;
    char *unite[] = {"", "I", "II", "III", "IV", "V", "VI", "VII", "VIII", "IX"};
    char *dizaine[] = {"", "X", "XX", "XXX", "XL", "L", "LX", "LXX", "LXXX", "XC"};
    char *centaine[] = {"", "C", "CC", "CCC", "CD", "D", "DC", "DCC", "DCCC", "CM"};
    char *millier[] = {"", "M", "MM", "MMM"};

    if(dec[0] == '0')
    {
        strcpy(rom, "Nombre invalide [1->3999] !");
        return 0;
    }

    len = strlen(dec);
    strcpy(rom, "");

    for(i = 0; i < len; i++)
    {
        if(dec[i] >= '0' && dec[i] <= '9')
        {
            switch(len - i)
            {
                case 1:
                    strcat(rom, unite[dec[i] - '0']);
                    break;

                case 2:
                    strcat(rom, dizaine[dec[i] - '0']);
                    break;

                case 3:
                    strcat(rom, centaine[dec[i] - '0']);
                    break;

                case 4:
                    if(dec[0] <= '3')
                        strcpy(rom, millier[dec[i] - '0']);
                    else {
                        strcpy(rom, "Nombre invalide [1->3999] !");
                        return 0;
                    }
                    break;

                default:
                    strcpy(rom, "Nombre invalide [1->3999] !");
                    return 0;
            }
        }
        else
        {
            strcpy(rom, "Nombre invalide [1->3999] !");
            return 0;
        }
    }
    return 1;
}
