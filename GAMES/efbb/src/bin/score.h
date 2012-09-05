#ifndef __SCORE_H__
#define __SCORE_H__ 1

#ifdef __cplusplus
extern "C" {
#endif

typedef struct _Score Score;

Score *score_add(Evas_Object *win, int hiscore);
int score_get(Score *score);
void score_inc(Score *score, int value);
void score_del(Score *score);

#ifdef __cplusplus
}
#endif

#endif /* __SCORE_H__ */

