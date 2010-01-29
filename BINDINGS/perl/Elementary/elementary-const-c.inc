#define PERL_constant_NOTFOUND	1
#define PERL_constant_NOTDEF	2
#define PERL_constant_ISIV	3
#define PERL_constant_ISNO	4
#define PERL_constant_ISNV	5
#define PERL_constant_ISPV	6
#define PERL_constant_ISPVN	7
#define PERL_constant_ISSV	8
#define PERL_constant_ISUNDEF	9
#define PERL_constant_ISUV	10
#define PERL_constant_ISYES	11

#ifndef NVTYPE
typedef double NV; /* 5.6 and later define NVTYPE, and typedef NV to it.  */
#endif
#ifndef aTHX_
#define aTHX_ /* 5.6 or later define this for threading support.  */
#endif
#ifndef pTHX_
#define pTHX_ /* 5.6 or later define this for threading support.  */
#endif

static int
constant_13 (pTHX_ const char *name, IV *iv_return) {
  /* When generated this function returned values for the list of names given
     here.  However, subsequent manual editing may have added or removed some.
     ELM_ICON_FILE ELM_ICON_NONE ELM_WIN_BASIC */
  /* Offset 11 gives the best switch position.  */
  switch (name[11]) {
  case 'I':
    if (memEQ(name, "ELM_WIN_BASIC", 13)) {
    /*                          ^        */
      *iv_return = ELM_WIN_BASIC;
      return PERL_constant_ISIV;
    }
    break;
  case 'L':
    if (memEQ(name, "ELM_ICON_FILE", 13)) {
    /*                          ^        */
      *iv_return = ELM_ICON_FILE;
      return PERL_constant_ISIV;
    }
    break;
  case 'N':
    if (memEQ(name, "ELM_ICON_NONE", 13)) {
    /*                          ^        */
      *iv_return = ELM_ICON_NONE;
      return PERL_constant_ISIV;
    }
    break;
  }
  return PERL_constant_NOTFOUND;
}

static int
constant_14 (pTHX_ const char *name, IV *iv_return) {
  /* When generated this function returned values for the list of names given
     here.  However, subsequent manual editing may have added or removed some.
     ELM_LIST_LIMIT ELM_WIN_SPLASH EVAS_HINT_FILL */
  /* Offset 10 gives the best switch position.  */
  switch (name[10]) {
  case 'F':
    if (memEQ(name, "EVAS_HINT_FILL", 14)) {
    /*                         ^          */
      *iv_return = EVAS_HINT_FILL;
      return PERL_constant_ISIV;
    }
    break;
  case 'I':
    if (memEQ(name, "ELM_LIST_LIMIT", 14)) {
    /*                         ^          */
      *iv_return = ELM_LIST_LIMIT;
      return PERL_constant_ISIV;
    }
    break;
  case 'L':
    if (memEQ(name, "ELM_WIN_SPLASH", 14)) {
    /*                         ^          */
      *iv_return = ELM_WIN_SPLASH;
      return PERL_constant_ISIV;
    }
    break;
  }
  return PERL_constant_NOTFOUND;
}

static int
constant_15 (pTHX_ const char *name, IV *iv_return) {
  /* When generated this function returned values for the list of names given
     here.  However, subsequent manual editing may have added or removed some.
     ELM_LIST_SCROLL ELM_POLICY_LAST ELM_POLICY_QUIT ELM_WIN_DESKTOP
     ELM_WIN_TOOLBAR ELM_WIN_UTILITY */
  /* Offset 13 gives the best switch position.  */
  switch (name[13]) {
  case 'A':
    if (memEQ(name, "ELM_WIN_TOOLBAR", 15)) {
    /*                            ^        */
      *iv_return = ELM_WIN_TOOLBAR;
      return PERL_constant_ISIV;
    }
    break;
  case 'I':
    if (memEQ(name, "ELM_POLICY_QUIT", 15)) {
    /*                            ^        */
      *iv_return = ELM_POLICY_QUIT;
      return PERL_constant_ISIV;
    }
    break;
  case 'L':
    if (memEQ(name, "ELM_LIST_SCROLL", 15)) {
    /*                            ^        */
      *iv_return = ELM_LIST_SCROLL;
      return PERL_constant_ISIV;
    }
    break;
  case 'O':
    if (memEQ(name, "ELM_WIN_DESKTOP", 15)) {
    /*                            ^        */
      *iv_return = ELM_WIN_DESKTOP;
      return PERL_constant_ISIV;
    }
    break;
  case 'S':
    if (memEQ(name, "ELM_POLICY_LAST", 15)) {
    /*                            ^        */
      *iv_return = ELM_POLICY_LAST;
      return PERL_constant_ISIV;
    }
    break;
  case 'T':
    if (memEQ(name, "ELM_WIN_UTILITY", 15)) {
    /*                            ^        */
      *iv_return = ELM_WIN_UTILITY;
      return PERL_constant_ISIV;
    }
    break;
  }
  return PERL_constant_NOTFOUND;
}

static int
constant_19 (pTHX_ const char *name, IV *iv_return) {
  /* When generated this function returned values for the list of names given
     here.  However, subsequent manual editing may have added or removed some.
     ELM_HOVER_AXIS_BOTH ELM_HOVER_AXIS_NONE ELM_WIN_KEYBOARD_ON */
  /* Offset 17 gives the best switch position.  */
  switch (name[17]) {
  case 'N':
    if (memEQ(name, "ELM_HOVER_AXIS_NONE", 19)) {
    /*                                ^        */
      *iv_return = ELM_HOVER_AXIS_NONE;
      return PERL_constant_ISIV;
    }
    break;
  case 'O':
    if (memEQ(name, "ELM_WIN_KEYBOARD_ON", 19)) {
    /*                                ^        */
      *iv_return = ELM_WIN_KEYBOARD_ON;
      return PERL_constant_ISIV;
    }
    break;
  case 'T':
    if (memEQ(name, "ELM_HOVER_AXIS_BOTH", 19)) {
    /*                                ^        */
      *iv_return = ELM_HOVER_AXIS_BOTH;
      return PERL_constant_ISIV;
    }
    break;
  }
  return PERL_constant_NOTFOUND;
}

static int
constant_20 (pTHX_ const char *name, IV *iv_return) {
  /* When generated this function returned values for the list of names given
     here.  However, subsequent manual editing may have added or removed some.
     ELM_PANEL_ORIENT_TOP ELM_POLICY_QUIT_NONE ELM_WIN_DIALOG_BASIC
     ELM_WIN_KEYBOARD_HEX ELM_WIN_KEYBOARD_OFF ELM_WIN_KEYBOARD_PIN */
  /* Offset 19 gives the best switch position.  */
  switch (name[19]) {
  case 'C':
    if (memEQ(name, "ELM_WIN_DIALOG_BASI", 19)) {
    /*                                  C      */
      *iv_return = ELM_WIN_DIALOG_BASIC;
      return PERL_constant_ISIV;
    }
    break;
  case 'E':
    if (memEQ(name, "ELM_POLICY_QUIT_NON", 19)) {
    /*                                  E      */
      *iv_return = ELM_POLICY_QUIT_NONE;
      return PERL_constant_ISIV;
    }
    break;
  case 'F':
    if (memEQ(name, "ELM_WIN_KEYBOARD_OF", 19)) {
    /*                                  F      */
      *iv_return = ELM_WIN_KEYBOARD_OFF;
      return PERL_constant_ISIV;
    }
    break;
  case 'N':
    if (memEQ(name, "ELM_WIN_KEYBOARD_PI", 19)) {
    /*                                  N      */
      *iv_return = ELM_WIN_KEYBOARD_PIN;
      return PERL_constant_ISIV;
    }
    break;
  case 'P':
    if (memEQ(name, "ELM_PANEL_ORIENT_TO", 19)) {
    /*                                  P      */
      *iv_return = ELM_PANEL_ORIENT_TOP;
      return PERL_constant_ISIV;
    }
    break;
  case 'X':
    if (memEQ(name, "ELM_WIN_KEYBOARD_HE", 19)) {
    /*                                  X      */
      *iv_return = ELM_WIN_KEYBOARD_HEX;
      return PERL_constant_ISIV;
    }
    break;
  }
  return PERL_constant_NOTFOUND;
}

static int
constant_21 (pTHX_ const char *name, IV *iv_return) {
  /* When generated this function returned values for the list of names given
     here.  However, subsequent manual editing may have added or removed some.
     ELM_GENLIST_ITEM_NONE ELM_IMAGE_ORIENT_NONE ELM_NOTIFY_ORIENT_TOP
     ELM_PANEL_ORIENT_LEFT */
  /* Offset 7 gives the best switch position.  */
  switch (name[7]) {
  case 'E':
    if (memEQ(name, "ELM_PANEL_ORIENT_LEFT", 21)) {
    /*                      ^                    */
      *iv_return = ELM_PANEL_ORIENT_LEFT;
      return PERL_constant_ISIV;
    }
    break;
  case 'G':
    if (memEQ(name, "ELM_IMAGE_ORIENT_NONE", 21)) {
    /*                      ^                    */
      *iv_return = ELM_IMAGE_ORIENT_NONE;
      return PERL_constant_ISIV;
    }
    break;
  case 'I':
    if (memEQ(name, "ELM_NOTIFY_ORIENT_TOP", 21)) {
    /*                      ^                    */
      *iv_return = ELM_NOTIFY_ORIENT_TOP;
      return PERL_constant_ISIV;
    }
    break;
  case 'L':
    if (memEQ(name, "ELM_GENLIST_ITEM_NONE", 21)) {
    /*                      ^                    */
      *iv_return = ELM_GENLIST_ITEM_NONE;
      return PERL_constant_ISIV;
    }
    break;
  }
  return PERL_constant_NOTFOUND;
}

static int
constant_22 (pTHX_ const char *name, IV *iv_return) {
  /* When generated this function returned values for the list of names given
     here.  However, subsequent manual editing may have added or removed some.
     ELM_IMAGE_ROTATE_90_CW ELM_NOTIFY_ORIENT_LEFT ELM_PANEL_ORIENT_RIGHT
     ELM_SCROLLER_POLICY_ON ELM_WIN_KEYBOARD_ALPHA */
  /* Offset 4 gives the best switch position.  */
  switch (name[4]) {
  case 'I':
    if (memEQ(name, "ELM_IMAGE_ROTATE_90_CW", 22)) {
    /*                   ^                        */
      *iv_return = ELM_IMAGE_ROTATE_90_CW;
      return PERL_constant_ISIV;
    }
    break;
  case 'N':
    if (memEQ(name, "ELM_NOTIFY_ORIENT_LEFT", 22)) {
    /*                   ^                        */
      *iv_return = ELM_NOTIFY_ORIENT_LEFT;
      return PERL_constant_ISIV;
    }
    break;
  case 'P':
    if (memEQ(name, "ELM_PANEL_ORIENT_RIGHT", 22)) {
    /*                   ^                        */
      *iv_return = ELM_PANEL_ORIENT_RIGHT;
      return PERL_constant_ISIV;
    }
    break;
  case 'S':
    if (memEQ(name, "ELM_SCROLLER_POLICY_ON", 22)) {
    /*                   ^                        */
      *iv_return = ELM_SCROLLER_POLICY_ON;
      return PERL_constant_ISIV;
    }
    break;
  case 'W':
    if (memEQ(name, "ELM_WIN_KEYBOARD_ALPHA", 22)) {
    /*                   ^                        */
      *iv_return = ELM_WIN_KEYBOARD_ALPHA;
      return PERL_constant_ISIV;
    }
    break;
  }
  return PERL_constant_NOTFOUND;
}

static int
constant_23 (pTHX_ const char *name, IV *iv_return) {
  /* When generated this function returned values for the list of names given
     here.  However, subsequent manual editing may have added or removed some.
     ELM_HOVER_AXIS_VERTICAL ELM_IMAGE_FLIP_VERTICAL ELM_IMAGE_ROTATE_180_CW
     ELM_IMAGE_ROTATE_90_CCW ELM_NOTIFY_ORIENT_RIGHT ELM_PANEL_ORIENT_BOTTOM
     ELM_SCROLLER_POLICY_OFF */
  /* Offset 10 gives the best switch position.  */
  switch (name[10]) {
  case 'A':
    if (memEQ(name, "ELM_HOVER_AXIS_VERTICAL", 23)) {
    /*                         ^                   */
      *iv_return = ELM_HOVER_AXIS_VERTICAL;
      return PERL_constant_ISIV;
    }
    break;
  case 'E':
    if (memEQ(name, "ELM_SCROLLER_POLICY_OFF", 23)) {
    /*                         ^                   */
      *iv_return = ELM_SCROLLER_POLICY_OFF;
      return PERL_constant_ISIV;
    }
    break;
  case 'F':
    if (memEQ(name, "ELM_IMAGE_FLIP_VERTICAL", 23)) {
    /*                         ^                   */
      *iv_return = ELM_IMAGE_FLIP_VERTICAL;
      return PERL_constant_ISIV;
    }
    break;
  case 'O':
    if (memEQ(name, "ELM_PANEL_ORIENT_BOTTOM", 23)) {
    /*                         ^                   */
      *iv_return = ELM_PANEL_ORIENT_BOTTOM;
      return PERL_constant_ISIV;
    }
    break;
  case 'R':
    if (memEQ(name, "ELM_IMAGE_ROTATE_180_CW", 23)) {
    /*                         ^                   */
      *iv_return = ELM_IMAGE_ROTATE_180_CW;
      return PERL_constant_ISIV;
    }
    if (memEQ(name, "ELM_IMAGE_ROTATE_90_CCW", 23)) {
    /*                         ^                   */
      *iv_return = ELM_IMAGE_ROTATE_90_CCW;
      return PERL_constant_ISIV;
    }
    break;
  case '_':
    if (memEQ(name, "ELM_NOTIFY_ORIENT_RIGHT", 23)) {
    /*                         ^                   */
      *iv_return = ELM_NOTIFY_ORIENT_RIGHT;
      return PERL_constant_ISIV;
    }
    break;
  }
  return PERL_constant_NOTFOUND;
}

static int
constant_24 (pTHX_ const char *name, IV *iv_return) {
  /* When generated this function returned values for the list of names given
     here.  However, subsequent manual editing may have added or removed some.
     ELM_IMAGE_FLIP_TRANSPOSE ELM_MAP_ZOOM_MODE_MANUAL ELM_NOTIFY_ORIENT_BOTTOM
     ELM_SCROLLER_POLICY_AUTO ELM_WIN_KEYBOARD_NUMERIC ELM_WIN_KEYBOARD_UNKNOWN
     */
  /* Offset 23 gives the best switch position.  */
  switch (name[23]) {
  case 'C':
    if (memEQ(name, "ELM_WIN_KEYBOARD_NUMERI", 23)) {
    /*                                      C      */
      *iv_return = ELM_WIN_KEYBOARD_NUMERIC;
      return PERL_constant_ISIV;
    }
    break;
  case 'E':
    if (memEQ(name, "ELM_IMAGE_FLIP_TRANSPOS", 23)) {
    /*                                      E      */
      *iv_return = ELM_IMAGE_FLIP_TRANSPOSE;
      return PERL_constant_ISIV;
    }
    break;
  case 'L':
    if (memEQ(name, "ELM_MAP_ZOOM_MODE_MANUA", 23)) {
    /*                                      L      */
      *iv_return = ELM_MAP_ZOOM_MODE_MANUAL;
      return PERL_constant_ISIV;
    }
    break;
  case 'M':
    if (memEQ(name, "ELM_NOTIFY_ORIENT_BOTTO", 23)) {
    /*                                      M      */
      *iv_return = ELM_NOTIFY_ORIENT_BOTTOM;
      return PERL_constant_ISIV;
    }
    break;
  case 'N':
    if (memEQ(name, "ELM_WIN_KEYBOARD_UNKNOW", 23)) {
    /*                                      N      */
      *iv_return = ELM_WIN_KEYBOARD_UNKNOWN;
      return PERL_constant_ISIV;
    }
    break;
  case 'O':
    if (memEQ(name, "ELM_SCROLLER_POLICY_AUT", 23)) {
    /*                                      O      */
      *iv_return = ELM_SCROLLER_POLICY_AUTO;
      return PERL_constant_ISIV;
    }
    break;
  }
  return PERL_constant_NOTFOUND;
}

static int
constant_25 (pTHX_ const char *name, IV *iv_return) {
  /* When generated this function returned values for the list of names given
     here.  However, subsequent manual editing may have added or removed some.
     ELM_GENLIST_ITEM_SUBITEMS ELM_HOVER_AXIS_HORIZONTAL
     ELM_IMAGE_FLIP_HORIZONTAL ELM_IMAGE_FLIP_TRANSVERSE
     ELM_WIN_KEYBOARD_PASSWORD ELM_WIN_KEYBOARD_TERMINAL */
  /* Offset 20 gives the best switch position.  */
  switch (name[20]) {
  case 'I':
    if (memEQ(name, "ELM_GENLIST_ITEM_SUBITEMS", 25)) {
    /*                                   ^           */
      *iv_return = ELM_GENLIST_ITEM_SUBITEMS;
      return PERL_constant_ISIV;
    }
    break;
  case 'M':
    if (memEQ(name, "ELM_WIN_KEYBOARD_TERMINAL", 25)) {
    /*                                   ^           */
      *iv_return = ELM_WIN_KEYBOARD_TERMINAL;
      return PERL_constant_ISIV;
    }
    break;
  case 'O':
    if (memEQ(name, "ELM_HOVER_AXIS_HORIZONTAL", 25)) {
    /*                                   ^           */
      *iv_return = ELM_HOVER_AXIS_HORIZONTAL;
      return PERL_constant_ISIV;
    }
    if (memEQ(name, "ELM_IMAGE_FLIP_HORIZONTAL", 25)) {
    /*                                   ^           */
      *iv_return = ELM_IMAGE_FLIP_HORIZONTAL;
      return PERL_constant_ISIV;
    }
    break;
  case 'S':
    if (memEQ(name, "ELM_WIN_KEYBOARD_PASSWORD", 25)) {
    /*                                   ^           */
      *iv_return = ELM_WIN_KEYBOARD_PASSWORD;
      return PERL_constant_ISIV;
    }
    break;
  case 'V':
    if (memEQ(name, "ELM_IMAGE_FLIP_TRANSVERSE", 25)) {
    /*                                   ^           */
      *iv_return = ELM_IMAGE_FLIP_TRANSVERSE;
      return PERL_constant_ISIV;
    }
    break;
  }
  return PERL_constant_NOTFOUND;
}

static int
constant_26 (pTHX_ const char *name, IV *iv_return) {
  /* When generated this function returned values for the list of names given
     here.  However, subsequent manual editing may have added or removed some.
     ELM_MAP_ZOOM_MODE_AUTO_FIT ELM_NOTIFY_ORIENT_TOP_LEFT
     ELM_TEXT_FORMAT_PLAIN_UTF8 */
  /* Offset 11 gives the best switch position.  */
  switch (name[11]) {
  case 'M':
    if (memEQ(name, "ELM_MAP_ZOOM_MODE_AUTO_FIT", 26)) {
    /*                          ^                     */
      *iv_return = ELM_MAP_ZOOM_MODE_AUTO_FIT;
      return PERL_constant_ISIV;
    }
    break;
  case 'O':
    if (memEQ(name, "ELM_NOTIFY_ORIENT_TOP_LEFT", 26)) {
    /*                          ^                     */
      *iv_return = ELM_NOTIFY_ORIENT_TOP_LEFT;
      return PERL_constant_ISIV;
    }
    break;
  case 'R':
    if (memEQ(name, "ELM_TEXT_FORMAT_PLAIN_UTF8", 26)) {
    /*                          ^                     */
      *iv_return = ELM_TEXT_FORMAT_PLAIN_UTF8;
      return PERL_constant_ISIV;
    }
    break;
  }
  return PERL_constant_NOTFOUND;
}

static int
constant_27 (pTHX_ const char *name, IV *iv_return) {
  /* When generated this function returned values for the list of names given
     here.  However, subsequent manual editing may have added or removed some.
     ELM_MAP_ZOOM_MODE_AUTO_FILL ELM_NOTIFY_ORIENT_TOP_RIGHT
     ELM_TEXT_FORMAT_MARKUP_UTF8 */
  /* Offset 11 gives the best switch position.  */
  switch (name[11]) {
  case 'M':
    if (memEQ(name, "ELM_MAP_ZOOM_MODE_AUTO_FILL", 27)) {
    /*                          ^                      */
      *iv_return = ELM_MAP_ZOOM_MODE_AUTO_FILL;
      return PERL_constant_ISIV;
    }
    break;
  case 'O':
    if (memEQ(name, "ELM_NOTIFY_ORIENT_TOP_RIGHT", 27)) {
    /*                          ^                      */
      *iv_return = ELM_NOTIFY_ORIENT_TOP_RIGHT;
      return PERL_constant_ISIV;
    }
    break;
  case 'R':
    if (memEQ(name, "ELM_TEXT_FORMAT_MARKUP_UTF8", 27)) {
    /*                          ^                      */
      *iv_return = ELM_TEXT_FORMAT_MARKUP_UTF8;
      return PERL_constant_ISIV;
    }
    break;
  }
  return PERL_constant_NOTFOUND;
}

static int
constant_29 (pTHX_ const char *name, IV *iv_return) {
  /* When generated this function returned values for the list of names given
     here.  However, subsequent manual editing may have added or removed some.
     ELM_FLIP_ROTATE_X_CENTER_AXIS ELM_FLIP_ROTATE_Y_CENTER_AXIS
     ELM_NOTIFY_ORIENT_BOTTOM_LEFT ELM_PHOTOCAM_ZOOM_MODE_MANUAL
     ELM_WIN_KEYBOARD_PHONE_NUMBER */
  /* Offset 16 gives the best switch position.  */
  switch (name[16]) {
  case 'M':
    if (memEQ(name, "ELM_PHOTOCAM_ZOOM_MODE_MANUAL", 29)) {
    /*                               ^                   */
      *iv_return = ELM_PHOTOCAM_ZOOM_MODE_MANUAL;
      return PERL_constant_ISIV;
    }
    break;
  case 'T':
    if (memEQ(name, "ELM_NOTIFY_ORIENT_BOTTOM_LEFT", 29)) {
    /*                               ^                   */
      *iv_return = ELM_NOTIFY_ORIENT_BOTTOM_LEFT;
      return PERL_constant_ISIV;
    }
    break;
  case 'X':
    if (memEQ(name, "ELM_FLIP_ROTATE_X_CENTER_AXIS", 29)) {
    /*                               ^                   */
      *iv_return = ELM_FLIP_ROTATE_X_CENTER_AXIS;
      return PERL_constant_ISIV;
    }
    break;
  case 'Y':
    if (memEQ(name, "ELM_FLIP_ROTATE_Y_CENTER_AXIS", 29)) {
    /*                               ^                   */
      *iv_return = ELM_FLIP_ROTATE_Y_CENTER_AXIS;
      return PERL_constant_ISIV;
    }
    break;
  case '_':
    if (memEQ(name, "ELM_WIN_KEYBOARD_PHONE_NUMBER", 29)) {
    /*                               ^                   */
      *iv_return = ELM_WIN_KEYBOARD_PHONE_NUMBER;
      return PERL_constant_ISIV;
    }
    break;
  }
  return PERL_constant_NOTFOUND;
}

static int
constant_30 (pTHX_ const char *name, IV *iv_return) {
  /* When generated this function returned values for the list of names given
     here.  However, subsequent manual editing may have added or removed some.
     ELM_FLIP_ROTATE_XZ_CENTER_AXIS ELM_FLIP_ROTATE_YZ_CENTER_AXIS
     ELM_NOTIFY_ORIENT_BOTTOM_RIGHT EVAS_ASPECT_CONTROL_HORIZONTAL */
  /* Offset 16 gives the best switch position.  */
  switch (name[16]) {
  case 'R':
    if (memEQ(name, "EVAS_ASPECT_CONTROL_HORIZONTAL", 30)) {
    /*                               ^                    */
      *iv_return = EVAS_ASPECT_CONTROL_HORIZONTAL;
      return PERL_constant_ISIV;
    }
    break;
  case 'T':
    if (memEQ(name, "ELM_NOTIFY_ORIENT_BOTTOM_RIGHT", 30)) {
    /*                               ^                    */
      *iv_return = ELM_NOTIFY_ORIENT_BOTTOM_RIGHT;
      return PERL_constant_ISIV;
    }
    break;
  case 'X':
    if (memEQ(name, "ELM_FLIP_ROTATE_XZ_CENTER_AXIS", 30)) {
    /*                               ^                    */
      *iv_return = ELM_FLIP_ROTATE_XZ_CENTER_AXIS;
      return PERL_constant_ISIV;
    }
    break;
  case 'Y':
    if (memEQ(name, "ELM_FLIP_ROTATE_YZ_CENTER_AXIS", 30)) {
    /*                               ^                    */
      *iv_return = ELM_FLIP_ROTATE_YZ_CENTER_AXIS;
      return PERL_constant_ISIV;
    }
    break;
  }
  return PERL_constant_NOTFOUND;
}

static int
constant (pTHX_ const char *name, STRLEN len, IV *iv_return) {
  /* Initially switch on the length of the name.  */
  /* When generated this function returned values for the list of names given
     in this section of perl code.  Rather than manually editing these functions
     to add or remove constants, which would result in this comment and section
     of code becoming inaccurate, we recommend that you edit this section of
     code, and use it to regenerate a new set of constant functions which you
     then use to replace the originals.

     Regenerate these constant functions by feeding this entire source file to
     perl -x

#!/usr/bin/perl -w
use ExtUtils::Constant qw (constant_types C_constant XS_constant);

my $types = {map {($_, 1)} qw(IV)};
my @names = (qw(),
            {name=>"ELM_FLIP_ROTATE_XZ_CENTER_AXIS", type=>"IV", macro=>"1"},
            {name=>"ELM_FLIP_ROTATE_X_CENTER_AXIS", type=>"IV", macro=>"1"},
            {name=>"ELM_FLIP_ROTATE_YZ_CENTER_AXIS", type=>"IV", macro=>"1"},
            {name=>"ELM_FLIP_ROTATE_Y_CENTER_AXIS", type=>"IV", macro=>"1"},
            {name=>"ELM_GENLIST_ITEM_NONE", type=>"IV", macro=>"1"},
            {name=>"ELM_GENLIST_ITEM_SUBITEMS", type=>"IV", macro=>"1"},
            {name=>"ELM_HOVER_AXIS_BOTH", type=>"IV", macro=>"1"},
            {name=>"ELM_HOVER_AXIS_HORIZONTAL", type=>"IV", macro=>"1"},
            {name=>"ELM_HOVER_AXIS_NONE", type=>"IV", macro=>"1"},
            {name=>"ELM_HOVER_AXIS_VERTICAL", type=>"IV", macro=>"1"},
            {name=>"ELM_ICON_FILE", type=>"IV", macro=>"1"},
            {name=>"ELM_ICON_NONE", type=>"IV", macro=>"1"},
            {name=>"ELM_ICON_STANDARD", type=>"IV", macro=>"1"},
            {name=>"ELM_IMAGE_FLIP_HORIZONTAL", type=>"IV", macro=>"1"},
            {name=>"ELM_IMAGE_FLIP_TRANSPOSE", type=>"IV", macro=>"1"},
            {name=>"ELM_IMAGE_FLIP_TRANSVERSE", type=>"IV", macro=>"1"},
            {name=>"ELM_IMAGE_FLIP_VERTICAL", type=>"IV", macro=>"1"},
            {name=>"ELM_IMAGE_ORIENT_NONE", type=>"IV", macro=>"1"},
            {name=>"ELM_IMAGE_ROTATE_180_CW", type=>"IV", macro=>"1"},
            {name=>"ELM_IMAGE_ROTATE_90_CCW", type=>"IV", macro=>"1"},
            {name=>"ELM_IMAGE_ROTATE_90_CW", type=>"IV", macro=>"1"},
            {name=>"ELM_LIST_COMPRESS", type=>"IV", macro=>"1"},
            {name=>"ELM_LIST_LIMIT", type=>"IV", macro=>"1"},
            {name=>"ELM_LIST_SCROLL", type=>"IV", macro=>"1"},
            {name=>"ELM_MAP_ZOOM_MODE_AUTO_FILL", type=>"IV", macro=>"1"},
            {name=>"ELM_MAP_ZOOM_MODE_AUTO_FIT", type=>"IV", macro=>"1"},
            {name=>"ELM_MAP_ZOOM_MODE_MANUAL", type=>"IV", macro=>"1"},
            {name=>"ELM_NOTIFY_ORIENT_BOTTOM", type=>"IV", macro=>"1"},
            {name=>"ELM_NOTIFY_ORIENT_BOTTOM_LEFT", type=>"IV", macro=>"1"},
            {name=>"ELM_NOTIFY_ORIENT_BOTTOM_RIGHT", type=>"IV", macro=>"1"},
            {name=>"ELM_NOTIFY_ORIENT_LEFT", type=>"IV", macro=>"1"},
            {name=>"ELM_NOTIFY_ORIENT_RIGHT", type=>"IV", macro=>"1"},
            {name=>"ELM_NOTIFY_ORIENT_TOP", type=>"IV", macro=>"1"},
            {name=>"ELM_NOTIFY_ORIENT_TOP_LEFT", type=>"IV", macro=>"1"},
            {name=>"ELM_NOTIFY_ORIENT_TOP_RIGHT", type=>"IV", macro=>"1"},
            {name=>"ELM_PANEL_ORIENT_BOTTOM", type=>"IV", macro=>"1"},
            {name=>"ELM_PANEL_ORIENT_LEFT", type=>"IV", macro=>"1"},
            {name=>"ELM_PANEL_ORIENT_RIGHT", type=>"IV", macro=>"1"},
            {name=>"ELM_PANEL_ORIENT_TOP", type=>"IV", macro=>"1"},
            {name=>"ELM_PHOTOCAM_ZOOM_MODE_AUTO_FILL", type=>"IV", macro=>"1"},
            {name=>"ELM_PHOTOCAM_ZOOM_MODE_AUTO_FIT", type=>"IV", macro=>"1"},
            {name=>"ELM_PHOTOCAM_ZOOM_MODE_MANUAL", type=>"IV", macro=>"1"},
            {name=>"ELM_PI", type=>"IV", macro=>"1"},
            {name=>"ELM_POLICY_LAST", type=>"IV", macro=>"1"},
            {name=>"ELM_POLICY_QUIT", type=>"IV", macro=>"1"},
            {name=>"ELM_POLICY_QUIT_LAST_WINDOW_CLOSED", type=>"IV", macro=>"1"},
            {name=>"ELM_POLICY_QUIT_NONE", type=>"IV", macro=>"1"},
            {name=>"ELM_SCROLLER_POLICY_AUTO", type=>"IV", macro=>"1"},
            {name=>"ELM_SCROLLER_POLICY_OFF", type=>"IV", macro=>"1"},
            {name=>"ELM_SCROLLER_POLICY_ON", type=>"IV", macro=>"1"},
            {name=>"ELM_TEXT_FORMAT_MARKUP_UTF8", type=>"IV", macro=>"1"},
            {name=>"ELM_TEXT_FORMAT_PLAIN_UTF8", type=>"IV", macro=>"1"},
            {name=>"ELM_WIN_BASIC", type=>"IV", macro=>"1"},
            {name=>"ELM_WIN_DESKTOP", type=>"IV", macro=>"1"},
            {name=>"ELM_WIN_DIALOG_BASIC", type=>"IV", macro=>"1"},
            {name=>"ELM_WIN_DOCK", type=>"IV", macro=>"1"},
            {name=>"ELM_WIN_KEYBOARD_ALPHA", type=>"IV", macro=>"1"},
            {name=>"ELM_WIN_KEYBOARD_HEX", type=>"IV", macro=>"1"},
            {name=>"ELM_WIN_KEYBOARD_NUMERIC", type=>"IV", macro=>"1"},
            {name=>"ELM_WIN_KEYBOARD_OFF", type=>"IV", macro=>"1"},
            {name=>"ELM_WIN_KEYBOARD_ON", type=>"IV", macro=>"1"},
            {name=>"ELM_WIN_KEYBOARD_PASSWORD", type=>"IV", macro=>"1"},
            {name=>"ELM_WIN_KEYBOARD_PHONE_NUMBER", type=>"IV", macro=>"1"},
            {name=>"ELM_WIN_KEYBOARD_PIN", type=>"IV", macro=>"1"},
            {name=>"ELM_WIN_KEYBOARD_TERMINAL", type=>"IV", macro=>"1"},
            {name=>"ELM_WIN_KEYBOARD_UNKNOWN", type=>"IV", macro=>"1"},
            {name=>"ELM_WIN_MENU", type=>"IV", macro=>"1"},
            {name=>"ELM_WIN_SPLASH", type=>"IV", macro=>"1"},
            {name=>"ELM_WIN_TOOLBAR", type=>"IV", macro=>"1"},
            {name=>"ELM_WIN_UTILITY", type=>"IV", macro=>"1"},
            {name=>"EVAS_ASPECT_CONTROL_HORIZONTAL", type=>"IV", macro=>"1"},
            {name=>"EVAS_ASPECT_CONTROL_VERTICAL", type=>"IV", macro=>"1"},
            {name=>"EVAS_HINT_EXPAND", type=>"IV", macro=>"1"},
            {name=>"EVAS_HINT_FILL", type=>"IV", macro=>"1"});

print constant_types(), "\n"; # macro defs
foreach (C_constant ("EFL::Elementary", 'constant', 'IV', $types, undef, 3, @names) ) {
    print $_, "\n"; # C constant subs
}
print "\n#### XS Section:\n";
print XS_constant ("EFL::Elementary", $types);
__END__
   */

  switch (len) {
  case 6:
    if (memEQ(name, "ELM_PI", 6)) {
      *iv_return = ELM_PI;
      return PERL_constant_ISIV;
    }
    break;
  case 12:
    /* Names all of length 12.  */
    /* ELM_WIN_DOCK ELM_WIN_MENU */
    /* Offset 8 gives the best switch position.  */
    switch (name[8]) {
    case 'D':
      if (memEQ(name, "ELM_WIN_DOCK", 12)) {
      /*                       ^          */
        *iv_return = ELM_WIN_DOCK;
        return PERL_constant_ISIV;
      }
      break;
    case 'M':
      if (memEQ(name, "ELM_WIN_MENU", 12)) {
      /*                       ^          */
        *iv_return = ELM_WIN_MENU;
        return PERL_constant_ISIV;
      }
      break;
    }
    break;
  case 13:
    return constant_13 (aTHX_ name, iv_return);
    break;
  case 14:
    return constant_14 (aTHX_ name, iv_return);
    break;
  case 15:
    return constant_15 (aTHX_ name, iv_return);
    break;
  case 16:
    if (memEQ(name, "EVAS_HINT_EXPAND", 16)) {
      *iv_return = EVAS_HINT_EXPAND;
      return PERL_constant_ISIV;
    }
    break;
  case 17:
    /* Names all of length 17.  */
    /* ELM_ICON_STANDARD ELM_LIST_COMPRESS */
    /* Offset 15 gives the best switch position.  */
    switch (name[15]) {
    case 'R':
      if (memEQ(name, "ELM_ICON_STANDARD", 17)) {
      /*                              ^        */
        *iv_return = ELM_ICON_STANDARD;
        return PERL_constant_ISIV;
      }
      break;
    case 'S':
      if (memEQ(name, "ELM_LIST_COMPRESS", 17)) {
      /*                              ^        */
        *iv_return = ELM_LIST_COMPRESS;
        return PERL_constant_ISIV;
      }
      break;
    }
    break;
  case 19:
    return constant_19 (aTHX_ name, iv_return);
    break;
  case 20:
    return constant_20 (aTHX_ name, iv_return);
    break;
  case 21:
    return constant_21 (aTHX_ name, iv_return);
    break;
  case 22:
    return constant_22 (aTHX_ name, iv_return);
    break;
  case 23:
    return constant_23 (aTHX_ name, iv_return);
    break;
  case 24:
    return constant_24 (aTHX_ name, iv_return);
    break;
  case 25:
    return constant_25 (aTHX_ name, iv_return);
    break;
  case 26:
    return constant_26 (aTHX_ name, iv_return);
    break;
  case 27:
    return constant_27 (aTHX_ name, iv_return);
    break;
  case 28:
    if (memEQ(name, "EVAS_ASPECT_CONTROL_VERTICAL", 28)) {
      *iv_return = EVAS_ASPECT_CONTROL_VERTICAL;
      return PERL_constant_ISIV;
    }
    break;
  case 29:
    return constant_29 (aTHX_ name, iv_return);
    break;
  case 30:
    return constant_30 (aTHX_ name, iv_return);
    break;
  case 31:
    if (memEQ(name, "ELM_PHOTOCAM_ZOOM_MODE_AUTO_FIT", 31)) {
      *iv_return = ELM_PHOTOCAM_ZOOM_MODE_AUTO_FIT;
      return PERL_constant_ISIV;
    }
    break;
  case 32:
    if (memEQ(name, "ELM_PHOTOCAM_ZOOM_MODE_AUTO_FILL", 32)) {
      *iv_return = ELM_PHOTOCAM_ZOOM_MODE_AUTO_FILL;
      return PERL_constant_ISIV;
    }
    break;
  case 34:
    if (memEQ(name, "ELM_POLICY_QUIT_LAST_WINDOW_CLOSED", 34)) {
      *iv_return = ELM_POLICY_QUIT_LAST_WINDOW_CLOSED;
      return PERL_constant_ISIV;
    }
    break;
  }
  return PERL_constant_NOTFOUND;
}

