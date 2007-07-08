
typedef enum _Pointers Pointer;

enum _Pointers
{
    POINTER_NORMAL,
    POINTER_CROSS,
    POINTER_HAND,
    POINTER_ARROW
};

void                pointer_init(void);

void                pointer_set(int);
void                pointer_move(int, int);

void                pointer_push_and_set(int);
void                pointer_pop(int);
int                 curr_pointer_get(void);
