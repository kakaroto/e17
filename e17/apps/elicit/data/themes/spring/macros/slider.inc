#define SLIDER(NAME, TO1, REL1, OFF1, TO2, REL2, OFF2, CLIP) \
BUTTON_CLIPPED(NAME"-left", "slider-left1.png", "slider-left2.png", 5, 8, \
               TO1, REL1, OFF1, TO2, REL2, OFF2, 0 0, CLIP) \
BUTTON_CLIPPED(NAME"-right", "slider-right1.png", "slider-right2.png", 5, 8, \
               TO1, REL1, OFF1, TO2, REL2, OFF2, 1 0, CLIP) \
part { \
  name, NAME"-bar"; \
  clip_to, CLIP; \
  description { \
    state, "default" 0.0; \
    min, 0 6; \
    max, 9999 6; \
    align, 0.5 0.5; \
    color, 255 255 255 128; \
    rel1 { \
      to, NAME"-left"; \
      relative, 1 0, \
      offset, 0 1; \
    } \
    rel2 { \
      to, NAME"-right"; \
      relative, 0 1, \
      offset, -1 -2; \
    } \
    image { \
      normal, "slider-bar.png"; \
    } \
    border, 5 6 0 0; \
  } \
} \
part { \
  name, NAME"-confine"; \
  type, RECT; \
  description { \
    state, "default" 0.0; \
    color, 0 0 0 0; \
    rel1 { \
      to, NAME"-bar"; \
      relative, 0 0; \
      offset, 0 2; \
    } \
    rel2 { \
      to, NAME"-bar"; \
      relative, 1 1; \
      offset, 0 1; \
    } \
  } \
} \
      part {				\
        name, NAME"-slider";		\
        clip_to, CLIP;                  \
        dragable { \
          x, 1 1 1; \
          y, 0 0 0; \
          confine, NAME-"confine"; \
        } \
	description {			\
	  state, "default" 0.0;		\
	  min, 9 9;			\
	  max, 9 9;			\
	  color, 255 255 255 127;	\
	  rel1 {			\
	    to, NAME"-confine";         \
	    relative, 0 0;      	\
	    offset, 0 1;		\
	  }				\
	  rel2 {			\
	    to, NAME"-confine";         \
	    relative, 1 1;      	\
	    offset, -1 -1;              \
	  }				\
	  image {			\
	    normal, "slider-handle1.png";	\
	  }				\
	}				\
	description {			\
	  state, "over" 0.0;		\
	  min, 9 9;			\
	  max, 9 9; 			\
	  color, 255 255 255 255;	\
	  rel1 {			\
	    to, NAME"-confine";         \
	    relative, 0 0;      	\
	    offset, 0 1;		\
	  }				\
	  rel2 {			\
	    to, NAME"-confine";         \
	    relative, 1 1;      	\
	    offset, -1 -1;              \
	  }				\
	  image {			\
	    normal, "slider-handle1.png";		\
	  }				\
	}				\
	description {			\
	  state, "down" 0.0;		\
	  min, 9 9;			\
	  max, 9 9; 			\
	  color, 255 255 255 255;	\
	  rel1 {			\
	    to, NAME"-confine";         \
	    relative, 0 0;      	\
	    offset, 0 0;		\
	  }				\
	  rel2 {			\
	    to, NAME"-confine";         \
	    relative, 1 1;      	\
	    offset, -1 -1;              \
	  }				\
	  image {			\
	    normal, "slider-handle2.png";\
	  }				\
	}				\
      }					


#define SLIDER_PROG(NAME, SIG_BIT) \
BUTTON_PROG(NAME"-left", "elicit,"SIG_BIT",down,start", "elicit,"SIG_BIT",down,stop") \
BUTTON_PROG(NAME"-right", "elicit,"SIG_BIT",up,start", "elicit,"SIG_BIT",up,stop") \
BUTTON_PROG_ANIM(NAME"-slider");


