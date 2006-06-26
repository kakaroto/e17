#ifndef _BLING_COMPOSITE_H
#define _BLING_COMPOSITE_H

#if COMPOSITE_MAJOR > 0 || COMPOSITE_MINOR >= 2
#define HAS_NAME_WINDOW_PIXMAP 1
#endif

#define CAN_DO_USABLE 0

#define true 1
#define false 0

typedef struct _Win {
    Ecore_X_Window		id;
#if HAS_NAME_WINDOW_PIXMAP
    Ecore_X_Pixmap		pixmap;
#endif
    Ecore_X_Window_Attributes	a;
    Bool       focused;
#if CAN_DO_USABLE
    Bool		usable;		    /* mapped and all damaged at one point */
    Ecore_X_Rectangle	damage_bounds;	    /* bounds of damage */
#endif
    Bool                fade;
    Bool                isInFade;
    int			mode;
    int			damaged;
    Ecore_X_Damage	damage;
    Ecore_X_Picture		picture;
    Ecore_X_Picture		alphaPict;
    Ecore_X_Picture		shadowPict;
    Ecore_X_Region	borderSize;
    Ecore_X_Region	extents;
    unsigned int	preShadeOpacity;
    Ecore_X_Picture		shadow;
    int			shadow_dx;
    int			shadow_dy;
    int			shadow_width;
    int			shadow_height;
    unsigned int	opacity;
    unsigned int	shadowSize;
    Ecore_X_Window_Type windowType;
    unsigned long	damage_sequence;    /* sequence when damage was created */

    /* for drawing translucent windows */
    Ecore_X_Region	borderClip;
    struct _Win        *prev_trans;
} Win;

typedef struct _Conv {
    int	    size;
    double  *data;
} Conv;

typedef struct _Fade {
    Win			*w;
    Ecore_Animator *anim;
    double     start;
    double		cur;
    double		finish;
    double		step;
    double     start_time;
    double     interval;
    void		(*callback) (Win *w, Bool gone);
    Display		*dpy;
    Bool		gone;
} Fade;

#define TRANSLUCENT	0xe0000000
#define OPAQUE		0xffffffff

#define WINDOW_SOLID	0
#define WINDOW_TRANS	1
#define WINDOW_ARGB	2

#define TRANS_OPACITY	0.75

#define DEBUG_REPAINT 0
#define DEBUG_EVENTS 0
#define MONITOR_REPAINT 0

#define SHADOWS		1
#define SHARP_SHADOW	0

typedef enum _CompMode {
    CompSimple,		/* looks like a regular X server */
    CompServerShadows,	/* use window alpha for shadow; sharp, but precise */
    CompClientShadows,	/* use window extents for shadow, blurred */
} CompMode;

#endif
