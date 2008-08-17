
typedef enum _Pointers Pointer;

enum _Pointers
{
    POINTER_NORMAL,
    POINTER_CROSS,
    POINTER_HAND,
    POINTER_ARROW
};

void                pointer_init(void);

void                pointer_set(long);
void                pointer_move(int, int);

void                pointer_push_and_set(long);
void                pointer_pop(long);
long                curr_pointer_get(void);
