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
constant_14 (pTHX_ const char *name, IV *iv_return) {
  /* When generated this function returned values for the list of names given
     here.  However, subsequent manual editing may have added or removed some.
     EVAS_HINT_FILL EVAS_LAYER_MAX EVAS_LAYER_MIN */
  /* Offset 12 gives the best switch position.  */
  switch (name[12]) {
  case 'A':
    if (memEQ(name, "EVAS_LAYER_MAX", 14)) {
    /*                           ^        */
#ifdef EVAS_LAYER_MAX
      *iv_return = EVAS_LAYER_MAX;
      return PERL_constant_ISIV;
#else
      return PERL_constant_NOTDEF;
#endif
    }
    break;
  case 'I':
    if (memEQ(name, "EVAS_LAYER_MIN", 14)) {
    /*                           ^        */
#ifdef EVAS_LAYER_MIN
      *iv_return = EVAS_LAYER_MIN;
      return PERL_constant_ISIV;
#else
      return PERL_constant_NOTDEF;
#endif
    }
    break;
  case 'L':
    if (memEQ(name, "EVAS_HINT_FILL", 14)) {
    /*                           ^        */
#ifdef EVAS_HINT_FILL
      *iv_return = EVAS_HINT_FILL;
      return PERL_constant_ISIV;
#else
      return PERL_constant_NOTDEF;
#endif
    }
    break;
  }
  return PERL_constant_NOTFOUND;
}

static int
constant_15 (pTHX_ const char *name, IV *iv_return) {
  /* When generated this function returned values for the list of names given
     here.  However, subsequent manual editing may have added or removed some.
     EVAS_RENDER_ADD EVAS_RENDER_MUL EVAS_RENDER_SUB */
  /* Offset 14 gives the best switch position.  */
  switch (name[14]) {
  case 'B':
    if (memEQ(name, "EVAS_RENDER_SU", 14)) {
    /*                             B      */
      *iv_return = EVAS_RENDER_SUB;
      return PERL_constant_ISIV;
    }
    break;
  case 'D':
    if (memEQ(name, "EVAS_RENDER_AD", 14)) {
    /*                             D      */
      *iv_return = EVAS_RENDER_ADD;
      return PERL_constant_ISIV;
    }
    break;
  case 'L':
    if (memEQ(name, "EVAS_RENDER_MU", 14)) {
    /*                             L      */
      *iv_return = EVAS_RENDER_MUL;
      return PERL_constant_ISIV;
    }
    break;
  }
  return PERL_constant_NOTFOUND;
}

static int
constant_16 (pTHX_ const char *name, IV *iv_return) {
  /* When generated this function returned values for the list of names given
     here.  However, subsequent manual editing may have added or removed some.
     EVAS_BUTTON_NONE EVAS_HINT_EXPAND EVAS_RENDER_COPY EVAS_RENDER_MASK
     EVAS_RENDER_TINT EVAS_TEXTURE_PAD */
  /* Offset 12 gives the best switch position.  */
  switch (name[12]) {
  case 'C':
    if (memEQ(name, "EVAS_RENDER_COPY", 16)) {
    /*                           ^          */
      *iv_return = EVAS_RENDER_COPY;
      return PERL_constant_ISIV;
    }
    break;
  case 'M':
    if (memEQ(name, "EVAS_RENDER_MASK", 16)) {
    /*                           ^          */
      *iv_return = EVAS_RENDER_MASK;
      return PERL_constant_ISIV;
    }
    break;
  case 'N':
    if (memEQ(name, "EVAS_BUTTON_NONE", 16)) {
    /*                           ^          */
      *iv_return = EVAS_BUTTON_NONE;
      return PERL_constant_ISIV;
    }
    break;
  case 'P':
    if (memEQ(name, "EVAS_HINT_EXPAND", 16)) {
    /*                           ^          */
#ifdef EVAS_HINT_EXPAND
      *iv_return = EVAS_HINT_EXPAND;
      return PERL_constant_ISIV;
#else
      return PERL_constant_NOTDEF;
#endif
    }
    break;
  case 'T':
    if (memEQ(name, "EVAS_RENDER_TINT", 16)) {
    /*                           ^          */
      *iv_return = EVAS_RENDER_TINT;
      return PERL_constant_ISIV;
    }
    break;
  case '_':
    if (memEQ(name, "EVAS_TEXTURE_PAD", 16)) {
    /*                           ^          */
#ifdef EVAS_TEXTURE_PAD
      *iv_return = EVAS_TEXTURE_PAD;
      return PERL_constant_ISIV;
#else
      return PERL_constant_NOTDEF;
#endif
    }
    break;
  }
  return PERL_constant_NOTFOUND;
}

static int
constant_17 (pTHX_ const char *name, IV *iv_return) {
  /* When generated this function returned values for the list of names given
     here.  However, subsequent manual editing may have added or removed some.
     EVAS_CALLBACK_DEL EVAS_RENDER_BLEND EVAS_TEXT_INVALID EVAS_TEXT_SPECIAL */
  /* Offset 14 gives the best switch position.  */
  switch (name[14]) {
  case 'D':
    if (memEQ(name, "EVAS_CALLBACK_DEL", 17)) {
    /*                             ^         */
      *iv_return = EVAS_CALLBACK_DEL;
      return PERL_constant_ISIV;
    }
    break;
  case 'E':
    if (memEQ(name, "EVAS_RENDER_BLEND", 17)) {
    /*                             ^         */
      *iv_return = EVAS_RENDER_BLEND;
      return PERL_constant_ISIV;
    }
    break;
  case 'I':
    if (memEQ(name, "EVAS_TEXT_SPECIAL", 17)) {
    /*                             ^         */
#ifdef EVAS_TEXT_SPECIAL
      *iv_return = EVAS_TEXT_SPECIAL;
      return PERL_constant_ISIV;
#else
      return PERL_constant_NOTDEF;
#endif
    }
    break;
  case 'L':
    if (memEQ(name, "EVAS_TEXT_INVALID", 17)) {
    /*                             ^         */
#ifdef EVAS_TEXT_INVALID
      *iv_return = EVAS_TEXT_INVALID;
      return PERL_constant_ISIV;
#else
      return PERL_constant_NOTDEF;
#endif
    }
    break;
  }
  return PERL_constant_NOTFOUND;
}

static int
constant_18 (pTHX_ const char *name, IV *iv_return) {
  /* When generated this function returned values for the list of names given
     here.  However, subsequent manual editing may have added or removed some.
     EVAS_CALLBACK_FREE EVAS_CALLBACK_HIDE EVAS_CALLBACK_HOLD
     EVAS_CALLBACK_MOVE EVAS_CALLBACK_SHOW */
  /* Offset 16 gives the best switch position.  */
  switch (name[16]) {
  case 'D':
    if (memEQ(name, "EVAS_CALLBACK_HIDE", 18)) {
    /*                               ^        */
      *iv_return = EVAS_CALLBACK_HIDE;
      return PERL_constant_ISIV;
    }
    break;
  case 'E':
    if (memEQ(name, "EVAS_CALLBACK_FREE", 18)) {
    /*                               ^        */
      *iv_return = EVAS_CALLBACK_FREE;
      return PERL_constant_ISIV;
    }
    break;
  case 'L':
    if (memEQ(name, "EVAS_CALLBACK_HOLD", 18)) {
    /*                               ^        */
      *iv_return = EVAS_CALLBACK_HOLD;
      return PERL_constant_ISIV;
    }
    break;
  case 'O':
    if (memEQ(name, "EVAS_CALLBACK_SHOW", 18)) {
    /*                               ^        */
      *iv_return = EVAS_CALLBACK_SHOW;
      return PERL_constant_ISIV;
    }
    break;
  case 'V':
    if (memEQ(name, "EVAS_CALLBACK_MOVE", 18)) {
    /*                               ^        */
      *iv_return = EVAS_CALLBACK_MOVE;
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
     EVAS_RENDER_ADD_REL EVAS_RENDER_SUB_REL EVAS_TEXTURE_REPEAT */
  /* Offset 14 gives the best switch position.  */
  switch (name[14]) {
  case 'B':
    if (memEQ(name, "EVAS_RENDER_SUB_REL", 19)) {
    /*                             ^           */
      *iv_return = EVAS_RENDER_SUB_REL;
      return PERL_constant_ISIV;
    }
    break;
  case 'D':
    if (memEQ(name, "EVAS_RENDER_ADD_REL", 19)) {
    /*                             ^           */
      *iv_return = EVAS_RENDER_ADD_REL;
      return PERL_constant_ISIV;
    }
    break;
  case 'E':
    if (memEQ(name, "EVAS_TEXTURE_REPEAT", 19)) {
    /*                             ^           */
#ifdef EVAS_TEXTURE_REPEAT
      *iv_return = EVAS_TEXTURE_REPEAT;
      return PERL_constant_ISIV;
#else
      return PERL_constant_NOTDEF;
#endif
    }
    break;
  }
  return PERL_constant_NOTFOUND;
}

static int
constant_20 (pTHX_ const char *name, IV *iv_return) {
  /* When generated this function returned values for the list of names given
     here.  However, subsequent manual editing may have added or removed some.
     EVAS_CALLBACK_KEY_UP EVAS_CALLBACK_RESIZE EVAS_EVENT_FLAG_NONE
     EVAS_LOAD_ERROR_NONE EVAS_RENDER_COPY_REL EVAS_RENDER_TINT_REL
     EVAS_TEXTURE_REFLECT EVAS_TEXT_STYLE_GLOW */
  /* Offset 12 gives the best switch position.  */
  switch (name[12]) {
  case 'C':
    if (memEQ(name, "EVAS_RENDER_COPY_REL", 20)) {
    /*                           ^              */
      *iv_return = EVAS_RENDER_COPY_REL;
      return PERL_constant_ISIV;
    }
    break;
  case 'K':
    if (memEQ(name, "EVAS_CALLBACK_KEY_UP", 20)) {
    /*                           ^              */
      *iv_return = EVAS_CALLBACK_KEY_UP;
      return PERL_constant_ISIV;
    }
    if (memEQ(name, "EVAS_CALLBACK_RESIZE", 20)) {
    /*                           ^              */
      *iv_return = EVAS_CALLBACK_RESIZE;
      return PERL_constant_ISIV;
    }
    break;
  case 'L':
    if (memEQ(name, "EVAS_EVENT_FLAG_NONE", 20)) {
    /*                           ^              */
      *iv_return = EVAS_EVENT_FLAG_NONE;
      return PERL_constant_ISIV;
    }
    break;
  case 'R':
    if (memEQ(name, "EVAS_LOAD_ERROR_NONE", 20)) {
    /*                           ^              */
#ifdef EVAS_LOAD_ERROR_NONE
      *iv_return = EVAS_LOAD_ERROR_NONE;
      return PERL_constant_ISIV;
#else
      return PERL_constant_NOTDEF;
#endif
    }
    break;
  case 'T':
    if (memEQ(name, "EVAS_RENDER_TINT_REL", 20)) {
    /*                           ^              */
      *iv_return = EVAS_RENDER_TINT_REL;
      return PERL_constant_ISIV;
    }
    break;
  case 'Y':
    if (memEQ(name, "EVAS_TEXT_STYLE_GLOW", 20)) {
    /*                           ^              */
      *iv_return = EVAS_TEXT_STYLE_GLOW;
      return PERL_constant_ISIV;
    }
    break;
  case '_':
    if (memEQ(name, "EVAS_TEXTURE_REFLECT", 20)) {
    /*                           ^              */
#ifdef EVAS_TEXTURE_REFLECT
      *iv_return = EVAS_TEXTURE_REFLECT;
      return PERL_constant_ISIV;
#else
      return PERL_constant_NOTDEF;
#endif
    }
    break;
  }
  return PERL_constant_NOTFOUND;
}

static int
constant_21 (pTHX_ const char *name, IV *iv_return) {
  /* When generated this function returned values for the list of names given
     here.  However, subsequent manual editing may have added or removed some.
     EVAS_ALLOC_ERROR_NONE EVAS_BORDER_FILL_NONE EVAS_CALLBACK_RESTACK
     EVAS_COLOR_SPACE_AHSV EVAS_COLOR_SPACE_ARGB EVAS_RENDER_BLEND_REL
     EVAS_TEXTURE_RESTRICT EVAS_TEXT_STYLE_PLAIN */
  /* Offset 20 gives the best switch position.  */
  switch (name[20]) {
  case 'B':
    if (memEQ(name, "EVAS_COLOR_SPACE_ARG", 20)) {
    /*                                   B      */
#ifdef EVAS_COLOR_SPACE_ARGB
      *iv_return = EVAS_COLOR_SPACE_ARGB;
      return PERL_constant_ISIV;
#else
      return PERL_constant_NOTDEF;
#endif
    }
    break;
  case 'E':
    if (memEQ(name, "EVAS_ALLOC_ERROR_NON", 20)) {
    /*                                   E      */
#ifdef EVAS_ALLOC_ERROR_NONE
      *iv_return = EVAS_ALLOC_ERROR_NONE;
      return PERL_constant_ISIV;
#else
      return PERL_constant_NOTDEF;
#endif
    }
    if (memEQ(name, "EVAS_BORDER_FILL_NON", 20)) {
    /*                                   E      */
      *iv_return = EVAS_BORDER_FILL_NONE;
      return PERL_constant_ISIV;
    }
    break;
  case 'K':
    if (memEQ(name, "EVAS_CALLBACK_RESTAC", 20)) {
    /*                                   K      */
      *iv_return = EVAS_CALLBACK_RESTACK;
      return PERL_constant_ISIV;
    }
    break;
  case 'L':
    if (memEQ(name, "EVAS_RENDER_BLEND_RE", 20)) {
    /*                                   L      */
      *iv_return = EVAS_RENDER_BLEND_REL;
      return PERL_constant_ISIV;
    }
    break;
  case 'N':
    if (memEQ(name, "EVAS_TEXT_STYLE_PLAI", 20)) {
    /*                                   N      */
      *iv_return = EVAS_TEXT_STYLE_PLAIN;
      return PERL_constant_ISIV;
    }
    break;
  case 'T':
    if (memEQ(name, "EVAS_TEXTURE_RESTRIC", 20)) {
    /*                                   T      */
#ifdef EVAS_TEXTURE_RESTRICT
      *iv_return = EVAS_TEXTURE_RESTRICT;
      return PERL_constant_ISIV;
#else
      return PERL_constant_NOTDEF;
#endif
    }
    break;
  case 'V':
    if (memEQ(name, "EVAS_COLOR_SPACE_AHS", 20)) {
    /*                                   V      */
#ifdef EVAS_COLOR_SPACE_AHSV
      *iv_return = EVAS_COLOR_SPACE_AHSV;
      return PERL_constant_ISIV;
#else
      return PERL_constant_NOTDEF;
#endif
    }
    break;
  }
  return PERL_constant_NOTFOUND;
}

static int
constant_22 (pTHX_ const char *name, IV *iv_return) {
  /* When generated this function returned values for the list of names given
     here.  However, subsequent manual editing may have added or removed some.
     EVAS_ALLOC_ERROR_FATAL EVAS_BORDER_FILL_SOLID EVAS_CALLBACK_FOCUS_IN
     EVAS_CALLBACK_KEY_DOWN EVAS_CALLBACK_MOUSE_IN EVAS_CALLBACK_MOUSE_UP
     EVAS_FONT_HINTING_AUTO EVAS_FONT_HINTING_NONE EVAS_PIXEL_FORMAT_NONE
     EVAS_TEXT_STYLE_SHADOW */
  /* Offset 16 gives the best switch position.  */
  switch (name[16]) {
  case 'C':
    if (memEQ(name, "EVAS_CALLBACK_FOCUS_IN", 22)) {
    /*                               ^            */
      *iv_return = EVAS_CALLBACK_FOCUS_IN;
      return PERL_constant_ISIV;
    }
    break;
  case 'G':
    if (memEQ(name, "EVAS_FONT_HINTING_AUTO", 22)) {
    /*                               ^            */
      *iv_return = EVAS_FONT_HINTING_AUTO;
      return PERL_constant_ISIV;
    }
    if (memEQ(name, "EVAS_FONT_HINTING_NONE", 22)) {
    /*                               ^            */
      *iv_return = EVAS_FONT_HINTING_NONE;
      return PERL_constant_ISIV;
    }
    break;
  case 'S':
    if (memEQ(name, "EVAS_TEXT_STYLE_SHADOW", 22)) {
    /*                               ^            */
      *iv_return = EVAS_TEXT_STYLE_SHADOW;
      return PERL_constant_ISIV;
    }
    break;
  case 'T':
    if (memEQ(name, "EVAS_PIXEL_FORMAT_NONE", 22)) {
    /*                               ^            */
#ifdef EVAS_PIXEL_FORMAT_NONE
      *iv_return = EVAS_PIXEL_FORMAT_NONE;
      return PERL_constant_ISIV;
#else
      return PERL_constant_NOTDEF;
#endif
    }
    break;
  case 'U':
    if (memEQ(name, "EVAS_CALLBACK_MOUSE_IN", 22)) {
    /*                               ^            */
      *iv_return = EVAS_CALLBACK_MOUSE_IN;
      return PERL_constant_ISIV;
    }
    if (memEQ(name, "EVAS_CALLBACK_MOUSE_UP", 22)) {
    /*                               ^            */
      *iv_return = EVAS_CALLBACK_MOUSE_UP;
      return PERL_constant_ISIV;
    }
    break;
  case 'Y':
    if (memEQ(name, "EVAS_CALLBACK_KEY_DOWN", 22)) {
    /*                               ^            */
      *iv_return = EVAS_CALLBACK_KEY_DOWN;
      return PERL_constant_ISIV;
    }
    break;
  case '_':
    if (memEQ(name, "EVAS_ALLOC_ERROR_FATAL", 22)) {
    /*                               ^            */
#ifdef EVAS_ALLOC_ERROR_FATAL
      *iv_return = EVAS_ALLOC_ERROR_FATAL;
      return PERL_constant_ISIV;
#else
      return PERL_constant_NOTDEF;
#endif
    }
    if (memEQ(name, "EVAS_BORDER_FILL_SOLID", 22)) {
    /*                               ^            */
      *iv_return = EVAS_BORDER_FILL_SOLID;
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
     EVAS_CALLBACK_FOCUS_OUT EVAS_CALLBACK_MOUSE_OUT EVAS_EVENT_FLAG_ON_HOLD
     EVAS_LOAD_ERROR_GENERIC EVAS_TEXTBLOCK_TEXT_RAW EVAS_TEXT_STYLE_OUTLINE */
  /* Offset 14 gives the best switch position.  */
  switch (name[14]) {
  case 'E':
    if (memEQ(name, "EVAS_TEXT_STYLE_OUTLINE", 23)) {
    /*                             ^               */
      *iv_return = EVAS_TEXT_STYLE_OUTLINE;
      return PERL_constant_ISIV;
    }
    break;
  case 'F':
    if (memEQ(name, "EVAS_CALLBACK_FOCUS_OUT", 23)) {
    /*                             ^               */
      *iv_return = EVAS_CALLBACK_FOCUS_OUT;
      return PERL_constant_ISIV;
    }
    break;
  case 'G':
    if (memEQ(name, "EVAS_EVENT_FLAG_ON_HOLD", 23)) {
    /*                             ^               */
      *iv_return = EVAS_EVENT_FLAG_ON_HOLD;
      return PERL_constant_ISIV;
    }
    break;
  case 'M':
    if (memEQ(name, "EVAS_CALLBACK_MOUSE_OUT", 23)) {
    /*                             ^               */
      *iv_return = EVAS_CALLBACK_MOUSE_OUT;
      return PERL_constant_ISIV;
    }
    break;
  case 'R':
    if (memEQ(name, "EVAS_LOAD_ERROR_GENERIC", 23)) {
    /*                             ^               */
#ifdef EVAS_LOAD_ERROR_GENERIC
      *iv_return = EVAS_LOAD_ERROR_GENERIC;
      return PERL_constant_ISIV;
#else
      return PERL_constant_NOTDEF;
#endif
    }
    break;
  case '_':
    if (memEQ(name, "EVAS_TEXTBLOCK_TEXT_RAW", 23)) {
    /*                             ^               */
      *iv_return = EVAS_TEXTBLOCK_TEXT_RAW;
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
     EVAS_ASPECT_CONTROL_BOTH EVAS_ASPECT_CONTROL_NONE EVAS_BORDER_FILL_DEFAULT
     EVAS_BUTTON_DOUBLE_CLICK EVAS_BUTTON_TRIPLE_CLICK EVAS_CALLBACK_MOUSE_DOWN
     EVAS_CALLBACK_MOUSE_MOVE EVAS_COLORSPACE_ARGB8888 EVAS_PIXEL_FORMAT_ARGB32
     EVAS_SMART_CLASS_VERSION */
  /* Offset 20 gives the best switch position.  */
  switch (name[20]) {
  case '8':
    if (memEQ(name, "EVAS_COLORSPACE_ARGB8888", 24)) {
    /*                                   ^          */
      *iv_return = EVAS_COLORSPACE_ARGB8888;
      return PERL_constant_ISIV;
    }
    break;
  case 'A':
    if (memEQ(name, "EVAS_BORDER_FILL_DEFAULT", 24)) {
    /*                                   ^          */
      *iv_return = EVAS_BORDER_FILL_DEFAULT;
      return PERL_constant_ISIV;
    }
    break;
  case 'B':
    if (memEQ(name, "EVAS_ASPECT_CONTROL_BOTH", 24)) {
    /*                                   ^          */
      *iv_return = EVAS_ASPECT_CONTROL_BOTH;
      return PERL_constant_ISIV;
    }
    break;
  case 'D':
    if (memEQ(name, "EVAS_CALLBACK_MOUSE_DOWN", 24)) {
    /*                                   ^          */
      *iv_return = EVAS_CALLBACK_MOUSE_DOWN;
      return PERL_constant_ISIV;
    }
    break;
  case 'G':
    if (memEQ(name, "EVAS_PIXEL_FORMAT_ARGB32", 24)) {
    /*                                   ^          */
#ifdef EVAS_PIXEL_FORMAT_ARGB32
      *iv_return = EVAS_PIXEL_FORMAT_ARGB32;
      return PERL_constant_ISIV;
#else
      return PERL_constant_NOTDEF;
#endif
    }
    break;
  case 'L':
    if (memEQ(name, "EVAS_BUTTON_DOUBLE_CLICK", 24)) {
    /*                                   ^          */
      *iv_return = EVAS_BUTTON_DOUBLE_CLICK;
      return PERL_constant_ISIV;
    }
    if (memEQ(name, "EVAS_BUTTON_TRIPLE_CLICK", 24)) {
    /*                                   ^          */
      *iv_return = EVAS_BUTTON_TRIPLE_CLICK;
      return PERL_constant_ISIV;
    }
    break;
  case 'M':
    if (memEQ(name, "EVAS_CALLBACK_MOUSE_MOVE", 24)) {
    /*                                   ^          */
      *iv_return = EVAS_CALLBACK_MOUSE_MOVE;
      return PERL_constant_ISIV;
    }
    break;
  case 'N':
    if (memEQ(name, "EVAS_ASPECT_CONTROL_NONE", 24)) {
    /*                                   ^          */
      *iv_return = EVAS_ASPECT_CONTROL_NONE;
      return PERL_constant_ISIV;
    }
    break;
  case 'S':
    if (memEQ(name, "EVAS_SMART_CLASS_VERSION", 24)) {
    /*                                   ^          */
#ifdef EVAS_SMART_CLASS_VERSION
      *iv_return = EVAS_SMART_CLASS_VERSION;
      return PERL_constant_ISIV;
#else
      return PERL_constant_NOTDEF;
#endif
    }
    break;
  }
  return PERL_constant_NOTFOUND;
}

static int
constant_26 (pTHX_ const char *name, IV *iv_return) {
  /* When generated this function returned values for the list of names given
     here.  However, subsequent manual editing may have added or removed some.
     EVAS_ALLOC_ERROR_RECOVERED EVAS_COLORSPACE_RGB565_A5P
     EVAS_FONT_HINTING_BYTECODE EVAS_IMAGE_SCALE_HINT_NONE
     EVAS_TEXTBLOCK_TEXT_MARKUP EVAS_TEXT_STYLE_FAR_SHADOW */
  /* Offset 11 gives the best switch position.  */
  switch (name[11]) {
  case 'E':
    if (memEQ(name, "EVAS_ALLOC_ERROR_RECOVERED", 26)) {
    /*                          ^                     */
#ifdef EVAS_ALLOC_ERROR_RECOVERED
      *iv_return = EVAS_ALLOC_ERROR_RECOVERED;
      return PERL_constant_ISIV;
#else
      return PERL_constant_NOTDEF;
#endif
    }
    break;
  case 'I':
    if (memEQ(name, "EVAS_FONT_HINTING_BYTECODE", 26)) {
    /*                          ^                     */
      *iv_return = EVAS_FONT_HINTING_BYTECODE;
      return PERL_constant_ISIV;
    }
    break;
  case 'O':
    if (memEQ(name, "EVAS_TEXTBLOCK_TEXT_MARKUP", 26)) {
    /*                          ^                     */
      *iv_return = EVAS_TEXTBLOCK_TEXT_MARKUP;
      return PERL_constant_ISIV;
    }
    break;
  case 'P':
    if (memEQ(name, "EVAS_COLORSPACE_RGB565_A5P", 26)) {
    /*                          ^                     */
      *iv_return = EVAS_COLORSPACE_RGB565_A5P;
      return PERL_constant_ISIV;
    }
    break;
  case 'S':
    if (memEQ(name, "EVAS_IMAGE_SCALE_HINT_NONE", 26)) {
    /*                          ^                     */
      *iv_return = EVAS_IMAGE_SCALE_HINT_NONE;
      return PERL_constant_ISIV;
    }
    break;
  case 'T':
    if (memEQ(name, "EVAS_TEXT_STYLE_FAR_SHADOW", 26)) {
    /*                          ^                     */
      *iv_return = EVAS_TEXT_STYLE_FAR_SHADOW;
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
     EVAS_ASPECT_CONTROL_NEITHER EVAS_OBJECT_BOX_API_VERSION
     EVAS_TEXT_STYLE_SOFT_SHADOW */
  /* Offset 24 gives the best switch position.  */
  switch (name[24]) {
  case 'D':
    if (memEQ(name, "EVAS_TEXT_STYLE_SOFT_SHADOW", 27)) {
    /*                                       ^         */
      *iv_return = EVAS_TEXT_STYLE_SOFT_SHADOW;
      return PERL_constant_ISIV;
    }
    break;
  case 'H':
    if (memEQ(name, "EVAS_ASPECT_CONTROL_NEITHER", 27)) {
    /*                                       ^         */
      *iv_return = EVAS_ASPECT_CONTROL_NEITHER;
      return PERL_constant_ISIV;
    }
    break;
  case 'I':
    if (memEQ(name, "EVAS_OBJECT_BOX_API_VERSION", 27)) {
    /*                                       ^         */
#ifdef EVAS_OBJECT_BOX_API_VERSION
      *iv_return = EVAS_OBJECT_BOX_API_VERSION;
      return PERL_constant_ISIV;
#else
      return PERL_constant_NOTDEF;
#endif
    }
    break;
  }
  return PERL_constant_NOTFOUND;
}

static int
constant_28 (pTHX_ const char *name, IV *iv_return) {
  /* When generated this function returned values for the list of names given
     here.  However, subsequent manual editing may have added or removed some.
     EVAS_ASPECT_CONTROL_VERTICAL EVAS_IMAGE_SCALE_HINT_STATIC
     EVAS_LOAD_ERROR_CORRUPT_FILE EVAS_TEXTURE_RESTRICT_REPEAT
     EVAS_TEXT_STYLE_SOFT_OUTLINE */
  /* Offset 24 gives the best switch position.  */
  switch (name[24]) {
  case 'A':
    if (memEQ(name, "EVAS_IMAGE_SCALE_HINT_STATIC", 28)) {
    /*                                       ^          */
      *iv_return = EVAS_IMAGE_SCALE_HINT_STATIC;
      return PERL_constant_ISIV;
    }
    break;
  case 'F':
    if (memEQ(name, "EVAS_LOAD_ERROR_CORRUPT_FILE", 28)) {
    /*                                       ^          */
#ifdef EVAS_LOAD_ERROR_CORRUPT_FILE
      *iv_return = EVAS_LOAD_ERROR_CORRUPT_FILE;
      return PERL_constant_ISIV;
#else
      return PERL_constant_NOTDEF;
#endif
    }
    break;
  case 'I':
    if (memEQ(name, "EVAS_ASPECT_CONTROL_VERTICAL", 28)) {
    /*                                       ^          */
      *iv_return = EVAS_ASPECT_CONTROL_VERTICAL;
      return PERL_constant_ISIV;
    }
    break;
  case 'L':
    if (memEQ(name, "EVAS_TEXT_STYLE_SOFT_OUTLINE", 28)) {
    /*                                       ^          */
      *iv_return = EVAS_TEXT_STYLE_SOFT_OUTLINE;
      return PERL_constant_ISIV;
    }
    break;
  case 'P':
    if (memEQ(name, "EVAS_TEXTURE_RESTRICT_REPEAT", 28)) {
    /*                                       ^          */
#ifdef EVAS_TEXTURE_RESTRICT_REPEAT
      *iv_return = EVAS_TEXTURE_RESTRICT_REPEAT;
      return PERL_constant_ISIV;
#else
      return PERL_constant_NOTDEF;
#endif
    }
    break;
  }
  return PERL_constant_NOTFOUND;
}

static int
constant_29 (pTHX_ const char *name, IV *iv_return) {
  /* When generated this function returned values for the list of names given
     here.  However, subsequent manual editing may have added or removed some.
     EVAS_CALLBACK_IMAGE_PRELOADED EVAS_IMAGE_SCALE_HINT_DYNAMIC
     EVAS_PIXEL_FORMAT_YUV420P_601 EVAS_TEXTURE_RESTRICT_REFLECT */
  /* Offset 14 gives the best switch position.  */
  switch (name[14]) {
  case 'E':
    if (memEQ(name, "EVAS_TEXTURE_RESTRICT_REFLECT", 29)) {
    /*                             ^                     */
#ifdef EVAS_TEXTURE_RESTRICT_REFLECT
      *iv_return = EVAS_TEXTURE_RESTRICT_REFLECT;
      return PERL_constant_ISIV;
#else
      return PERL_constant_NOTDEF;
#endif
    }
    break;
  case 'I':
    if (memEQ(name, "EVAS_CALLBACK_IMAGE_PRELOADED", 29)) {
    /*                             ^                     */
      *iv_return = EVAS_CALLBACK_IMAGE_PRELOADED;
      return PERL_constant_ISIV;
    }
    break;
  case 'L':
    if (memEQ(name, "EVAS_IMAGE_SCALE_HINT_DYNAMIC", 29)) {
    /*                             ^                     */
      *iv_return = EVAS_IMAGE_SCALE_HINT_DYNAMIC;
      return PERL_constant_ISIV;
    }
    break;
  case 'M':
    if (memEQ(name, "EVAS_PIXEL_FORMAT_YUV420P_601", 29)) {
    /*                             ^                     */
#ifdef EVAS_PIXEL_FORMAT_YUV420P_601
      *iv_return = EVAS_PIXEL_FORMAT_YUV420P_601;
      return PERL_constant_ISIV;
#else
      return PERL_constant_NOTDEF;
#endif
    }
    break;
  }
  return PERL_constant_NOTFOUND;
}

static int
constant_30 (pTHX_ const char *name, IV *iv_return) {
  /* When generated this function returned values for the list of names given
     here.  However, subsequent manual editing may have added or removed some.
     EVAS_ASPECT_CONTROL_HORIZONTAL EVAS_LOAD_ERROR_DOES_NOT_EXIST
     EVAS_LOAD_ERROR_UNKNOWN_FORMAT EVAS_TEXT_STYLE_OUTLINE_SHADOW */
  /* Offset 22 gives the best switch position.  */
  switch (name[22]) {
  case 'E':
    if (memEQ(name, "EVAS_TEXT_STYLE_OUTLINE_SHADOW", 30)) {
    /*                                     ^              */
      *iv_return = EVAS_TEXT_STYLE_OUTLINE_SHADOW;
      return PERL_constant_ISIV;
    }
    break;
  case 'N':
    if (memEQ(name, "EVAS_LOAD_ERROR_UNKNOWN_FORMAT", 30)) {
    /*                                     ^              */
#ifdef EVAS_LOAD_ERROR_UNKNOWN_FORMAT
      *iv_return = EVAS_LOAD_ERROR_UNKNOWN_FORMAT;
      return PERL_constant_ISIV;
#else
      return PERL_constant_NOTDEF;
#endif
    }
    break;
  case 'O':
    if (memEQ(name, "EVAS_LOAD_ERROR_DOES_NOT_EXIST", 30)) {
    /*                                     ^              */
#ifdef EVAS_LOAD_ERROR_DOES_NOT_EXIST
      *iv_return = EVAS_LOAD_ERROR_DOES_NOT_EXIST;
      return PERL_constant_ISIV;
#else
      return PERL_constant_NOTDEF;
#endif
    }
    break;
  case 'R':
    if (memEQ(name, "EVAS_ASPECT_CONTROL_HORIZONTAL", 30)) {
    /*                                     ^              */
      *iv_return = EVAS_ASPECT_CONTROL_HORIZONTAL;
      return PERL_constant_ISIV;
    }
    break;
  }
  return PERL_constant_NOTFOUND;
}

static int
constant_31 (pTHX_ const char *name, IV *iv_return) {
  /* When generated this function returned values for the list of names given
     here.  However, subsequent manual editing may have added or removed some.
     EVAS_COLORSPACE_YCBCR422P601_PL EVAS_COLORSPACE_YCBCR422P709_PL
     EVAS_OBJECT_POINTER_MODE_NOGRAB EVAS_TEXT_STYLE_FAR_SOFT_SHADOW */
  /* Offset 27 gives the best switch position.  */
  switch (name[27]) {
  case '1':
    if (memEQ(name, "EVAS_COLORSPACE_YCBCR422P601_PL", 31)) {
    /*                                          ^          */
      *iv_return = EVAS_COLORSPACE_YCBCR422P601_PL;
      return PERL_constant_ISIV;
    }
    break;
  case '9':
    if (memEQ(name, "EVAS_COLORSPACE_YCBCR422P709_PL", 31)) {
    /*                                          ^          */
      *iv_return = EVAS_COLORSPACE_YCBCR422P709_PL;
      return PERL_constant_ISIV;
    }
    break;
  case 'A':
    if (memEQ(name, "EVAS_TEXT_STYLE_FAR_SOFT_SHADOW", 31)) {
    /*                                          ^          */
      *iv_return = EVAS_TEXT_STYLE_FAR_SOFT_SHADOW;
      return PERL_constant_ISIV;
    }
    break;
  case 'G':
    if (memEQ(name, "EVAS_OBJECT_POINTER_MODE_NOGRAB", 31)) {
    /*                                          ^          */
      *iv_return = EVAS_OBJECT_POINTER_MODE_NOGRAB;
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
my @names = (qw(EVAS_ALLOC_ERROR_FATAL EVAS_ALLOC_ERROR_NONE
	       EVAS_ALLOC_ERROR_RECOVERED EVAS_COLOR_SPACE_AHSV
	       EVAS_COLOR_SPACE_ARGB EVAS_HINT_EXPAND EVAS_HINT_FILL
	       EVAS_LAYER_MAX EVAS_LAYER_MIN EVAS_LOAD_ERROR_CORRUPT_FILE
	       EVAS_LOAD_ERROR_DOES_NOT_EXIST EVAS_LOAD_ERROR_GENERIC
	       EVAS_LOAD_ERROR_NONE EVAS_LOAD_ERROR_PERMISSION_DENIED
	       EVAS_LOAD_ERROR_RESOURCE_ALLOCATION_FAILED
	       EVAS_LOAD_ERROR_UNKNOWN_FORMAT EVAS_OBJECT_BOX_API_VERSION
	       EVAS_PIXEL_FORMAT_ARGB32 EVAS_PIXEL_FORMAT_NONE
	       EVAS_PIXEL_FORMAT_YUV420P_601 EVAS_SMART_CLASS_VERSION
	       EVAS_TEXTURE_PAD EVAS_TEXTURE_REFLECT EVAS_TEXTURE_REPEAT
	       EVAS_TEXTURE_RESTRICT EVAS_TEXTURE_RESTRICT_REFLECT
	       EVAS_TEXTURE_RESTRICT_REPEAT EVAS_TEXT_INVALID
	       EVAS_TEXT_SPECIAL),
            {name=>"EVAS_ASPECT_CONTROL_BOTH", type=>"IV", macro=>"1"},
            {name=>"EVAS_ASPECT_CONTROL_HORIZONTAL", type=>"IV", macro=>"1"},
            {name=>"EVAS_ASPECT_CONTROL_NEITHER", type=>"IV", macro=>"1"},
            {name=>"EVAS_ASPECT_CONTROL_NONE", type=>"IV", macro=>"1"},
            {name=>"EVAS_ASPECT_CONTROL_VERTICAL", type=>"IV", macro=>"1"},
            {name=>"EVAS_BORDER_FILL_DEFAULT", type=>"IV", macro=>"1"},
            {name=>"EVAS_BORDER_FILL_NONE", type=>"IV", macro=>"1"},
            {name=>"EVAS_BORDER_FILL_SOLID", type=>"IV", macro=>"1"},
            {name=>"EVAS_BUTTON_DOUBLE_CLICK", type=>"IV", macro=>"1"},
            {name=>"EVAS_BUTTON_NONE", type=>"IV", macro=>"1"},
            {name=>"EVAS_BUTTON_TRIPLE_CLICK", type=>"IV", macro=>"1"},
            {name=>"EVAS_CALLBACK_CHANGED_SIZE_HINTS", type=>"IV", macro=>"1"},
            {name=>"EVAS_CALLBACK_DEL", type=>"IV", macro=>"1"},
            {name=>"EVAS_CALLBACK_FOCUS_IN", type=>"IV", macro=>"1"},
            {name=>"EVAS_CALLBACK_FOCUS_OUT", type=>"IV", macro=>"1"},
            {name=>"EVAS_CALLBACK_FREE", type=>"IV", macro=>"1"},
            {name=>"EVAS_CALLBACK_HIDE", type=>"IV", macro=>"1"},
            {name=>"EVAS_CALLBACK_HOLD", type=>"IV", macro=>"1"},
            {name=>"EVAS_CALLBACK_IMAGE_PRELOADED", type=>"IV", macro=>"1"},
            {name=>"EVAS_CALLBACK_KEY_DOWN", type=>"IV", macro=>"1"},
            {name=>"EVAS_CALLBACK_KEY_UP", type=>"IV", macro=>"1"},
            {name=>"EVAS_CALLBACK_MOUSE_DOWN", type=>"IV", macro=>"1"},
            {name=>"EVAS_CALLBACK_MOUSE_IN", type=>"IV", macro=>"1"},
            {name=>"EVAS_CALLBACK_MOUSE_MOVE", type=>"IV", macro=>"1"},
            {name=>"EVAS_CALLBACK_MOUSE_OUT", type=>"IV", macro=>"1"},
            {name=>"EVAS_CALLBACK_MOUSE_UP", type=>"IV", macro=>"1"},
            {name=>"EVAS_CALLBACK_MOUSE_WHEEL", type=>"IV", macro=>"1"},
            {name=>"EVAS_CALLBACK_MOVE", type=>"IV", macro=>"1"},
            {name=>"EVAS_CALLBACK_RESIZE", type=>"IV", macro=>"1"},
            {name=>"EVAS_CALLBACK_RESTACK", type=>"IV", macro=>"1"},
            {name=>"EVAS_CALLBACK_SHOW", type=>"IV", macro=>"1"},
            {name=>"EVAS_COLORSPACE_ARGB8888", type=>"IV", macro=>"1"},
            {name=>"EVAS_COLORSPACE_RGB565_A5P", type=>"IV", macro=>"1"},
            {name=>"EVAS_COLORSPACE_YCBCR422P601_PL", type=>"IV", macro=>"1"},
            {name=>"EVAS_COLORSPACE_YCBCR422P709_PL", type=>"IV", macro=>"1"},
            {name=>"EVAS_EVENT_FLAG_NONE", type=>"IV", macro=>"1"},
            {name=>"EVAS_EVENT_FLAG_ON_HOLD", type=>"IV", macro=>"1"},
            {name=>"EVAS_FONT_HINTING_AUTO", type=>"IV", macro=>"1"},
            {name=>"EVAS_FONT_HINTING_BYTECODE", type=>"IV", macro=>"1"},
            {name=>"EVAS_FONT_HINTING_NONE", type=>"IV", macro=>"1"},
            {name=>"EVAS_IMAGE_SCALE_HINT_DYNAMIC", type=>"IV", macro=>"1"},
            {name=>"EVAS_IMAGE_SCALE_HINT_NONE", type=>"IV", macro=>"1"},
            {name=>"EVAS_IMAGE_SCALE_HINT_STATIC", type=>"IV", macro=>"1"},
            {name=>"EVAS_OBJECT_POINTER_MODE_AUTOGRAB", type=>"IV", macro=>"1"},
            {name=>"EVAS_OBJECT_POINTER_MODE_NOGRAB", type=>"IV", macro=>"1"},
            {name=>"EVAS_OBJECT_TABLE_HOMOGENEOUS_ITEM", type=>"IV", macro=>"1"},
            {name=>"EVAS_OBJECT_TABLE_HOMOGENEOUS_NONE", type=>"IV", macro=>"1"},
            {name=>"EVAS_OBJECT_TABLE_HOMOGENEOUS_TABLE", type=>"IV", macro=>"1"},
            {name=>"EVAS_RENDER_ADD", type=>"IV", macro=>"1"},
            {name=>"EVAS_RENDER_ADD_REL", type=>"IV", macro=>"1"},
            {name=>"EVAS_RENDER_BLEND", type=>"IV", macro=>"1"},
            {name=>"EVAS_RENDER_BLEND_REL", type=>"IV", macro=>"1"},
            {name=>"EVAS_RENDER_COPY", type=>"IV", macro=>"1"},
            {name=>"EVAS_RENDER_COPY_REL", type=>"IV", macro=>"1"},
            {name=>"EVAS_RENDER_MASK", type=>"IV", macro=>"1"},
            {name=>"EVAS_RENDER_MUL", type=>"IV", macro=>"1"},
            {name=>"EVAS_RENDER_SUB", type=>"IV", macro=>"1"},
            {name=>"EVAS_RENDER_SUB_REL", type=>"IV", macro=>"1"},
            {name=>"EVAS_RENDER_TINT", type=>"IV", macro=>"1"},
            {name=>"EVAS_RENDER_TINT_REL", type=>"IV", macro=>"1"},
            {name=>"EVAS_TEXTBLOCK_TEXT_MARKUP", type=>"IV", macro=>"1"},
            {name=>"EVAS_TEXTBLOCK_TEXT_PLAIN", type=>"IV", macro=>"1"},
            {name=>"EVAS_TEXTBLOCK_TEXT_RAW", type=>"IV", macro=>"1"},
            {name=>"EVAS_TEXT_STYLE_FAR_SHADOW", type=>"IV", macro=>"1"},
            {name=>"EVAS_TEXT_STYLE_FAR_SOFT_SHADOW", type=>"IV", macro=>"1"},
            {name=>"EVAS_TEXT_STYLE_GLOW", type=>"IV", macro=>"1"},
            {name=>"EVAS_TEXT_STYLE_OUTLINE", type=>"IV", macro=>"1"},
            {name=>"EVAS_TEXT_STYLE_OUTLINE_SHADOW", type=>"IV", macro=>"1"},
            {name=>"EVAS_TEXT_STYLE_OUTLINE_SOFT_SHADOW", type=>"IV", macro=>"1"},
            {name=>"EVAS_TEXT_STYLE_PLAIN", type=>"IV", macro=>"1"},
            {name=>"EVAS_TEXT_STYLE_SHADOW", type=>"IV", macro=>"1"},
            {name=>"EVAS_TEXT_STYLE_SOFT_OUTLINE", type=>"IV", macro=>"1"},
            {name=>"EVAS_TEXT_STYLE_SOFT_SHADOW", type=>"IV", macro=>"1"});

print constant_types(), "\n"; # macro defs
foreach (C_constant ("EFL::Evas", 'constant', 'IV', $types, undef, 3, @names) ) {
    print $_, "\n"; # C constant subs
}
print "\n#### XS Section:\n";
print XS_constant ("EFL::Evas", $types);
__END__
   */

  switch (len) {
  case 14:
    return constant_14 (aTHX_ name, iv_return);
    break;
  case 15:
    return constant_15 (aTHX_ name, iv_return);
    break;
  case 16:
    return constant_16 (aTHX_ name, iv_return);
    break;
  case 17:
    return constant_17 (aTHX_ name, iv_return);
    break;
  case 18:
    return constant_18 (aTHX_ name, iv_return);
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
    /* Names all of length 25.  */
    /* EVAS_CALLBACK_MOUSE_WHEEL EVAS_TEXTBLOCK_TEXT_PLAIN */
    /* Offset 24 gives the best switch position.  */
    switch (name[24]) {
    case 'L':
      if (memEQ(name, "EVAS_CALLBACK_MOUSE_WHEE", 24)) {
      /*                                       L      */
        *iv_return = EVAS_CALLBACK_MOUSE_WHEEL;
        return PERL_constant_ISIV;
      }
      break;
    case 'N':
      if (memEQ(name, "EVAS_TEXTBLOCK_TEXT_PLAI", 24)) {
      /*                                       N      */
        *iv_return = EVAS_TEXTBLOCK_TEXT_PLAIN;
        return PERL_constant_ISIV;
      }
      break;
    }
    break;
  case 26:
    return constant_26 (aTHX_ name, iv_return);
    break;
  case 27:
    return constant_27 (aTHX_ name, iv_return);
    break;
  case 28:
    return constant_28 (aTHX_ name, iv_return);
    break;
  case 29:
    return constant_29 (aTHX_ name, iv_return);
    break;
  case 30:
    return constant_30 (aTHX_ name, iv_return);
    break;
  case 31:
    return constant_31 (aTHX_ name, iv_return);
    break;
  case 32:
    if (memEQ(name, "EVAS_CALLBACK_CHANGED_SIZE_HINTS", 32)) {
      *iv_return = EVAS_CALLBACK_CHANGED_SIZE_HINTS;
      return PERL_constant_ISIV;
    }
    break;
  case 33:
    /* Names all of length 33.  */
    /* EVAS_LOAD_ERROR_PERMISSION_DENIED EVAS_OBJECT_POINTER_MODE_AUTOGRAB */
    /* Offset 8 gives the best switch position.  */
    switch (name[8]) {
    case 'D':
      if (memEQ(name, "EVAS_LOAD_ERROR_PERMISSION_DENIED", 33)) {
      /*                       ^                               */
#ifdef EVAS_LOAD_ERROR_PERMISSION_DENIED
        *iv_return = EVAS_LOAD_ERROR_PERMISSION_DENIED;
        return PERL_constant_ISIV;
#else
        return PERL_constant_NOTDEF;
#endif
      }
      break;
    case 'E':
      if (memEQ(name, "EVAS_OBJECT_POINTER_MODE_AUTOGRAB", 33)) {
      /*                       ^                               */
        *iv_return = EVAS_OBJECT_POINTER_MODE_AUTOGRAB;
        return PERL_constant_ISIV;
      }
      break;
    }
    break;
  case 34:
    /* Names all of length 34.  */
    /* EVAS_OBJECT_TABLE_HOMOGENEOUS_ITEM EVAS_OBJECT_TABLE_HOMOGENEOUS_NONE */
    /* Offset 30 gives the best switch position.  */
    switch (name[30]) {
    case 'I':
      if (memEQ(name, "EVAS_OBJECT_TABLE_HOMOGENEOUS_ITEM", 34)) {
      /*                                             ^          */
        *iv_return = EVAS_OBJECT_TABLE_HOMOGENEOUS_ITEM;
        return PERL_constant_ISIV;
      }
      break;
    case 'N':
      if (memEQ(name, "EVAS_OBJECT_TABLE_HOMOGENEOUS_NONE", 34)) {
      /*                                             ^          */
        *iv_return = EVAS_OBJECT_TABLE_HOMOGENEOUS_NONE;
        return PERL_constant_ISIV;
      }
      break;
    }
    break;
  case 35:
    /* Names all of length 35.  */
    /* EVAS_OBJECT_TABLE_HOMOGENEOUS_TABLE EVAS_TEXT_STYLE_OUTLINE_SOFT_SHADOW
       */
    /* Offset 10 gives the best switch position.  */
    switch (name[10]) {
    case 'S':
      if (memEQ(name, "EVAS_TEXT_STYLE_OUTLINE_SOFT_SHADOW", 35)) {
      /*                         ^                               */
        *iv_return = EVAS_TEXT_STYLE_OUTLINE_SOFT_SHADOW;
        return PERL_constant_ISIV;
      }
      break;
    case 'T':
      if (memEQ(name, "EVAS_OBJECT_TABLE_HOMOGENEOUS_TABLE", 35)) {
      /*                         ^                               */
        *iv_return = EVAS_OBJECT_TABLE_HOMOGENEOUS_TABLE;
        return PERL_constant_ISIV;
      }
      break;
    }
    break;
  case 42:
    if (memEQ(name, "EVAS_LOAD_ERROR_RESOURCE_ALLOCATION_FAILED", 42)) {
#ifdef EVAS_LOAD_ERROR_RESOURCE_ALLOCATION_FAILED
      *iv_return = EVAS_LOAD_ERROR_RESOURCE_ALLOCATION_FAILED;
      return PERL_constant_ISIV;
#else
      return PERL_constant_NOTDEF;
#endif
    }
    break;
  }
  return PERL_constant_NOTFOUND;
}

