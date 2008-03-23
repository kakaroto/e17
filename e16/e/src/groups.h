/*
 * Copyright (C) 2000-2007 Carsten Haitzler, Geoff Harrison and various contributors
 * Copyright (C) 2004-2007 Kim Woelders
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to
 * deal in the Software without restriction, including without limitation the
 * rights to use, copy, modify, merge, publish, distribute, sublicense, and/or
 * sell copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in
 * all copies of the Software, its documentation and marketing & publicity
 * materials, and acknowledgment shall be given in the documentation, materials
 * and software packages that this Software was used.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL
 * THE AUTHORS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER
 * IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN
 * CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
 */
#ifndef _GROUPS_H_
#define _GROUPS_H_

#define GROUP_SELECT_ALL             0
#define GROUP_SELECT_EWIN_ONLY       1
#define GROUP_SELECT_ALL_EXCEPT_EWIN 2

/* For window group listing */
#define GROUP_ACTION_ANY                     0
#define GROUP_ACTION_MOVE                    1
#define GROUP_ACTION_RAISE                   2
#define GROUP_ACTION_LOWER                   3
#define GROUP_ACTION_KILL                    4
#define GROUP_ACTION_STICK                   5
#define GROUP_ACTION_ICONIFY                 6
#define GROUP_ACTION_SHADE                   7
#define GROUP_ACTION_SET_WINDOW_BORDER       8
#define GROUP_ACTION_RAISE_LOWER             9

typedef struct _groupconfig {
   char                iconify;
   char                kill;
   char                mirror;
   char                move;
   char                raise;
   char                set_border;
   char                shade;
   char                stick;
} GroupConfig;

struct _group {
   int                 index;
   EWin              **members;
   int                 num_members;
   GroupConfig         cfg;
};

/* finders.c */
EWin              **ListWinGroupMembersForEwin(const EWin * ewin, int action,
					       char nogroup, int *num);

/* groups.c */
Group              *BuildWindowGroup(EWin ** ewins, int num);
Group              *EwinsInGroup(const EWin * ewin1, const EWin * ewin2);
void                AddEwinToGroup(EWin * ewin, Group * g);
Group              *GroupFind(int gid);
void                GroupSetId(Group * group, int gid);
void                GroupsEwinRemove(EWin * ewin);
void                SaveGroups(void);
Group             **GroupsGetList(int *pnum);
Group             **ListWinGroups(const EWin * ewin, char group_select,
				  int *num);
int                 GroupsGetSwapmove(void);

#endif /* _GROUPS_H_ */
