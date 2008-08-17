
void                magnet_attach(void *);
void                magnet_detach(void *);
void                magnet_off_all(void);
void                magnet_set_mask(int);
void                magnet_mouse_move(int x, int y);
void                magnet_get_xy(int *, double *, double *);

typedef struct _MC  MC;
typedef struct _MG  MG;

#define MC_EP1 1<<0
#define MC_EP2 1<<1
#define MC_MP  1<<2
#define MC_CP  1<<3

#define MG_OFF 0
#define MG_EP  1<<0
#define MG_MP  1<<1
#define MG_CT  1<<2
#define MG_ALL 0xff

#define MG_XY 	1<<0
#define MG_X 	1<<1
#define MG_Y 	1<<2
#define MG_AB 	1<<3
#define MG_AR 	1<<4

#define CAND_XY 	1
#define CAND_ARC 	2
#define CAND_LINE	3
#define CAND_X		4
#define CAND_Y		5

struct _MC
{
    int                 flags;
    void               *parent;
    MG                 *ep1;
    MG                 *ep2;
    MG                 *mp;
    MG                 *cp;
};

struct _MG
{
    int                 mtype;
    int                 flags;
    int                 on;
    double              x, y;
    double              a, b;
    double              ax, ay;
    double              ar1, ar2;
    double              angle;
    Evas_Object        *o;
    MC                 *parent;
};

/*
struct _HE{
    double y;
};

struct _VE{
    double x;    
};

struct _LE{
    double a, b;
};

struct _AE{
    double x, y;
    double r1, r2;
};

*/
