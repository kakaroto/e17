#define TAB(N, NAME, ICON, W, H) \
  part { \
    name, "tab."NAME; \
    clip_to, "panel.clip"; \
    description { \
      state, "default" 0.0; \
      min, 15 13; \
      max, 15 13; \
      rel1 { \
        to, "tabs.box"; \
        relative, 0.0 0.0; \
        offset, N*15 0; \
      } \
      rel2 { \
        to, "tabs.box"; \
        relative, 0.0 1.0; \
        offset, 15+N*15 -1; \
      } \
      image { \
        normal, "tab-unsel.png"; \
        border, 2 2 2 2; \
      } \
    } \
    description { \
      state, "selected" 0.0; \
      min, 15 13; \
      max, 15 13; \
      rel1 { \
        to, "tabs.box"; \
        relative, 0.0 0.0; \
        offset, N*15 0; \
      } \
      rel2 { \
        to, "tabs.box"; \
        relative, 0.0 1.0; \
        offset, 15+N*15 -1; \
      } \
      image { \
        normal, "tab-sel.png"; \
        border, 2 2 2 2; \
      } \
    } \
  } \
  part { \
    name, "tab."NAME".icon"; \
    clip_to, "panel.clip"; \
    mouse_events, 0; \
    description { \
      state, "default" 0.0; \
      min, W H; \
      max, W H; \
      color, 255 255 255 200; \
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
    transition, DECELERATE 0.25; \
    target, "tab."NAME; \
    target, NAME".clip"; \
  } 

