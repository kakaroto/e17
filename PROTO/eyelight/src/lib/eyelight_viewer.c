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

#ifdef HAVE_CONFIG_H
# include <config.h>
#endif

#include "eyelight_viewer.h"

static int _eyelight_init_count = 0;
int _eyelight_log_dom_global = -1;

void _eyelight_viewer_end_transition_cb(void *data, Evas_Object *o, const char *emission, const char *source);


int eyelight_init()
{
    if (++_eyelight_init_count != 1) return _eyelight_init_count;

    if (!eina_init())
        return --_eyelight_init_count;

    _eyelight_log_dom_global = eina_log_domain_register("Eyelight", EINA_COLOR_BLUE);
    if (_eyelight_log_dom_global < 0)
    {
        EINA_LOG_ERR("Eyelight could not create a default log domain\n");
	goto shutdown_eina;
    }

    if (!edje_init())
	goto shutdown_ecore_evas;

    if (!ecore_evas_init())
	goto shutdown_eina;

    return _eyelight_init_count;

 shutdown_ecore_evas:
    ecore_evas_shutdown();
 shutdown_eina:
    eina_shutdown();

    return --_eyelight_init_count;
}

int eyelight_shutdown()
{
    if (--_eyelight_init_count != 0)
        return _eyelight_init_count;

    ecore_evas_shutdown();
    edje_shutdown();
    eina_log_domain_unregister(_eyelight_log_dom_global);
    _eyelight_log_dom_global = -1;
    eina_shutdown();

    return _eyelight_init_count;
}

/*
 * @create a new viewer
 */
Eyelight_Viewer* eyelight_viewer_new(Evas* evas, const char* presentation, const char* theme, int with_border)
{
    Eyelight_Viewer* pres;

    pres  = calloc(1,sizeof(Eyelight_Viewer));
    pres->state= EYELIGHT_VIEWER_STATE_DEFAULT;
    pres->evas = evas;
    pres->slide_with_transition[0] = NULL;
    pres->slide_with_transition[1] = NULL;
    pres->current = 0;
    pres->default_size_w = 1024;
    pres->default_size_h = 768;
    pres->current_scale = 1;

    pres->thumbnails.default_size_w = pres->default_size_w/4;
    pres->thumbnails.default_size_h = pres->default_size_h/4;

    eyelight_viewer_border_set(pres, with_border);
    eyelight_viewer_theme_file_set(pres,theme);
    eyelight_viewer_presentation_file_set(pres, presentation);

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

    pres -> compiler = eyelight_elt_load(pres->elt_file, pres->dump_out, pres->current_size_w, pres->current_size_h);
    pres->size = eyelight_nb_slides_get(pres->compiler);

    int i;
    for(i=0; i<pres->size; i++)
    {
        pres->slides = eina_list_append(pres->slides, eyelight_slide_new(pres));
    }

    if(eyelight_viewer_size_get(pres)>0)
        eyelight_viewer_slide_goto(pres,0);
    else
        pres->current = -1;

    eyelight_viewer_thumbnails_background_load_stop(pres);
    eyelight_viewer_thumbnails_background_load_start(pres);

    DBG("## Presentation file: %s",pres->elt_file);
    DBG("## Theme: %s",pres->theme);
    DBG("## Number of slides: %d",pres->size);
}

int eyelight_viewer_new_presentation_file_set(Eyelight_Viewer *pres, const char* presentation)
{
    EYELIGHT_FREE(pres->elt_file);

    eyelight_viewer_clean(pres);

    if(presentation && ecore_file_exists(presentation))
        pres->elt_file = strdup(presentation);
    else
        pres->elt_file = NULL;

    pres -> compiler = eyelight_elt_load(pres->elt_file, pres->dump_out, pres->current_size_w, pres->current_size_h);
    pres->size = eyelight_nb_slides_get(pres->compiler);

    int i;
    for(i=0; i<pres->size; i++)
    {
        pres->slides = eina_list_append(pres->slides, eyelight_slide_new(pres));
    }


    pres->current = -1;

    eyelight_viewer_thumbnails_background_load_stop(pres);
    eyelight_viewer_thumbnails_background_load_start(pres);

    DBG("## Presentation file: %s",pres->elt_file);
    DBG("## Theme: %s",pres->theme);
    DBG("## Number of slides: %d",pres->size);
}

int eyelight_viewer_eye_file_set(Eyelight_Viewer *pres, const char *eye)
{
    EYELIGHT_FREE(pres->dump_in);
    if (!eye || !ecore_file_exists(eye))
      return EINA_FALSE;

    eyelight_viewer_clean(pres);

    EYELIGHT_FREE(pres->elt_file);
    pres->dump_in = strdup(eye);

    pres -> compiler = eyelight_eye_load(eye);
    pres->size = eyelight_nb_slides_get(pres->compiler);

    int i;
    for(i=0; i<pres->size; i++)
    {
        pres->slides = eina_list_append(pres->slides, eyelight_slide_new(pres));
    }

    eyelight_viewer_slide_goto(pres,0);

    eyelight_viewer_thumbnails_background_load_stop(pres);
    eyelight_viewer_thumbnails_background_load_start(pres);

    DBG("## EYE Presentation file: %s",pres->dump_in);
    DBG("## Theme: %s",pres->theme);
    DBG("## Number of slides: %d",pres->size);

    return EINA_TRUE;
}

int eyelight_viewer_dump_file_set(Eyelight_Viewer *pres, const char *dump)
{
    EYELIGHT_FREE(pres->dump_out);
    if (!dump)
      return EINA_FALSE;

    pres->dump_out = strdup(dump);

    if (pres->theme)
      {
	 int res = ecore_file_cp(pres->theme, pres->dump_out);
	 DBG("Inserting theme file [%s]", pres->theme);
      }

    return EINA_TRUE;
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
        INFO("The theme doesn't exists , use the default");
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
    Eyelight_Slide *slide;
    EINA_LIST_FREE(pres->slides, slide)
    {
        eyelight_slide_clean(slide);
        EYELIGHT_FREE(slide->thumb.thumb);
        EYELIGHT_FREE(slide);
    }

    eyelight_compiler_free(&(pres->compiler));

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
    EYELIGHT_FREE((*pres)->dump_in);
    EYELIGHT_FREE((*pres)->dump_out);

    eyelight_viewer_clean(*pres);

    EYELIGHT_FREE(*pres);
}

/**
 * Create a new slide
 */
Eyelight_Slide* eyelight_slide_new(Eyelight_Viewer *pres)
{
    Eyelight_Slide *slide = calloc(1,sizeof(Eyelight_Slide));
    slide->pres = pres;
    slide->thumb.pos = -1;
    return slide;
}

/**
 * clean a slide
 * The method does not destroy slide
 */
void eyelight_slide_clean(Eyelight_Slide *slide)
{
    Evas_Object *o;
    Eyelight_Video *video;
    Eyelight_Area *area;
    Eyelight_Edit *edit;

    evas_object_del(slide->obj);
    slide->obj=NULL;

    eina_list_free(slide->items_all);
    slide->items_all = NULL;

    EINA_LIST_FREE(slide->items_edje, o)
        evas_object_del(o);

    EINA_LIST_FREE(slide->items_video, video)
    {
        evas_object_del(video->o_inter);
        EYELIGHT_FREE(video);
    }

    EINA_LIST_FREE(slide->areas, area)
    {
        evas_object_del(area->obj);
        EYELIGHT_FREE(area->name);
        EYELIGHT_FREE(area);
    }

    EINA_LIST_FREE(slide->edits, edit)
        free(edit);
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
    Eina_List *l;
    Eyelight_Slide *slide;
    pres->smart_obj = obj;

    EINA_LIST_FOREACH(pres->slides, l, slide)
        evas_object_smart_member_add(slide->obj, obj);

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
    Eina_List *l;
    Eyelight_Slide *slide;

    pres->current_size_w = w;
    pres->current_size_h = h;

    pres->thumbnails.default_size_w = pres->current_size_w/4;
    pres->thumbnails.default_size_h = pres->current_size_h/4;

    EINA_LIST_FOREACH(pres->slides, l, slide)
    {
        evas_object_resize(slide->obj, w, h);
        edje_object_signal_emit(slide->obj, "resize", "eyelight");
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
    Eina_List *l;
    Eyelight_Slide *slide;

    pres->current_scale = ratio;

    EINA_LIST_FOREACH(pres->slides, l, slide)
    {
        edje_object_scale_set(slide->obj, ratio);
        Eina_List *l2;
        Evas_Object *obj;
        EINA_LIST_FOREACH(slide->items_all, l2, obj)
            edje_object_scale_set(obj, ratio);
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
    Eina_List *l;
    Eyelight_Slide *slide;

    pres->current_clip = clip;

    EINA_LIST_FOREACH(pres->slides, l, slide)
        evas_object_clip_set(slide->obj, clip);

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
    Eina_List *l;
    Eyelight_Slide *slide;

    pres->current_pos_x = x;
    pres->current_pos_y = y;

    EINA_LIST_FOREACH(pres->slides, l, slide)
            evas_object_move(slide->obj, x, y);

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
void _eyelight_viewer_end_transition_cb(void *data, Evas_Object *o, const char *emission __UNUSED__, const char *source __UNUSED__)
{
    Eyelight_Viewer*pres = (Eyelight_Viewer*)data;
    int i;
    for(i=0;i<2;i++)
        if(pres->slide_with_transition[i] && pres->slide_with_transition[i]->obj==o)
                pres->slide_with_transition[i] = NULL;
}

/*
 * @brief load the transitions of a slide
 */
void eyelight_viewer_slide_transitions_load(Eyelight_Viewer*pres, Eyelight_Slide *slide, int id)
{
    slide->obj = eyelight_viewer_slide_get(pres,slide,id);
    const char* trans = edje_object_data_get(slide->obj, "transition");
    slide->transition_effect_previous = trans;
    slide->transition_effect_next = trans;

    trans = edje_object_data_get(slide->obj, "transition_next");
    if(trans)
    {
        slide->transition_effect_next = trans;
    }
    trans = edje_object_data_get(slide->obj, "transition_previous");
    if(trans)
    {
        slide->transition_effect_previous = trans;
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
Evas_Object* eyelight_viewer_slide_get(Eyelight_Viewer*pres,Eyelight_Slide *slide,int pos)
{
    if(!slide->obj)
    {
        eyelight_slide_clean(slide);

        slide->obj = eyelight_viewer_slide_load(pres,slide,pos);
        eyelight_viewer_slide_transitions_load(pres,slide,pos);
        evas_object_move (slide->obj, pres->current_pos_x, pres->current_pos_y);
        evas_object_resize(slide->obj,pres->current_size_w,pres->current_size_h);
        evas_object_clip_set(slide->obj, pres->current_clip);
        evas_object_show(slide->obj);
        edje_object_signal_emit(slide->obj,"hide","eyelight");
        edje_object_signal_callback_add(slide->obj,"transition,end","eyelight",_eyelight_viewer_end_transition_cb,pres);

        switch(pres->state)
        {
            case EYELIGHT_VIEWER_STATE_EXPOSE:
                evas_object_stack_below(slide->obj
                        ,pres->expose_background);
                break;
            case EYELIGHT_VIEWER_STATE_SLIDESHOW:
                evas_object_stack_below(slide->obj
                        ,pres->slideshow_background);
                break;
            case EYELIGHT_VIEWER_STATE_TABLEOFCONTENTS:
                evas_object_stack_below(slide->obj
                        ,pres->tableofcontents_background);
                break;
            case EYELIGHT_VIEWER_STATE_GOTOSLIDE:
                evas_object_stack_below(slide->obj
                        ,pres->gotoslide_object);
                break;
            default: break;
        }
        edje_object_signal_emit(slide->obj,"resize","eyelight");
    }
    return slide->obj;
}

Evas_Object* eyelight_viewer_slide_load(Eyelight_Viewer*pres,Eyelight_Slide *slide, int pos)
{
    char buf[EYELIGHT_BUFLEN];
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
                    default:
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
    slide->obj = edje_object_add(pres->evas);
    edje_object_scale_set(slide->obj, pres->current_scale);
    evas_object_smart_member_add(slide->obj, pres->smart_obj);

    if(!edje_file_group_exists(pres->theme,buf))
    {
        WARN("The layout \"%s\" does not exists !",layout);
        return slide->obj;
    }
    if(edje_object_file_set(slide->obj, pres->theme, buf) ==  0)
        ERR("edje_object_file_set() erreur! %d",
                edje_object_load_error_get(slide->obj));

    if(pres->with_border)
        edje_object_signal_emit(slide->obj, "border,show","eyelight");
    eyelight_compile(pres, slide, pos);

    return slide->obj;
}


/*
 * @brief clear slides we dont need
 */
void eyelight_viewer_clear(Eyelight_Viewer *pres)
{
    int i;
    Eina_List *l;
    Eyelight_Slide *slide;
    if(pres->do_not_clear_cache)
        return ;

    i=0;
    EINA_LIST_FOREACH(pres->slides, l, slide)
    {
        if(slide->obj && (i<pres->current-1 || i>pres->current+1))
            eyelight_slide_clean(slide);
        else if(!slide->obj && i>=pres->current-1 && i<=pres->current+1)
        {
            slide->obj = eyelight_viewer_slide_get(pres,slide,i);
        }
        i++;
    }
}

void eyelight_viewer_transitions_stop(Eyelight_Viewer* pres)
{
    int i;
    for(i=0;i<2;i++)
    {
        if(pres->slide_with_transition[i])
        {
            edje_object_signal_emit(pres->slide_with_transition[i]->obj,"stop,transition","eyelight");
            Eyelight_Slide *current_slide = eina_list_nth(pres->slides, pres->current);
            if(pres->slide_with_transition[i]==current_slide)
                edje_object_signal_emit(pres->slide_with_transition[i]->obj,"show","eyelight");
            else
                edje_object_signal_emit(pres->slide_with_transition[i]->obj,"hide","eyelight");
        }
    }
}

void eyelight_viewer_slide_next(Eyelight_Viewer*pres)
{
    char buf[EYELIGHT_BUFLEN];
    const char* trans_previous;
    const char* trans_next;

    if(pres->current>=pres->size-1)
        return ;

    eyelight_slide_transitions_get(pres,pres->current,&trans_previous, &trans_next);

    Eyelight_Slide *slide = eina_list_nth(pres->slides,pres->current);
    Eyelight_Slide *slide_next = eina_list_nth(pres->slides,pres->current+1);

    eyelight_viewer_transitions_stop(pres);
    if(strcmp(trans_next,"none")==0)
    {
        slide_next->obj = eyelight_viewer_slide_get(pres,slide_next,pres->current+1);
        edje_object_signal_emit(slide_next->obj,"show","eyelight");
        slide->obj = eyelight_viewer_slide_get(pres,slide,pres->current);
        edje_object_signal_emit(slide->obj,"hide","eyelight");
    }
    else
    {
        snprintf(buf,EYELIGHT_BUFLEN,"%s,%s",trans_next,"next,next");
        slide_next->obj = eyelight_viewer_slide_get(pres,slide_next,pres->current+1);
        edje_object_signal_emit(slide_next->obj,buf,"eyelight");

        snprintf(buf,EYELIGHT_BUFLEN,"%s,%s",trans_next,"current,next");
        slide->obj = eyelight_viewer_slide_get(pres,slide,pres->current);
        edje_object_signal_emit(slide->obj,buf,"eyelight");
    }
    pres->slide_with_transition[0] = slide;
    pres->slide_with_transition[1] = slide_next;
    slide_next->obj = eyelight_viewer_slide_get(pres,slide_next, pres->current+1);
    pres->current++;

    if(pres->slide_change_cb)
        pres->slide_change_cb(pres, pres->current-1, pres->current, pres->slide_change_data);

    eyelight_viewer_clear (pres);
}

void eyelight_viewer_slide_previous(Eyelight_Viewer*pres)
{
    char buf[EYELIGHT_BUFLEN];
    const char* trans_previous;
    const char* trans_next;

    if(pres->current<=0)
        return ;

    eyelight_slide_transitions_get(pres,pres->current,&trans_previous, &trans_next);

    Eyelight_Slide *slide = eina_list_nth(pres->slides,pres->current);
    Eyelight_Slide *slide_prev = eina_list_nth(pres->slides,pres->current-1);

    eyelight_viewer_transitions_stop(pres);

    if(strcmp(trans_previous,"none")==0)
    {
        slide_prev->obj = eyelight_viewer_slide_get(pres,slide_prev,pres->current-1);
        edje_object_signal_emit(slide_prev->obj,"show","eyelight");
        slide->obj = eyelight_viewer_slide_get(pres, slide, pres->current);
        edje_object_signal_emit(slide->obj,"hide","eyelight");
    }
    else
    {
        snprintf(buf,EYELIGHT_BUFLEN,"%s,%s",trans_previous,"previous,previous");
        slide_prev->obj = eyelight_viewer_slide_get(pres,slide_prev, pres->current-1);
        edje_object_signal_emit(slide_prev->obj,buf,"eyelight");

        snprintf(buf,EYELIGHT_BUFLEN,"%s,%s",trans_previous,"current,previous");
        slide->obj = eyelight_viewer_slide_get(pres,slide,pres->current);
        edje_object_signal_emit(slide->obj,buf,"eyelight");
    }
    pres->slide_with_transition[0] = slide;
    pres->slide_with_transition[1] = slide_prev;
    slide_prev->obj = eyelight_viewer_slide_get(pres,slide_prev,pres->current-1);
    pres->current--;
    if(pres->slide_change_cb)
        pres->slide_change_cb(pres, pres->current+1, pres->current, pres->slide_change_data);
    eyelight_viewer_clear (pres);
}

void eyelight_viewer_slide_goto(Eyelight_Viewer* pres, int slide_id)
{
    EYELIGHT_ASSERT_RETURN_VOID(!!pres);
    EYELIGHT_ASSERT_RETURN_VOID(slide_id>=0);
    EYELIGHT_ASSERT_RETURN_VOID(slide_id<eyelight_viewer_size_get(pres));

    int old_slide = pres->current;
    Eyelight_Slide *slide = eina_list_nth(pres->slides, slide_id);

    eyelight_viewer_transitions_stop(pres);

    slide->obj = eyelight_viewer_slide_get(pres,slide, slide_id);
    edje_object_signal_emit(slide->obj,"show","eyelight");
    if(pres->current!=slide_id && pres->current>=0)
    {
        slide = eina_list_nth(pres->slides, pres->current);
        slide->obj = eyelight_viewer_slide_get(pres,slide, pres->current);
        edje_object_signal_emit(slide->obj,"hide","eyelight");
    }

    pres->current = slide_id;
    pres->slide_with_transition[0] = NULL;
    pres->slide_with_transition[1] = NULL;

    eyelight_viewer_clear (pres);

    if(pres->slide_change_cb)
        pres->slide_change_cb(pres, old_slide, slide_id, pres->slide_change_data);
}

void eyelight_viewer_thumbnails_done_cb_set(Eyelight_Viewer* pres, Eyelight_Thumbnails_slide_done_cb cb, void *user_data)
{
    pres->thumbnails.done_cb = cb;
    pres->thumbnails.done_cb_data = user_data;
}

void eyelight_viewer_slide_change_cb_set(Eyelight_Viewer* pres, Eyelight_Slide_Change_Cb cb, void *user_data)
{
    pres->slide_change_cb = cb;
    pres->slide_change_data = user_data;
}
