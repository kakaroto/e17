#ifdef HAVE_CONFIG_H
# include <config.h>
#endif

#include "Eyelight_Edit.h"
#include "edit.h"

Eyelight_Node_Name eyelight_edit_name_get(Eyelight_Edit *edit)
{
    EYELIGHT_ASSERT_RETURN(!!edit);

    return edit->node->name;
}

int eyelight_edit_save(Eyelight_Viewer *pres, const char *file)
{
    EYELIGHT_ASSERT_RETURN(!!pres);
    EYELIGHT_ASSERT_RETURN(!!file);

    return eyelight_save(pres->compiler->root, file);
}

void eyelight_edit_obj_unselect(Eyelight_Edit *edit)
{
    EYELIGHT_ASSERT_RETURN_VOID(!!edit);
    edje_object_signal_emit(edit->obj, "unselect", "eyelight");
}

void eyelight_edit_object_down(Eyelight_Viewer *pres, Eyelight_Edit *edit)
{
    Eyelight_Node *father, *node, *next_next=NULL;
    Eina_List *l;
    int find = 0;
    char buf[EYELIGHT_BUFLEN];

    EYELIGHT_ASSERT_RETURN_VOID(!!edit);
    EYELIGHT_ASSERT_RETURN_VOID(!!pres);

    Eyelight_Node *n_area = _get_area_of_obj(edit);
    Eyelight_Node *n_area_name = eyelight_retrieve_node_prop(n_area, EYELIGHT_NAME_NAME);
    const char *name = eyelight_retrieve_value_of_prop(n_area_name, 0);
    if(!name) return;
    Evas_Object *o_area = eyelight_object_area_obj_get(pres, edit->slide, name, buf);
    if(!n_area) return;

    //up the node
    father = edit->node->father;
    // note: Some items are not displayed (area's name ...), we ignore these items
    // if(node->obj) then the item is displayed
    EINA_LIST_FOREACH(father->l, l, node)
    {
        if(node->obj && node == edit->node)
            find = 1;
        else if(node->obj && find == 1)
            find = 2;
        else if(node->obj && find == 2)
        {
            next_next = node;
            break;
        }
    }

    if(!next_next)
    {
        father->l = eina_list_remove(father->l, edit->node);
        father->l = eina_list_append(father->l, edit->node);

        edje_object_part_box_remove(o_area, "area.custom", edit->obj);
        edje_object_part_box_append(o_area, "area.custom", edit->obj);
    }
    else
    {
        father->l = eina_list_remove(father->l, edit->node);
        father->l = eina_list_prepend_relative(father->l, edit->node, next_next);

        edje_object_part_box_remove(o_area, "area.custom", edit->obj);
        edje_object_part_box_insert_before(o_area, "area.custom", edit->obj, next_next->obj);
    }
}

void eyelight_edit_object_up(Eyelight_Viewer *pres, Eyelight_Edit *edit)
{
    Eyelight_Node *father, *node, *previous=NULL;
    Eina_List *l;
    char buf[EYELIGHT_BUFLEN];

    EYELIGHT_ASSERT_RETURN_VOID(!!edit);
    EYELIGHT_ASSERT_RETURN_VOID(!!pres);

    Eyelight_Node *n_area = _get_area_of_obj(edit);
    Eyelight_Node *n_area_name = eyelight_retrieve_node_prop(n_area, EYELIGHT_NAME_NAME);
    const char *name = eyelight_retrieve_value_of_prop(n_area_name, 0);
    if(!name) return;
    Evas_Object *o_area = eyelight_object_area_obj_get(pres, edit->slide, name, buf);
    if(!n_area) return;

    //up the node
    father = edit->node->father;
    EINA_LIST_FOREACH(father->l, l, node)
    {
        if(node->obj && node != edit->node)
            previous = node;
        else if(node->obj)
            break;
    }

    if(!previous)
    {
        father->l = eina_list_remove(father->l, edit->node);
        father->l = eina_list_prepend(father->l, edit->node);

        edje_object_part_box_remove(o_area, "area.custom", edit->obj);
        edje_object_part_box_prepend(o_area, "area.custom", edit->obj);
    }
    else
    {
        father->l = eina_list_remove(father->l, edit->node);
        father->l = eina_list_prepend_relative(father->l, edit->node, previous);

        edje_object_part_box_remove(o_area, "area.custom", edit->obj);
        edje_object_part_box_insert_before(o_area, "area.custom", edit->obj, previous->obj);
    }
}

void eyelight_edit_object_delete(Eyelight_Edit *edit)
{
    EYELIGHT_ASSERT_RETURN_VOID(!!edit);

    Eyelight_Node *father = edit->node->father;
    Eyelight_Slide *slide = edit->slide;
    father->l = eina_list_remove(father->l, edit->node);

    switch(edit->node->name)
    {
        case EYELIGHT_NAME_EDJ:
            printf("EDJ\n");
            break;
        case EYELIGHT_NAME_VIDEO:
            printf("VIDEO\n");
            break;
        default:
            slide->items_all = eina_list_remove(slide->items_all, edit->obj);
    }

    evas_object_del(edit->node->obj);
    eyelight_node_free(&(edit->node), NULL);
    slide->edits = eina_list_remove(slide->edits, edit);
    EYELIGHT_FREE(edit);
}

