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

#include "eyelight_compiler.h"

char *eyelight_compile_image_path_new(Eyelight_Viewer *pres, char *image);


void eyelight_compile_block_image(Eyelight_Viewer *pres, int id_slide, Eyelight_Node *node_image, Evas_Object *o_slide, const char *area);
void eyelight_compile_block_area(Eyelight_Viewer *pres, int id_slide, Eyelight_Node *node_area, Evas_Object *o_slide, int id_summary);
int eyelight_compile_block_items(Eyelight_Viewer *pres, int id_slide, Eyelight_Node *node_items, Evas_Object *o_slide, int id_summary, int id_item, const char *area, int depth);
void eyelight_compile_block_item(Eyelight_Viewer *pres, int id_slide, Eyelight_Node *node_item, Evas_Object *o_slide, const char *area, int depth, char *numbering, int numbering_id);

void eyelight_compile_block_edj(Eyelight_Viewer *pres, int id_slide, Eyelight_Node *node_edj, Evas_Object *o_slide, const char *area);
void eyelight_compile_block_video(Eyelight_Viewer *pres, int id_slide, Eyelight_Node *node_edj, Evas_Object *o_slide, const char *area);

void eyelight_compile_block_slide(Eyelight_Viewer *pres, Eyelight_Node* node_slide, Evas_Object *o_slide,
        int slide_number, int nb_slides, int id_summary,
        char *default_title, char *default_subtitle,
        char *default_header_image,
        char *default_foot_text, char *default_foot_image
        );
int eyelight_nb_slides_get(Eyelight_Compiler* compiler);

void eyelight_slide_transitions_get(Eyelight_Viewer* pres,int id_slide, const char** previous, const char** next);

/**
 * create an image path from the path of the presentation and the image file
 */
char *eyelight_compile_image_path_new(Eyelight_Viewer *pres, char *image)
{
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
void eyelight_compile_block_item(Eyelight_Viewer *pres, int id_slide, Eyelight_Node *node_item, Evas_Object *o_slide, const char *area, int depth, char *numbering, int numbering_id)
{
    Eyelight_Node* node;
    ecore_list_first_goto(node_item->l);
    while((node=ecore_list_next(node_item->l)))
    {
        if(node->type == EYELIGHT_NODE_TYPE_PROP
                && node->name == EYELIGHT_NAME_TEXT)
        {
            if(!numbering)
                eyelight_object_item_simple_text_add(
                        pres,id_slide, o_slide,area,depth,
                        eyelight_retrieve_value_of_prop(node,0));
            else if(strcmp(numbering,"normal")==0)
            {
                char dec[EYELIGHT_BUFLEN];
                snprintf(dec,EYELIGHT_BUFLEN,"%d",
                        numbering_id);

                eyelight_object_item_numbering_text_add(
                        pres,id_slide, o_slide,area,dec,depth,
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
                        pres,id_slide, o_slide,area,roman,depth,
                        eyelight_retrieve_value_of_prop(node,0));

            }
        }
    }
}

/*
 * @brief compile a block image (scale, size, image)
 */
void eyelight_compile_block_image(Eyelight_Viewer *pres, int id_slide, Eyelight_Node *node_image, Evas_Object *o_slide, const char *area)
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


    eyelight_object_item_image_add(pres,id_slide, o_slide,area,
            eyelight_retrieve_value_of_prop(node_image_file,0),
            border,shadow);

        /*if(node_image)
    {
        char* image = eyelight_retrieve_value_of_prop(node_image,0);
        if(node_relative)
        {
            char* rel_w = eyelight_retrieve_value_of_prop(node_relative,0);
            char* rel_h = eyelight_retrieve_value_of_prop(node_relative,1);
            fprintf(output,"add_image_relative_%s(\"%s\",%d,%d,\"%d\",%d,\"%s\",%s, %s,%s);\n",
                    layout,area,item_number-1,item_number,item_number,number_item,image,rel_w,rel_h,border);
        }

        else if(node_size)
        {
            char* size_w = eyelight_retrieve_value_of_prop(node_size,0);
            char* size_h = eyelight_retrieve_value_of_prop(node_size,1);
            fprintf(output,"add_image_size_%s(\"%s\",%d,%d,\"%d\",%d,\"%s\",%s, %s,%s);\n",
                    layout,area,item_number-1,item_number,item_number,number_item,image,size_w,size_h,border);
        }
        else if(node_scale)
        {
            char* scale_w = eyelight_retrieve_value_of_prop(node_scale
                    ,0);
            char* scale_h = eyelight_retrieve_value_of_prop(node_scale
                    ,1);
            double scale_w_d,scale_h_d;
            scale_w_d = strtod(scale_w,NULL);
            scale_h_d = strtod(scale_h,NULL);

            //retrieve the size of the image
            Ecore_Evas* ee;
            Evas *e;
            Evas_Object* img;
            ee = ecore_evas_buffer_new(100,100);
            e = ecore_evas_get(ee);
            char buf[EYELIGHT_BUFLEN];

            img = evas_object_image_add(e);
            snprintf(buf,EYELIGHT_BUFLEN,"%s/%s"
                    ,ecore_file_dir_get(compiler->input_file),image);
            evas_object_image_file_set(img,buf,NULL);
            int w,h;
            evas_object_image_size_get(img,&w,&h);
            evas_free(e);
            fprintf(output,"add_image_size_%s(\"%s\",%d,%d,\"%d\",%d,\"%s\",%f, %f,%s);\n",
                    layout,area,item_number-1,item_number,item_number,number_item,image
                    ,w*scale_w_d,h*scale_h_d,border);
        }
        else
        {
            fprintf(output,"add_image_%s(\"%s\",%d,%d,\"%d\",%d,\"%s\",%s);\n",
                    layout,area,item_number-1,item_number,item_number,number_item,image,border);
        }
        eyelight_image_add(compiler,image);
    }*/

}

/*
 * @brief compile a block video (file, alpha)
 */
void eyelight_compile_block_video(Eyelight_Viewer *pres, int id_slide, Eyelight_Node *node_edj, Evas_Object *o_slide, const char *area)
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
        eyelight_object_item_video_add(pres, id_slide, o_slide, area,
                                eyelight_retrieve_value_of_prop(node_file,0),alpha,autoplay,replay,border,shadow);
    }
}


/*
 * @brief compile a block video (file, macro)
 */
void eyelight_compile_block_edj(Eyelight_Viewer *pres, int id_slide, Eyelight_Node *node_edj, Evas_Object *o_slide, const char *area)
{
    Eyelight_Node* node_file, *node_group;

    node_file = eyelight_retrieve_node_prop(node_edj,EYELIGHT_NAME_FILE);
    node_group = eyelight_retrieve_node_prop(node_edj,EYELIGHT_NAME_GROUP);
    if(node_group && node_file)
    {
        eyelight_object_item_edje_add(pres,id_slide, o_slide,area,
                eyelight_retrieve_value_of_prop(node_file,0),
                eyelight_retrieve_value_of_prop(node_group,0));
    }
}



/*
 * @brief compile a block items (text, numbering, item, items)
 */
int eyelight_compile_block_items(Eyelight_Viewer *pres, int id_slide, Eyelight_Node *node_items, Evas_Object *o_slide, int id_summary, int id_item, const char *area, int depth)
{
    Eyelight_Node* node,*node_numbering;
    char *numbering = NULL;
    int numbering_id = 1;

    node_numbering = eyelight_retrieve_node_prop(node_items
            ,EYELIGHT_NAME_NUMBERING);
    if(node_numbering)
        numbering = eyelight_retrieve_value_of_prop(node_numbering,0);

    if(numbering && strcmp(numbering,"none")==0)
        numbering = NULL;

    ecore_list_first_goto(node_items->l);
    while((node=ecore_list_next(node_items->l)))
    {
        switch(node->type)
        {
            case EYELIGHT_NODE_TYPE_BLOCK:
                switch(node->name)
                {
                    case EYELIGHT_NAME_ITEMS:
                        id_item = eyelight_compile_block_items(pres, id_slide, node,
                                o_slide, id_summary, id_item, area, depth+1);
                        break;
                    case EYELIGHT_NAME_ITEM:
                        eyelight_compile_block_item(pres, id_slide, node,
                                o_slide, area, depth+1, numbering, numbering_id);
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
                                    pres,id_slide, o_slide,area,depth,
                                    eyelight_retrieve_value_of_prop(node,0));
                        else if(strcmp(numbering,"normal")==0)
                        {
                            char dec[EYELIGHT_BUFLEN];
                            snprintf(dec,EYELIGHT_BUFLEN,"%d",
                                    numbering_id);

                            eyelight_object_item_numbering_text_add(
                                    pres,id_slide, o_slide,area,dec,depth,
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
                                    pres,id_slide,o_slide,area,roman,depth,
                                    eyelight_retrieve_value_of_prop(node,0));
                        }
                        numbering_id++;
                        id_item++;
                        break;
                    case EYELIGHT_NAME_TEXT_SUMMARY:
                        eyelight_object_item_summary_text_add(
                                pres,id_slide, o_slide,id_summary, id_item, area,depth,
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
void eyelight_compile_block_area(Eyelight_Viewer *pres, int id_slide, Eyelight_Node *node_area, Evas_Object *o_slide, int id_summary)
{
    Eyelight_Node* node;
    char* area;
    int number_item = 0;
    int item_number = 0;
    char* layout;
    char* value;

    node = eyelight_retrieve_node_prop(node_area, EYELIGHT_NAME_NAME);
    if(!node)
    {
        fprintf(stderr,"An area doesn't have a name\n");
        exit(EXIT_FAILURE);
    }
    area = eyelight_retrieve_value_of_prop(node,0);

    Eyelight_Node *node_layout = eyelight_retrieve_node_prop(node_area, EYELIGHT_NAME_LAYOUT);
    if(node_layout)
    {
        char buf[EYELIGHT_BUFLEN];
        char *layout = eyelight_retrieve_value_of_prop(node_layout, 0);

        Evas_Object *o_area = eyelight_object_area_obj_get(pres,id_slide,
                o_slide, area, &buf);

        if(o_area == o_slide)
            snprintf(buf,EYELIGHT_BUFLEN, "area,%s,layout,%s", area, layout);
        else
            snprintf(buf,EYELIGHT_BUFLEN, "area,custom,layout,%s", layout);

        edje_object_signal_emit(o_area, buf, "eyelight");
    }

    ecore_list_first_goto(node_area->l);
    while((node=ecore_list_next(node_area->l)))
    {
        switch(node->type)
        {
            case EYELIGHT_NODE_TYPE_BLOCK:
                switch(node->name)
                {
                    case EYELIGHT_NAME_ITEMS:
                        eyelight_compile_block_items(pres, id_slide, node, o_slide, id_summary, 1, area, 1);
                        break;
                    case EYELIGHT_NAME_IMAGE:
                        eyelight_compile_block_image(pres, id_slide, node, o_slide, area);
                        break;
                    case EYELIGHT_NAME_EDJ:
                        eyelight_compile_block_edj(pres, id_slide, node, o_slide, area);
                        break;
                    case EYELIGHT_NAME_VIDEO:
                        eyelight_compile_block_video(pres, id_slide, node, o_slide, area);
                        break;
                    default : break;
                }
                break;
            case EYELIGHT_NODE_TYPE_PROP:
                switch(node->name)
                {
                    case EYELIGHT_NAME_TEXT:
                        eyelight_object_item_text_add(pres,id_slide, o_slide, area,
                                eyelight_retrieve_value_of_prop(node,0));
                        break;
                    case EYELIGHT_NAME_IMAGE:
                        eyelight_object_item_image_add(pres,id_slide, o_slide, area,
                                eyelight_retrieve_value_of_prop(node,0),0,0);
                        break;
                    case EYELIGHT_NAME_VIDEO:
                        eyelight_object_item_video_add(pres, id_slide, o_slide, area,
                                eyelight_retrieve_value_of_prop(node,0),255,0,1,0,0);
                }
                break;
        }
    }
}

/*
 * @brief compile a block slide (area, title, subtitle ...)
 */
void eyelight_compile_block_slide(Eyelight_Viewer *pres, Eyelight_Node* node_slide, Evas_Object *o_slide, int slide_number, int nb_slides,int id_summary,
        char *default_title, char *default_subtitle,
        char *default_header_image,
        char *default_foot_text, char *default_foot_image
        )
{
    Eyelight_Node * node;

    eyelight_object_title_add(pres,node_slide,o_slide,default_title);
    eyelight_object_subtitle_add(pres,node_slide,o_slide,default_subtitle);
    eyelight_object_header_image_add(pres,node_slide,o_slide,default_header_image);
    eyelight_object_foot_text_add(pres,node_slide,o_slide,default_foot_text);
    eyelight_object_foot_image_add(pres,node_slide,o_slide,default_foot_image);

    eyelight_object_pages_add(pres,o_slide,slide_number, nb_slides);

    ecore_list_first_goto(node_slide->l);
    while( (node = ecore_list_next(node_slide->l)) )
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

                            eyelight_object_custom_area_add(pres, slide_number,o_slide,name,rel1_x,rel1_y,rel2_x,rel2_y);
                        }
                        break;
                }
                break;
            case EYELIGHT_NODE_TYPE_BLOCK:
                switch(node->name)
                {
                    case EYELIGHT_NAME_AREA:
                        eyelight_compile_block_area(pres,slide_number, node,o_slide, id_summary);
                        break;
                }
                break;
        }
    }
}

/*
 * @brief main compile function
 */
void eyelight_compile(Eyelight_Viewer *pres, int id_slide, Evas_Object *o_slide)
{
    Eyelight_Node* node;
    int slide_number = 1;
    Eyelight_Compiler *compiler = pres->compiler;
    char* image,*edc_file;
    int nb_slides;

    int id_summary = 0;
    int old_summary = id_summary;

    char *default_foot_text = NULL;
    char *default_foot_image = NULL;

    char *default_header_image = NULL;

    char *default_title = NULL;
    char *default_subtitle = NULL;

    nb_slides = eyelight_nb_slides_get(compiler);

    ecore_list_first_goto(compiler->root->l);
    int i_slide = -1;
    Eyelight_Node *node_slide = NULL;
    while( (node = ecore_list_next(compiler->root->l)) && i_slide<id_slide)
    {
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
    }
    eyelight_compile_block_slide(pres, node_slide, o_slide, id_slide, nb_slides, id_summary,
            default_title, default_subtitle,
            default_header_image,
            default_foot_text, default_foot_image
            );
}

int eyelight_nb_slides_get(Eyelight_Compiler* compiler)
{
    Eyelight_Node* node;
    int number = 0;

    ecore_list_first_goto(compiler->root->l);
    while( (node = ecore_list_next(compiler->root->l)) )
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

    ecore_list_first_goto(compiler->root->l);
    while( (node = ecore_list_next(compiler->root->l)) && i_slide<id_slide)
    {
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
