#define BUTTON(NAME, IMG, IMG2, W, H, REL1_TO, REL1_REL, REL1_OFF, REL2_TO, REL2_REL, REL2_OFF) \
      part {				\
        name, NAME;			\
	description {			\
	  state, "default" 0.0;		\
	  min, W H;			\
	  max, W H;			\
	  color, 255 255 255 127;	\
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
	  state, "over" 0.0;		\
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
      }					



#define BUTTON_CLIPPED(NAME, IMG, IMG2, W, H, REL1_TO, REL1_REL, REL1_OFF, REL2_TO, REL2_REL, REL2_OFF, CLIP) \
      part {				\
        name, NAME;			\
        clip_to, CLIP;                  \
	description {			\
	  state, "default" 0.0;		\
	  min, W H;			\
	  max, W H;			\
	  color, 255 255 255 127;	\
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
	  state, "over" 0.0;		\
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
      }					






#define BUTTON_PROG_ANIM(NAME)	        \
      program {				\
        name, NAME"-in";		\
        signal, "mouse,in";		\
        source, NAME;			\
        action, STATE_SET "over" 0.0;	\
        transition, DECELERATE 0.5;	\
        target, NAME;			\
      }					\
      program {				\
        name, NAME"-out";		\
        signal, "mouse,out";		\
        source, NAME;			\
        action, STATE_SET "default" 0.0;\
        transition, DECELERATE 0.5;	\
        target, NAME;			\
      }					\
      program {				\
        name, NAME"-down2";		\
	signal, "mouse,down,1";		\
	source, NAME;			\
	action, STATE_SET "down" 0.0;	\
        transition, LINEAR 0.0;	\
	target, NAME;			\
      }					\
      program {				\
        name, NAME"-up";		\
	signal, "mouse,up,1";		\
	source, NAME;			\
	action, STATE_SET "over" 0.0;	\
        transition, LINEAR 0.0;	\
	target, NAME;			\
      }					

#define BUTTON_PROG_SIG(NAME, SIG_DOWN, SIG_UP)	        \
      program {				\
        name, NAME"-sig-dn";		\
	signal, "mouse,down,1";		\
	source, NAME;			\
	action, SIGNAL_EMIT SIG_DOWN NAME;	\
      }					\
      program {				\
        name, NAME"-sig-up";		\
	signal, "mouse,up,1";		\
	source, NAME;			\
	action, SIGNAL_EMIT SIG_UP NAME;	\
      }					

#define BUTTON_PROG(NAME, SIG_DOWN, SIG_UP) \
  BUTTON_PROG_ANIM(NAME) \
  BUTTON_PROG_SIG(NAME, SIG_DOWN, SIG_UP)

