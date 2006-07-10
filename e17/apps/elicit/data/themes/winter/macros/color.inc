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
          offset, 8 0; \
        } \
        rel2 { \
          to, NAME"-box"; \
          relative, 0.0 0.0; \
          offset, 17 10; \
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
          to, NAME"-label"; \
          relative, 1.0 0.0; \
          offset, 5 0; \
        } \
        rel2 { \
          to, NAME"-label"; \
          relative, 1.0 1.0; \
          offset, 30 -1; \
        } \
        color, 140 140 140 255; \
        text { \
          text, "255"; \
          font, "Vera"; \
          size, TEXT_SIZE; \
          fit, 0 0; \
          min, 0 0; \
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
      name: NAME"-event"; \
      type: RECT; \
      repeat_events: 1; \
      description { \
        state: "default" 0.0; \
        rel1.to: NAME"-box"; \
        rel2.to: NAME"-box"; \
        color: 0 0 0 0; \
      } \
    }


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


