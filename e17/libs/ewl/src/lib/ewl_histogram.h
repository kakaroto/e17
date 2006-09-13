#ifndef EWL_HISTOGRAM_H
#define EWL_HISTOGRAM_H

/**
 * @addtogroup Ew_lHistogram Ewl_Histogram: A Simple Histogram widget
 *
 * @{
 */

/**
 * @def EWL_HISTOGRAM_TYPE
 * The type name for the Ewl_Histogram widget
 */
#define EWL_HISTOGRAM_TYPE "histogram"

enum Ewl_Histogram_Channel
{
	EWL_HISTOGRAM_CHANNEL_Y,
	EWL_HISTOGRAM_CHANNEL_R,
	EWL_HISTOGRAM_CHANNEL_G,
	EWL_HISTOGRAM_CHANNEL_B,
	EWL_HISTOGRAM_CHANNEL_MAX
};

typedef enum Ewl_Histogram_Channel Ewl_Histogram_Channel;

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
 * Inherit from the Ewl_Image to create a histogram widget
 */
struct Ewl_Histogram
{
	Ewl_Image image; /**< Inherit from the image widget */
	Ewl_Image *source; /**< Source image used for data graph */
	Ewl_Color_Set color; /**< Color for drawing the histogram */

	const unsigned int *data; /**< Image data for calculating values */
	int width, height; /**< Dimensions of the image data */
	int graph[256]; /**< Calculated histogram points */
	int maxv; /**< Largest data point in graph */
	Ewl_Histogram_Channel channel; /**< Channel of data for graphing */
};

Ewl_Widget *ewl_histogram_new(void);
int ewl_histogram_init(Ewl_Histogram *histogram);
void ewl_histogram_color_set(Ewl_Histogram *histogram, int r, int g, int b, int a);
void ewl_histogram_color_get(Ewl_Histogram *histogram, int *r, int *g, int *b, int *a);
void ewl_histogram_image_set(Ewl_Histogram *histogram, Ewl_Image *image);
Ewl_Image *ewl_histogram_image_get(Ewl_Histogram *histogram);
void ewl_histogram_channel_set(Ewl_Histogram *histogram, Ewl_Histogram_Channel channel);
Ewl_Histogram_Channel ewl_histogram_channel_get(Ewl_Histogram *histogram);

/**
 * Internally used callbacks, override at your own risk
 */
void ewl_histogram_cb_configure(Ewl_Widget *w, void *event, void *data);

/**
 * @}
 */

#endif
