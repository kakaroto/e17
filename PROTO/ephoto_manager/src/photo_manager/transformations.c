// vim:ts=8:sw=3:sts=8:noexpandtab:cino=>5n-3f0^-2{2

#include "photo_manager_private.h"
#include <stdio.h>
#include <errno.h>
#include <sys/types.h>
#include <unistd.h>
#include <sys/wait.h>


struct PM_Trans_Job
{
    PM_Trans_Job_Type type;
    const char *file;
    const char *res_file;

    PM_Trans_History *history;

    PM_Trans_Done_Cb cb;
    void *data;

    pid_t pid;
};

struct PM_Trans_History_Item
{
    const char *file;
    PM_Trans_Job_Type type;
};

struct PM_Trans_History
{
    Eina_List *history;
    PM_Trans_History_Item *current;

    const char *folder;
};

static Eina_List *l_jobs = NULL;
static PM_Trans_Job *job_current = NULL;
static pid_t pid_job_current = -1;
static int running = 0;

static void _pm_trans_history_add(PM_Trans_History *h, const char *file, PM_Trans_Job_Type type);

static void _job_free(PM_Trans_Job *job);
static void _job_next();
static void _wait_fork(void *data);
static void _end_cb(void *data);

static void _trans_rotate_180(Evas *evas, const char *file, const char *res);
static void _pm_trans_rotate_90(Evas *evas, const char *file, const char *res);
static void _pm_trans_rotate_R90(Evas *evas, const char *file, const char *res);
static void _pm_trans_flip_vertical(Evas *evas, const char *file, const char *res);
static void _pm_trans_flip_horizontal(Evas *evas, const char *file, const char *res);
static void _pm_trans_blur(Evas *evas, const char *file, const char *res);
static void _pm_trans_sharpen(Evas *evas, const char *file, const char *res);
static void _pm_trans_grayscale(Evas *evas, const char *file, const char *res);
static void _pm_trans_sepia(Evas *evas, const char *file, const char *res);


PM_Trans_Job *pm_trans_job_add(PM_Trans_History *h, const char *file, PM_Trans_Job_Type type, PM_Trans_Done_Cb cb, void *data)
{
    ASSERT_RETURN(h != NULL);
    ASSERT_RETURN(file!=NULL);

    PM_Trans_Job *job = calloc(1, sizeof(PM_Trans_Job));
    job->file =  eina_stringshare_add(file);
    job->type = type;
    job->cb = cb;
    job->data = data;
    job->history = h;

    l_jobs = eina_list_append(l_jobs, job);

    _job_next();

    return job;
}

void pm_trans_job_del(PM_Trans_Job *job)
{
    ASSERT_RETURN_VOID(job != NULL);

    if(job == job_current)
    {
        job_current = NULL;
        kill(pid_job_current, SIGKILL);
    }

    l_jobs = eina_list_remove(l_jobs, job);

    _job_free(job);
}

PM_Trans_History *pm_trans_history_new(const char *file)
{
    char buf[PATH_MAX];
    int i = 0;

    ASSERT_RETURN(file!= NULL);

    PM_Trans_History *h = calloc(1, sizeof(PM_Trans_History));

    snprintf(buf, PATH_MAX, TEMP"/ephoto_manager");
    if(!ecore_file_exists(buf))
        ecore_file_mkdir(buf);

    do
    {
        snprintf(buf, PATH_MAX, TEMP"/ephoto_manager/%d", i);
        i++;
    } while(ecore_file_exists(buf));

    ecore_file_mkdir(buf);
    h->folder = eina_stringshare_add(buf);
    
    snprintf(buf, PATH_MAX, "%s/%d%s", h->folder, 
            eina_list_count(h->history), strrchr(file,'.'));
    ecore_file_cp(file, buf);
    _pm_trans_history_add(h, buf, PM_TRANS_ROTATE_180);

    return h;
}

void pm_trans_history_free(PM_Trans_History *h)
{
    ASSERT_RETURN_VOID(h != NULL);

    pm_trans_history_clear(h);
    ecore_file_recursive_rm(h->folder);
    EINA_STRINGSHARE_DEL(h->folder);
    FREE(h);
}

const Eina_List *pm_trans_history_get(const PM_Trans_History *h)
{
    ASSERT_RETURN(h != NULL);
    return h->history;
}

const PM_Trans_History_Item *pm_trans_history_current_get(const PM_Trans_History *h)
{
    ASSERT_RETURN(h != NULL);
    return h->current;
}


void pm_trans_history_clear(PM_Trans_History *h)
{
    PM_Trans_History_Item *item;
    ASSERT_RETURN_VOID(h != NULL);

    EINA_LIST_FREE(h->history, item)
    {
        ecore_file_recursive_rm(item->file);
        EINA_STRINGSHARE_DEL(item->file);
        FREE(item);
    }
    h->current = NULL;
}

const char *pm_trans_history_goto(PM_Trans_History *h, int pos)
{
    ASSERT_RETURN(h != NULL);
    ASSERT_RETURN(pos >= 0 && pos < eina_list_count(h->history));

    h->current = eina_list_nth(h->history, pos);
    return h->current->file;
}

const char *pm_trans_history_item_file_get(const PM_Trans_History_Item *item)
{
    ASSERT_RETURN(item != NULL);
    return item->file;
}

PM_Trans_Job_Type pm_trans_history_item_type_get(const PM_Trans_History_Item *item)
{
    ASSERT_RETURN(item != NULL);
    return item->type;
}

static void _pm_trans_history_add(PM_Trans_History *h, const char *file, PM_Trans_Job_Type type)
{
    PM_Trans_History_Item *item;
    Eina_List *l, *l2;

    ASSERT_RETURN_VOID(h != NULL);
    ASSERT_RETURN_VOID(file != NULL);

    l = eina_list_data_find_list(h->history, h->current);
    l = eina_list_next(l);
    EINA_LIST_FOREACH(l, l2, item)
    {
        h->history = eina_list_remove(h->history, item);
        ecore_file_recursive_rm(item->file);
        EINA_STRINGSHARE_DEL(item->file);
        FREE(item);
    }

    item = calloc(1, sizeof(PM_Trans_History_Item));
    item->file = eina_stringshare_add(file);
    item->type = type;


    h->history = eina_list_append(h->history, item);
    h->current = item;
}

static void _job_free(PM_Trans_Job *job)
{
    ASSERT_RETURN_VOID(job != NULL);

    EINA_STRINGSHARE_DEL(job->file);
    EINA_STRINGSHARE_DEL(job->res_file);
    FREE(job);
}

static void _job_next()
{
    Ecore_Evas *ee;
    Evas *evas;
    char buf[PATH_MAX];

    if(running)
        return;

    if(!l_jobs)
        return ;

    running = 1;
    PM_Trans_Job *job = eina_list_nth(l_jobs,0);
    l_jobs = eina_list_remove(l_jobs, job);
    job_current = job;

    snprintf(buf, PATH_MAX, "%s/%d%s", job->history->folder, 
            eina_list_count(job->history->history), strrchr(job->file,'.'));
    job->res_file = eina_stringshare_add(buf);

    pid_job_current = fork();

    switch(pid_job_current) {
        case -1:   
            LOG_ERR("Can not create a fork : (%d) %s",errno,sys_errlist[errno]);

            _job_free(job);
            _job_next();
            break;
        case 0:
            ee = ecore_evas_buffer_new(1,1);
            evas = ecore_evas_get(ee);
            switch(job->type)
            {
                case PM_TRANS_ROTATE_180:
                    _trans_rotate_180(evas, job->file, buf);
                    break;
                case PM_TRANS_ROTATE_90:
                    _pm_trans_rotate_90(evas, job->file, buf);
                    break;
                case PM_TRANS_ROTATE_R90:
                    _pm_trans_rotate_R90(evas, job->file, buf);
                    break;
                case PM_TRANS_FLIP_VERTICAL:
                    _pm_trans_flip_vertical(evas, job->file, buf);
                    break;
                case PM_TRANS_FLIP_HORIZONTAL:
                    _pm_trans_flip_horizontal(evas, job->file, buf);
                    break;
                case PM_TRANS_BLUR:
                    _pm_trans_blur(evas, job->file, buf);
                    break;
                case PM_TRANS_SHARPEN:
                    _pm_trans_sharpen(evas, job->file, buf);
                    break;                    
                case PM_TRANS_GRAYSCALE:
                    _pm_trans_grayscale(evas, job->file, buf);
                    break;
                case PM_TRANS_SEPIA:
                    _pm_trans_sepia(evas, job->file, buf);
                    break;
            }
            ecore_evas_free(ee);
            exit(1);
        default:   
            job->pid = pid_job_current;
            ecore_thread_run(_wait_fork, _end_cb, job);
    }
}




static void _wait_fork(void *data)
{
    int status;
    PM_Trans_Job *job = data;
    waitpid(job->pid, &status, 0);
}

static void _end_cb(void *data)
{   
    PM_Trans_Job *job = data;
    
    if(!job_current) //job deleted by the user
        goto end;

    if(job->history)
        _pm_trans_history_add(job->history, job->res_file, job->type);

    if(job->cb)
        job->cb(job->data, job, job->res_file);

    _job_free(job);
end:
    running = 0;
    _job_next();
}

static void _trans_rotate_180(Evas *evas, const char *file, const char *res)
{
    ASSERT_RETURN_VOID(file !=NULL);
    ASSERT_RETURN_VOID(res !=NULL);

    unsigned int   *data;
    unsigned int   *p1, *p2, tmp;
    int             x, hw, iw, ih;

    LOG_INFO("Start rotation 180° on %s", file);

    Evas_Object *obj = evas_object_image_add(evas);
    evas_object_image_file_set(obj, file, NULL);

    evas_object_image_size_get(obj, &iw, &ih);
    data = evas_object_image_data_get(obj, 1);

    hw = iw * ih;
    x = (hw / 2);
    p1 = data;
    p2 = data + hw - 1;
    for (; --x > 0;)
    {
        tmp = *p1;
        *p1 = *p2;
        *p2 = tmp;
        p1++;
        p2--;
    }
    evas_object_image_data_set(obj, data);
    evas_object_image_data_update_add(obj, 0, 0, iw, ih);

    evas_object_image_save(obj, res, NULL, "quality=100 compress=9");
    evas_object_del(obj);

    LOG_INFO("End rotation 180° on %s", file);
}

static void _pm_trans_rotate_90(Evas *evas, const char *file, const char *res)
{
    ASSERT_RETURN_VOID(file !=NULL);
    ASSERT_RETURN_VOID(res !=NULL);

    unsigned int *im_data, *im_data_new;
    int index, ind, i, j, ni, nj, ew, eh, nw, nh;

    LOG_INFO("Start rotation 90° on %s", file);

    Evas_Object *obj = evas_object_image_add(evas);
    evas_object_image_file_set(obj, file, NULL);

    evas_object_image_size_get(obj, &ew, &eh);
    im_data = evas_object_image_data_get(obj, 0);

    index = 0;

    im_data_new = malloc(sizeof(unsigned int) * ew * eh);

    nw = eh;
    nh = ew;

    for (i = 0; i < nh; i++)
    {
        for (j = 0; j < nw; j++)
        {
            ni = j;
            nj = nh - i - 1;

            ind = ni * nh + nj;

            im_data_new[index] = im_data[ind];

            index++;
        }
    }

    evas_object_image_size_set(obj, eh, ew);
    evas_object_image_data_set(obj, im_data_new);
    evas_object_image_data_update_add(obj, 0, 0, eh, ew);

    evas_object_image_save(obj, res, NULL, "quality=100 compress=9");
    evas_object_del(obj);

    LOG_INFO("End rotation 90° on %s", file);
}

static void _pm_trans_rotate_R90(Evas *evas, const char *file, const char *res)
{
    ASSERT_RETURN_VOID(file !=NULL);
    ASSERT_RETURN_VOID(res !=NULL);

    unsigned int *im_data, *im_data_new;
    int index, ind, i, j, ni, nj, ew, eh, nw, nh;

    LOG_INFO("Start rotation -90° on %s", file);

    Evas_Object *obj = evas_object_image_add(evas);
    evas_object_image_file_set(obj, file, NULL);

    evas_object_image_size_get(obj, &ew, &eh);
    im_data = evas_object_image_data_get(obj, 0);

    index = 0;

    im_data_new = malloc(sizeof(unsigned int) * ew * eh);

    nw = eh;
    nh = ew;

    for (i = 0; i < nh; i++)
    {
        for (j = 0; j < nw; j++)
        {
            ni = nw - j - 1;
            nj = i;

            ind = ni * nh + nj;

            im_data_new[index] = im_data[ind];

            index++;
        }
    }

    evas_object_image_size_set(obj, eh, ew);
    evas_object_image_data_set(obj, im_data_new);
    evas_object_image_data_update_add(obj, 0, 0, eh, ew);

    evas_object_image_save(obj, res, NULL, "quality=100 compress=9");
    evas_object_del(obj);

    LOG_INFO("End rotation -90° on %s", file);
}

static void _pm_trans_flip_vertical(Evas *evas, const char *file, const char *res)
{
    ASSERT_RETURN_VOID(file !=NULL);
    ASSERT_RETURN_VOID(res !=NULL);

    unsigned int   *data;
    unsigned int   *p1, *p2, tmp;
    int             x, y, iw, ih;

    LOG_INFO("Start vertical flip on %s", file);

    Evas_Object *obj = evas_object_image_add(evas);
    evas_object_image_file_set(obj, file, NULL);

    evas_object_image_size_get(obj, &iw, &ih);
    data = evas_object_image_data_get(obj, 1);

    for (y = 0; y < (ih >> 1); y++)
    {
        p1 = data + (y * iw);
        p2 = data + ((ih - 1 - y) * iw);
        for (x = 0; x < iw; x++)
        {
            tmp = *p1;
            *p1 = *p2;
            *p2 = tmp;
            p1++;
            p2++;
        }
    }

    evas_object_image_data_set(obj, data);
    evas_object_image_data_update_add(obj, 0, 0, iw, ih);

    evas_object_image_save(obj, res, NULL, "quality=100 compress=9");
    evas_object_del(obj);

    LOG_INFO("End vertical flip on %s", file);
}

static void _pm_trans_flip_horizontal(Evas *evas, const char *file, const char *res)
{
    ASSERT_RETURN_VOID(file !=NULL);
    ASSERT_RETURN_VOID(res !=NULL);

    unsigned int   *data;
    unsigned int   *p1, *p2, tmp;
    int             x, y, iw, ih;

    LOG_INFO("Start horizontal flip on %s", file);

    Evas_Object *obj = evas_object_image_add(evas);
    evas_object_image_file_set(obj, file, NULL);

    evas_object_image_size_get(obj, &iw, &ih);
    data = evas_object_image_data_get(obj, 1);

    for (y = 0; y < ih; y++)
    {
        p1 = data + (y * iw);
        p2 = data + ((y + 1) * iw) - 1;
        for (x = 0; x < (iw >> 1); x++)
        {
            tmp = *p1;
            *p1 = *p2;
            *p2 = tmp;
            p1++;
            p2--;
        }
    }

    evas_object_image_data_set(obj, data);
    evas_object_image_data_update_add(obj, 0, 0, iw, ih);

    evas_object_image_save(obj, res, NULL, "quality=100 compress=9");
    evas_object_del(obj);

    LOG_INFO("End horizontal flip on %s", file);
}

static void _pm_trans_blur(Evas *evas, const char *file, const char *res)
{
    ASSERT_RETURN_VOID(file !=NULL);
    ASSERT_RETURN_VOID(res !=NULL);

    unsigned int *im_data, *im_data_new, *p1, *p2;
    int *as, *rs, *gs, *bs;
    int rad = 2;
    int x, y, w, h, mx, my, mw, mh, mt, xx, yy;
    int a, r, g, b;

    LOG_INFO("Start blur on %s", file);

    Evas_Object *obj = evas_object_image_add(evas);
    evas_object_image_file_set(obj, file, NULL);

    evas_object_image_size_get(obj, &w, &h);
    im_data = evas_object_image_data_get(obj, 0);

    im_data_new = malloc(sizeof(unsigned int) * w * h);
    as = malloc(sizeof(int) * w);
    rs = malloc(sizeof(int) * w);
    gs = malloc(sizeof(int) * w);
    bs = malloc(sizeof(int) * w);

    for (y = 0; y < h; y++)
    {
        my = y - rad;
        mh = (rad << 1) + 1;
        if (my < 0)
        {
            mh += my;
            my = 0;
        }
        if ((my + mh) > h)
        {
            mh = h - my;
        }
        p1 = im_data_new + (y * w);
        memset(as, 0, w * sizeof(int));
        memset(rs, 0, w * sizeof(int));
        memset(gs, 0, w * sizeof(int));
        memset(bs, 0, w * sizeof(int));

        for (yy = 0; yy < mh; yy++)
        {
            p2 = im_data + ((yy + my) * w);
            for (x = 0; x < w; x++)
            {
                as[x] += (*p2 >> 24) & 0xff;
                rs[x] += (*p2 >> 16) & 0xff;
                gs[x] += (*p2 >> 8) & 0xff;
                bs[x] += *p2 & 0xff;
                p2++;
            }
        }
        if (w > ((rad << 1) + 1))
        {
            for (x = 0; x < w; x++)
            {
                a = 0;
                r = 0;
                g = 0;
                b = 0;
                mx = x - rad;
                mw = (rad << 1) + 1;
                if (mx < 0)
                {
                    mw += mx;
                    mx = 0;
                }
                if ((mx + mw) > w)
                {
                    mw = w - mx;
                }
                mt = mw * mh;
                for (xx = mx; xx < (mw + mx); xx++)
                {
                    a += as[xx];
                    r += rs[xx];
                    g += gs[xx];
                    b += bs[xx];
                }
                a = a / mt;
                r = r / mt;
                g = g / mt;
                b = b / mt;
                *p1 = (a << 24) | (r << 16) | (g << 8) | b;
                p1 ++;
            }
        }
    }
    free(as);
    free(rs);
    free(gs);
    free(bs);

    evas_object_image_size_set(obj, w, h);
    evas_object_image_data_set(obj, im_data_new);
    evas_object_image_data_update_add(obj, 0, 0, w, h);

    evas_object_image_save(obj, res, NULL, "quality=100 compress=9");
    evas_object_del(obj);

    LOG_INFO("End blur on %s", file);
}


static void _pm_trans_sharpen(Evas *evas, const char *file, const char *res)
{
    ASSERT_RETURN_VOID(file !=NULL);
    ASSERT_RETURN_VOID(res !=NULL);

    unsigned int *im_data, *im_data_new, *p1, *p2;
    int a, r, g, b, x, y, w, h;
    int mul, mul2, tot;
    int rad = 2;

    LOG_INFO("Start Sharpen on %s", file);

    Evas_Object *obj = evas_object_image_add(evas);
    evas_object_image_file_set(obj, file, NULL);

    evas_object_image_size_get(obj, &w, &h);
    im_data = evas_object_image_data_get(obj, 0);

    im_data_new = malloc(sizeof(unsigned int) * w * h);

    mul = (rad * 4) + 1;
    mul2 = rad;
    tot = mul - (mul2 * 4);
    for (y = 1; y < (h - 1); y ++)
    {
        p1 = im_data + 1 + (y * w);
        p2 = im_data_new + 1 + (y * w);
        for (x = 1; x < (w - 1); x++)
        {
            b = (int)((p1[0]) & 0xff) * 5;
            g = (int)((p1[0] >> 8) & 0xff) * 5;
            r = (int)((p1[0] >> 16) & 0xff) * 5;
            a = (int)((p1[0] >> 24) & 0xff) * 5;
            b -= (int)((p1[-1]) & 0xff);
            g -= (int)((p1[-1] >> 8) & 0xff);
            r -= (int)((p1[-1] >> 16) & 0xff);
            a -= (int)((p1[-1] >> 24) & 0xff);
            b -= (int)((p1[1]) & 0xff);
            g -= (int)((p1[1] >> 8) & 0xff);
            r -= (int)((p1[1] >> 16) & 0xff);
            a -= (int)((p1[1] >> 24) & 0xff);
            b -= (int)((p1[-w]) & 0xff);
            g -= (int)((p1[-w] >> 8) & 0xff);
            r -= (int)((p1[-w] >> 16) & 0xff);
            a -= (int)((p1[-w] >> 24) & 0xff);
            b -= (int)((p1[-w]) & 0xff);
            g -= (int)((p1[-w] >> 8) & 0xff);
            r -= (int)((p1[-w] >> 16) & 0xff);
            a -= (int)((p1[-w] >> 24) & 0xff);

            a = (a & ((~a) >> 16));
            a = ((a | ((a & 256) - ((a & 256) >> 8))));
            r = (r & ((~r) >> 16));
            r = ((r | ((r & 256) - ((r & 256) >> 8))));
            g = (g & ((~g) >> 16));
            g = ((g | ((g & 256) - ((g & 256) >> 8))));
            b = (b & ((~b) >> 16));
            b = ((b | ((b & 256) - ((b & 256) >> 8))));

            *p2 = (a << 24) | (r << 16) | (g << 8) | b;
            p2++;
            p1++;
        }
    }

    evas_object_image_size_set(obj, w, h);
    evas_object_image_data_set(obj, im_data_new);
    evas_object_image_data_update_add(obj, 0, 0, w, h);

    evas_object_image_save(obj, res, NULL, "quality=100 compress=9");
    evas_object_del(obj);

    LOG_INFO("End Sharpen on %s", file);
}


static void _pm_trans_grayscale(Evas *evas, const char *file, const char *res)
{
    ASSERT_RETURN_VOID(file !=NULL);
    ASSERT_RETURN_VOID(res !=NULL);

    unsigned int *im_data, *im_data_new;
    int gray, i, r, g, b, a, ew, eh;

    LOG_INFO("Start Grayscale on %s", file);

    Evas_Object *obj = evas_object_image_add(evas);
    evas_object_image_file_set(obj, file, NULL);

    evas_object_image_size_get(obj, &ew, &eh);
    im_data = evas_object_image_data_get(obj, 0);

    im_data_new = malloc(sizeof(unsigned int) * ew * eh);

    for (i = 0; i < (ew * eh); i++)
    {
        b = (int)((im_data[i]) & 0xff);
        g = (int)((im_data[i] >> 8) & 0xff);
        r = (int)((im_data[i] >> 16) & 0xff);
        a = (int)((im_data[i] >> 24) & 0xff);

        if (a > 0 && a < 255)
        {
            b = b * (255 / a);
            g = g * (255 / a);
            r = r * (255 / a);
        }

        gray = (int)((0.3 * r) + (0.59 * g) + (0.11 * b));

        if (a >= 0 && a < 255) gray = (gray * a) / 255;

        im_data_new[i] = (a << 24) | (gray << 16) | (gray << 8) | gray;
    }

    evas_object_image_size_set(obj, ew, eh);
    evas_object_image_data_set(obj, im_data_new);
    evas_object_image_data_update_add(obj, 0, 0, ew, eh);

    evas_object_image_save(obj, res, NULL, "quality=100 compress=9");
    evas_object_del(obj);

    LOG_INFO("End Grayscale on %s", file);
}


static void _pm_trans_sepia(Evas *evas, const char *file, const char *res)
{
    ASSERT_RETURN_VOID(file !=NULL);
    ASSERT_RETURN_VOID(res !=NULL);

    unsigned int *im_data, *im_data_new;
    int i, r, rr, g, gg, b, bb, a, ew, eh;

    LOG_INFO("Start Sepia on %s", file);

    Evas_Object *obj = evas_object_image_add(evas);
    evas_object_image_file_set(obj, file, NULL);

    evas_object_image_size_get(obj, &ew, &eh);
    im_data = evas_object_image_data_get(obj, 0);

    im_data_new = malloc(sizeof(unsigned int) * ew * eh);

    for (i = 0; i < (ew * eh); i++)
    {
        b = (int)((im_data[i]) & 0xff);
        g = (int)((im_data[i] >> 8) & 0xff);
        r = (int)((im_data[i] >> 16) & 0xff);
        a = (int)((im_data[i] >> 24) & 0xff);

        if (a > 0 && a < 255)
        {
            b = b * (255 / a);
            g = g * (255 / a);
            r = r * (255 / a);
        }

        rr = (int)(((r + g + b)/3)+40);
        if (rr < 0) rr = 0;
        if (rr > 255) rr = 255;
        gg = (int)(((r + g + b)/3)+2);
        if (gg < 0) gg = 0;
        if (gg > 255) gg = 255;
        bb = (int)(((r + g + b)/3)+2);             
        if (bb < 0) bb = 0;
        if (bb > 255) bb = 255;

        if (a >= 0 && a < 255)
        {
            rr = (rr * a) / 255;
            gg = (gg * a) / 255;
            bb = (bb * a) / 255;
        }

        im_data_new[i] = (a << 24) | (rr << 16) | (gg << 8) | bb;
    }

    evas_object_image_size_set(obj, ew, eh);
    evas_object_image_data_set(obj, im_data_new);
    evas_object_image_data_update_add(obj, 0, 0, ew, eh);

    evas_object_image_save(obj, res, NULL, "quality=100 compress=9");
    evas_object_del(obj);

    LOG_INFO("End Sepia on %s", file);
}

