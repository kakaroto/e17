#define SCROLLBAR(NAME, TO1_X, TO1_Y, REL1, OFF1, TO2_X, TO2_Y, REL2, OFF2) \
  part { \
    name, NAME".scroll.trough"; \
    clip_to, NAME"-clip"; \
    description { \
      state, "default" 0.0; \
      rel1 { \
        to_x, TO1_X; \
        to_y, TO1_Y; \
        relative, REL1; \
        offset, OFF1; \
      } \
      rel2 { \
        to_x, TO2_X; \
        to_y, TO2_Y; \
        relative, REL2; \
        offset, OFF2; \
      } \
      image { \
        normal, "scroll-trough.png"; \
        border, 1 2 0 0; \
      } \
    } \
  } \
 \
  part { \
    name, NAME".scroll.up"; \
    clip_to, NAME"-clip"; \
    description { \
      state, "default" 0.0; \
      min, 9 7; \
      max, 9 7; \
      rel1 { \
        to, NAME".scroll.trough"; \
        relative, 0 0; \
        offset, -1 -1; \
      } \
      rel2 { \
        to, NAME".scroll.trough"; \
        relative, 1 0; \
        offset, 0 6; \
      } \
      image { \
        normal, "scroll-box.png"; \
        border, 1 2 1 2; \
      } \
    } \
  } \
  part { \
    name, NAME".scroll.dn"; \
    clip_to, NAME"-clip"; \
    description { \
      state, "default" 0.0; \
      min, 9 7; \
      max, 9 7; \
      rel1 { \
        to, NAME".scroll.trough"; \
        relative, 0 1; \
        offset, -1 -6; \
      } \
      rel2 { \
        to, NAME".scroll.trough"; \
        relative, 1 1; \
        offset, 0 1; \
      } \
      image { \
        normal, "scroll-box.png"; \
        border, 1 2 1 2; \
      } \
    } \
  } \
 \
 \
  part { \
    name, NAME".scroll.up.deco"; \
    clip_to, NAME"-clip"; \
    mouse_events, 0; \
    description { \
      state, "default" 0.0; \
      min, 5 3; \
      max, 5 3; \
      color, 255 255 255 128; \
      rel1 { \
        to, NAME".scroll.up"; \
        relative, 0 0; \
        offset, 0 0; \
      } \
      rel2 { \
        to, NAME".scroll.up"; \
        relative, 1 1; \
        offset, -1 -1; \
      } \
      image { \
        normal, "arrow-up.png"; \
      } \
    } \
    description { \
      state, "over" 0.0; \
      min, 5 3; \
      max, 5 3; \
      color, 255 255 255 255; \
      rel1 { \
        to, NAME".scroll.up"; \
        relative, 0 0; \
        offset, 0 0; \
      } \
      rel2 { \
        to, NAME".scroll.up"; \
        relative, 1 1; \
        offset, -1 -1; \
      } \
      image { \
        normal, "arrow-up.png"; \
      } \
    } \
  } \
 \
 \
  part { \
    name, NAME".scroll.dn.deco"; \
    clip_to, NAME"-clip"; \
    mouse_events, 0; \
    description { \
      state, "default" 0.0; \
      min, 5 3; \
      max, 5 3; \
      color, 255 255 255 128; \
      rel1 { \
        to, NAME".scroll.dn"; \
        relative, 0 0; \
        offset, 0 0; \
      } \
      rel2 { \
        to, NAME".scroll.dn"; \
        relative, 1 1; \
        offset, -1 -1; \
      } \
      image { \
        normal, "arrow-dn.png"; \
      } \
    } \
    description { \
      state, "over" 0.0; \
      min, 5 3; \
      max, 5 3; \
      color, 255 255 255 255; \
      rel1 { \
        to, NAME".scroll.dn"; \
        relative, 0 0; \
        offset, 0 0; \
      } \
      rel2 { \
        to, NAME".scroll.dn"; \
        relative, 1 1; \
        offset, -1 -1; \
      } \
      image { \
        normal, "arrow-dn.png"; \
      } \
    } \
  } \
 \
 \
  part { \
    name, NAME".scroll.confine"; \
    clip_to, NAME"-clip"; \
    type, RECT; \
    description { \
      state, "default" 0.0; \
      visible, 0; \
      rel1 { \
        to, NAME".scroll.up"; \
        relative, 0 1; \
        offset, 0 -1; \
      } \
      rel2 { \
        to, NAME".scroll.dn"; \
        relative, 1 0; \
        offset, 0 0; \
      } \
    } \
  } \
 \
  part { \
    name, NAME".scroll.bar"; \
    clip_to, NAME"-clip"; \
    dragable { \
      confine, NAME".scroll.confine"; \
      x, 0 0 0; \
      y, 1 1 1; \
    } \
    description { \
      state, "default" 0.0; \
      min, 9 15; \
      max, 9 15; \
      rel1 { \
        to, NAME".scroll.up"; \
        relative, 0 1; \
        offset, -1 0; \
      } \
      rel2 { \
        to, NAME".scroll.dn"; \
        relative, 1 0; \
        offset, 0 0; \
      } \
      image { \
        normal, "scroll-box.png"; \
        border, 0 0 1 2; \
      } \
    } \
  } 

