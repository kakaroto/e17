#ifdef HAVE_CONFIG_H
# include <config.h>
#endif

#ifdef HAVE_ALLOCA_H
# include <alloca.h>
#elif defined __GNUC__
# define alloca __builtin_alloca
#elif defined _AIX
# define alloca __alloca
#elif defined _MSC_VER
# include <malloc.h>
# define alloca _alloca
#else
# include <stddef.h>
# ifdef  __cplusplus
extern "C"
# endif
void *alloca (size_t);
#endif

#include "Eyelight_Edit.h"
#include "edit.h"

void eyelight_edit_area_geometry_get(Eyelight_Edit *edit, double *rel1_x, double *rel1_y, double *rel2_x, double *rel2_y)
{
    EYELIGHT_ASSERT_RETURN_VOID(!!edit);
    EYELIGHT_ASSERT_RETURN_VOID(edit->node->name == EYELIGHT_NAME_CUSTOM_AREA
            || edit->node->name == EYELIGHT_NAME_THEME_AREA);

    *rel1_x = atof(eyelight_retrieve_value_of_prop(edit->node,1));
    *rel1_y = atof(eyelight_retrieve_value_of_prop(edit->node,2));
    *rel2_x = atof(eyelight_retrieve_value_of_prop(edit->node,3));
    *rel2_y = atof(eyelight_retrieve_value_of_prop(edit->node,4));
}

const char* eyelight_edit_area_layout_get(Eyelight_Edit *edit)
{
    Eyelight_Node *node_layout, *node_area;

    EYELIGHT_ASSERT_RETURN(!!edit);
    EYELIGHT_ASSERT_RETURN(edit->node->name == EYELIGHT_NAME_CUSTOM_AREA
            || edit->node->name == EYELIGHT_NAME_THEME_AREA);

    node_area = _get_area_from_area(edit->slide, edit->node);
    if(!node_area)
        return "vertical";
    node_layout = eyelight_retrieve_node_prop(node_area, EYELIGHT_NAME_LAYOUT);
    if(!node_layout)
        return "vertical";

    return eyelight_retrieve_value_of_prop(node_layout,0);
}

void eyelight_edit_area_layout_set(Eyelight_Edit *edit, const char *layout)
{
    Eyelight_Node *node_layout, *node_layout_value, *node_area;
    Eyelight_Area *e_area;
    char buf[EYELIGHT_BUFLEN];

    EYELIGHT_ASSERT_RETURN_VOID(!!edit);
    EYELIGHT_ASSERT_RETURN_VOID(edit->node->name == EYELIGHT_NAME_CUSTOM_AREA
            || edit->node->name == EYELIGHT_NAME_THEME_AREA);
    EYELIGHT_ASSERT_RETURN_VOID(!!layout);

    node_area = _get_area_from_area(edit->slide, edit->node);
    if(!node_area) return;
    node_layout = eyelight_retrieve_node_prop(node_area, EYELIGHT_NAME_LAYOUT);
    if(!node_layout)
    {
        node_layout = eyelight_node_new(EYELIGHT_NODE_TYPE_PROP, EYELIGHT_NAME_LAYOUT, node_area);
        node_layout_value = eyelight_node_new(EYELIGHT_NODE_TYPE_VALUE, EYELIGHT_NAME_NONE, node_layout);
        node_layout_value->value = strdup(layout);
    }
    else
    {
        Eyelight_Node *node_layout_value = eina_list_data_get(node_layout->l);
        EYELIGHT_FREE(node_layout_value->value);
        node_layout_value->value = strdup(layout);
    }

    e_area = eyelight_retrieve_area_from_node(edit->slide, edit->node);
    if(!e_area) return;
    snprintf(buf,EYELIGHT_BUFLEN, "area,custom,layout,%s", layout);
    edje_object_signal_emit(e_area->obj, buf, "eyelight");
}

void eyelight_edit_area_move(Eyelight_Edit *edit, double rel1_x, double rel1_y, double rel2_x, double rel2_y)
{
    char buf[EYELIGHT_BUFLEN];
    Eyelight_Node *data;

    EYELIGHT_ASSERT_RETURN_VOID(!!edit);
    EYELIGHT_ASSERT_RETURN_VOID(edit->node->name == EYELIGHT_NAME_CUSTOM_AREA
            || edit->node->name == EYELIGHT_NAME_THEME_AREA);

    Eyelight_Area *e_area = eyelight_retrieve_area_from_node(edit->slide, edit->node);
    edit->node->name = EYELIGHT_NAME_CUSTOM_AREA;


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

    EYELIGHT_ASSERT_RETURN_VOID(!!edit);
    EYELIGHT_ASSERT_RETURN_VOID(edit->node->name == EYELIGHT_NAME_CUSTOM_AREA
            || edit->node->name == EYELIGHT_NAME_THEME_AREA);

    Eyelight_Area *e_area = eyelight_retrieve_area_from_node(edit->slide,edit->node);
    edit->node->name = EYELIGHT_NAME_CUSTOM_AREA;


    Eyelight_Node *father = edit->node->father;

    //find the next area
    EINA_LIST_FOREACH(father->l, l, node)
    {
        if(node == edit->node)
            find_current = 1;
        else if(find_current==1
                && (node->name == EYELIGHT_NAME_CUSTOM_AREA
                    || node->name == EYELIGHT_NAME_THEME_AREA) )
            find_current = 2;
        else if(find_current==2
                && (node->name == EYELIGHT_NAME_CUSTOM_AREA
                    || node->name == EYELIGHT_NAME_THEME_AREA))
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

    EYELIGHT_ASSERT_RETURN_VOID(!!edit);
    EYELIGHT_ASSERT_RETURN_VOID(edit->node->name == EYELIGHT_NAME_CUSTOM_AREA
            || edit->node->name == EYELIGHT_NAME_THEME_AREA);

    Eyelight_Area *e_area = eyelight_retrieve_area_from_node(edit->slide,edit->node);
    edit->node->name = EYELIGHT_NAME_CUSTOM_AREA;

    Eyelight_Node *father = edit->node->father;

    //find the previous area
    EINA_LIST_FOREACH(father->l, l, node)
    {
        if(node == edit->node)
            break;
        else if(node->name == EYELIGHT_NAME_CUSTOM_AREA
                || node->name == EYELIGHT_NAME_THEME_AREA)
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
    Eyelight_Area *area;

    EYELIGHT_ASSERT_RETURN_VOID(!!pres);
    EYELIGHT_ASSERT_RETURN_VOID(id_slide>=0);
    EYELIGHT_ASSERT_RETURN_VOID(id_slide<eyelight_viewer_size_get(pres));

    //search the slide
    Eyelight_Slide *slide = eina_list_nth(pres->slides, id_slide);

    //create the name of the area
    while(!name)
    {
        snprintf(buf, EYELIGHT_BUFLEN, "custom_area_%d", id_name);
        name = buf;
        EINA_LIST_FOREACH(slide->areas, l, area)
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
    Eyelight_Node *node_area = eyelight_node_new(EYELIGHT_NODE_TYPE_PROP, EYELIGHT_NAME_CUSTOM_AREA, node);
    //name
    Eyelight_Node* node_area_value = eyelight_node_new(EYELIGHT_NODE_TYPE_VALUE, EYELIGHT_NAME_NONE, node_area);
    node_area_value->value = strdup(buf);

    //rel1_x
    node_area_value = eyelight_node_new(EYELIGHT_NODE_TYPE_VALUE, EYELIGHT_NAME_NONE, node_area);
    node_area_value->value = strdup("0.3");

    //rel1_y
    node_area_value = eyelight_node_new(EYELIGHT_NODE_TYPE_VALUE, EYELIGHT_NAME_NONE, node_area);
    node_area_value->value = strdup("0.3");

    //rel2_x
    node_area_value = eyelight_node_new(EYELIGHT_NODE_TYPE_VALUE, EYELIGHT_NAME_NONE, node_area);
    node_area_value->value = strdup("0.7");

    //rel2_y
    node_area_value = eyelight_node_new(EYELIGHT_NODE_TYPE_VALUE, EYELIGHT_NAME_NONE, node_area);
    node_area_value->value = strdup("0.7");

    eyelight_object_custom_area_add(pres, slide, node_area, buf, 0.3,0.3,0.7,0.7);



    //create the node of the new area
    node_area = eyelight_node_new(EYELIGHT_NODE_TYPE_BLOCK, EYELIGHT_NAME_AREA, node);
    //add the name
    Eyelight_Node *node_name = eyelight_node_new(EYELIGHT_NODE_TYPE_PROP, EYELIGHT_NAME_NAME, node_area);
    Eyelight_Node *node_name_value = eyelight_node_new(EYELIGHT_NODE_TYPE_VALUE, EYELIGHT_NAME_NONE, node_name);
    node_name_value->value = strdup(buf);

    eyelight_compile_block_area(pres, slide, node_area, -1);

    edje_object_signal_emit(node_area->obj, "select", "eyelight");
}

void eyelight_edit_area_delete(Eyelight_Edit *edit)
{
    Eyelight_Node *father = edit->node->father;
    Eyelight_Area *area;
    Eyelight_Node *node;
    Eina_List *l;

    EYELIGHT_ASSERT_RETURN_VOID(!!edit);
    EYELIGHT_ASSERT_RETURN_VOID(edit->node->name == EYELIGHT_NAME_CUSTOM_AREA
            || edit->node->name == EYELIGHT_NAME_THEME_AREA);

    char *name = eyelight_retrieve_value_of_prop(edit->node,0);
    EYELIGHT_ASSERT_RETURN_VOID(!!name);

    evas_object_del(edit->obj);

    if(edit->node->name == EYELIGHT_NAME_THEME_AREA)
    {
        //a theme area can not be deleted as the area is defined in the theme
        //we mark it as ignored
        Eyelight_Node *node_ignore = eyelight_node_new(EYELIGHT_NODE_TYPE_PROP, EYELIGHT_NAME_IGNORE_AREA, edit->slide->node);
        Eyelight_Node *node_ignore_value = eyelight_node_new(EYELIGHT_NODE_TYPE_VALUE, EYELIGHT_NAME_NONE, node_ignore);
        node_ignore_value->value = strdup(name);
    }

    EINA_LIST_FOREACH(edit->slide->areas, l, area)
    {
        if(strcmp(area->name, name) == 0)
        {
            edit->slide->areas = eina_list_remove(edit->slide->areas, area);
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




Eyelight_Node *_get_area_from_area(Eyelight_Slide *slide, Eyelight_Node *area)
{
    Eyelight_Area *e_area = eyelight_retrieve_area_from_node(slide, area);
    EYELIGHT_ASSERT_RETURN(!!e_area);

    if(!e_area->node_area)
    {
        const char* area_name = eyelight_retrieve_value_of_prop(area,0);
        EYELIGHT_ASSERT_RETURN(!!area_name);

        //Create the area
        Eyelight_Node *node_area = eyelight_node_new(EYELIGHT_NODE_TYPE_BLOCK, EYELIGHT_NAME_AREA,slide->node);
        Eyelight_Node *node_name = eyelight_node_new(EYELIGHT_NODE_TYPE_PROP, EYELIGHT_NAME_NAME, node_area);
        Eyelight_Node *node_value = eyelight_node_new(EYELIGHT_NODE_TYPE_VALUE, EYELIGHT_NAME_NONE, node_name);
        node_value->value = strdup(area_name);

        e_area->node_area = node_area;
        return node_area;
    }

    return e_area->node_area;
}

Eyelight_Node *_get_area_of_obj(Eyelight_Edit *edit)
{
    Eyelight_Node *area = edit->node;

    while(area && area->name != EYELIGHT_NAME_AREA)
        area = area->father;

    return area;
}

