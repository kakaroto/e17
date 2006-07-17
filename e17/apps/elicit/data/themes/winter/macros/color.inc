#define COLOR(NAME, LABEL, OFF1, OFF2) \
    part { \
      name, NAME"-box"; \
      type, RECT; \
      mouse_events, 0; \
      clip_to, "color-clip"; \
      description { \
        state, "default" 0.0; \
        color, 0 0 0 0; \
        rel1 { \
          to, "left-panel"; \
          relative, 0.0 0.0; \
          offset, OFF1;  \
        } \
        rel2 { \
          to, "left-panel"; \
          relative, 1.0 0.0; \
          offset, OFF2;  \
        } \
      } \
    } \
    part { \
      name, NAME"-label"; \
      clip_to, "color-clip"; \
      type, TEXT; \
      description { \
        state, "default" 0.0; \
        align, 0.0 0.5; \
        rel1 { \
          to, NAME"-box"; \
          relative, 0.0 0.0; \
          offset, 2 0; \
        } \
        rel2 { \
          to, NAME"-box"; \
          relative, 0.0 1.0; \
          offset, 2 -1; \
        } \
        color, 140 140 140 255; \
        text { \
          text, LABEL; \
          font, "Vera"; \
          size, TEXT_SIZE; \
          fit, 0 0; \
          min, 1 0; \
          align, 0.5 0.5; \
        } \
      } \
    } \
 \
    part { \
      name, NAME"-val"; \
      clip_to, "color-clip"; \
      type, TEXT; \
      description { \
        state, "default" 0.0; \
        align, 0.0 0.5; \
        min, 0 0; \
        max, 9999 9999; \
        rel1 { \
          to, NAME"-trough"; \
          relative, 1.0 0.0; \
          offset, 1 0; \
        } \
        rel2 { \
          to, NAME"-box"; \
          relative, 1.0 1.0; \
          offset, -1 -1; \
        } \
        color, 140 140 140 255; \
        text { \
          text, "255"; \
          font, "Vera"; \
          size, 8; \
          fit, 0 0; \
          min, 1 0; \
          align, 0.0 0.5; \
        } \
      } \
    } \
BUTTON_CLIPPED(NAME"-up", "arrow-up.png", "arrow-up.png", 5, 3, \
               NAME"-val", 1 0, 4 1, \
               NAME"-val", 1 0, 9 4, "color-clip") \
BUTTON_CLIPPED(NAME"-dn", "arrow-dn.png", "arrow-dn.png", 5, 3, \
               NAME"-val", 1 0, 4 5, \
               NAME"-val", 1 0, 9 8, "color-clip") \
part { \
  name: NAME"-trough"; \
  clip_to, "color-clip"; \
  description { \
    state: "default" 0.0; \
    min: 6 6; \
    max: 9999 6; \
    fixed: 0 1; \
    image.normal: "trough.png"; \
    image.border: 3 4 2 3; \
    rel1 { \
      to: NAME"-label"; \
      relative: 1 0; \
      offset: -3 0; \
    } \
    rel2 { \
      to: NAME"-box"; \
      relative: 1 1; \
      offset: -23 -1; \
    } \
  } \
} \
part { \
  name: NAME"-spectrum"; \
  clip_to, "color-clip"; \
  type: SWALLOW; \
  description { \
    state: "default" 0.0; \
    rel1 { \
      to: NAME"-trough"; \
      offset: 2 2; \
    } \
    rel2 { \
      to: NAME"-trough"; \
      offset: -3 -3; \
    } \
 \
  } \
} \
part { \
  name: NAME"-confine"; \
  clip_to, "color-clip"; \
  type: RECT; \
  description { \
    state: "default" 0.0; \
    color: 0 0 0 0; \
    rel1.to: NAME"-trough"; \
    rel2.to: NAME"-trough"; \
    rel2.offset: 1 1; \
  } \
} \
part { \
  name: NAME"-slider"; \
  clip_to, "color-clip"; \
  dragable { \
    x: 1 1 1; \
    y: 0 0 0; \
    confine: NAME"-confine"; \
  } \
  description { \
    state: "default" 0.0; \
    min: 15 8; \
    max: 15 8; \
    fixed: 1 1; \
    image.normal: "slider.png"; \
  } \
} \
    part { \
      name: NAME"-event"; \
      type: RECT; \
      repeat_events: 1; \
      description { \
        state: "default" 0.0; \
        rel1.to: NAME"-box"; \
        rel2.to: NAME"-box"; \
        color: 0 0 0 0; \
      } \
    } \



#define COLOR_PROG(NAME, SIG_BIT) \
  BUTTON_PROG(NAME"-up", "elicit,"SIG_BIT",up,start", \
                         "elicit,"SIG_BIT",up,stop") \
  BUTTON_PROG(NAME"-dn", "elicit,"SIG_BIT",down,start", \
                         "elicit,"SIG_BIT",down,stop") \
  program { \
    name: NAME"-scroll-up"; \
    signal: "mouse,wheel,0,-1"; \
    source: NAME"-event"; \
    action: SIGNAL_EMIT "elicit,"SIG_BIT",up" ""; \
  } \
  program { \
    name: NAME"-scroll-down"; \
    signal: "mouse,wheel,0,1"; \
    source: NAME"-event"; \
    action: SIGNAL_EMIT "elicit,"SIG_BIT",down" ""; \
  }


