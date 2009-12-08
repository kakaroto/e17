#include "Eyelight_Edit.h"
#include "edit.h"

void eyelight_edit_area_image_add(Eyelight_Viewer *pres, Eyelight_Edit *edit, const char* image)
{
    EYELIGHT_ASSERT_RETURN_VOID(edit != NULL);
    EYELIGHT_ASSERT_RETURN_VOID(edit->node->name == EYELIGHT_NAME_CUSTOM_AREA
            || edit->node->name == EYELIGHT_NAME_THEME_AREA);
    EYELIGHT_ASSERT_RETURN_VOID(image!=NULL);

    char *area_name = eyelight_retrieve_value_of_prop(edit->node,0);

    //create the node
    Eyelight_Node *node = eyelight_node_new(EYELIGHT_NODE_TYPE_BLOCK, EYELIGHT_NAME_IMAGE,
            _get_area_from_area(edit->slide, edit->node));
    Eyelight_Node *node_image = eyelight_node_new(EYELIGHT_NODE_TYPE_PROP, EYELIGHT_NAME_IMAGE, node);
    Eyelight_Node *node_value = eyelight_node_new(EYELIGHT_NODE_TYPE_VALUE, EYELIGHT_NAME_NAME, node_image);
    node_value->value = strdup(image);

    eyelight_object_item_image_add(pres,edit->slide, node, area_name, image, 0, 0, 0, 0, 1);
}

void eyelight_edit_image_properties_get(Eyelight_Edit *edit, char **file, int *border, int *shadow)
{
    Eyelight_Node *node;
    EYELIGHT_ASSERT_RETURN_VOID(edit != NULL);
    EYELIGHT_ASSERT_RETURN_VOID(edit->node->name == EYELIGHT_NAME_IMAGE);

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

    EYELIGHT_ASSERT_RETURN_VOID(edit != NULL);
    EYELIGHT_ASSERT_RETURN_VOID(edit->node->name == EYELIGHT_NAME_IMAGE);

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




