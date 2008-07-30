/* vim: set sw=4 ts=4 sts=4 expandtab: */
#include "elitaire_card_private.h"

#define VELOCITY(eli) ((eli)->velocity * (eli)->w / 500)

/* ****************************************************************************
 * Declarations 
 * ****************************************************************************/
static void _elitaire_card_shadow_on           (Evas_Object* card);
static void _elitaire_card_shadow_off          (Evas_Object* card);
static void _elitaire_card_drag                (Evas_Object* card);
static void _elitaire_card_drop                (Evas_Object* card);
static void _elitaire_card_fade_out            (Evas_Object* card);
static void _elitaire_card_fade_wait           (Evas_Object* card);
static void _elitaire_card_fade_in             (Evas_Object* card);

/*** some internals for the job managment ***/
static Elitaire_Card_Job *	_elitaire_card_job_new(Evas_Object * card);
static void _elitaire_card_job_timer_add(Elitaire_Job_List * list, void * data, int (*func)(void*));

static Evas_Bool _elitaire_card_job_slide(Elitaire_Job_List * list, 
                                                                void * data);
static Evas_Bool _elitaire_card_job_del_chain(Elitaire_Job_List * list, 
                                                                void * data);
static Evas_Bool _elitaire_card_job_flip(Elitaire_Job_List * list, void * data);
static Evas_Bool _elitaire_card_job_dragged(Elitaire_Job_List * list, 
                                                                void * data);
static Evas_Bool _elitaire_card_job_dragged(Elitaire_Job_List * list, 
                                                                void * data);
static Evas_Bool _elitaire_card_job_drag(Elitaire_Job_List * list, void * data);
static Evas_Bool _elitaire_card_job_drop(Elitaire_Job_List * list, void * data);
static Evas_Bool _elitaire_card_job_wait_mouse(Elitaire_Job_List * list, 
                                                                void * data);
static Evas_Bool _elitaire_card_job_wait_time(Elitaire_Job_List * list, 
                                                                void * data);
static Evas_Bool _elitaire_card_job_fade_in(Elitaire_Job_List * list, 
                                                                void * data);
static Evas_Bool _elitaire_card_job_fade_out(Elitaire_Job_List * list, 
                                                                void * data);

/*** callbacks ***/
static int _elitaire_card_slide_cb             (void * data);
static int _elitaire_card_flip_cb              (void * data);
static int _elitaire_card_dragged_cb           (void * data);
static int _elitaire_card_drag_cb              (void * data);
static int _elitaire_card_drop_cb              (void * data);
static int _elitaire_card_wait_cb              (void * data);
static int _elitaire_card_fade_in_cb           (void * data);
static int _elitaire_card_fade_out_cb          (void * data);
static int _elitaire_card_wait_time_cb	       (void * data);

/* ****************************************************************************
 * external API 
 * ****************************************************************************/
void 
elitaire_card_chain_del(Evas_Object * card)
{
    Elitaire_Card_Job * job;
    Elitaire_Card * ecard;

    ecard = (Elitaire_Card *) evas_object_smart_data_get(card);
    job = _elitaire_card_job_new(card);
    elitaire_job_add(ecard->jobs, _elitaire_card_job_del_chain, 
		    				ELITAIRE_JOB_SYNC_WAIT, job);
}

void 
elitaire_card_slide(Evas_Object * card, Evas_Coord x, Evas_Coord y)
{
    Elitaire_Card_Job * job;
    Elitaire_Card * ecard;

    ecard = (Elitaire_Card *) evas_object_smart_data_get(card);

    job = _elitaire_card_job_new(card);
    job->x.ec = x;
    job->y.ec = y;
    job->w = ecard->eli->w;

    _elitaire_card_drag(card);
    elitaire_job_add(ecard->jobs, _elitaire_card_job_slide, 
    	    			ELITAIRE_JOB_SYNC_NEXT_WAIT, job);
    
    if (!ecard->eli->shadow) _elitaire_card_drop(card);
}

void 
elitaire_card_flip(Evas_Object * card)
{
    Elitaire_Card_Job * job;
    Elitaire_Card * ecard;

    ecard = (Elitaire_Card *) evas_object_smart_data_get(card);
    job = _elitaire_card_job_new(card);
    job->y.ec = -1;
    _elitaire_card_drag(card);

    elitaire_job_add(ecard->jobs, _elitaire_card_job_flip, 
		    				ELITAIRE_JOB_SYNC_ALLOW, job);

    if (!ecard->eli->shadow) _elitaire_card_drop(card);
}

void 
elitaire_card_dragged(Evas_Object * card)
{
    Elitaire_Card_Job * job;
    Evas * e;
    Evas_Coord mx, my, x, y;
    Elitaire_Card * ecard;

    ecard = (Elitaire_Card *) evas_object_smart_data_get(card);
    e = evas_object_evas_get(card);

    evas_object_geometry_get(card, &x, &y, NULL, NULL);
    evas_pointer_canvas_xy_get(e, &mx, &my);

    job = _elitaire_card_job_new(card);
    job->x.ec = mx - x;
    job->y.ec = my - y;

    _elitaire_card_drag(card);
    elitaire_job_add(ecard->jobs, _elitaire_card_job_dragged, 
		    				ELITAIRE_JOB_SYNC_WAIT, job);
    
    if (!ecard->eli->shadow) _elitaire_card_drop(card);
}

void 
elitaire_card_fade(Evas_Object * card)
{
    _elitaire_card_fade_out(card);
    _elitaire_card_fade_wait(card);
    _elitaire_card_fade_in(card);
    elitaire_card_chain_del(card);
}

void 
elitaire_card_wait(Evas_Object * card, int t)
{
    Elitaire_Card_Job * job;
    Elitaire_Card * ecard;

    ecard = (Elitaire_Card *) evas_object_smart_data_get(card);

    job = _elitaire_card_job_new(card);
    job->x.i =
        (t * ecard->eli->card_h * ecard->eli->frame_rate) /
        (ecard->eli->velocity);
    elitaire_job_add(ecard->jobs, _elitaire_card_job_wait_time, 
		    				ELITAIRE_JOB_SYNC_WAIT, job);
}

/* ***************************************************************************
 * some interals for the job managment
 * ***************************************************************************/

/* allocate memory */
static Elitaire_Card_Job * 
_elitaire_card_job_new(Evas_Object * card)
{
    Elitaire_Card_Job * job;

    job = (Elitaire_Card_Job *) calloc(1, sizeof(Elitaire_Card_Job));
    job->card = card;
    job->timer = NULL;
    job->t = 0.0;
    elitaire_card_job_reg(elitaire_card_elitaire_get(card));
    
    return job;
}

static double 
_elitaire_card_job_frametime_get(Elitaire_Card_Job * job)
{
    double ft;

    if (job->t == 0.0) {
        Elitaire_Card * ecard;

        ecard = (Elitaire_Card *) evas_object_smart_data_get(job->card);
        ft = 1.0 / ecard->eli->frame_rate;
        job->t = ecore_time_get();
    }
    else {
        double nt;

        nt = ecore_time_get();
        ft = nt - job->t;
        job->t = nt;
    }

    return ft;
}

Evas_Bool 
elitaire_card_job_del_cb(Elitaire_Job_List * list, void * data)
{
    Elitaire_Card_Job * job;

    job = (Elitaire_Card_Job *) data;

    if (job->timer) {
        ecore_animator_del(job->timer);
        job->timer = NULL;
    }
    
    elitaire_card_job_unreg(elitaire_card_elitaire_get(job->card));
    if (data) free(data);
        
    return 1;
}

Evas_Bool 
elitaire_card_job_clean_up(Elitaire_Job_List * list, void * data)
{
    Elitaire_Card * ecard;
    Evas_Object * card;

    card = (Evas_Object *) data;
    ecard = (Elitaire_Card *) evas_object_smart_data_get(card);

    if (ecard->shadow) {
        _elitaire_card_drop(card);
        elitaire_card_chain_del(card);
        return 0;
    }
    
    if (elitaire_card_next_card_get(card)) elitaire_card_chain_delete(card);
    return 0;
}

static void 
_elitaire_card_job_timer_add(Elitaire_Job_List * list, void * data,
                                  int (*func) (void *))
{
    Elitaire_Card_Job * job;

    job = (Elitaire_Card_Job *) data;
    job->timer = ecore_animator_add(func, data);
}

static Evas_Bool 
_elitaire_card_job_slide(Elitaire_Job_List * list, void * data)
{
    _elitaire_card_job_timer_add(list, data, _elitaire_card_slide_cb);
    return 0;
}

static Evas_Bool 
_elitaire_card_job_del_chain(Elitaire_Job_List * list, void * data)
{
    Elitaire_Card_Job * job;

    job = (Elitaire_Card_Job *) data;

    elitaire_card_chain_delete(job->card);
    return 1;
}

static Evas_Bool 
_elitaire_card_job_flip(Elitaire_Job_List * list, void * data)
{
    Elitaire_Card_Job * job;
    Evas_Object * image;
    Elitaire_Card * eli_card;
    Evas_Coord x;

    job = (Elitaire_Card_Job *) data;
    eli_card = (Elitaire_Card *) evas_object_smart_data_get(job->card);
    
    image = eli_card->image;
    evas_object_geometry_get(image, &x, NULL, NULL, NULL);
    _elitaire_card_job_timer_add(list, data, _elitaire_card_flip_cb);

    return 0;
}

static Evas_Bool 
_elitaire_card_job_dragged(Elitaire_Job_List * list, void * data)
{
    _elitaire_card_job_timer_add(list, data, _elitaire_card_dragged_cb);
    return 0;
}

static Evas_Bool 
_elitaire_card_job_drag(Elitaire_Job_List * list, void * data)
{
    int fr;
    Elitaire_Card * ecard;
    Elitaire_Card_Job * job;

    job = (Elitaire_Card_Job *) data;
    ecard = (Elitaire_Card *) evas_object_smart_data_get(job->card);

    fr = ecard->eli->frame_rate;
    evas_object_raise(job->card);

    ecard->flying = true;

    if (ecard->eli->shadow) {
        _elitaire_card_shadow_on(job->card);
        job->timer = ecore_animator_add(_elitaire_card_drag_cb, data);
    }
    else {
        job->x.i = 1;
        job->timer = ecore_animator_add(_elitaire_card_wait_time_cb, data);
    }

    return 0;
}

static Evas_Bool 
_elitaire_card_job_drop(Elitaire_Job_List * list, void * data)
{
    int fr;
    Elitaire_Card * ecard;
    Elitaire_Card_Job * job;

    job = (Elitaire_Card_Job *) data;
    ecard = (Elitaire_Card *) evas_object_smart_data_get(job->card);

    fr = ecard->eli->frame_rate;

    ecard->flying = false;

    if (ecard->eli->shadow) ecore_animator_add(_elitaire_card_drop_cb, data);
    else {
        //elitaire_card_chain_del(job->card);
        elitaire_stack_layer_reinit(ecard->eli, ecard->pcard);
        return 1;
    }
    return 0;
}

static Evas_Bool 
_elitaire_card_job_wait_mouse(Elitaire_Job_List * list, void * data)
{
    _elitaire_card_job_timer_add(list, data, _elitaire_card_wait_cb);
    return 0;
}

static Evas_Bool 
_elitaire_card_job_wait_time(Elitaire_Job_List * list, void * data)
{
    int fr;
    Elitaire_Card * ecard;
    Elitaire_Card_Job * job;

    job = (Elitaire_Card_Job *) data;
    ecard = (Elitaire_Card *) evas_object_smart_data_get(job->card);

    fr = ecard->eli->frame_rate;

    ecard->waiting = true;
    job->timer = ecore_animator_add(_elitaire_card_wait_time_cb, data);

    return 0;
}

static Evas_Bool 
_elitaire_card_job_fade_in(Elitaire_Job_List * list, void * data)
{
    _elitaire_card_job_timer_add(list, data, _elitaire_card_fade_in_cb);
    return 0;
}

static Evas_Bool 
_elitaire_card_job_fade_out(Elitaire_Job_List * list, void * data)
{
    _elitaire_card_job_timer_add(list, data, _elitaire_card_fade_out_cb);
    return 0;
}

/* ***************************************************************************
 *  other internals
 * ***************************************************************************/
static void 
_elitaire_card_shadow_on(Evas_Object * card)
{
    Elitaire_Card * data = NULL;

    data = (Elitaire_Card *) evas_object_smart_data_get(card);

    if (data) {
        Evas_Coord x, y, w, h, dy;
        int n;

        if (data->shadow) return;
            
        data->shadow = edje_object_add(data->eli->evas);
        evas_object_smart_member_add(data->shadow, card);
        if (!edje_object_file_set(data->shadow, data->eli->file, "elitaire/card/shadow"))
            fprintf(stderr, _("Elitaire Error: The shadow pic is missed!\n"));

        n = elitaire_card_chain_length_calc(card);
        dy = elitaire_card_offset_get(data->eli);

        evas_object_geometry_get(card, &x, &y, &w, &h);
        evas_object_move(data->shadow, x, y);
        evas_object_resize(data->shadow, w, h + n * dy);
        evas_object_clip_set(data->shadow, data->eli->clip);
        evas_object_show(data->shadow);
        evas_object_stack_below(data->shadow, data->image);
    }
}

static void 
_elitaire_card_shadow_off(Evas_Object * card)
{
    Elitaire_Card * data = NULL;

    data = (Elitaire_Card *) evas_object_smart_data_get(card);

    if (data) {
        if (data->shadow) evas_object_del(data->shadow);
        data->shadow = NULL;
    }
}

static void 
_elitaire_card_drag(Evas_Object * card)
{
    Elitaire_Card_Job * job;
    Elitaire_Card * ecard;

    ecard = (Elitaire_Card *) evas_object_smart_data_get(card);

    job = _elitaire_card_job_new(card);
    elitaire_job_add(ecard->jobs, _elitaire_card_job_drag, 
                                                ELITAIRE_JOB_SYNC_ALLOW, job);
}

static void 
_elitaire_card_drop(Evas_Object * card)
{
    Elitaire_Card_Job * job;
    Elitaire_Card * ecard;

    ecard = (Elitaire_Card *) evas_object_smart_data_get(card);

    job = _elitaire_card_job_new(card);
    elitaire_job_add(ecard->jobs, _elitaire_card_job_drop, 
                                                ELITAIRE_JOB_SYNC_ALLOW, job);
}

static void 
_elitaire_card_fade_out(Evas_Object * card)
{
    Elitaire_Card_Job * job;
    Elitaire_Card * ecard;

    ecard = (Elitaire_Card *) evas_object_smart_data_get(card);

    job = _elitaire_card_job_new(card);
    job->x.i = 256;
    elitaire_job_add(ecard->jobs, _elitaire_card_job_fade_out, 
                                            ELITAIRE_JOB_SYNC_NEXT_WAIT, job);
}

static void 
_elitaire_card_fade_wait(Evas_Object * card)
{
    Elitaire_Card_Job * job;
    Elitaire_Card * ecard;

    ecard = (Elitaire_Card *) evas_object_smart_data_get(card);

    job = _elitaire_card_job_new(card);
    job->x.i = 2;
    elitaire_job_add(ecard->jobs, _elitaire_card_job_wait_mouse, 
                                            ELITAIRE_JOB_SYNC_WAIT, job);
}

static void 
_elitaire_card_fade_in(Evas_Object * card)
{
    Elitaire_Card_Job * job;
    Elitaire_Card * ecard;

    ecard = (Elitaire_Card *) evas_object_smart_data_get(card);

    job = _elitaire_card_job_new(card);
    job->x.i = 30;
    elitaire_job_add(ecard->jobs, _elitaire_card_job_fade_in, 
                                            ELITAIRE_JOB_SYNC_ALLOW, job);
}

/* ***************************************************************************
 * callbacks
 * ***************************************************************************/
static int 
_elitaire_card_slide_cb(void * data)
{
    Elitaire_Card_Job * job;
    Evas_Object * card;
    Elitaire_Card * eli_card;
    Evas_Coord to_x, to_y;
    Evas_Coord from_x, from_y;
    Evas_Coord new_x, new_y;
    double m, ft;

    job = (Elitaire_Card_Job *) data;
    card = job->card;
    eli_card = (Elitaire_Card *) evas_object_smart_data_get(card);
    ft = _elitaire_card_job_frametime_get(job);

    if (job->w == eli_card->eli->w) {
        to_x = job->x.ec;
        to_y = job->y.ec;
    }
    else {
        elitaire_card_xy_get(card, &to_x, &to_y);
        job->w = eli_card->eli->w;
        job->x.ec = to_x;
        job->y.ec = to_y;
    }

    evas_object_geometry_get(card, &from_x, &from_y, NULL, NULL);

    m = VELOCITY(eli_card->eli) * ft
        / sqrt((double) (SQR(to_x - from_x) + SQR(to_y - from_y)));

    new_x = (Evas_Coord) (m * (to_x - from_x)) + from_x;
    new_y = (Evas_Coord) (m * (to_y - from_y)) + from_y;

    if (ABS(new_x - from_x) >= ABS(to_x - from_x)
        && ABS(new_y - from_y) >= ABS(to_y - from_y)) {
        /* Job now ends */
        evas_object_move(card, to_x, to_y);
        elitaire_job_del(eli_card->jobs, data);
        return 0;
    }
    evas_object_move(card, new_x, new_y);

    return 1;
}

static int 
_elitaire_card_flip_cb(void * data)
{
    Elitaire_Card_Job * job;
    Evas_Object * card, * image, * shadow;
    Elitaire_Card * eli_card;
    Evas_Coord w, h;
    Evas_Coord im_x, im_w, im_y;
    Evas_Coord new_x, new_w;
    Evas_Coord sh_y, sh_x;
    Evas_Coord x;
    
    job = (Elitaire_Card_Job *) data;
    card = job->card;
    eli_card = (Elitaire_Card *) evas_object_smart_data_get(card);
    image = eli_card->image;
    shadow = eli_card->shadow;
    
    evas_object_geometry_get(card, &x, NULL, &w, &h);
    evas_object_geometry_get(image, &im_x, &im_y, &im_w, NULL);
    if (shadow)
        evas_object_geometry_get(shadow, &sh_x, &sh_y, NULL, NULL);
    
    new_w = im_w + job->y.ec * (Evas_Coord) (VELOCITY(eli_card->eli)
                                              / eli_card->eli->frame_rate);
    if (new_w < 0) {
        job->y.ec = 1;
        new_w = ABS(new_w);
        elitaire_card_reinit(card);
    }
    if (new_w >= w) {
        /* Job now ends */
        new_w = eli_card->eli->card_w;
        new_x = x;
        evas_object_resize(image, new_w, h);
        evas_object_move(image, new_x, im_y);
        if (shadow) {
            sh_x += (im_w - new_w) / 2;
            evas_object_resize(shadow, new_w, h);
            evas_object_move(shadow, sh_x, sh_y);
        }
        elitaire_job_del(eli_card->jobs, data);
        return 0;
    }
    new_x = im_x + (im_w - new_w) / 2;
    evas_object_resize(image, new_w, h);
    evas_object_move(image, new_x, im_y);
    if (shadow) {
        sh_x += (im_w - new_w) / 2;
        evas_object_resize(shadow, new_w, h);
        evas_object_move(shadow, sh_x, sh_y);
    }

    return 1;
}

static int 
_elitaire_card_dragged_cb(void * data)
{
    Elitaire_Card_Job * job;
    Evas_Object * card;
    Elitaire_Card * eli_card;
    Evas_Coord mx, my;
    Evas * evas;

    job = (Elitaire_Card_Job *) data;
    card = job->card;
    eli_card = (Elitaire_Card *) evas_object_smart_data_get(card);

    evas = evas_object_evas_get(card);
    evas_pointer_canvas_xy_get(evas, &mx, &my);

    if ((evas_pointer_button_down_mask_get(evas) - 1) % 2) {
        /* Job now ends */
        Evas_Coord to_x, to_y, x, y, w, h;
        Elitaire *eli;
        int clstack, moved = 0;

        eli = elitaire_card_elitaire_get(card);
        evas_object_geometry_get(card, &x, &y, &w, &h);

        clstack = elitaire_clicked_stack_get(eli, x + w / 2, y + h / 2);

        if (clstack >= 0)
            moved =
                eli->game->move(elitaire_card_playingCard_get(card), clstack);
        if (!moved) {
            if (eli->animate) {
                elitaire_card_xy_get(card, &to_x, &to_y);
                if (!(x == to_x && y == to_y))
                    elitaire_card_slide(card, to_x, to_y);
            }
            else
                elitaire_reinit(eli);
        }
        elitaire_job_del(eli_card->jobs, data);
        return 0;
    }

    evas_object_move(card, mx - job->x.ec, my - job->y.ec);

    return 1;
}

static int 
_elitaire_card_drag_cb(void * data)
{
    Elitaire_Card_Job * job;
    Elitaire_Card * eli_card;
    Evas_Coord x, y, w, d;
    Evas_Coord image_x, image_y;
    Evas_Object * card = NULL;

    job = (Elitaire_Card_Job *) data;
    card = job->card;
    eli_card = (Elitaire_Card *) evas_object_smart_data_get(card);

    evas_object_geometry_get(eli_card->shadow, &x, &y, &w, NULL);
    evas_object_geometry_get(eli_card->image, &image_x, &image_y, NULL, NULL);

    d = (Evas_Coord) (VELOCITY(eli_card->eli) / (eli_card->eli->frame_rate * 2));
    image_x -= d;
    image_y -= d;

    if (abs(x - image_x) >= w / 10) {
        /* Job now ends */
        evas_object_move(eli_card->image, x - w / 10, y - w / 10);
        
        if (eli_card->next_card) {
            Evas_Coord dy;

            dy = elitaire_card_offset_get(eli_card->eli);
            evas_object_move(eli_card->next_card, x - w / 10, y - w / 10 + dy);
        }

        elitaire_job_del(eli_card->jobs, data);
        return 0;
    }

    evas_object_move(eli_card->image, image_x, image_y);

    if (eli_card->next_card) {
        Evas_Coord dy;

        dy = elitaire_card_offset_get(eli_card->eli);
        evas_object_move(eli_card->next_card, image_x, image_y + dy);
    }

    return 1;
}

static int 
_elitaire_card_drop_cb(void * data)
{
    Elitaire_Card_Job * job;
    Evas_Object * card = NULL;
    Evas_Coord x, y, w, d;
    Evas_Coord image_x, image_y;
    Elitaire_Card * eli_card;

    job = (Elitaire_Card_Job *) data;
    card = job->card;

    eli_card = (Elitaire_Card *) evas_object_smart_data_get(card);

    evas_object_geometry_get(card, &x, &y, &w, NULL);
    evas_object_geometry_get(eli_card->image, &image_x, &image_y, NULL, NULL);

    d = (Evas_Coord) (VELOCITY(eli_card->eli) / eli_card->eli->frame_rate);
    image_x += d;
    image_y += d;

    if (image_x >= x || image_y >= y) {
        /* Job now ends */
        evas_object_move(eli_card->image, x, y);
        if (eli_card->next_card) {
            Evas_Coord dy;

            dy = elitaire_card_offset_get(eli_card->eli);
            evas_object_move(eli_card->next_card, x, y + dy);
        }
        _elitaire_card_shadow_off(card);
        elitaire_stack_layer_reinit(eli_card->eli, eli_card->pcard);
        elitaire_job_del(eli_card->jobs, data);
        return 0;
    }

    evas_object_move(eli_card->image, image_x, image_y);

    if (eli_card->next_card) {
        Evas_Coord dy;

        dy = elitaire_card_offset_get(eli_card->eli);
        evas_object_move(eli_card->next_card, image_x, image_y + dy);
    }
    return 1;
}

static int 
_elitaire_card_wait_cb(void * data)
{
    Evas * evas;
    Elitaire_Card_Job * job;

    job = (Elitaire_Card_Job *) data;
    evas = evas_object_evas_get(job->card);

    if (evas_pointer_button_down_mask_get(evas) & job->x.i) return 1;
    else {
        Elitaire_Card * eli_card;

        eli_card = (Elitaire_Card *) evas_object_smart_data_get(job->card);
        elitaire_job_del(eli_card->jobs, data);
        return 0;
    }
}

static int 
_elitaire_card_fade_in_cb(void * data)
{
    Elitaire_Card_Job * job;
    Evas_Object * card;
    Elitaire * eli;
    Elitaire_Card * eli_card;

    job = (Elitaire_Card_Job *) data;
    card = job->card;
    eli = elitaire_card_elitaire_get(card);
    eli_card = (Elitaire_Card *) evas_object_smart_data_get(card);

    job->x.i += (int) (eli->velocity / eli->frame_rate);

    if (job->x.i >= 256) {
        evas_object_color_set(card, 256, 256, 256, 256);
        elitaire_job_del(eli_card->jobs, data);
        return 0;
    }
    else {
        evas_object_color_set(card, job->x.i, job->x.i, job->x.i, job->x.i);
        return 1;
    }
}

static int 
_elitaire_card_fade_out_cb(void * data)
{
    Elitaire_Card_Job * job;
    Evas_Object * card;
    Elitaire * eli;
    Elitaire_Card * eli_card;

    job = (Elitaire_Card_Job *) data;
    card = job->card;
    eli_card = (Elitaire_Card *) evas_object_smart_data_get(card);
    eli = eli_card->eli;

    job->x.i -= (int) (eli->velocity / eli->frame_rate);

    if (job->x.i <= 30) {
        evas_object_color_set(card, 30, 30, 30, 30);
        elitaire_job_del(eli_card->jobs, data);
        return 0;
    }
    else {
        evas_object_color_set(card, job->x.i, job->x.i, job->x.i, job->x.i);
        return 1;
    }
}

static int 
_elitaire_card_wait_time_cb(void * data)
{
    Elitaire_Card_Job * job;
    Evas_Object * card;
    Elitaire_Card * eli_card;

    job = (Elitaire_Card_Job *) data;
    card = job->card;
    eli_card = (Elitaire_Card *) evas_object_smart_data_get(card);

    if (card && eli_card) {
        if (--(job->x.i)) return 1;
            
        eli_card->waiting = false;
        elitaire_job_del(eli_card->jobs, data);
        return 0;
    }
    return 0;
}

