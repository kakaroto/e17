#ifndef _ECOMPMGR_H
#define _ECOMPMGR_H

#if USE_COMPOSITE

typedef struct
{
   char                enable;
   int                 shadow;
} cfg_composite;

int                 ECompMgrActive(void);	/* FIXME - Remove */
void                ECompMgrParseArgs(const char *args);

void                ECompMgrWinNew(EObj * eo);
void                ECompMgrWinDel(EObj * eo, Bool gone, Bool do_fade);
void                ECompMgrWinChangeOpacity(EObj * eo, unsigned int opacity);
Pixmap              ECompMgrWinGetPixmap(const EObj * eo);
void                ECompMgrConfigGet(cfg_composite * cfg);
void                ECompMgrConfigSet(const cfg_composite * cfg);

void                ECompMgrMoveResizeFix(EObj * eo, int x, int y, int w,
					  int h);
#define ExMoveResizeWindow ECompMgrMoveResizeFix

#else

#define ECompMgrActive() 0	/* FIXME - Remove */

#define ECompMgrWinDel(eo, gone, do_fade)
#define ExMoveResizeWindow(eo, x, y, w, h) EMoveResizeWindow((eo)->win, x, y, w, h)

#endif

#endif /* _ECOMPMGR_H */
