
#include "Eyelight_Edit.h"
#include "eyelight_compiler_parser.h"

static Eyelight_Node *_get_area_from_area(Eyelight_Edit *edit);

const char* eyelight_edit_slide_title_get(Eyelight_Viewer *pres, int id_slide)
{
    const char *title = NULL;
    Eyelight_Node *node_slide, *node;
    Eina_List *l;
    Eyelight_Compiler *compiler = pres->compiler;
    int nb_slides = eyelight_nb_slides_get(compiler);

    //retrieve the default title of the slide
    //it is the title define outside a slide block
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
                    case EYELIGHT_NAME_TITLE:
                        title = eyelight_retrieve_value_of_prop(node,0);
                        break;
                    default: ;
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

    //retrieve the title defines in the slide
    Eyelight_Node *node_title = eyelight_retrieve_node_prop(node, EYELIGHT_NAME_TITLE);
    if(node_title)
        title = eyelight_retrieve_value_of_prop(node_title,0);

    return title;
}

void eyelight_edit_slide_insert(Eyelight_Viewer *pres, int after)
{
    Eyelight_Slide *slide = calloc(1,sizeof(Eyelight_Slide));

    if(after<0)
        pres->slides = eina_list_prepend(pres->slides, slide);
    else if(after>=pres->size)
        pres->slides = eina_list_append(pres->slides, slide);
    else
    {
        Eyelight_Slide *slide_prev = eina_list_nth(pres->slides, after);
        pres->slides = eina_list_append_relative(pres->slides, slide, slide_prev);
    }

    //now we create the node of this slide
    //we don't add a title, foot ..., by default we use the defaults values.
    Eyelight_Node *node_slide = calloc(1, sizeof(Eyelight_Node));
    node_slide->type = EYELIGHT_NODE_TYPE_BLOCK;
    node_slide->name = EYELIGHT_NAME_SLIDE;
    //add the default layout "1_area"
    Eyelight_Node *node_layout = calloc(1, sizeof(Eyelight_Node));
    node_layout->type = EYELIGHT_NODE_TYPE_PROP;
    node_layout->name = EYELIGHT_NAME_LAYOUT;
    node_slide->l = eina_list_append(node_slide->l, node_layout);
    Eyelight_Node *node_layout_value = calloc(1,sizeof(Eyelight_Node));
    node_layout_value->type = EYELIGHT_NODE_TYPE_VALUE;
    node_layout_value->value = strdup("1_area");
    node_layout->l = eina_list_append(node_layout->l, node_layout_value);

    //insert the node
    if(after<0)
    {
        pres->compiler->root->l = eina_list_prepend(pres->compiler->root->l, node_slide);
    }
    else
    {
        Eina_List *l;
        Eyelight_Node *node;
        int i=0;
        EINA_LIST_FOREACH(pres->compiler->root->l, l, node)
        {
            if(node->type == EYELIGHT_NODE_TYPE_BLOCK
                    && node->name == EYELIGHT_NAME_SLIDE)
            {
                if(i==after)
                    break;
                else
                    i++;
            }
        }
        pres->compiler->root->l = eina_list_append_relative_list(pres->compiler->root->l, node_slide, l);
    }

    pres->size++;

    //reload all slides (the number of slides which is displayed on each slide has changed)
    int i = 0;
    Eina_List *l;
    EINA_LIST_FOREACH(pres->slides, l, slide)
    {
        if(slide->obj)
        {
            evas_object_del(slide->obj);
            slide->obj = NULL;
            slide->obj = eyelight_viewer_slide_get(pres, slide, i);
        }
        i++;
    }

    //go to the new slide
    if(pres->current>after)
        pres->current++;

    eyelight_viewer_slide_goto(pres, after + 1);

    //create the thumbnail
    if(pres->thumbnails.is_background_load)
    {
        eyelight_viewer_thumbnails_background_load_stop(pres);
        eyelight_viewer_thumbnails_background_load_start(pres);
    }
}

void eyelight_edit_slide_delete(Eyelight_Viewer *pres, int id_slide)
{
    Eyelight_Slide *slide = eina_list_nth(pres->slides, id_slide);
    if(!slide) return;

    //free the slide
    pres->slides = eina_list_remove(pres->slides,slide);
    eyelight_slide_clean(slide);
    EYELIGHT_FREE(slide);

    //delete the slide from the tree
    Eina_List *l;
    Eyelight_Node *node;
    int i=0;
    EINA_LIST_FOREACH(pres->compiler->root->l, l, node)
    {
        if(node->type == EYELIGHT_NODE_TYPE_BLOCK
                && node->name == EYELIGHT_NAME_SLIDE)
        {
            if(i==id_slide)
                break;
            else
                i++;
        }
    }
    pres->compiler->root->l = eina_list_remove_list(pres->compiler->root->l, l);
    //all summary are created from the same node, so we must not delete him
    if(node != pres->compiler->node_summary)
        eyelight_node_free(&node, NULL);


    pres->size--;
    if(pres->current == id_slide)
    {
        pres->current = 0;
        eyelight_viewer_slide_goto(pres, 0);
    }

    //reload all slides (the number of slides which is displayed on each slide has changed)
    i = 0;
    EINA_LIST_FOREACH(pres->slides, l, slide)
    {
        if(slide->obj)
        {
            evas_object_del(slide->obj);
            slide->obj = NULL;
            slide->obj = eyelight_viewer_slide_get(pres, slide, i);
        }
        i++;
    }
}


void eyelight_edit_slide_move(Eyelight_Viewer *pres, int id_slide, int id_after)
{
    Eyelight_Slide *slide = eina_list_nth(pres->slides, id_slide);
    Eyelight_Slide *slide_relative = eina_list_nth(pres->slides, id_after);

    if(!slide) return;

    //move the slide
    pres->slides = eina_list_remove(pres->slides,slide);
    if(slide_relative)
        pres->slides = eina_list_append_relative(pres->slides, slide, slide_relative);
    else
        pres->slides = eina_list_prepend(pres->slides, slide);

    //move the slide in the tree
    Eina_List *l;
    Eyelight_Node *node;
    int i=0;
    Eyelight_Node *node_relative = NULL;
    Eyelight_Node *node_slide = NULL;
    Eina_List *l_slide = NULL;
    Eina_List *l_relative;
    EINA_LIST_FOREACH(pres->compiler->root->l, l, node)
    {
        if(node->type == EYELIGHT_NODE_TYPE_BLOCK
                && node->name == EYELIGHT_NAME_SLIDE)
        {
            if(i==id_slide)
            {
                node_slide = node;
                l_slide = l;
            }
            else if(i==id_after)
            {
                node_relative = node;
                l_relative = l;
            }
            i++;

            if(node_slide && (node_relative || id_after == -1))
                break;
        }
    }
    pres->compiler->root->l = eina_list_remove_list(pres->compiler->root->l, l_slide);
    if(node_relative)
        pres->compiler->root->l = eina_list_append_relative_list(pres->compiler->root->l, node_slide, l_relative);
    else
        pres->compiler->root->l = eina_list_prepend(pres->compiler->root->l, node_slide);

    //reload all slides the position of a slide could change
    i = 0;
    EINA_LIST_FOREACH(pres->slides, l, slide)
    {
        if(slide->obj)
        {
            evas_object_del(slide->obj);
            slide->obj = NULL;
            slide->obj = eyelight_viewer_slide_get(pres, slide, i);
        }
        i++;
    }
    eyelight_viewer_slide_goto(pres, id_after + 1);
}

void eyelight_edit_edit_mode_set(Eyelight_Viewer *pres, Edje_Signal_Cb cb, void *data)
{
    pres->edit_mode = 1;
    pres->edit_cb = cb;
    pres->edit_data = cb;
}

void eyelight_edit_obj_unselect(Eyelight_Edit *edit)
{
    edje_object_signal_emit(edit->obj, "unselect", "eyelight");
}

Eyelight_Node_Name eyelight_edit_name_get(Eyelight_Edit *edit)
{
    return edit->node->name;
}


void eyelight_edit_area_geometry_get(Eyelight_Edit *edit, double *rel1_x, double *rel1_y, double *rel2_x, double *rel2_y)
{
    *rel1_x = atof(eyelight_retrieve_value_of_prop(edit->node,1));
    *rel1_y = atof(eyelight_retrieve_value_of_prop(edit->node,2));
    *rel2_x = atof(eyelight_retrieve_value_of_prop(edit->node,3));
    *rel2_y = atof(eyelight_retrieve_value_of_prop(edit->node,4));
}

void eyelight_edit_area_move(Eyelight_Edit *edit, double rel1_x, double rel1_y, double rel2_x, double rel2_y)
{
    char buf[EYELIGHT_BUFLEN];
    Eyelight_Node *data;

    snprintf(buf,EYELIGHT_BUFLEN,"%f",rel1_x);
    data = eina_list_nth(edit->node->l,1);
    EYELIGHT_FREE(data -> value);
    data -> value = strdup(buf);

    snprintf(buf,EYELIGHT_BUFLEN,"%f",rel1_y);
    data = eina_list_nth(edit->node->l,2);
    EYELIGHT_FREE(data -> value);
    data -> value = strdup(buf);

    snprintf(buf,EYELIGHT_BUFLEN,"%f",rel2_x);
    data = eina_list_nth(edit->node->l,3);
    EYELIGHT_FREE(data -> value);
    data -> value = strdup(buf);

    snprintf(buf,EYELIGHT_BUFLEN,"%f",rel2_y);
    data = eina_list_nth(edit->node->l,4);
    EYELIGHT_FREE(data -> value);
    data -> value = strdup(buf);

    //set the size
    Edje_Message_Float_Set *msg = alloca(sizeof(Edje_Message_Float_Set) + (3 * sizeof(float)));
    msg->count=4;
    msg->val[0] = rel1_x;
    msg->val[1] = rel1_y;
    msg->val[2] = rel2_x;
    msg->val[3] = rel2_y;
    edje_object_message_send(edit->obj,EDJE_MESSAGE_FLOAT_SET , 0, msg);
}

void eyelight_edit_area_up(Eyelight_Edit *edit)
{
    Eina_List *l;
    Eyelight_Node *next_next = NULL;
    Eyelight_Node *node;
    int find_current = 0;

    Eyelight_Node *father = edit->node->father;

    //find the next area
    EINA_LIST_FOREACH(father->l, l, node)
    {
        if(node == edit->node)
            find_current = 1;
        else if(find_current==1 && node->name == EYELIGHT_NAME_CUSTOM_AREA)
            find_current = 2;
        else if(find_current==2 && node->name == EYELIGHT_NAME_CUSTOM_AREA)
        {
            next_next = node;
            break;
        }
    }

    if(!next_next)
    {
        father->l = eina_list_remove(father->l, edit->node);
        father->l = eina_list_append(father->l, edit->node);

        edje_object_part_box_remove(edit->slide->obj, "area.custom", edit->obj);
        edje_object_part_box_append(edit->slide->obj, "area.custom", edit->obj);
    }
    else
    {
        father->l = eina_list_remove(father->l, edit->node);
        father->l = eina_list_prepend_relative(father->l, edit->node, next_next);

        edje_object_part_box_remove(edit->slide->obj, "area.custom", edit->obj);
        edje_object_part_box_insert_before(edit->slide->obj, "area.custom", edit->obj, next_next->obj);
    }
}

void eyelight_edit_area_down(Eyelight_Edit *edit)
{
    Eina_List *l;
    Eyelight_Node *prev = NULL;
    Eyelight_Node *node;

    Eyelight_Node *father = edit->node->father;

    //find the previous area
    EINA_LIST_FOREACH(father->l, l, node)
    {
        if(node == edit->node)
            break;
        else if(node->name == EYELIGHT_NAME_CUSTOM_AREA)
            prev = node;
    }

    if(!prev)
        return ;

    father->l = eina_list_remove(father->l, edit->node);
    father->l = eina_list_prepend_relative(father->l, edit->node, prev);

    edje_object_part_box_remove(edit->slide->obj, "area.custom", edit->obj);
    edje_object_part_box_insert_before(edit->slide->obj, "area.custom", edit->obj, prev->obj);
}

void eyelight_edit_area_add(Eyelight_Viewer *pres, int id_slide)
{
    char buf[EYELIGHT_BUFLEN];
    int id_name = 0;
    char *name = NULL;
    Eina_List *l;
    Eyelight_Custom_Area *area;

    if(id_slide<0 || id_slide>=pres->size)
        return;


    //search the slide
    Eyelight_Slide *slide = eina_list_nth(pres->slides, id_slide);

    //create the name of the area
    while(!name)
    {
        snprintf(buf, EYELIGHT_BUFLEN, "custom_area_%d", id_name);
        name = buf;
        EINA_LIST_FOREACH(slide->custom_areas, l, area)
        {
            if(strcmp(area->name, buf) == 0)
            {
                id_name++;
                name = NULL;
                break;
            }
        }
    }

    //search the node of the slide
    Eyelight_Node *node = NULL, *_node;
    int id = 0;
    EINA_LIST_FOREACH(pres->compiler->root->l, l, _node)
    {
        if(id == id_slide && _node->name == EYELIGHT_NAME_SLIDE)
        {
            node = _node;
            break;
        }
        else if(_node->name == EYELIGHT_NAME_SLIDE)
            id++;
    }
    if(!node) return;

    //create the node wich describe the custom area
    Eyelight_Node *node_custom_area = eyelight_node_new(EYELIGHT_NODE_TYPE_PROP, EYELIGHT_NAME_CUSTOM_AREA, node);
    //name
    Eyelight_Node* node_custom_area_value = eyelight_node_new(EYELIGHT_NODE_TYPE_VALUE, EYELIGHT_NAME_NONE, node_custom_area);
    node_custom_area_value->value = strdup(buf);

    //rel1_x
    node_custom_area_value = eyelight_node_new(EYELIGHT_NODE_TYPE_VALUE, EYELIGHT_NAME_NONE, node_custom_area);
    node_custom_area_value->value = strdup("0.3");

    //rel1_y
    node_custom_area_value = eyelight_node_new(EYELIGHT_NODE_TYPE_VALUE, EYELIGHT_NAME_NONE, node_custom_area);
    node_custom_area_value->value = strdup("0.3");

    //rel2_x
    node_custom_area_value = eyelight_node_new(EYELIGHT_NODE_TYPE_VALUE, EYELIGHT_NAME_NONE, node_custom_area);
    node_custom_area_value->value = strdup("0.7");

    //rel2_y
    node_custom_area_value = eyelight_node_new(EYELIGHT_NODE_TYPE_VALUE, EYELIGHT_NAME_NONE, node_custom_area);
    node_custom_area_value->value = strdup("0.7");

    eyelight_object_custom_area_add(pres, slide, id_slide, node_custom_area, buf, 0.3,0.3,0.7,0.7);



    //create the node of the new area
    Eyelight_Node *node_area = eyelight_node_new(EYELIGHT_NODE_TYPE_BLOCK, EYELIGHT_NAME_AREA, node);
    //add the name
    Eyelight_Node *node_name = eyelight_node_new(EYELIGHT_NODE_TYPE_PROP, EYELIGHT_NAME_NAME, node_area);
    Eyelight_Node *node_name_value = eyelight_node_new(EYELIGHT_NODE_TYPE_BLOCK, EYELIGHT_NAME_NONE, node_name);
    node_name_value->value = strdup(buf);

    eyelight_compile_block_area(pres, slide, id_slide, node_area, -1);

    edje_object_signal_emit(node_custom_area->obj, "select", "eyelight");
}


void eyelight_edit_area_delete(Eyelight_Viewer *pres, Eyelight_Edit *edit)
{
    Eyelight_Node *father = edit->node->father;
    char *name = eyelight_retrieve_value_of_prop(edit->node,0);
    Eyelight_Custom_Area *area;
    Eyelight_Node *node;
    Eina_List *l;

    evas_object_del(edit->obj);

    EINA_LIST_FOREACH(edit->slide->custom_areas, l, area)
    {
        if(strcmp(area->name, name) == 0)
        {
            edit->slide->custom_areas = eina_list_remove(edit->slide->custom_areas, area);
            free(area->name);
            free(area);
            break;
        }
    }

    //remove the area
    EINA_LIST_FOREACH(father->l, l, node)
    {
        if(node->name == EYELIGHT_NAME_AREA)
        {
            Eyelight_Node *node_name = eyelight_retrieve_node_prop(node, EYELIGHT_NAME_NAME);
            char *_name = eyelight_retrieve_value_of_prop(node_name, 0);

            if(strcmp(name, _name) == 0)
            {
                father->l = eina_list_remove(father->l, node);
                eyelight_node_free(&node, NULL);
            }
        }
    }

    //remove the custom area
    father->l = eina_list_remove(father->l, edit->node);
    eyelight_node_free(&(edit->node), NULL);

}


void eyelight_edit_area_image_add(Eyelight_Viewer *pres, Eyelight_Edit *edit, const char* image)
{
    char *area_name = eyelight_retrieve_value_of_prop(edit->node,0);

    //id_slide
    Eina_List *l;
    Eyelight_Slide *slide;
    int i = 0;
    EINA_LIST_FOREACH(pres->slides, l, slide)
    {
        if(slide == edit->slide)
            break;
        else
            i++;
    }
    //create the node
    Eyelight_Node *node = eyelight_node_new(EYELIGHT_NODE_TYPE_BLOCK, EYELIGHT_NAME_IMAGE,
            _get_area_from_area(edit));
    Eyelight_Node *node_image = eyelight_node_new(EYELIGHT_NODE_TYPE_PROP, EYELIGHT_NAME_IMAGE, node);
    Eyelight_Node *node_value = eyelight_node_new(EYELIGHT_NODE_TYPE_VALUE, EYELIGHT_NAME_NAME, node_image);
    node_value->value = strdup(image);

    eyelight_object_item_image_add(pres,edit->slide, i, node, area_name, image, 0, 0);

}

void eyelight_edit_image_properties_get(Eyelight_Edit *edit, char **file, int *border, int *shadow)
{
    Eyelight_Node *node;

    if(edit->node->type == EYELIGHT_NODE_TYPE_PROP)
    {
        *file = eyelight_retrieve_value_of_prop(edit->node, 0);
        *border = 0;
        *shadow = 0;
        return ;
    }

    if(file)
    {
        node = eyelight_retrieve_node_prop(edit->node, EYELIGHT_NAME_IMAGE);
        if(node)
            *file = eyelight_retrieve_value_of_prop(node, 0);
        else
            *file = NULL;
    }

    if(border)
    {
        node = eyelight_retrieve_node_prop(edit->node, EYELIGHT_NAME_BORDER);
        if(node)
        {
            char *s = eyelight_retrieve_value_of_prop(node, 0);
            *border = atof(s);
        }
        else
            *border = 0;
    }


    if(shadow)
    {
        node = eyelight_retrieve_node_prop(edit->node, EYELIGHT_NAME_SHADOW);
        if(node)
        {
            char *s = eyelight_retrieve_value_of_prop(node, 0);
            *shadow = atof(s);
        }
        else
            *shadow = 0;
    }
}

void eyelight_edit_image_properties_set(Eyelight_Viewer *pres, Eyelight_Edit *edit, char *file, int border, int shadow)
{
    Eyelight_Node *node;
    char buf[EYELIGHT_BUFLEN];

    if(edit->node->type == EYELIGHT_NODE_TYPE_PROP)
    {
        //change the prop into a block
        char *_file = strdup(eyelight_retrieve_value_of_prop(edit->node, 0));

        edit->node->type = EYELIGHT_NODE_TYPE_BLOCK;
        Eyelight_Node *node_image_value = eina_list_nth(edit->node->l, 0);
        edit->node->l = eina_list_remove(edit->node->l, node_image_value);
        eyelight_node_free( &node_image_value, NULL );

        Eyelight_Node *node_image = eyelight_node_new(EYELIGHT_NODE_TYPE_PROP,
                EYELIGHT_NAME_IMAGE, edit->node);
        Eyelight_Node *node_value = eyelight_node_new(EYELIGHT_NODE_TYPE_VALUE,
                EYELIGHT_NAME_NAME, node_image);
        node_value->value = _file;
    }

    if(file)
    {
        node = eyelight_retrieve_node_prop(edit->node, EYELIGHT_NAME_IMAGE);
        if(node)
        {
            node = eina_list_nth(node->l, 0);
            EYELIGHT_FREE(node->value);
            node->value = strdup(file);
        }
        else
        {
            node = eyelight_node_new(EYELIGHT_NODE_TYPE_PROP, EYELIGHT_NAME_IMAGE, edit->node);
            node = eyelight_node_new(EYELIGHT_NODE_TYPE_VALUE, EYELIGHT_NAME_NONE, node);
            node->value = strdup(file);
        }
        const Evas_Object *part_image = edje_object_part_object_get(edit->obj, "object.image");
        char *image_path = eyelight_compile_image_path_new(pres,file);
        evas_object_image_file_set((Evas_Object*)part_image, image_path, NULL);
        EYELIGHT_FREE(image_path);
    }

    if(border > -1)
    {
        snprintf(buf, EYELIGHT_BUFLEN, "%d",border);
        node = eyelight_retrieve_node_prop(edit->node, EYELIGHT_NAME_BORDER);
        if(node)
        {
            node = eina_list_nth(node->l, 0);
            EYELIGHT_FREE(node->value);
            node->value = strdup(buf);
        }
        else
        {
            node = eyelight_node_new(EYELIGHT_NODE_TYPE_PROP, EYELIGHT_NAME_BORDER, edit->node);
            node = eyelight_node_new(EYELIGHT_NODE_TYPE_VALUE, EYELIGHT_NAME_NONE, node);
            node->value = strdup(buf);
        }
        if(border)
            edje_object_signal_emit(edit->obj, "border,show","eyelight");
        else
            edje_object_signal_emit(edit->obj, "border,hide","eyelight");
    }

    if(shadow > -1)
    {
        snprintf(buf, EYELIGHT_BUFLEN, "%d",shadow);
        node = eyelight_retrieve_node_prop(edit->node, EYELIGHT_NAME_SHADOW);
        if(node)
        {
            node = eina_list_nth(node->l, 0);
            EYELIGHT_FREE(node->value);
            node->value = strdup(buf);
        }
        else
        {
            node = eyelight_node_new(EYELIGHT_NODE_TYPE_PROP, EYELIGHT_NAME_SHADOW, edit->node);
            node = eyelight_node_new(EYELIGHT_NODE_TYPE_VALUE, EYELIGHT_NAME_NONE, node);
            node->value = strdup(buf);
        }
        if(shadow)
            edje_object_signal_emit(edit->obj, "shadow,show","eyelight");
        else
            edje_object_signal_emit(edit->obj, "shadow,hide","eyelight");

    }
}


static Eyelight_Node *_get_area_from_area(Eyelight_Edit *edit)
{
    Eyelight_Node *parent = edit->node->father;
    Eina_List *l;
    Eyelight_Node *node;
    char *area_name = eyelight_retrieve_value_of_prop(edit->node,0);

    EINA_LIST_FOREACH(parent->l, l, node)
    {
        if(node->name == EYELIGHT_NAME_AREA)
        {
            Eyelight_Node *node_name = eyelight_retrieve_node_prop(node, EYELIGHT_NAME_NAME);
            char *name = eyelight_retrieve_value_of_prop(node_name,0);
            if(name && strcmp(name, area_name) == 0)
                return node;
        }
    }
    return NULL;
}


