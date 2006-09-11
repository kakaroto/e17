#ifndef EWL_HISTOGRAM_H
#define EWL_HISTOGRAM_H

typedef struct Ewl_Histogram Ewl_Histogram;
#define EWL_HISTOGRAM(histogram) ((Ewl_Histogram *)histogram)
struct Ewl_Histogram
{
	Ewl_Image image;
	int type;
	unsigned int *data;
	int width, height;
	int graph[256];
	int maxv;
	unsigned char r, g, b, a;
};

Ewl_Widget *ewl_histogram_new(void);
int ewl_histogram_init(Ewl_Histogram *histogram);
void ewl_histogram_color_set(Ewl_Histogram *histogram, char r, char g, char b, char a);
void ewl_histogram_data_set(Ewl_Histogram *histogram, unsigned int *data, int width, int height);

void ewl_histogram_cb_configure(Ewl_Widget *w, void *event, void *data);

#endif
