#define TAB(N, NAME, ICON, W, H) \
  part { \
    name, "tab."NAME; \
    clip_to, "left-clip"; \
    description { \
      state, "default" 0.0; \
      min, 13 14; \
      max, 13 14; \
      rel1 { \
        to, "tab.panel"; \
        relative, 0.0 0.0; \
        offset, 0 N*13; \
      } \
      rel2 { \
        to, "tab.panel"; \
        relative, 0.0 0.0; \
        offset, 13 14+N*13; \
      } \
      image { \
        normal, "tab-unsel.png"; \
        border, 0 2 1 2; \
      } \
    } \
    description { \
      state, "selected" 0.0; \
      min, 13 14; \
      max, 13 14; \
      rel1 { \
        to, "tab.panel"; \
        relative, 0.0 0.0; \
        offset, 0 N*13; \
      } \
      rel2 { \
        to, "tab.panel"; \
        relative, 0.0 0.0; \
        offset, 13 14+N*13; \
      } \
      image { \
        normal, "tab-sel.png"; \
        border, 0 2 1 2; \
      } \
    } \
  } \
  part { \
    name, "tab."NAME".icon"; \
    clip_to, "left-clip"; \
    mouse_events, 0; \
    description { \
      state, "default" 0.0; \
      min, W H; \
      max, W H; \
      color, 255 255 255 128; \
      rel1 { \
        to, "tab."NAME; \
        relative, 0.0 0.0; \
        offset, 0 0; \
      } \
      rel2 { \
        to, "tab."NAME; \
        relative, 1.0 1.0; \
        offset, -1 -1; \
      } \
      image { \
        normal, ICON; \
      } \
    } \
    description { \
      state, "over" 0.0; \
      min, W H; \
      max, W H; \
      color, 255 255 255 255; \
      rel1 { \
        to, "tab."NAME; \
        relative, 0.0 0.0; \
        offset, 0 0; \
      } \
      rel2 { \
        to, "tab."NAME; \
        relative, 1.0 1.0; \
        offset, -1 -1; \
      } \
      image { \
        normal, ICON; \
      } \
    } \
  } 


#define TAB_PROG_ANIM(NAME)	        \
      program {				\
        name, "tab."NAME".in";		\
        signal, "mouse,in";		\
        source, "tab."NAME;			\
        action, STATE_SET "over" 0.0;	\
        transition, DECELERATE 0.5;	\
        target, "tab."NAME".icon";			\
      }					\
      program {				\
        name, "tab."NAME".out";		\
        signal, "mouse,out";		\
        source, "tab."NAME;			\
        action, STATE_SET "default" 0.0;\
        transition, DECELERATE 0.5;	\
        target, "tab."NAME".icon";			\
      }					\


#define TAB_PROG(NAME) \
TAB_PROG_ANIM(NAME) \
  program { \
    name, "tab."NAME".down"; \
    signal, "mouse,down,1"; \
    source, "tab."NAME; \
    after, "tabs.hideall"; \
    after, "tab."NAME".show"; \
  } \
  program { \
    name, "tab."NAME".show"; \
    action, STATE_SET "selected" 0.0; \
    transition, LINEAR 0.0; \
    target, "tab."NAME; \
    target, NAME"-clip"; \
  } 

