/*
 * =====================================================================================
 *
 *       Filename:  eyelight_lib.c
 *
 *    Description:
 *
 *        Version:  1.0
 *        Created:  25/06/08 12:05:25 UTC
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  (Watchwolf), Atton Jonathan <watchwolf@watchwolf.fr>
 *        Company:
 *
 * =====================================================================================
 */

#include "eyelight_viewer.h"
#include "../../config.h"

void _eyelight_viewer_end_transition_cb(void *data, Evas_Object *o, const char *emission, const char *source);

/*
 * @create a new viewer
 */
Eyelight_Viewer* eyelight_viewer_new(Evas* evas, const char* presentation, const char* theme, int with_border)
{
    char* str;
    Eyelight_Viewer* pres;
    char buf[EYELIGHT_BUFLEN];
    pres  = calloc(1,sizeof(Eyelight_Viewer));
    pres->state= EYELIGHT_VIEWER_STATE_DEFAULT;
    pres->evas = evas;
    pres->slide_with_transition[0] = NULL;
    pres->slide_with_transition[1] = NULL;
    pres->current = 0;
    pres->default_size_w = 1024;
    pres->default_size_h = 768;
    pres->current_scale = 1;
    eyelight_viewer_thumbnails_init(pres);

    eyelight_viewer_border_set(pres, with_border);
    eyelight_viewer_theme_file_set(pres,theme);
    eyelight_viewer_presentation_file_set(pres, presentation);

    pres->video_module = strdup("xine");
    return pres;
}

/**
 * Set the presentation elt file
 */
int eyelight_viewer_presentation_file_set(Eyelight_Viewer *pres, const char* presentation)
{
    EYELIGHT_FREE(pres->elt_file);

    eyelight_viewer_clean(pres);

    if(presentation && ecore_file_exists(presentation))
        pres->elt_file = strdup(presentation);
    else
        pres->elt_file = NULL;

    pres -> compiler = eyelight_elt_load(pres->elt_file);
    pres->size = eyelight_nb_slides_get(pres->compiler);

    pres->slides = calloc(pres->size,sizeof(Evas_Object*));
    pres->edje_objects = calloc(pres->size,sizeof(Eina_List*));
    pres->edje_items = calloc(pres->size,sizeof(Eina_List*));
    pres->video_objects = calloc(pres->size,sizeof(Eina_List*));
    pres->custom_areas = calloc(pres->size,sizeof(Eina_List*));
    pres->transition_effect_next = calloc(pres->size,sizeof(char*));
    pres->transition_effect_previous = calloc(pres->size,sizeof(char*));

    if(eyelight_viewer_size_get(pres)>0)
        eyelight_viewer_slide_goto(pres,0);


    eyelight_viewer_thumbnails_init(pres);
    eyelight_viewer_thumbnails_background_load_start(pres);

    printf("## Presentation file: %s\n",pres->elt_file);
    printf("## Theme: %s\n",pres->theme);
    printf("## Number of slides: %d\n\n",pres->size);
}

/**
 * Set the presentation elt file
 */
int eyelight_viewer_theme_file_set(Eyelight_Viewer *pres, const char* theme)
{
    EYELIGHT_FREE(pres->theme);
    if(!theme)
        theme = PACKAGE_DATA_DIR"/themes/default/theme.edj";
    pres->theme = strdup(theme);
    if(!ecore_file_exists(pres->theme))
    {
        EYELIGHT_FREE(pres->theme);
        fprintf(stderr,"The theme doesn't exists , use the default\n");
        theme = PACKAGE_DATA_DIR"/themes/default/theme.edj";
        pres->theme = strdup(theme);
    }

    //we re-set the presentation, this will init all objects
    char *file = (pres->elt_file?strdup(pres->elt_file):NULL);
    eyelight_viewer_presentation_file_set(pres, file);
    EYELIGHT_FREE(file);
}

EAPI const char* eyelight_viewer_presentation_file_get(Eyelight_Viewer *pres)
{
    return pres->elt_file;
}

EAPI const char* eyelight_viewer_theme_file_get(Eyelight_Viewer *pres)
{
    return pres->theme;
}

/**
 * Set if the border of each area should been display or not
 */
void eyelight_viewer_border_set(Eyelight_Viewer *pres, int border)
{
    pres->with_border = border;
    //we re-set the presentation, this will init all objects
    char *file = (pres->elt_file?strdup(pres->elt_file):NULL);
    eyelight_viewer_presentation_file_set(pres, file);
    EYELIGHT_FREE(file);
}

void eyelight_viewer_clean(Eyelight_Viewer *pres)
{
    int i;

    for(i=0;i<pres->size;i++)
        if(pres->edje_items[i])
        {
            eina_list_free(pres->edje_items[i]);
            pres->edje_items[i] = NULL;
        }

    for(i=0;i<pres->size;i++)
        if(pres->edje_objects[i])
        {
            Eina_List *l, *l_next;
            Evas_Object *data;
            EINA_LIST_FOREACH_SAFE(pres->edje_objects[i], l, l_next, data)
            {
                evas_object_del(data);
                pres->edje_objects[i] = eina_list_remove_list(pres->edje_objects[i], l);
            }
        }

    for(i=0;i<pres->size;i++)
        if(pres->video_objects[i])
        {
            Eina_List *l, *l_next;
            Eyelight_Video *data;
            EINA_LIST_FOREACH_SAFE(pres->video_objects[i], l, l_next, data)
            {
                evas_object_del(data->o_inter);
                EYELIGHT_FREE(data);
                pres->video_objects[i] = eina_list_remove_list(pres->video_objects[i], l);
            }
        }


    for(i=0;i<pres->size;i++)
        if(pres->custom_areas[i])
        {
            Eina_List *l, *l_next;
            Eyelight_Custom_Area *data;
            EINA_LIST_FOREACH_SAFE(pres->custom_areas[i], l, l_next, data)
            {
                evas_object_del(data->obj);
                EYELIGHT_FREE(data);
                pres->custom_areas[i] = eina_list_remove_list(pres->custom_areas[i], l);
            }
        }


    EYELIGHT_FREE(pres->slides);
    EYELIGHT_FREE(pres->edje_objects);
    EYELIGHT_FREE(pres->video_objects);
    EYELIGHT_FREE(pres->custom_areas);
    eyelight_compiler_free(&(pres->compiler));
    EYELIGHT_FREE(pres->transition_effect_next);
    EYELIGHT_FREE(pres->transition_effect_previous);

    switch(pres->state)
    {
        case EYELIGHT_VIEWER_STATE_EXPOSE:
            eyelight_viewer_expose_stop(pres);
            break;
        case EYELIGHT_VIEWER_STATE_SLIDESHOW:
            eyelight_viewer_slideshow_stop(pres);
            break;
        case EYELIGHT_VIEWER_STATE_GOTOSLIDE:
            eyelight_viewer_gotoslide_stop(pres);
            break;
        case EYELIGHT_VIEWER_STATE_TABLEOFCONTENTS:
            eyelight_viewer_tableofcontents_stop(pres);
            break;
        default:
            break;
    }
    eyelight_viewer_thumbnails_destroy(pres);
}

/**
 * @brief destroy a viewer
 */
void eyelight_viewer_destroy(Eyelight_Viewer**pres)
{
    if(!pres) return;
    if(!(*pres)) return ;

    EYELIGHT_FREE((*pres)->theme);
    EYELIGHT_FREE((*pres)->elt_file);
    EYELIGHT_FREE((*pres)->video_module);

    eyelight_viewer_clean(*pres);

    EYELIGHT_FREE(*pres);
}

/*
 * @brief return the current state of the viewer (none, expose ...)
 */
Eyelight_Viewer_State eyelight_viewer_state_get(Eyelight_Viewer* pres)
{
    return pres->state;
}


/**
 * If the presentation should be a member of a smart object
 */
void eyelight_viewer_smart_obj_set(Eyelight_Viewer *pres, Evas_Object *obj)
{
    int i;
    pres->smart_obj = obj;

    for(i=0;i<pres->size;i++)
        if(pres->slides[i])
            evas_object_smart_member_add(pres->slides[i], obj);

    switch(pres->state)
    {
        case EYELIGHT_VIEWER_STATE_TABLEOFCONTENTS:
            eyelight_viewer_tableofcontents_smart_obj_set(pres,obj);
            break;
        case EYELIGHT_VIEWER_STATE_GOTOSLIDE:
            eyelight_viewer_gotoslide_smart_obj_set(pres,obj);
            break;
        case EYELIGHT_VIEWER_STATE_EXPOSE:
            eyelight_viewer_expose_smart_obj_set(pres,obj);
            break;
        case EYELIGHT_VIEWER_STATE_SLIDESHOW:
            eyelight_viewer_slideshow_smart_obj_set(pres,obj);
            break;
        default: break;
    }
}


/*
 * Resize all slides
 */
void eyelight_viewer_resize(Eyelight_Viewer*pres, Evas_Coord w, Evas_Coord h)
{
    int i;

    pres->current_size_w = w;
    pres->current_size_h = h;

    for(i=0;i<pres->size;i++)
    {
        if(pres->slides[i])
        {
            evas_object_resize(pres->slides[i], w, h);
            edje_object_signal_emit(pres->slides[i],"resize","eyelight");
        }
    }
    switch(pres->state)
    {
        case EYELIGHT_VIEWER_STATE_TABLEOFCONTENTS:
            eyelight_viewer_tableofcontents_resize(pres,w,h);
            break;
        case EYELIGHT_VIEWER_STATE_GOTOSLIDE:
            eyelight_viewer_gotoslide_resize(pres,w,h);
            break;
        case EYELIGHT_VIEWER_STATE_EXPOSE:
            eyelight_viewer_expose_resize(pres,w,h);
            break;
        case EYELIGHT_VIEWER_STATE_SLIDESHOW:
            eyelight_viewer_slideshow_resize(pres,w,h);
            break;
        default: break;
    }
    double ratio_w = w/1024.;
    double ratio_h = h/768.;
    double ratio = (ratio_w+ratio_h)/2;
    eyelight_viewer_scale_set(pres, ratio);
}

/*
 * Scale all slides
 */
void eyelight_viewer_scale_set(Eyelight_Viewer*pres, double ratio)
{
    int i;

    pres->current_scale = ratio;

    for(i=0;i<pres->size;i++)
    {
        if(pres->slides[i])
        {
            edje_object_scale_set(pres->slides[i], ratio);
            Eina_List *l;
            Evas_Object *o;
            EINA_LIST_FOREACH(pres->edje_items[i],l,o)
                edje_object_scale_set(o, ratio);
        }
    }

    switch(pres->state)
    {
        case EYELIGHT_VIEWER_STATE_TABLEOFCONTENTS:
            eyelight_viewer_tableofcontents_scale_set(pres,ratio);
            break;
        case EYELIGHT_VIEWER_STATE_GOTOSLIDE:
            eyelight_viewer_gotoslide_scale_set(pres,ratio);
            break;
        case EYELIGHT_VIEWER_STATE_EXPOSE:
            eyelight_viewer_expose_scale_set(pres,ratio);
            break;
        case EYELIGHT_VIEWER_STATE_SLIDESHOW:
            eyelight_viewer_slideshow_scale_set(pres,ratio);
            break;
        default: break;
    }
}

/*
 * Clip all slides
 */
void eyelight_viewer_clip_set(Eyelight_Viewer*pres, Evas_Object *clip)
{
    int i;

    pres->current_clip = clip;

    for(i=0;i<pres->size;i++)
    {
        if(pres->slides[i])
        {
            evas_object_clip_set(pres->slides[i], clip);
        }
    }
    switch(pres->state)
    {
        case EYELIGHT_VIEWER_STATE_TABLEOFCONTENTS:
            eyelight_viewer_tableofcontents_clip_set(pres,clip);
            break;
        case EYELIGHT_VIEWER_STATE_GOTOSLIDE:
            eyelight_viewer_gotoslide_clip_set(pres,clip);
            break;
        case EYELIGHT_VIEWER_STATE_EXPOSE:
            eyelight_viewer_expose_clip_set(pres,clip);
            break;
        case EYELIGHT_VIEWER_STATE_SLIDESHOW:
            eyelight_viewer_slideshow_clip_set(pres,clip);
            break;
        default: break;
    }
}

/*
 * Move all slides
 */
void eyelight_viewer_move(Eyelight_Viewer*pres, Evas_Coord x, Evas_Coord y)
{
    int i;

    pres->current_pos_x = x;
    pres->current_pos_y = y;

    for(i=0;i<pres->size;i++)
    {
        if(pres->slides[i])
            evas_object_move(pres->slides[i], x, y);
    }
    switch(pres->state)
    {
        case EYELIGHT_VIEWER_STATE_TABLEOFCONTENTS:
            eyelight_viewer_tableofcontents_move(pres,x,y);
            break;
        case EYELIGHT_VIEWER_STATE_GOTOSLIDE:
            eyelight_viewer_gotoslide_move(pres,x,y);
            break;
        case EYELIGHT_VIEWER_STATE_EXPOSE:
            eyelight_viewer_expose_move(pres,x,y);
            break;
        case EYELIGHT_VIEWER_STATE_SLIDESHOW:
            eyelight_viewer_slideshow_move(pres,x,y);
            break;
        default: break;
    }
}

/*
 * @brief call when a transition is end
 */
void _eyelight_viewer_end_transition_cb(void *data, Evas_Object *o, const char *emission, const char *source)
{
    Eyelight_Viewer*pres = (Eyelight_Viewer*)data;
    int i;
    for(i=0;i<2;i++)
        if(pres->slide_with_transition[i]==o)
                pres->slide_with_transition[i] = NULL;
}

/*
 * @brief load the transitions of a slide
 */
void eyelight_viewer_slide_transitions_load(Eyelight_Viewer*pres,int slideid)
{
    Evas_Object* slide = eyelight_viewer_slide_get(pres,slideid);
    const char* trans = edje_object_data_get(slide, "transition");
    pres->transition_effect_previous[slideid] = trans;
    pres->transition_effect_next[slideid] = trans;

    trans = edje_object_data_get(slide, "transition_next");
    if(trans)
    {
        pres->transition_effect_next[slideid] = trans;
    }
    trans = edje_object_data_get(slide, "transition_previous");
    if(trans)
    {
        pres->transition_effect_previous[slideid] = trans;
    }
}

/*
 * @brief return the id of the current slide
 */
int eyelight_viewer_current_id_get(Eyelight_Viewer* pres)
{
    return pres->current;
}

/*
 * @brief return the number of slide
 */
int eyelight_viewer_size_get(Eyelight_Viewer*pres)
{
    return pres->size;
}

/**
 * If 1 the cache of slides is never cleared
 */
void eyelight_viewer_clear_cache_set(Eyelight_Viewer *pres, int clear)
{
    pres->do_not_clear_cache = clear;
}

/*
 * @brief return a slide, load it if necessary
 */
Evas_Object* eyelight_viewer_slide_get(Eyelight_Viewer*pres,int pos)
{
    int w,h;

    if(!pres->slides[pos])
    {
        //you clear the others data of the slide,
        //because maybe the slide has to be remove with eyelight_edit_remove_slide
        {
            eina_list_free(pres->edje_items[pos]);
            pres->edje_items[pos] = NULL;

            Eina_List *l, *l_next;
            {
                Evas_Object *data;
                EINA_LIST_FOREACH_SAFE(pres->edje_objects[pos], l, l_next, data)
                {
                    evas_object_del(data);
                    pres->edje_objects[pos] = eina_list_remove_list(pres->edje_objects[pos], l);
                }
            }
            {
                Eyelight_Custom_Area *data;
                EINA_LIST_FOREACH_SAFE(pres->custom_areas[pos], l, l_next, data)
                {
                    evas_object_del(data->obj);
                    EYELIGHT_FREE(data);
                    pres->custom_areas[pos] = eina_list_remove_list(pres->custom_areas[pos], l);
                }
            }
            {
                Eyelight_Video *data;
                EINA_LIST_FOREACH_SAFE(pres->video_objects[pos], l, l_next, data)
                {
                    evas_object_del(data->o_inter);
                    EYELIGHT_FREE(data);
                    pres->video_objects[pos] = eina_list_remove_list(pres->video_objects[pos], l);
                }
            }
        }
        pres->slides[pos] = eyelight_viewer_slide_load(pres,pos);
        eyelight_viewer_slide_transitions_load(pres,pos);
        evas_object_move (pres->slides[pos], pres->current_pos_x, pres->current_pos_y);
        evas_object_resize(pres->slides[pos],pres->current_size_w,pres->current_size_h);
        evas_object_clip_set(pres->slides[pos], pres->current_clip);
        evas_object_show(pres->slides[pos]);
        edje_object_signal_emit(pres->slides[pos],"hide","eyelight");
        edje_object_signal_callback_add(pres->slides[pos],"transition,end","eyelight",_eyelight_viewer_end_transition_cb,pres);

        switch(pres->state)
        {
            case EYELIGHT_VIEWER_STATE_EXPOSE:
                evas_object_stack_below(pres->slides[pos]
                        ,pres->expose_background);
                break;
            case EYELIGHT_VIEWER_STATE_SLIDESHOW:
                evas_object_stack_below(pres->slides[pos]
                        ,pres->slideshow_background);
                break;
            case EYELIGHT_VIEWER_STATE_TABLEOFCONTENTS:
                evas_object_stack_below(pres->slides[pos]
                        ,pres->tableofcontents_background);
                break;
            case EYELIGHT_VIEWER_STATE_GOTOSLIDE:
                evas_object_stack_below(pres->slides[pos]
                        ,pres->gotoslide_object);
                break;
            default: break;
        }
        edje_object_signal_emit(pres->slides[pos],"resize","eyelight");
    }
    return pres->slides[pos];
}

Evas_Object* eyelight_viewer_slide_load(Eyelight_Viewer*pres,int pos)
{
    char buf[EYELIGHT_BUFLEN];
    Evas_Object* slide;
    Eyelight_Node *node, *node_slide = NULL;
    char *default_layout = NULL;
    Eina_List *l;

    Eyelight_Compiler *compiler = pres->compiler;
    l = compiler->root->l;
    int i_slide = -1;


    //retrieve the slide and the layout
    while( l && i_slide < pos)
    {
        node = eina_list_data_get(l);
        switch(node->type)
        {
            case EYELIGHT_NODE_TYPE_PROP:
                switch(node->name)
                {
                    case EYELIGHT_NAME_LAYOUT:
                        default_layout =eyelight_retrieve_value_of_prop(node,0);
                        break;
                    default: break;
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

    //search the layout
    Eyelight_Node *node_layout = eyelight_retrieve_node_prop(node_slide, EYELIGHT_NAME_LAYOUT);
    char * layout = NULL;
    if(!node_layout && !default_layout)
        layout = "blank";
    else if(!node_layout)
        layout = default_layout;
    else
        layout = eyelight_retrieve_value_of_prop(node_layout,0);

    //load the slide
    snprintf(buf,EYELIGHT_BUFLEN,"eyelight/layout_%s",layout);
    slide = edje_object_add(pres->evas);
    edje_object_scale_set(slide, pres->current_scale);
    evas_object_smart_member_add(slide, pres->smart_obj);

    if(!edje_file_group_exists(pres->theme,buf))
    {
        printf("The layout \"%s\" doesnt exists !\n",layout);
        exit(0);
    }
    if(edje_object_file_set(slide, pres->theme, buf) ==  0)
        printf("eyelight_viewer_slide_load(), edje_object_file_set() erreur! %d \n",
                edje_object_load_error_get(slide));

    if(pres->with_border)
        edje_object_signal_emit(slide, "border,show","eyelight");
    eyelight_compile(pres, pos, slide);

    return slide;
}


/*
 * @brief clear slides we dont need
 */
void eyelight_viewer_clear(Eyelight_Viewer *pres)
{
    int i;
    if(pres->do_not_clear_cache)
        return ;

    for(i=0;i<pres->size;i++)
    {
        if(pres->slides[i] && (i<pres->current-1 || i>pres->current+1))
        {
            evas_object_del(pres->slides[i]);
            pres->slides[i]=NULL;

            eina_list_free(pres->edje_items[i]);
            pres->edje_items[i] = NULL;

            Eina_List *l, *l_next;
            {
                Evas_Object *data;
                EINA_LIST_FOREACH_SAFE(pres->edje_objects[i], l, l_next, data)
                {
                    evas_object_del(data);
                    pres->edje_objects[i] = eina_list_remove_list(pres->edje_objects[i], l);
                }
            }
            {
                Eyelight_Custom_Area *data;
                EINA_LIST_FOREACH_SAFE(pres->custom_areas[i], l, l_next, data)
                {
                    evas_object_del(data->obj);
                    EYELIGHT_FREE(data);
                    pres->custom_areas[i] = eina_list_remove_list(pres->custom_areas[i], l);
                }
            }
            {
                Eyelight_Video *data;
                EINA_LIST_FOREACH_SAFE(pres->video_objects[i], l, l_next, data)
                {
                    evas_object_del(data->o_inter);
                    EYELIGHT_FREE(data);
                    pres->video_objects[i] = eina_list_remove_list(pres->video_objects[i], l);
                }
            }
        }
        else if(!pres->slides[i] && i>=pres->current-1 && i<=pres->current+1)
        {
            pres->slides[i] = eyelight_viewer_slide_get(pres,i);
        }
    }
}

void eyelight_viewer_transitions_stop(Eyelight_Viewer* pres)
{
    int i;
    for(i=0;i<2;i++)
    {
        if(pres->slide_with_transition[i]!=NULL)
        {
            edje_object_signal_emit(pres->slide_with_transition[i],"stop,transition","eyelight");
            if(pres->slide_with_transition[i]==pres->slides[pres->current])
                edje_object_signal_emit(pres->slide_with_transition[i],"show","eyelight");
            else
                edje_object_signal_emit(pres->slide_with_transition[i],"hide","eyelight");
        }
    }
}

void eyelight_viewer_slide_next(Eyelight_Viewer*pres)
{
    char buf[EYELIGHT_BUFLEN];
    const char* trans_previous;
    const char* trans_next;
    eyelight_slide_transitions_get(pres,pres->current,&trans_previous, &trans_next);
    Evas_Object* slide;

    if(pres->current>=pres->size-1)
        return ;
    eyelight_viewer_transitions_stop(pres);
    if(strcmp(trans_next,"none")==0)
    {
        slide = eyelight_viewer_slide_get(pres,eyelight_viewer_current_id_get(pres)+1);
        edje_object_signal_emit(slide,"show","eyelight");
        slide = eyelight_viewer_slide_get(pres,eyelight_viewer_current_id_get(pres));
        edje_object_signal_emit(slide,"hide","eyelight");
    }
    else
    {
        snprintf(buf,EYELIGHT_BUFLEN,"%s,%s",trans_next,"next,next");
        slide = eyelight_viewer_slide_get(pres,eyelight_viewer_current_id_get(pres)+1);
        edje_object_signal_emit(slide,buf,"eyelight");

        snprintf(buf,EYELIGHT_BUFLEN,"%s,%s",trans_next,"current,next");
        slide = eyelight_viewer_slide_get(pres,eyelight_viewer_current_id_get(pres));
        edje_object_signal_emit(slide,buf,"eyelight");
    }
    pres->slide_with_transition[0] = eyelight_viewer_slide_get(pres,pres->current);
    pres->slide_with_transition[1] = eyelight_viewer_slide_get(pres,pres->current+1);
    pres->current++;
    slide = eyelight_viewer_slide_get(pres,eyelight_viewer_current_id_get(pres));
    eyelight_viewer_clear (pres);
}

void eyelight_viewer_slide_previous(Eyelight_Viewer*pres)
{
    char buf[EYELIGHT_BUFLEN];
    const char* trans_previous;
    const char* trans_next;
    Evas_Object* slide;
    eyelight_slide_transitions_get(pres,pres->current,&trans_previous, &trans_next);

    if(pres->current<=0)
        return ;

    eyelight_viewer_transitions_stop(pres);

    if(strcmp(trans_previous,"none")==0)
    {
        slide = eyelight_viewer_slide_get(pres,eyelight_viewer_current_id_get(pres)-1);
        edje_object_signal_emit(slide,"show","eyelight");
        slide = eyelight_viewer_slide_get(pres,eyelight_viewer_current_id_get(pres));
        edje_object_signal_emit(slide,"hide","eyelight");
    }
    else
    {
        snprintf(buf,EYELIGHT_BUFLEN,"%s,%s",trans_previous,"previous,previous");
        slide = eyelight_viewer_slide_get(pres,eyelight_viewer_current_id_get(pres)-1);
        edje_object_signal_emit(slide,buf,"eyelight");

        snprintf(buf,EYELIGHT_BUFLEN,"%s,%s",trans_previous,"current,previous");
        slide = eyelight_viewer_slide_get(pres,eyelight_viewer_current_id_get(pres));
        edje_object_signal_emit(slide,buf,"eyelight");
    }
    pres->slide_with_transition[0] = eyelight_viewer_slide_get(pres,pres->current);
    pres->slide_with_transition[1] = eyelight_viewer_slide_get(pres,pres->current-1);
    pres->current--;
    slide = eyelight_viewer_slide_get(pres,eyelight_viewer_current_id_get(pres));
    eyelight_viewer_clear (pres);
}

void eyelight_viewer_slide_goto(Eyelight_Viewer* pres, int slide_id)
{
    Evas_Object* slide;

    eyelight_viewer_transitions_stop(pres);

    slide = eyelight_viewer_slide_get(pres,slide_id);
    edje_object_signal_emit(slide,"show","eyelight");
    if(pres->current!=slide_id)
    {
        slide = eyelight_viewer_slide_get(pres,eyelight_viewer_current_id_get(pres));
        edje_object_signal_emit(slide,"hide","eyelight");
        pres->current = slide_id;
    }

    pres->slide_with_transition[0] = NULL;
    pres->slide_with_transition[1] = NULL;
    eyelight_viewer_clear (pres);
}


