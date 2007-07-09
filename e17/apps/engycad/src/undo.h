
void                append_undo_ptr(void *, void *, void *, int, int, void *);
void                append_undo_long(void *, long, long, int, int, void *);
void                append_undo_double(void *, double, double, int, int,
                                       void *);
void                append_undo_new_object(void *, int, int, void *);

void                apply_undo_forward(void);
void                apply_undo_backward(void);
