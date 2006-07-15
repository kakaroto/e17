#ifndef ELICIT_SPECTRUM_HEADER
#define ELICIT_SPECTRUM_HEADER

typedef enum {
  ELICIT_SPECTRUM_R,
  ELICIT_SPECTRUM_G,
  ELICIT_SPECTRUM_B,
  ELICIT_SPECTRUM_H,
  ELICIT_SPECTRUM_S,
  ELICIT_SPECTRUM_V,
  ELICIT_SPECTRUM_COUNT
} Elicit_Spectrum_Mode;

typedef struct _Elicit_Spectrum Elicit_Spectrum;
struct _Elicit_Spectrum 
{
  Evas_Object *grad;
  Elicit_Spectrum_Mode mode;
};

void elicit_spectrum_color_set(Evas_Object *o, int r, int g, int b, int h, float s, float v);
void elicit_spectrum_mode_set(Evas_Object *o, Elicit_Spectrum_Mode mode);
Evas_Object *elicit_spectrum_add(Evas *evas);
void elicit_spectra_shutdown(Elicit *el);
void elicit_spectra_update(Elicit *el);
void elicit_spectra_init(Elicit *el);
#endif
