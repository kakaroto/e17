/* vim: set sw=8 ts=8 sts=8 expandtab: */
#ifndef EWL_HISTOGRAM_H
#define EWL_HISTOGRAM_H

#include "ewl_image.h"

/**
 * @addtogroup Ewl_Histogram Ewl_Histogram: A Simple Histogram widget
 *
 * @remarks Inherits from Ewl_Image.
 * @if HAVE_IMAGES
 * @image html Ewl_Histogram_inheritance.png
 * @endif
 *
 * @{
 */

/**
 * @def EWL_HISTOGRAM_TYPE
 * The type name for the Ewl_Histogram widget
 */
#define EWL_HISTOGRAM_TYPE "histogram"

/**
 * @def EWL_HISTOGRAM_IS(w)
 * Returns TRUE if the widget is an Ewl_Histogram, FALSE otherwise
 */
#define EWL_HISTOGRAM_IS(w) (ewl_widget_type_is(EWL_WIDGET(w), EWL_HISTOGRAM_TYPE))

/**
 * @def EWL_HISTOGRAM(cd)
 * Typecast a pointer to an Ewl_Histogram pointer.
 */
#define EWL_HISTOGRAM(histogram) ((Ewl_Histogram *)histogram)

/**
 * The Ewl_Histogram
 */
typedef struct Ewl_Histogram Ewl_Histogram;

/**
 * @brief Inherit from the Ewl_Image and extends to create a histogram widget
 */
struct Ewl_Histogram
{
        Ewl_Image image; /**< Inherit from the image widget */
        Ewl_Image *source; /**< Source image used for data graph */
        Ewl_Color_Set color; /**< Color for drawing the histogram */

        int graph[256]; /**< Calculated histogram points */
        int maxv; /**< Largest data point in graph */
        Ewl_Histogram_Channel channel; /**< Channel of data for graphing */
};

Ewl_Widget              *ewl_histogram_new(void);
int                      ewl_histogram_init(Ewl_Histogram *histogram);

void                     ewl_histogram_color_set(Ewl_Histogram *histogram,
                                        unsigned int r, unsigned int g,
                                        unsigned int b, unsigned int a);
void                     ewl_histogram_color_get(Ewl_Histogram *histogram,
                                        unsigned int *r, unsigned int *g,
                                        unsigned int *b, unsigned int *a);

void                     ewl_histogram_image_set(Ewl_Histogram *histogram,
                                        Ewl_Image *image);
Ewl_Image               *ewl_histogram_image_get(Ewl_Histogram *histogram);

void                     ewl_histogram_channel_set(Ewl_Histogram *histogram,
                                        Ewl_Histogram_Channel channel);
Ewl_Histogram_Channel    ewl_histogram_channel_get(Ewl_Histogram *histogram);

/*
 * Internally used callbacks, override at your own risk
 */
void ewl_histogram_cb_configure(Ewl_Widget *w, void *event, void *data);

/**
 * @}
 */

#endif

