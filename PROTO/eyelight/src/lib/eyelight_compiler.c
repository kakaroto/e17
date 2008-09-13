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

void eyelight_compile_prop_slide(Eyelight_Compiler* compiler,FILE* output, Eyelight_Node* node, char** title, char** subtitle);
void eyelight_compile_block_item(Eyelight_Compiler* compiler,FILE* output, Eyelight_Node* current, char* area, char* layout, int item_number, int number_item, int depth, char* numbering, int numbering_id);
void eyelight_compile_block_image(Eyelight_Compiler* compiler,FILE* output, Eyelight_Node* current, char* area, char* layout, int item_number, int number_item);
int eyelight_compile_block_items(Eyelight_Compiler* compiler,FILE* output, Eyelight_Node* current, char* area, char* layout, int item_number, int number_item, int depth, char* numbering, int numbering_id);
void eyelight_compil_block_area(Eyelight_Compiler* compiler,FILE* output, Eyelight_Node* current);
void eyelight_compile_block_slide(Eyelight_Compiler* compiler,FILE* output, Eyelight_Node* current, int slide_number);
void edc_file_list_load(Eyelight_Compiler* compiler, Eyelight_Node* current);

/*
 * @brief compile the properties transition, transition_next, transition_previous
 */
void eyelight_compile_transition_slide(Eyelight_Compiler* compiler,FILE* output, Eyelight_Node* current)
{
    char* transition = NULL;
    char* transition_next = NULL;
    char* transition_previous = NULL;
    Eyelight_Node* node;

    node = eyelight_retrieve_node_prop(current, EYELIGHT_NAME_TRANSITION);
    if(!node)
    {
        transition = compiler->default_transition;
    }
    else
        transition = eyelight_retrieve_value_of_prop(node,0);

    node = eyelight_retrieve_node_prop(current, EYELIGHT_NAME_TRANSITION_NEXT);
    if(!node)
    {
        transition_next = compiler->default_transition_next;
    }
    else
        transition_next = eyelight_retrieve_value_of_prop(node,0);

    node = eyelight_retrieve_node_prop(current, EYELIGHT_NAME_TRANSITION_PREVIOUS);
    if(!node)
    {
        transition_previous = compiler->default_transition_previous;
    }
    else
        transition_previous = eyelight_retrieve_value_of_prop(node,0);

    fprintf(output,"data\n{\n");
    if(transition)
        fprintf(output,"item: transition %s;\n",transition);
    if(transition_next)
        fprintf(output,"item: transition_next %s;\n",transition_next);
    if(transition_previous)
        fprintf(output,"item: transition_previous %s;\n",transition_previous);
    fprintf(output,"}\n");
}

/*
 * @brief compile the properties of a block slide (layout, title  ....)
 * @return returns the title and the subtitle
 */
void eyelight_compile_prop_slide(Eyelight_Compiler* compiler,FILE* output, Eyelight_Node* current, char** title, char** subtitle)
{
    char* str = NULL;

    Eyelight_Node * node = eyelight_retrieve_node_prop(current, EYELIGHT_NAME_LAYOUT);
    if(!node)
    {
        str = compiler->default_layout;
    }
    else
        str = eyelight_retrieve_value_of_prop(node,0);
    if(str)
        fprintf(output,"use_layout_%s(%d);\n",str,compiler->display_areas);

    str=NULL;
    node = eyelight_retrieve_node_prop(current, EYELIGHT_NAME_FOOT_TEXT);
    if(!node)
    {
        str = compiler->default_foot_text;
    }
    else
        str = eyelight_retrieve_value_of_prop(node,0);
    if(str)
        fprintf(output,"set_foot_text(\"%s\");\n",str);

    str = NULL;
    node = eyelight_retrieve_node_prop(current, EYELIGHT_NAME_FOOT_IMAGE);
    if(!node)
    {
        str = compiler->default_foot_image;
    }
    else
        str = eyelight_retrieve_value_of_prop(node,0);
    if(str)
    {
        fprintf(output,"set_foot_image(\"%s\");\n",str);
        eyelight_image_add(compiler,str);
    }

    str = NULL;
    node = eyelight_retrieve_node_prop(current, EYELIGHT_NAME_HEADER_IMAGE);
    if(!node)
    {
        str = compiler->default_header_image;
    }
    else
        str = eyelight_retrieve_value_of_prop(node,0);
    if(str)
    {
        fprintf(output,"set_header_image(\"%s\");\n",str);
        eyelight_image_add(compiler,str);
    }


    str = NULL;
    node = eyelight_retrieve_node_prop(current, EYELIGHT_NAME_TITLE);
    if(!node)
    {
        str = compiler->default_title;
    }
    else
        str = eyelight_retrieve_value_of_prop(node,0);
    if(str)
    {
        fprintf(output,"set_title(\"%s\");\n",str);
        *title = str;
    }



    str = NULL;
    node = eyelight_retrieve_node_prop(current, EYELIGHT_NAME_SUBTITLE);
    if(!node)
    {
        str = compiler->default_subtitle;
    }
    else
        str = eyelight_retrieve_value_of_prop(node,0);
    if(str)
    {
        fprintf(output,"set_subtitle(\"%s\");\n",str);
        *subtitle = str;
    }

    //custom_area
    ecore_list_first_goto(current->l);
    while( (node = ecore_list_next(current->l)))
    {
        if(node->name == EYELIGHT_NAME_CUSTOM_AREA)
        {
            fprintf(output,"custom_area(\"%s\",%s,%s,%s,%s,%d);\n",
                    eyelight_retrieve_value_of_prop(node,0),
                    eyelight_retrieve_value_of_prop(node,1),
                    eyelight_retrieve_value_of_prop(node,2),
                    eyelight_retrieve_value_of_prop(node,3),
                    eyelight_retrieve_value_of_prop(node,4),
                    compiler->display_areas);
        }
    }
}


/*
 * @brief compile a block item (text ...)
 */
void eyelight_compile_block_item(Eyelight_Compiler* compiler,FILE* output, Eyelight_Node* current, char* area, char* layout, int item_number, int number_item, int depth, char* numbering, int numbering_id)
{
    Eyelight_Node* node;
    ecore_list_first_goto(current->l);
    while((node=ecore_list_next(current->l)))
    {
        if(node->type == EYELIGHT_NODE_TYPE_PROP
                && node->name == EYELIGHT_NAME_TEXT)
        {
            if(!numbering)
                fprintf(output,"add_item_text_%s(\"%s\",%d,%d,\"%d\",%d,%d,\"%s\");\n"
                        ,layout,area,item_number-1,item_number,number_item,number_item,depth
                        ,eyelight_retrieve_value_of_prop(node,0));
            else if(strcmp(numbering,"normal")==0)
                fprintf(output,"add_item_text_%s_%s(\"%s\",%d,%d,\"%d\",%d,%d,\"%s\",%d);\n"
                        ,numbering,layout,area,item_number-1,item_number,item_number
                        ,number_item,depth
                        ,eyelight_retrieve_value_of_prop(node,0)
                        ,numbering_id);
            else //roman
            {
                char dec[EYELIGHT_BUFLEN], roman[EYELIGHT_BUFLEN];
                snprintf(dec,EYELIGHT_BUFLEN,"%d",numbering_id);
                eyelight_decimal_to_roman(dec,roman);
                fprintf(output,"add_item_text_%s_%s(\"%s\",%d,%d,\"%d\",%d,%d,\"%s\",%s);\n"
                        ,numbering,layout,area,item_number-1,item_number,item_number
                        ,number_item,depth
                        ,eyelight_retrieve_value_of_prop(node,0)
                        ,roman);
            }
        }
    }
}

/*
 * @brief compile a block image (scale, size, image)
 */
void eyelight_compile_block_image(Eyelight_Compiler* compiler,FILE* output, Eyelight_Node* current, char* area, char* layout, int item_number, int number_item)
{
    Eyelight_Node* node_image,*node_size, *node_scale, *node_border, *node_relative;
    char* border =  "0";
    ecore_list_first_goto(current->l);

    node_image = eyelight_retrieve_node_prop(current,
            EYELIGHT_NAME_IMAGE);
    node_size = eyelight_retrieve_node_prop(current,
            EYELIGHT_NAME_SIZE);
    node_scale = eyelight_retrieve_node_prop(current,
            EYELIGHT_NAME_SCALE);
    node_border = eyelight_retrieve_node_prop(current,
            EYELIGHT_NAME_BORDER);
    node_relative = eyelight_retrieve_node_prop(current,
            EYELIGHT_NAME_RELATIVE);
    if(node_border)
        border = eyelight_retrieve_value_of_prop(node_border,0);

    if(node_image)
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
    }
}

/*
 * @brief compile a block edc (file, macro)
 */
void eyelight_compile_block_edc(Eyelight_Compiler* compiler,FILE* output, Eyelight_Node* current, char* area, char* layout, int item_number, int number_item)
{
    Eyelight_Node* node_macro, *node_rel;
    ecore_list_first_goto(current->l);

    node_macro = eyelight_retrieve_node_prop(current,EYELIGHT_NAME_MACRO);
    node_rel = eyelight_retrieve_node_prop(current,EYELIGHT_NAME_RELATIVE);
    if(node_macro)
    {
        char* macro = eyelight_retrieve_value_of_prop(node_macro,0);
        if(!node_rel)
            fprintf(output,"add_edc_%s(\"%s\",%d,%d,\"%d\",%d,%s);\n",layout,area,item_number-1,
                    item_number,item_number,number_item,macro);
        else
        {
            char* rel_w = eyelight_retrieve_value_of_prop(node_rel,0);
            char* rel_h = eyelight_retrieve_value_of_prop(node_rel,1);
            fprintf(output,"add_edc_relative_%s(\"%s\",%d,%d,\"%d\",%d,%s,%s,%s);\n",layout,area,item_number-1,
                    item_number,item_number,number_item,macro,
                    rel_w,rel_h);
        }
    }
}


/*
 * @brief compile a block items (text, numbering, item, items)
 */
int eyelight_compile_block_items(Eyelight_Compiler* compiler,FILE* output, Eyelight_Node* current, char* area, char* layout, int item_number, int number_item, int depth, char* numbering, int numbering_id)
{
    Eyelight_Node* node,*node_numbering;

    node_numbering = eyelight_retrieve_node_prop(current
            ,EYELIGHT_NAME_NUMBERING);
    if(node_numbering)
        numbering = eyelight_retrieve_value_of_prop(node_numbering,0);

    if(numbering && strcmp(numbering,"none")==0)
        numbering = NULL;

    ecore_list_first_goto(current->l);
    while((node=ecore_list_next(current->l)))
    {
        switch(node->type)
        {
            case EYELIGHT_NODE_TYPE_BLOCK:
                switch(node->name)
                {
                    case EYELIGHT_NAME_ITEMS:
                        item_number=eyelight_compile_block_items(compiler,output
                            , node,area, layout, item_number,number_item
                            ,depth+1,numbering,1);
                        break;
                    case EYELIGHT_NAME_ITEM:
                        eyelight_compile_block_item(compiler,output, node, area
                            , layout,item_number, number_item,depth
                            ,numbering,numbering_id);
                        item_number++;
                        numbering_id++;
                        break;
                }
                break;
            case EYELIGHT_NODE_TYPE_PROP:
                switch(node->name)
                {
                    case EYELIGHT_NAME_TEXT:
                        if(!numbering)
                            fprintf(output,"add_item_text_%s(\"%s\",%d,%d,\"%d\",%d,%d,\"%s\");\n"
                                    ,layout,area,item_number-1,item_number,item_number,number_item,depth
                                    ,eyelight_retrieve_value_of_prop(node,0));
                        else if(strcmp(numbering,"normal")==0)
                            fprintf(output,"add_item_text_%s_%s(\"%s\",%d,%d,\"%d\",%d,%d,\"%s\",%d);\n"
                                    ,numbering,layout,area,item_number-1,item_number,item_number
                                    ,number_item,depth
                                    ,eyelight_retrieve_value_of_prop(node,0)
                                    ,numbering_id);
                        else //roman
                        {
                            char dec[EYELIGHT_BUFLEN], roman[EYELIGHT_BUFLEN];
                            snprintf(dec,EYELIGHT_BUFLEN,"%d",numbering_id);
                            eyelight_decimal_to_roman(dec,roman);
                            fprintf(output,"add_item_text_%s_%s(\"%s\",%d,%d,\"%d\",%d,%d,\"%s\",%s);\n"
                                    ,numbering,layout,area,item_number-1,item_number,item_number
                                    ,number_item,depth
                                    ,eyelight_retrieve_value_of_prop(node,0)
                                    ,roman);
                        }
                        item_number++;
                        numbering_id++;
                        break;
                }
                break;
        }
    }

    return item_number;
}

/*
 * @brief compile a block area (name, layout, items ...)
 */
void eyelight_compile_block_area(Eyelight_Compiler* compiler,FILE* output, Eyelight_Node* current)
{
    Eyelight_Node* node;
    //retrieve the name of the area
    ecore_list_first_goto(current->l);
    char* area;
    int number_item = 0;
    int item_number = 0;
    char* layout;
    char* value;

    node = eyelight_retrieve_node_prop(current, EYELIGHT_NAME_NAME);
    if(!node)
    {
        fprintf(stderr,"(line %d) An area doesn't have a name\n",compiler->line);
        exit(EXIT_FAILURE);
    }
    area = eyelight_retrieve_value_of_prop(node,0);

    node = eyelight_retrieve_node_prop(current, EYELIGHT_NAME_LAYOUT);
    if(!node)
        layout = "vbox";
    else
    {
        layout = eyelight_retrieve_value_of_prop(node,0);
        if(strcmp(layout,"horizontal")==0)
            layout = "hbox";
        else
            layout = "vbox";
    }


    number_item = eyelight_number_item_in_block(current);
    ecore_list_first_goto(current->l);
    while((node=ecore_list_next(current->l)))
    {
        switch(node->type)
        {
            case EYELIGHT_NODE_TYPE_BLOCK:
                switch(node->name)
                {
                    case EYELIGHT_NAME_ITEMS:
                        item_number=eyelight_compile_block_items(compiler,output, node,area,layout, item_number,number_item,0,NULL,1);
                        break;
                    case EYELIGHT_NAME_IMAGE:
                        eyelight_compile_block_image(compiler,output, node, area, layout, item_number, number_item);
                        item_number++;
                        break;
                    case EYELIGHT_NAME_EDC:
                        eyelight_compile_block_edc(compiler,output, node, area, layout, item_number, number_item);
                        item_number++;
                        break;
                }
                break;
            case EYELIGHT_NODE_TYPE_PROP:
                switch(node->name)
                {
                    case EYELIGHT_NAME_TEXT:
                        fprintf(output,"add_text_%s(\"%s\",%d,%d,\"%d\",%d,\"%s\");\n",layout,area,item_number-1,item_number,item_number,number_item,eyelight_retrieve_value_of_prop(node,0));
                        item_number++;
                        break;
                    case EYELIGHT_NAME_IMAGE:
                        value = eyelight_retrieve_value_of_prop(node,0);
                        fprintf(output,"add_image_%s(\"%s\",%d,%d,\"%d\",%d,\"%s\",0)\n",layout,area,item_number-1,item_number,item_number,number_item,value);
                        eyelight_image_add(compiler,value);
                        item_number++;
                        break;
                }
                break;
        }
    }
}

/*
 * @brief compile a block slide (area, title, subtitle ...)
 */
void eyelight_compile_block_slide(Eyelight_Compiler* compiler,FILE* output, Eyelight_Node* current, int slide_number)
{
    Eyelight_Node * node;
    char* title =  NULL, *subtitle = NULL;

    fprintf(output,"group\n{\n");

    fprintf(output,"name:\"slide_%d\";\n",slide_number);
    eyelight_compile_transition_slide(compiler,output, current);

    fprintf(output,"parts\n{\n");
    fprintf(output,"pre_fct();\n");

    eyelight_compile_prop_slide(compiler,output, current, &title, &subtitle);

    ecore_list_first_goto(current->l);
    while( (node = ecore_list_next(current->l)) )
    {
        if(node->type == EYELIGHT_NODE_TYPE_BLOCK)
        {
            if(node->name==EYELIGHT_NAME_AREA)
            {
                eyelight_compile_block_area(compiler,output, node);
            }
        }
    }
    fprintf(output,"post_fct();\n");
    fprintf(output,"}\n");
    //add the data with the title and the subtitle
    //these date will be used to create the table of contents

    fprintf(output,"data\n{\n");
    if(title)
        fprintf(output,"item: title \"%s\";\n",title);
    if(subtitle)
        fprintf(output,"item: subtitle \"%s\";\n",subtitle);
    fprintf(output,"}\n");

    fprintf(output,"program_list();\n");
    fprintf(output,"}\n");
}

/*
 * browse in the tree and save all edc file we will need to include
 */
void eyelight_edc_file_list_load(Eyelight_Compiler* compiler, Eyelight_Node* current)
{
    Eyelight_Node* node;
    ecore_list_first_goto(current->l);
    while( (node=ecore_list_next(current->l)))
    {
        if(node->type==EYELIGHT_NODE_TYPE_BLOCK)
            eyelight_edc_file_list_load(compiler,node);
        else if(node->type==EYELIGHT_NODE_TYPE_PROP
                && node->name == EYELIGHT_NAME_FILE
                && node->father->name == EYELIGHT_NAME_EDC)
            eyelight_edc_file_add(compiler,eyelight_retrieve_value_of_prop(node,0));
    }
}

/*
 * @brief main compile function
 */
void eyelight_compile(Eyelight_Compiler* compiler,FILE* output)
{
    Eyelight_Node* node;
    int slide_number = 1;
    char* image,*edc_file;

    //first we list all the edc file we want include
    eyelight_edc_file_list_load(compiler,compiler->root);
    ecore_list_first_goto(compiler->edc_files);
    while ((edc_file = ecore_list_next(compiler->edc_files)))
        fprintf(output,"#include \"%s\"\n\n",edc_file);

    fprintf(output,"collections\n{\n");
    fprintf(output, "expose();\n");
    fprintf(output, "gotoslide();\n");
    fprintf(output, "slideshow();\n");
    fprintf(output, "tableofcontents();\n\n");

    ecore_list_first_goto(compiler->root->l);
    while( (node = ecore_list_next(compiler->root->l)) )
    {
        switch(node->type)
        {
            case EYELIGHT_NODE_TYPE_PROP:
                switch(node->name)
                {
                    case EYELIGHT_NAME_FOOT_TEXT:
                        EYELIGHT_FREE(compiler->default_foot_text);
                        compiler->default_foot_text =
                            strdup(eyelight_retrieve_value_of_prop(node,0));
                        break;
                    case EYELIGHT_NAME_FOOT_IMAGE:
                        EYELIGHT_FREE(compiler->default_foot_image);
                        compiler->default_foot_image =
                            strdup(eyelight_retrieve_value_of_prop(node,0));
                        break;
                    case EYELIGHT_NAME_LAYOUT:
                        EYELIGHT_FREE(compiler->default_layout);
                        compiler->default_layout =
                            strdup(eyelight_retrieve_value_of_prop(node,0));
                        break;
                    case EYELIGHT_NAME_HEADER_IMAGE:
                        EYELIGHT_FREE(compiler->default_header_image);
                        compiler->default_header_image=
                            strdup(eyelight_retrieve_value_of_prop(node,0));
                        break;
                    case EYELIGHT_NAME_TITLE:
                        EYELIGHT_FREE(compiler->default_title);
                        compiler->default_title=
                            strdup(eyelight_retrieve_value_of_prop(node,0));
                        break;
                    case EYELIGHT_NAME_SUBTITLE:
                        EYELIGHT_FREE(compiler->default_subtitle);
                        compiler->default_subtitle=
                            strdup(eyelight_retrieve_value_of_prop(node,0));
                        break;
                    case EYELIGHT_NAME_TRANSITION:
                        EYELIGHT_FREE(compiler->default_transition);
                        compiler->default_transition =
                            strdup(eyelight_retrieve_value_of_prop(node,0));
                        break;
                    case EYELIGHT_NAME_TRANSITION_NEXT:
                        EYELIGHT_FREE(compiler->default_transition_next);
                        compiler->default_transition_next =
                            strdup(eyelight_retrieve_value_of_prop(node,0));
                        break;
                    case EYELIGHT_NAME_TRANSITION_PREVIOUS:
                        EYELIGHT_FREE(compiler->default_transition_previous);
                        compiler->default_transition_previous =
                            strdup(eyelight_retrieve_value_of_prop(node,0));
                        break;
                }
                break;
            case EYELIGHT_NODE_TYPE_BLOCK:
                switch(node->name)
                {
                    case EYELIGHT_NAME_SLIDE:
                        eyelight_compile_block_slide(compiler,output
                                , node,slide_number);
                        slide_number++;
                        break;
                }
                break;
        }
    }
    fprintf(output,"}\n");

    fprintf(output,"images\n{\n");
    ecore_list_first_goto(compiler->image_list);
    while( (image = ecore_list_next(compiler->image_list)) )
    {
        fprintf(output,"image: \"%s\" COMP;\n",image);
    }
    fprintf(output,"}\n");
}


