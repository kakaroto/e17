
typedef struct _Msg Msg;

struct _Msg
{
    struct
    {
        DATA32              major;
        DATA32              minor;
    }
    opcode;
    void               *obj;
};

void                gra_put_msg(Msg *);
void                gra_apply(void);
Msg                *msg_create(void);
void                msg_create_and_send(int, int, void *);

void                gra_pre_draw_put_data(Eina_List *);
void                gra_pre_draw_mouse_move(double, double);
void                gra_pre_draw_mouse_click(double, double);
void                gra_set_state(int);

enum States
{
    ST_NORMAL,
    ST_POINT,
    ST_LINE1,
    ST_LINE2,
    ST_CIRCLE1,
    ST_CIRCLE2,
    ST_ARC1,
    ST_ARC2,
    ST_ARC3,
    ST_ARC4,
    ST_ELLIPSE1,
    ST_ELLIPSE2,
    ST_ELLIPSE3,
    ST_EARC1,
    ST_EARC2,
    ST_EARC3,
    ST_EARC4,
    ST_EARC5,
    ST_TEXT1,
    ST_TEXT2,
    ST_TEXT3,
    ST_IMAGE1,
    ST_IMAGE2,
    ST_IMAGE3,
    ST_DIM1,
    ST_DIM2,
    ST_DIMH,
    ST_DIMV,
    ST_DIMA,
    ST_DIMR,
    ST_DIMD,
    ST_SEL1,
    ST_SEL2,
    ST_MOVE1,
    ST_MOVE2,
    ST_ROTATE1,
    ST_ROTATE2,
    ST_MIRROR1,
    ST_MIRROR2,
    ST_TRIM1,
    ST_TRIM2,
    ST_TRIM3,
    ST_RECT1,
    ST_RECT2
};
