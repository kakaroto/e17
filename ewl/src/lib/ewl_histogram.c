/* vim: set sw=8 ts=8 sts=8 expandtab: */
#include "ewl_base.h"
#include "ewl_histogram.h"
#include "ewl_macros.h"
#include "ewl_private.h"
#include "ewl_debug.h"

static void ewl_histogram_cb_data_load(Ewl_Widget *w, void *ev, void *h);
static void ewl_histogram_draw(Ewl_Histogram *hist);

/**
 * @return Returns a new Ewl_Histogram widget.
 * @brief Create a new Ewl_Histogram widget.
 */
Ewl_Widget *
ewl_histogram_new(void)
{
        Ewl_Histogram *hist;

        DENTER_FUNCTION(DLEVEL_STABLE);

        hist = NEW(Ewl_Histogram, 1);
        if (!hist)
                DRETURN_PTR(NULL, DLEVEL_STABLE);

        if (!ewl_histogram_init(hist)) {
                FREE(hist);
        }

        DRETURN_PTR(EWL_WIDGET(hist), DLEVEL_STABLE);
}

/**
 * @param hist: the histogram widget to initialize
 * @return Returns TRUE on success, FALSE on failure.
 * @brief Initialize a histogram widget to starting values.
 */
int
ewl_histogram_init(Ewl_Histogram *hist)
{
        Ewl_Widget *w = EWL_WIDGET(hist);

        DENTER_FUNCTION(DLEVEL_STABLE);
        DCHECK_PARAM_PTR_RET(hist, FALSE);

        if (!ewl_image_init(EWL_IMAGE(hist)))
                DRETURN_INT(FALSE, DLEVEL_STABLE);

        ewl_widget_appearance_set(w, EWL_HISTOGRAM_TYPE);
        ewl_widget_inherit(w, EWL_HISTOGRAM_TYPE);
        ewl_callback_append(w, EWL_CALLBACK_CONFIGURE,
                                    ewl_histogram_cb_configure, NULL);
        ewl_object_preferred_inner_size_set(EWL_OBJECT(hist), 256, 256);
        hist->channel = EWL_HISTOGRAM_CHANNEL_R;

        DRETURN_INT(TRUE, DLEVEL_STABLE);
}

/**
 * @param hist: the histogram widget to change display color
 * @param r: red value for histogram drawing color
 * @param g: green value for histogram drawing color
 * @param b: blue value for histogram drawing color
 * @param a: apha value for histogram drawing color
 * @return Returns no value.
 * @brief Changes the drawing color of a histogram.
 */
void
ewl_histogram_color_set(Ewl_Histogram *hist, unsigned int r, unsigned int g,
                                                unsigned int b, unsigned int a)
{
        DENTER_FUNCTION(DLEVEL_STABLE);
        DCHECK_PARAM_PTR(hist);
        DCHECK_TYPE(hist, EWL_HISTOGRAM_TYPE);

        hist->color.r = r;
        hist->color.g = g;
        hist->color.b = b;
        hist->color.a = a;
        ewl_widget_configure(EWL_WIDGET(hist));

        DLEAVE_FUNCTION(DLEVEL_STABLE);
}

/**
 * @param hist: the histogram widget to retrieve display color
 * @param r: red value for histogram drawing color
 * @param g: green value for histogram drawing color
 * @param b: blue value for histogram drawing color
 * @param a: apha value for histogram drawing color
 * @return Returns no value.
 * @brief Get the current color values for drawing the histogram.
 */
void
ewl_histogram_color_get(Ewl_Histogram *hist, unsigned int *r, unsigned int *g,
                                                unsigned int *b, unsigned int *a)
{
        DENTER_FUNCTION(DLEVEL_STABLE);
        DCHECK_PARAM_PTR(hist);
        DCHECK_TYPE(hist, EWL_HISTOGRAM_TYPE);

        if (r) *r = hist->color.r;
        if (g) *g = hist->color.g;
        if (b) *b = hist->color.b;
        if (a) *a = hist->color.a;

        DLEAVE_FUNCTION(DLEVEL_STABLE);
}

/**
 * @param hist: the histogram to change the channel drawn
 * @param channel: the color channel to draw in the histogram
 * @return Returns no value.
 * @brief Sets the color channel to graph in the histogram.
 */
void
ewl_histogram_channel_set(Ewl_Histogram *hist, Ewl_Histogram_Channel channel)
{
        /*
         * FIXME: Handle different histogram types, currently only outputs the Y
         * component of a YIQ conversion and RGB channels, need to handle HSV,
         * CMYK, etc.
         */

        DENTER_FUNCTION(DLEVEL_STABLE);
        DCHECK_PARAM_PTR(hist);
        DCHECK_TYPE(hist, EWL_HISTOGRAM_TYPE);

        hist->channel = channel;
        if (hist->source && REALIZED(hist->source))
                ewl_histogram_cb_data_load(EWL_WIDGET(hist), NULL, hist);

        DLEAVE_FUNCTION(DLEVEL_STABLE);
}

/**
 * @param hist: the histogram to change the channel drawn
 * @return Returns the currently drawn color channel.
 * @brief Sets the color channel to graph in the histogram.
 */
Ewl_Histogram_Channel
ewl_histogram_channel_get(Ewl_Histogram *hist)
{
        DENTER_FUNCTION(DLEVEL_STABLE);
        DCHECK_PARAM_PTR_RET(hist, EWL_HISTOGRAM_CHANNEL_Y);
        DCHECK_TYPE_RET(hist, EWL_HISTOGRAM_TYPE, EWL_HISTOGRAM_CHANNEL_Y);

        DRETURN_INT(hist->channel, DLEVEL_STABLE);
}

/**
 * @param hist: the histogram to change source image
 * @param image: the new source image for the histogram
 * @return Returns no value.
 * @brief Change the source image used to generate the histogram
 */
void
ewl_histogram_image_set(Ewl_Histogram *hist, Ewl_Image *image)
{
        DENTER_FUNCTION(DLEVEL_STABLE);
        DCHECK_PARAM_PTR(hist);
        DCHECK_PARAM_PTR(image);
        DCHECK_TYPE(hist, EWL_HISTOGRAM_TYPE);
        DCHECK_TYPE(image, EWL_IMAGE_TYPE);

        hist->source = image;
        if (REALIZED(image))
                ewl_histogram_cb_data_load(EWL_WIDGET(hist), NULL, hist);

        /* Append the callback to catch an obscure/reveal */
        ewl_callback_append(EWL_WIDGET(image), EWL_CALLBACK_REVEAL,
                        ewl_histogram_cb_data_load, hist);

        DLEAVE_FUNCTION(DLEVEL_STABLE);
}

/**
 * @param hist: the histogram to get source image
 * @return Returns a pointer to the current source image.
 * @brief Get the source image used to generate the histogram
 */
Ewl_Image *
ewl_histogram_image_get(Ewl_Histogram *hist)
{
        DENTER_FUNCTION(DLEVEL_STABLE);
        DCHECK_PARAM_PTR_RET(hist, NULL);
        DCHECK_TYPE_RET(hist, EWL_HISTOGRAM_TYPE, NULL);

        DRETURN_PTR(EWL_IMAGE(hist->source), DLEVEL_STABLE);
}

/**
 * @internal
 * @param w: The widget being configured
 * @param event: UNUSED
 * @param data: UNUSED
 * @return Returns no value
 * @brief Configures the histogram
 */
void
ewl_histogram_cb_configure(Ewl_Widget *w, void *event __UNUSED__,
                                                void *data __UNUSED__)
{
        DENTER_FUNCTION(DLEVEL_STABLE);
        DCHECK_PARAM_PTR(w);
        DCHECK_TYPE(w, EWL_HISTOGRAM_TYPE);

        /* Only bother drawing if we've seen some usable data. */
        if (EWL_HISTOGRAM(w)->maxv)
                ewl_histogram_draw(EWL_HISTOGRAM(w));

        DLEAVE_FUNCTION(DLEVEL_STABLE);
}

#define A_CALC(color) ((color) >> 24)
#define R_CALC(color) ((color << 8) >> 24)
#define G_CALC(color) ((color << 16) >> 24)
#define B_CALC(color) ((color << 24) >> 24)
#define Y_CALC(color) (((R_CALC(color) * 299) + (G_CALC(color) * 587) + (B_CALC(color) * 114)) / 1000)

static void
ewl_histogram_cb_data_load(Ewl_Widget *w __UNUSED__, void *ev __UNUSED__, void *h)
{
        int x, y;
        int maxv = 0;
        unsigned int *data;
        int width, height;
        Ewl_Histogram *hist = EWL_HISTOGRAM(h);

        DENTER_FUNCTION(DLEVEL_STABLE);
        DCHECK_PARAM_PTR(hist);
        DCHECK_TYPE(hist, EWL_HISTOGRAM_TYPE);

        if (!hist->source || !REALIZED(hist->source))
                DRETURN(DLEVEL_STABLE);

        data = ewl_image_data_get(hist->source, &width, &height,
                                                EWL_IMAGE_DATA_READ);

        for (y = 0; y < height; y++) {
                for (x = 0; x < width; x++) {
                        unsigned int color;
                        unsigned char brightness;

                        color = *data;
                        switch (hist->channel) {

                                case EWL_HISTOGRAM_CHANNEL_R:
                                        brightness = R_CALC(color);
                                        break;
                                case EWL_HISTOGRAM_CHANNEL_G:
                                        brightness = G_CALC(color);
                                        break;
                                case EWL_HISTOGRAM_CHANNEL_B:
                                        brightness = B_CALC(color);
                                        break;
                                case EWL_HISTOGRAM_CHANNEL_Y:
                                default:
                                        brightness = Y_CALC(color);
                                        break;
                        }
                        hist->graph[brightness]++;
                        data++;
                }
        }

        /*
         * Find the maximum value in the range to scale the graph. This is done
         * outside the data loop since that may have a much higher cost
         * and this lookup is over a fixed data size.
         */
        for (x = 0; x < 256; x++) {
                if (hist->graph[x] > maxv)
                        maxv = hist->graph[x];
        }

        hist->maxv = maxv;

        DLEAVE_FUNCTION(DLEVEL_STABLE);
}

static void
ewl_histogram_draw(Ewl_Histogram *hist)
{
        int x, y;
        unsigned int color;
        unsigned int *data, *dst;
        int img_w = 0, img_h = 0;
        Ewl_Image *img;

        DENTER_FUNCTION(DLEVEL_STABLE);
        DCHECK_PARAM_PTR(hist);
        DCHECK_TYPE(hist, EWL_HISTOGRAM_TYPE);

        img = EWL_IMAGE(hist);

        ewl_image_data_set(img, NULL, CURRENT_W(hist), CURRENT_H(hist),
                                                        EWL_COLORSPACE_ARGB);
        dst = data = ewl_image_data_get(img, &img_w, &img_h,
                                                        EWL_IMAGE_DATA_WRITE);
        if (!data)
                DRETURN(DLEVEL_STABLE);

        /*
         * If no color specified, choose a sane default for the channel
         * represented.
         */
        if (!hist->color.a) {
                color = (unsigned int)(128 << 24);
                switch (hist->channel) {
                        case EWL_HISTOGRAM_CHANNEL_R:
                                color |= (unsigned int)(128 << 16);
                                break;
                        case EWL_HISTOGRAM_CHANNEL_G:
                                color |= (unsigned int)(128 << 8);
                                break;
                        case EWL_HISTOGRAM_CHANNEL_B:
                                color |= (unsigned int)(128);
                                break;
                        default:
                                break;
                }
        }
        else
                color = (unsigned int)(hist->color.a << 24 |
                                        hist->color.r << 16 |
                                        hist->color.g << 8 |
                                        hist->color.b);

        for (y = 0; y < img_h; y++) {
                for (x = 0; x < img_w; x++) {
                        int index;
                        int cutoff;
                        int x_scale;
                        int w1, w2;

                        /* Determine the base index for this x position */
                        x_scale = x << 8;
                        index = x_scale / img_w;

                        /*
                         * Determine distance between index points.
                         * Used to determine rounding error and distance from
                         * next point.
                         * This is at x * 256 scale.
                         */
                        w1 = index * img_w;
                        w2 = w1 + img_w;

                        cutoff = hist->graph[index];

                        /* Determine if this index should be weighted */
                        if ((x_scale != w1) && (index < 255)
                                        && (cutoff || hist->graph[index + 1])) {
                                cutoff = (cutoff * (w2 - x_scale));
                                cutoff += (hist->graph[index + 1] * (x_scale - w1));
                                cutoff = (cutoff / (w2 - w1));
                        }

                        cutoff = (img_h * cutoff) / hist->maxv;

                        *dst = ((img_h - y) < cutoff ? color : 0x0);
                        dst++;
                }
        }

        ewl_image_data_update_add(img, 0, 0, img_w, img_h);

        DLEAVE_FUNCTION(DLEVEL_STABLE);
}

