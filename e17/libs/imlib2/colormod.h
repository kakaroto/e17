#ifndef __COLORMOD
#define __COLORMOD 1

typedef struct _imlib_color_modifier    ImlibColorModifier;

struct _imlib_color_modifier
{
   DATA8     red_mapping[256];
   DATA8     green_mapping[256];
   DATA8     blue_mapping[256];
   DATA8     alpha_mapping[256];
   DATABIG   modification_count;
};

#define CMOD_APPLY_RGB(cm, r, g, b) \
(r) = (cm)->red_mapping[(int)(r)]; \
(g) = (cm)->green_mapping[(int)(g)]; \
(b) = (cm)->blue_mapping[(int)(b)];

#define CMOD_APPLY_RGBA(cm, r, g, b, a) \
(r) = (cm)->red_mapping[(int)(r)]; \
(g) = (cm)->green_mapping[(int)(g)]; \
(b) = (cm)->blue_mapping[(int)(b)]; \
(a) = (cm)->alpha_mapping[(int)(a)];

ImlibColorModifier * __imlib_CreateCmod(void);
void                 __imlib_FreeCmod(ImlibColorModifier *cm);
void                 __imlib_CmodChanged(ImlibColorModifier *cm);
void                 __imlib_CmodSetTables(ImlibColorModifier *cm, DATA8 *r, 
					   DATA8 *g, DATA8 *b, DATA8 *a);
void                 __imlib_CmodReset(ImlibColorModifier *cm);
void                 __imlib_DataCmodApply(DATA32 *data, int w, int h, 
					   int jump,
					   ImlibColorModifier *cm);

void                 __imlib_CmodGetTables(ImlibColorModifier *cm, DATA8 *r, 
					   DATA8 *g, DATA8 *b, DATA8 *a);
void                 __imlib_CmodModBrightness(ImlibColorModifier *cm, 
					       double v);
void                 __imlib_CmodModContrast(ImlibColorModifier *cm, 
					     double v);
void                 __imlib_CmodModGamma(ImlibColorModifier *cm, 
					  double v);
#endif
