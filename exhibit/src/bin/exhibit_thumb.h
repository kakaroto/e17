/*
 * vim:ts=8:sw=3:sts=8:noexpandtab:cino=>5n-3f0^-2{2
 */
#ifndef _EX_THUMB_H
#define _EX_THUMB_H

int         _ex_thumb_exe_exit(void *data, int type, void *event);
int         _ex_thumb_complete(void *data, int type, void *event);
void        _ex_thumb_generate(void);
void        _ex_thumb_abort(void);
void        _ex_thumb_abort_all(void);
void        _ex_thumb_update_at_row(Etk_Tree_Row *row);

#endif
