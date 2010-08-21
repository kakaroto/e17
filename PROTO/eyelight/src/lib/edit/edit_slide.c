#ifdef HAVE_CONFIG_H
# include <config.h>
#endif

#include "Eyelight_Edit.h"
#include "edit.h"

const char* eyelight_edit_slide_title_get(Eyelight_Viewer *pres, int id_slide)
{
    const char *title = NULL;
    Eyelight_Node *node_slide, *node;
    Eina_List *l;

    EYELIGHT_ASSERT_RETURN(!!pres);
    EYELIGHT_ASSERT_RETURN(id_slide>=0);
    EYELIGHT_ASSERT_RETURN(id_slide<eyelight_viewer_size_get(pres));

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
                    default:
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
    EYELIGHT_ASSERT_RETURN_VOID(!!pres);

    Eyelight_Slide *slide = calloc(1,sizeof(Eyelight_Slide));

    slide->thumb.pos = -1;

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
    EYELIGHT_ASSERT_RETURN_VOID(!!pres);
    EYELIGHT_ASSERT_RETURN_VOID(id_slide>=0);
    EYELIGHT_ASSERT_RETURN_VOID(id_slide<eyelight_viewer_size_get(pres));

    Eyelight_Slide *slide = eina_list_nth(pres->slides, id_slide);

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
    EYELIGHT_ASSERT_RETURN_VOID(!!pres);
    EYELIGHT_ASSERT_RETURN_VOID(id_slide>=0);
    EYELIGHT_ASSERT_RETURN_VOID(id_slide<eyelight_viewer_size_get(pres));

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
    EYELIGHT_ASSERT_RETURN_VOID(!!pres);

    pres->edit_mode = 1;
    pres->edit_cb = cb;
    pres->edit_data = cb;
}

void eyelight_edit_slide_default_areas_reinit(Eyelight_Viewer *pres, int id_slide)
{
    Eina_List *l, *l2, *l_areas;
    char *area_name;
    Eyelight_Node *node;
    Eyelight_Area *area;

    EYELIGHT_ASSERT_RETURN_VOID(!!pres);
    EYELIGHT_ASSERT_RETURN_VOID(id_slide>=0);
    EYELIGHT_ASSERT_RETURN_VOID(id_slide<eyelight_viewer_size_get(pres));

    Eyelight_Slide *slide = eina_list_nth(pres->slides, id_slide);
    l_areas = eyelight_theme_areas_get(slide);
    EINA_LIST_FOREACH(l_areas, l, area_name)
    {
        //look if we have a custom area or a theme area with the same name
        EINA_LIST_FOREACH(slide->areas,l2, area)
        {
            if(area->name && strcmp(area->name, area_name) == 0)
            {
                slide->areas = eina_list_remove(slide->areas, area);
                slide->node->l = eina_list_remove(slide->node->l, area->node_def);
                eyelight_node_free(&(area->node_def), NULL);
                EYELIGHT_FREE(area);
                break;
            }
        }

        if( (node = eyelight_ignore_area_is(slide, area_name)))
        {
            slide->node->l = eina_list_remove(slide->node->l, node);
            eyelight_node_free(&node, NULL);
        }

    }
    EINA_LIST_FREE(l_areas, area_name)
        EYELIGHT_FREE(area_name);

    eyelight_slide_clean(slide);
    eyelight_viewer_slide_goto(pres, id_slide);
}



