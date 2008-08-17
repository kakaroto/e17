#define SLIDER(NAME, TO1, REL1, OFF1, TO2, REL2, OFF2, CLIP, LABEL) \
BUTTON_CLIPPED(NAME"-left", "slider-left1.png", "slider-left2.png", 5, 8, \
               NAME"-label", 1 0, 4 0, TO2, REL2, OFF2, 0 0, CLIP) \
BUTTON_CLIPPED(NAME"-right", "slider-right1.png", "slider-right2.png", 5, 8, \
               TO1, REL1, OFF1, NAME"-val2", 0 1, -5 -1, 1 0, CLIP) \
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
      relative, 1 0; \
      offset, 0 1; \
    } \
    rel2 { \
      to, NAME"-right"; \
      relative, 0 1; \
      offset, -1 -2; \
    } \
    image { \
    border, 5 6 0 0; \
      normal, "slider-bar.png"; \
    } \
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
      }					\
    part {  \
      name, NAME"-val2";  \
      clip_to, "edit-clip";  \
      type, TEXT;  \
      description {  \
        state, "default" 0.0;  \
        align, 1.0 0.5;  \
        max, 25 10; \
        rel1 {  \
          to, TO1; \
          relative, REL1;  \
          offset, OFF1;  \
        }  \
        rel2 {  \
          to, TO2; \
          relative, REL2;  \
          offset, OFF2;  \
        }  \
        color, 0 0 0 200;  \
        text {  \
          text, "255";  \
          font, "spring-italic";  \
          size, 7;  \
          fit, 0 0;  \
          min, 1 1;  \
          align, 1.0 0.5;  \
        }  \
      }  \
    }  \
    part {  \
      name, NAME"-label";  \
      clip_to, "edit-clip";  \
      type, TEXT;  \
      description {  \
        state, "default" 0.0;  \
        align, 0.0 0.5;  \
        max, 10 10; \
        rel1 {  \
          to, TO1; \
          relative, REL1;  \
          offset, OFF1;  \
        }  \
        rel2 {  \
          to, TO2; \
          relative, REL2;  \
          offset, OFF2;  \
        }  \
        color, 0 0 0 200;  \
        text {  \
          text, LABEL;  \
          font, "spring-italic";  \
          size, 7;  \
          fit, 0 0;  \
          min, 1 1;  \
          align, 0.5 0.5;  \
        }  \
      }  \
    }  \


#define SLIDER_IMAGE(NAME, TO1, REL1, OFF1, TO2, REL2, OFF2, CLIP, LABEL_IMAGE, W, H) \
BUTTON_CLIPPED(NAME"-left", "slider-left1.png", "slider-left2.png", 5, 8, \
               NAME"-label", 1 0, 4 0, TO2, REL2, OFF2, 0 0, CLIP) \
BUTTON_CLIPPED(NAME"-right", "slider-right1.png", "slider-right2.png", 5, 8, \
               TO1, REL1, OFF1, NAME"-val2", 0 1, -5 -3, 1 0, CLIP) \
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
      relative, 1 0; \
      offset, 0 1; \
    } \
    rel2 { \
      to, NAME"-right"; \
      relative, 0 1; \
      offset, -1 -2; \
    } \
    image { \
      normal, "slider-bar.png"; \
      border, 5 6 0 0; \
    } \
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
      }					\
    part {  \
      name, NAME"-val2";  \
      clip_to, "edit-clip";  \
      type, TEXT;  \
      description {  \
        state, "default" 0.0;  \
        align, 1.0 0.5;  \
        max, 25 10; \
        rel1 {  \
          to, TO1; \
          relative, REL1;  \
          offset, OFF1;  \
        }  \
        rel2 {  \
          to, TO2; \
          relative, REL2;  \
          offset, OFF2;  \
        }  \
        color, 0 0 0 200;  \
        text {  \
          text, "255";  \
          font, "spring-italic";  \
          size, 7;  \
          fit, 0 0;  \
          min, 1 1;  \
          align, 1.0 0.5;  \
        }  \
      }  \
    }  \
    part {  \
      name, NAME"-label";  \
      clip_to, "edit-clip";  \
      description {  \
        state, "default" 0.0;  \
        align, 0.0 0.5;  \
        min, W H; \
        max, W H; \
        rel1 {  \
          to, TO1; \
          relative, REL1;  \
          offset, OFF1;  \
        }  \
        rel2 {  \
          to, TO2; \
          relative, REL2;  \
          offset, OFF2;  \
        }  \
        image { \
          normal, LABEL_IMAGE; \
        } \
      }  \
    }  \


#define SLIDER_PROG(NAME, SIG_BIT) \
BUTTON_PROG(NAME"-left", "elicit,"SIG_BIT",down,start", "elicit,"SIG_BIT",down,stop") \
BUTTON_PROG(NAME"-right", "elicit,"SIG_BIT",up,start", "elicit,"SIG_BIT",up,stop") \
BUTTON_PROG_ANIM(NAME"-slider");


