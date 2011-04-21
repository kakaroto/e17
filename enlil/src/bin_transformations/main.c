#include "../../config.h"

#include <Ecore_Getopt.h>
#include <Ecore_Evas.h>
#include <Evas.h>

#include "Enlil.h"
#include "../define.h"

typedef void (*Trans_CB) (Evas *evas, const char *file, const char *res);
int LOG_DOMAIN;

typedef struct
{
    char *trans;
    Trans_CB cb;
} Tab_Struct;


static void _trans_rotate_180(Evas *evas, const char *file, const char *res);
static void _enlil_trans_rotate_90(Evas *evas, const char *file, const char *res);
static void _enlil_trans_rotate_R90(Evas *evas, const char *file, const char *res);
static void _enlil_trans_flip_vertical(Evas *evas, const char *file, const char *res);
static void _enlil_trans_flip_horizontal(Evas *evas, const char *file, const char *res);
static void _enlil_trans_blur(Evas *evas, const char *file, const char *res);
static void _enlil_trans_sharpen(Evas *evas, const char *file, const char *res);
static void _enlil_trans_grayscale(Evas *evas, const char *file, const char *res);
static void _enlil_trans_sepia(Evas *evas, const char *file, const char *res);

Tab_Struct tab[] = 
{
    { "Enlil_TRANS_ROTATE_180", _trans_rotate_180 },
    { "Enlil_TRANS_ROTATE_90",  _enlil_trans_rotate_90},
    { "Enlil_TRANS_ROTATE_R90", _enlil_trans_rotate_R90},
    { "Enlil_TRANS_FLIP_VERTICAL", _enlil_trans_flip_vertical},
    { "Enlil_TRANS_FLIP_HORIZONTAL",  _enlil_trans_flip_horizontal},
    { "Enlil_TRANS_BLUR", _enlil_trans_blur},
    { "Enlil_TRANS_SHARPEN", _enlil_trans_sharpen},
    { "Enlil_TRANS_GRAYSCALE", _enlil_trans_grayscale},
    { "Enlil_TRANS_SEPIA", _enlil_trans_sepia}
};



static const Ecore_Getopt options = {
    "Apply a transformation to an image (rotation, flip ...)",
    NULL,
    VERSION,
    "(C) 2009 Enlil transformations, see AUTHORS.",
    "LGPL with advertisement, see COPYING",
    "\n\n",
        1,
        {
            ECORE_GETOPT_VERSION('V', "version"),
            ECORE_GETOPT_COPYRIGHT('R', "copyright"),
            ECORE_GETOPT_LICENSE('L', "license"),
            ECORE_GETOPT_STORE_STR('i', "fin", "The source file."),
            ECORE_GETOPT_STORE_STR('o', "fout", "The output file."),
            ECORE_GETOPT_STORE_STR('t', "trans", "The transformation name."),
            ECORE_GETOPT_HELP('h', "help"),
            ECORE_GETOPT_SENTINEL
        }
};

int main(int argc, char **argv)
{
    unsigned char exit_option = 0;
    char *fin = NULL, *fout = NULL, *trans = NULL;
    Ecore_Evas *ee;
    Evas *evas;
    int i;

    enlil_init();

    //ecore_getopt
    Ecore_Getopt_Value values[] = {
        ECORE_GETOPT_VALUE_BOOL(exit_option),
        ECORE_GETOPT_VALUE_BOOL(exit_option),
        ECORE_GETOPT_VALUE_BOOL(exit_option),
        ECORE_GETOPT_VALUE_STR(fin),
        ECORE_GETOPT_VALUE_STR(fout),
        ECORE_GETOPT_VALUE_STR(trans),
        ECORE_GETOPT_VALUE_BOOL(exit_option),
    };
    ecore_app_args_set(argc, (const char **) argv);
    int nonargs = ecore_getopt_parse(&options, values, argc, argv);
    if (nonargs < 0)
        return 0;
    else if (nonargs != argc)
    {
        fputs("Invalid non-option argument\n", stderr);
        ecore_getopt_help(stderr, &options);
        return 1;
    }

    if(exit_option)
        return 0;

    if(!fin || !fout || !trans)
    {
        fputs("Missing argument\n", stderr);
        ecore_getopt_help(stderr, &options);
        return 0; 
    }
    //

    LOG_DOMAIN = eina_log_domain_register("enlil_transformations", "\033[34;1m");

    ee = ecore_evas_buffer_new(1,1);
    evas = ecore_evas_get(ee);

    for (i = 0; i < (int)sizeof (tab) / (int)sizeof (Trans_CB); ++i)
        if (strcmp(trans, tab[i].trans) == 0)
        {
            tab[i].cb(evas, fin, fout);
            break;
        }


    ecore_evas_free(ee);

    eina_log_domain_unregister(LOG_DOMAIN);

    return 1;
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

static void _enlil_trans_rotate_90(Evas *evas, const char *file, const char *res)
{
    ASSERT_RETURN_VOID(file !=NULL);
    ASSERT_RETURN_VOID(res !=NULL);

    unsigned int *im_data, *im_data_new;
    int _index, ind, i, j, ni, nj, ew, eh, nw, nh;

    LOG_INFO("Start rotation 90° on %s", file);

    Evas_Object *obj = evas_object_image_add(evas);
    evas_object_image_file_set(obj, file, NULL);

    evas_object_image_size_get(obj, &ew, &eh);
    im_data = evas_object_image_data_get(obj, 0);

    _index = 0;

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

            im_data_new[_index] = im_data[ind];

            _index++;
        }
    }

    evas_object_image_size_set(obj, eh, ew);
    evas_object_image_data_set(obj, im_data_new);
    evas_object_image_data_update_add(obj, 0, 0, eh, ew);

    evas_object_image_save(obj, res, NULL, "quality=100 compress=9");
    evas_object_del(obj);

    LOG_INFO("End rotation 90° on %s", file);
}

static void _enlil_trans_rotate_R90(Evas *evas, const char *file, const char *res)
{
    ASSERT_RETURN_VOID(file !=NULL);
    ASSERT_RETURN_VOID(res !=NULL);

    unsigned int *im_data, *im_data_new;
    int _index, ind, i, j, ni, nj, ew, eh, nw, nh;

    LOG_INFO("Start rotation -90° on %s", file);

    Evas_Object *obj = evas_object_image_add(evas);
    evas_object_image_file_set(obj, file, NULL);

    evas_object_image_size_get(obj, &ew, &eh);
    im_data = evas_object_image_data_get(obj, 0);

    _index = 0;

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

            im_data_new[_index] = im_data[ind];

            _index++;
        }
    }

    evas_object_image_size_set(obj, eh, ew);
    evas_object_image_data_set(obj, im_data_new);
    evas_object_image_data_update_add(obj, 0, 0, eh, ew);

    evas_object_image_save(obj, res, NULL, "quality=100 compress=9");
    evas_object_del(obj);

    LOG_INFO("End rotation -90° on %s", file);
}

static void _enlil_trans_flip_vertical(Evas *evas, const char *file, const char *res)
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

static void _enlil_trans_flip_horizontal(Evas *evas, const char *file, const char *res)
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

static void _enlil_trans_blur(Evas *evas, const char *file, const char *res)
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


static void _enlil_trans_sharpen(Evas *evas, const char *file, const char *res)
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


static void _enlil_trans_grayscale(Evas *evas, const char *file, const char *res)
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


static void _enlil_trans_sepia(Evas *evas, const char *file, const char *res)
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


