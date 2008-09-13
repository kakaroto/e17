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

void eyelight_viewer_edje_file_set(Eyelight_Viewer* pres,char* edje_file);
void eyelight_viewer_edc_file_set(Eyelight_Viewer* pres,char* edc_file);
void eyelight_viewer_elt_file_set(Eyelight_Viewer* pres,char* elt_file);
void eyelight_viewer_presentation_set(Eyelight_Viewer* pres,char* presentation);
void eyelight_viewer_theme_set(Eyelight_Viewer* pres,char* theme);

void eyelight_viewer_text_class_free(void* data)
{
    Eyelight_Text_Class *class = (Eyelight_Text_Class*)data;
    EYELIGHT_FREE(class->name);
    EYELIGHT_FREE(class->font);
    EYELIGHT_FREE(class);
}

/*
 * @create a new viewer
 */
Eyelight_Viewer* eyelight_viewer_new(Evas* evas, char* presentation, char* theme, int with_border)
{
    char* str;
    Eyelight_Viewer* pres;
    char buf[EYELIGHT_BUFLEN];
    pres  = calloc(1,sizeof(Eyelight_Viewer));
    pres->state= EYELIGHT_VIEWER_STATE_DEFAULT;
    pres->evas = evas;
    pres->with_border = with_border;

    pres->text_class = ecore_list_new();
    ecore_list_free_cb_set(pres->text_class,eyelight_viewer_text_class_free);

    str = ecore_file_realpath(presentation);
    eyelight_viewer_presentation_set(pres,str);
    EYELIGHT_FREE(str);
    if(!presentation || strlen(presentation)==0)
    {
        fprintf(stderr,"The presentation file doesn't exists !\n");
        eyelight_viewer_destroy(&pres);
        return NULL;
    }

    if(strcmp(pres->presentation+(strlen(pres->presentation)-4),".elt")==0)
        eyelight_viewer_elt_file_set(pres,pres->presentation);
    else if(strcmp(pres->presentation+(strlen(pres->presentation)-4),".edc")==0)
        eyelight_viewer_edc_file_set(pres,pres->presentation);
    else if(strcmp(pres->presentation+(strlen(pres->presentation)-4),".edj")==0)
        eyelight_viewer_edje_file_set(pres,pres->presentation);
    else
        return NULL;

    if(!pres->edje_file)
    {
        //if the presentation file is not an edje file, we need to set the theme
        if(!theme)
            theme = PACKAGE_DATA_DIR"/themes/default/theme.edc";
        str = ecore_file_realpath(theme);
        eyelight_viewer_theme_set(pres,str);
        EYELIGHT_FREE(str);
        if(ecore_file_is_dir(pres->theme))
        {
            snprintf(buf,EYELIGHT_BUFLEN,"%s/theme.edc",pres->theme);
            eyelight_viewer_theme_set(pres,buf);
        }
        if(!ecore_file_exists(theme))
        {
            snprintf(buf,EYELIGHT_BUFLEN, PACKAGE_DATA_DIR"/themes/%s",theme);
            if(ecore_file_is_dir(buf))
                snprintf(buf,EYELIGHT_BUFLEN, PACKAGE_DATA_DIR"/themes/%s/theme.edc",theme);
            eyelight_viewer_theme_set(pres,buf);
            if(!ecore_file_exists(pres->theme))
            {
                fprintf(stderr,"The theme doesn't exists !\n");
                eyelight_viewer_destroy(&pres);
                return NULL;
            }
        }
    }
    return pres;
}

/*
 * @brief destroy a viewer
 */
void eyelight_viewer_destroy(Eyelight_Viewer**pres)
{
    int i;
    EYELIGHT_FREE((*pres)->presentation);
    EYELIGHT_FREE((*pres)->theme);
    EYELIGHT_FREE((*pres)->edje_file);
    EYELIGHT_FREE((*pres)->edc_file);
    EYELIGHT_FREE((*pres)->elt_file);
    for(i=0;i<(*pres)->size;i++)
        if((*pres)->slides[i])
            evas_object_del((*pres)->slides[i]);
    EYELIGHT_FREE((*pres)->slides);
    EYELIGHT_FREE((*pres)->transition_effect_next);
    EYELIGHT_FREE((*pres)->transition_effect_previous);
    ecore_list_destroy((*pres)->text_class);


    switch((*pres)->state)
    {
        case EYELIGHT_VIEWER_STATE_EXPOSE:
            eyelight_viewer_expose_stop(*pres);
            break;
        case EYELIGHT_VIEWER_STATE_SLIDESHOW:
            eyelight_viewer_slideshow_stop(*pres);
            break;
        case EYELIGHT_VIEWER_STATE_GOTOSLIDE:
            eyelight_viewer_gotoslide_stop(*pres);
            break;
        case EYELIGHT_VIEWER_STATE_TABLEOFCONTENTS:
            eyelight_viewer_tableofcontents_stop(*pres);
            break;
        default:
            break;
    }

    eyelight_viewer_thumbnails_destroy(*pres);

    EYELIGHT_FREE(*pres);
}

/*
 * @brief init a viewer
 */
void eyelight_viewer_slides_init(Eyelight_Viewer*pres, int w, int h)
{
    int i,size;
    char buf[EYELIGHT_BUFLEN];

    if(!pres->edje_file || !ecore_file_exists(pres->edje_file))
    {
        char *str;
        if(!pres->edc_file || !ecore_file_exists(pres->edc_file))
        {
            str = eyelight_create_edc_from_elt(pres->elt_file,pres->with_border);
            eyelight_viewer_edc_file_set(pres,str);
            EYELIGHT_FREE(str);
        }
        str = eyelight_create_edj_from_edc(pres->edc_file, pres->theme);
        eyelight_viewer_edje_file_set(pres,str);
        EYELIGHT_FREE(str);
    }

    size = 0;
    while(1)
    {
        snprintf(buf,EYELIGHT_BUFLEN,"slide_%d",size+1);
        if(!edje_file_group_exists(pres->edje_file,buf))
            break;
        size++;
    }

    pres->slide_with_transition[0] = NULL;
    pres->slide_with_transition[1] = NULL;
    pres->size = size;
    pres->slides = calloc(pres->size,sizeof(Evas_Object*));
    pres->transition_effect_next = calloc(pres->size,sizeof(char*));
    pres->transition_effect_previous = calloc(pres->size,sizeof(char*));
    pres->current = 0;

    pres->default_size_w = w;
    pres->default_size_h = h;

    eyelight_viewer_thumbnails_init(pres);

    char* text_class;
    i = 1;
    while(1)
    {
        snprintf(buf,EYELIGHT_BUFLEN,"text_class_%d",i);
        text_class = edje_file_data_get(pres->edje_file,buf);
        if(!text_class)
            break;

        Eyelight_Text_Class* class = calloc(1,sizeof(Eyelight_Text_Class));
        class->size = -1;
        class->name = text_class;

        snprintf(buf,EYELIGHT_BUFLEN,"%s_font",class->name);
        class->font = edje_file_data_get(pres->edje_file,buf);
        snprintf(buf,EYELIGHT_BUFLEN,"%s_size",class->name);
        char* size = edje_file_data_get(pres->edje_file,buf);
        if(size)
            class->size = atoi(size);
        EYELIGHT_FREE(size);

        ecore_list_append(pres->text_class,class);

        i++;
    }

    eyelight_viewer_font_size_update(pres);
    printf("\n\n## presentation: %s\n",pres->presentation);
    printf("## Elt file: %s\n",pres->elt_file);
    printf("## Edc file: %s\n",pres->edc_file);
    printf("## Edje file: %s\n",pres->edje_file);
    printf("## Theme: %s\n",pres->theme);
    printf("## Number of slides: %d\n\n",pres->size);
}

/*
 * @brief set the edje file
 */
void eyelight_viewer_edje_file_set(Eyelight_Viewer* pres,char* edje_file)
{
    EYELIGHT_FREE(pres->edje_file);
    pres->edje_file = strdup(edje_file);
}


/*
 * @brief set the edc file
 */
void eyelight_viewer_edc_file_set(Eyelight_Viewer* pres,char* edc_file)
{
    EYELIGHT_FREE(pres->edc_file);
    pres->edc_file = strdup(edc_file);
}

/*
 * @brief set the elt file
 */
void eyelight_viewer_elt_file_set(Eyelight_Viewer* pres,char* elt_file)
{
    EYELIGHT_FREE(pres->elt_file);
    pres->elt_file = strdup(elt_file);
}

/*
 * @brief return the current state of the viewer (none, expose ...)
 */
Eyelight_Viewer_State eyelight_viewer_state_get(Eyelight_Viewer* pres)
{
    return pres->state;
}

/*
 * @brief set the presentation file
 * define the first file use to create the presentation
 * (an edc file if you display the presentation from an edc file)
 */
void eyelight_viewer_presentation_set(Eyelight_Viewer* pres,char* presentation)
{
    EYELIGHT_FREE(pres->presentation);
    pres->presentation = strdup(presentation);
}

/*
 * @brief set the theme
 */
void eyelight_viewer_theme_set(Eyelight_Viewer* pres,char* theme)
{
    EYELIGHT_FREE(pres->theme);
    pres->theme = strdup(theme);
}



/*
 * @resize all slides
 */
void eyelight_viewer_resize_screen(Eyelight_Viewer*pres, Evas_Coord w, Evas_Coord h)
{
    int i;
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
    eyelight_viewer_font_size_update(pres);

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
 * return the transition previous/next of a slide
 */
void eyelight_viewer_slide_transitions_get(Eyelight_Viewer* pres,int slide, const char** previous, const char** next)
{
    *previous = NULL;
    *next = NULL;
    if(pres->transition_effect_next[slide])
        *next = pres->transition_effect_next[slide];
    else
        *next = "none";

    if(pres->transition_effect_previous[slide])
        *previous = pres->transition_effect_previous[slide];
    else
        *previous = "none";
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

/*
 * @brief return a slide, load it if necessary
 */
Evas_Object* eyelight_viewer_slide_get(Eyelight_Viewer*pres,int pos)
{
    int w,h;

    if(!pres->slides[pos])
    {
        pres->slides[pos] = eyelight_viewer_slide_load(pres,pos);
        eyelight_viewer_slide_transitions_load(pres,pos);
        evas_object_move (pres->slides[pos], 0, 0);
        evas_output_viewport_get(pres->evas,NULL,NULL,&w,&h);
        evas_object_resize(pres->slides[pos],w,h);
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

    snprintf(buf,EYELIGHT_BUFLEN,"slide_%d",pos+1);
    slide = edje_object_add(pres->evas);
    if(edje_object_file_set(slide, pres->edje_file, buf) ==  0)
        printf("eyelight_viewer_slide_get(), edje_object_file_set() erreur! \n");

    return slide;
}


/*
 * @brief clear the slides we dont need
 */
void eyelight_viewer_clear(Eyelight_Viewer *pres)
{
    int i;
    for(i=0;i<pres->size;i++)
    {
        if(pres->slides[i] && (i<pres->current-1 || i>pres->current+1))
        {
            evas_object_del(pres->slides[i]);
            pres->slides[i]=NULL;
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
    eyelight_viewer_slide_transitions_get(pres,pres->current,&trans_previous, &trans_next);
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
    eyelight_viewer_slide_transitions_get(pres,pres->current,&trans_previous, &trans_next);

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



void eyelight_viewer_font_size_update(Eyelight_Viewer* pres)
{
    int w,h,i;
    double scale_w, scale_h,scale;
    evas_output_viewport_get(pres->evas,NULL,NULL,&w,&h);

    scale_w = w/(double)pres->default_size_w;
    scale_h = h/(double)pres->default_size_h;
    scale = (scale_w+scale_h)/2.0;

    ecore_list_first_goto(pres->text_class);
    Eyelight_Text_Class* class;
    while( (class = ecore_list_next(pres->text_class)))
    {
        int new_size = class->size*scale;
        edje_text_class_set(class->name,class->font,new_size);
    }
}

void eyelight_viewer_object_font_size_update(Eyelight_Viewer* pres,Evas_Object* o, int w, int h)
{
    double scale_w, scale_h,scale;

    scale_w = w/(double)pres->default_size_w;
    scale_h = h/(double)pres->default_size_h;
    scale = (scale_w+scale_h)/2.0;

    ecore_list_first_goto(pres->text_class);
    Eyelight_Text_Class* class;
    while( (class = ecore_list_next(pres->text_class)))
    {
        int new_size = class->size*scale;
        edje_object_text_class_set(o,class->name,class->font,new_size);
    }
}
