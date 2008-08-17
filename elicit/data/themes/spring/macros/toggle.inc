/* this makes life MUCH easier */

#define TOGGLE_BUTTON(NAME, IMG, IMG2, IMG3, IMG4, W, H, REL1_TO, REL1_REL, REL1_OFF, REL2_TO, REL2_REL, REL2_OFF) \
      part {                                    \
        name, NAME"-clip";		        \
	type, RECT;                             \
	mouse_events, 0;                        \
	description {                           \
	  state, "default" 0.0;                 \
	  visible, 1;                           \
	  rel1 {                                \
	    relative, REL1_REL;                 \
	    offset, REL1_OFF;                   \
	    to, REL1_TO;                        \
	  }                                     \
	  rel2 {                                \
	    relative, REL2_REL;                 \
	    offset, REL2_OFF;                   \
	    to, REL2_TO;                        \
	  }                                     \
	  color, 255 255 255 128;               \
	}                                       \
	description {                           \
	  state, "over" 0.0;                    \
	  visible, 1;                           \
	  rel1 {                                \
	    relative, REL1_REL;                 \
	    offset, REL1_OFF;                   \
	    to, REL1_TO;                        \
	  }                                     \
	  rel2 {                                \
	    relative, REL2_REL;                 \
	    offset, REL2_OFF;                   \
	    to, REL2_TO;                        \
	  }                                     \
	  color, 255 255 255 255;               \
	}                                       \
      }                                         \
      part {				\
        name, NAME;			\
        clip_to, NAME"-clip";           \
	description {			\
	  state, "default" 0.0;		\
	  min, W H;			\
	  max, W H;			\
	  visible, 1;			\
          color, 255 255 255 255;       \
	  rel1 {			\
	    to, REL1_TO;		\
	    relative, REL1_REL;		\
	    offset, REL1_OFF;		\
	  }				\
	  rel2 {			\
	    to, REL2_TO;		\
	    relative, REL2_REL;		\
	    offset, REL2_OFF;		\
	  }				\
	  image {			\
	    normal, IMG;		\
	  }				\
	}				\
	description {			\
	  state, "toggled" 0.0;		\
	  min, W H;			\
	  max, W H;			\
	  visible, 0;			\
          color, 255 255 255 0;         \
	  rel1 {			\
	    to, REL1_TO;		\
	    relative, REL1_REL;		\
	    offset, REL1_OFF;		\
	  }				\
	  rel2 {			\
	    to, REL2_TO;		\
	    relative, REL2_REL;		\
	    offset, REL2_OFF;		\
	  }				\
	  image {			\
	    normal, IMG;		\
	  }				\
	}				\
	description {			\
	  state, "down" 0.0;		\
	  min, W H;			\
	  max, W H; 			\
	  color, 255 255 255 255;	\
	  rel1 {			\
	    to, REL1_TO;		\
	    relative, REL1_REL;		\
	    offset, REL1_OFF;		\
	  }				\
	  rel2 {			\
	    to, REL2_TO;		\
	    relative, REL2_REL;		\
	    offset, REL2_OFF;		\
	  }				\
	  image {			\
	    normal, IMG2;		\
	  }				\
	}				\
      }					\
      part {				\
        name, NAME"-2";			\
        clip_to, NAME"-clip";           \
	description {			\
	  state, "default" 0.0;		\
	  min, W H;			\
	  max, W H;			\
	  visible, 0;			\
          color, 255 255 255 0;         \
	  rel1 {			\
	    to, REL1_TO;		\
	    relative, REL1_REL;		\
	    offset, REL1_OFF;		\
	  }				\
	  rel2 {			\
	    to, REL2_TO;		\
	    relative, REL2_REL;		\
	    offset, REL2_OFF;		\
	  }				\
	  image {			\
	    normal, IMG3;		\
	  }				\
	}				\
	description {			\
	  state, "toggled" 0.0;		\
	  min, W H;			\
	  max, W H;			\
	  visible, 1;                   \
          color, 255 255 255 255;       \
	  rel1 {			\
	    to, REL1_TO;		\
	    relative, REL1_REL;		\
	    offset, REL1_OFF;		\
	  }				\
	  rel2 {			\
	    to, REL2_TO;		\
	    relative, REL2_REL;		\
	    offset, REL2_OFF;		\
	  }				\
	  image {			\
	    normal, IMG3;		\
	  }				\
	}				\
	description {			\
	  state, "down" 0.0;		\
	  min, W H;			\
	  max, W H; 			\
	  color, 255 255 255 255;	\
	  rel1 {			\
	    to, REL1_TO;		\
	    relative, REL1_REL;		\
	    offset, REL1_OFF;		\
	  }				\
	  rel2 {			\
	    to, REL2_TO;		\
	    relative, REL2_REL;		\
	    offset, REL2_OFF;		\
	  }				\
	  image {			\
	    normal, IMG4;		\
	  }				\
	}				\
      }                                 \
      part {                                    \
        name, NAME"-grabber";		        \
	type, RECT;                             \
	mouse_events, 1;                        \
        repeat_events, 1;                       \
	description {                           \
	  state, "default" 0.0;                 \
	  visible, 1;                           \
	  color, 0 0 0 0;                       \
	  rel1 {                                \
	    relative, REL1_REL;                 \
	    offset, REL1_OFF;                   \
	    to, REL1_TO;                        \
	  }                                     \
	  rel2 {                                \
	    relative, REL2_REL;                 \
	    offset, REL2_OFF;                   \
	    to, REL2_TO;                        \
	  }                                     \
	}                                       \
      }                                         \



#define TOGGLE_BUTTON_PROG(NAME, SIG, SIG2) \
      program {                 \
        name, NAME"-toggle-on"; \
        signal, "mouse,clicked,1"; \
        source, NAME; \
        action, STATE_SET "toggled" 0.0; \
        transition, LINEAR 0.0; \
        target, NAME; \
        target, NAME"-2"; \
        after, NAME"-sig"; \
      } \
      program { \
        name, NAME"-sig"; \
        action, SIGNAL_EMIT SIG NAME; \
      } \
      program { \
        name, NAME"-toggle-off"; \
        signal, "mouse,clicked,1"; \
        source, NAME"-2"; \
        action, STATE_SET "default" 0.0; \
        transition, LINEAR 0.0; \
        target, NAME; \
        target, NAME"-2"; \
        after, NAME"-sig2"; \
      } \
      program { \
        name, NAME"-sig2"; \
        action, SIGNAL_EMIT SIG2 NAME"-2"; \
      } \
      program { \
        name, NAME"-grabber-in"; \
        signal, "mouse,in"; \
        source, NAME"-grabber"; \
        action, STATE_SET "over" 0.0; \
        transition, DECELERATE 0.5; \
        target, NAME"-clip"; \
      } \
      program { \
        name, NAME"-grabber-out"; \
        signal, "mouse,out"; \
        source, NAME"-grabber"; \
        action, STATE_SET "default" 0.0; \
        transition, DECELERATE 0.5; \
        target, NAME"-clip"; \
      } \
      program { \
        name, NAME"-down"; \
        signal, "mouse,down,1"; \
        source, NAME; \
        action, STATE_SET "down" 0.0; \
        transition, LINEAR 0.0; \
        target, NAME; \
      } \
      program { \
        name, NAME"-2-down"; \
        signal, "mouse,down,1"; \
        source, NAME"-2"; \
        action, STATE_SET "down" 0.0; \
        transition, LINEAR 0.0; \
        target, NAME"-2"; \
      } \


