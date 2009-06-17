/* yeah. i know. this file is messy right now. still playing with things... */

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


#define BUTTON_PROG(NAME, SIG)	        \
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
	name, NAME"-clicked";		\
	signal, "mouse,clicked,1";		\
	source, NAME;			\
	action, SIGNAL_EMIT SIG NAME;	\
      }					\
      program {				\
        name, NAME"-down2";		\
	signal, "mouse,down,1";		\
	source, NAME;			\
	action, STATE_SET "down" 0.0;	\
	target, NAME;			\
      }					\
      program {				\
        name, NAME"-up";		\
	signal, "mouse,up,1";		\
	source, NAME;			\
	action, STATE_SET "over" 0.0;	\
	target, NAME;			\
      }					\


#define BUTTON2(NAME, IMG, IMG2, IMG3, W, H, REL1_TO, REL1_REL, REL1_OFF, REL2_TO, REL2_REL, REL2_OFF) \
      part {				\
        name, NAME;			\
	description {			\
	  state, "default" 0.0;		\
	  min, W H;			\
	  max, W H;			\
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
	    normal, IMG2;		\
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
	    normal, IMG3;		\
	  }				\
	}				\
      }					








#define BUTTON_PROG2(NAME, P1, P2, P3, P4, P5, SIG) \
      program {				\
        name, P1;			\
        signal, "mouse,in";		\
        source, NAME;			\
        action, STATE_SET "over" 0.0;	\
        target, NAME;			\
      }					\
      program {				\
        name, P2;			\
        signal, "mouse,out";		\
        source, NAME;			\
        action, STATE_SET "default" 0.0;\
        target, NAME;			\
      }					\
      program {				\
        name, P3;			\
	signal, "mouse,up,1";		\
	source, NAME;			\
	action, SIGNAL_EMIT SIG NAME;	\
      }					\
      program {				\
        name, P4;			\
	signal, "mouse,down,1";		\
	source, NAME;			\
	action, STATE_SET "down" 0.0;	\
	target, NAME;			\
      }					\
      program {				\
        name, P5;			\
	signal, "mouse,up,1";		\
	source, NAME;			\
	action, STATE_SET "over" 0.0;	\
	target, NAME;			\
      }					\



#define BUTTON_CLIPPED(NAME, IMG, IMG2, W, H, REL1_TO, REL1_REL, REL1_OFF, REL2_TO, REL2_REL, REL2_OFF, CLIP)\
      part {				\
        name, NAME;			\
	clip_to, CLIP;			\
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



#define BUTTON_PROG3(NAME, SIG_DN, SIG_UP)\
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
        name, NAME"-down1";		\
	signal, "mouse,down,1";		\
	source, NAME;			\
	action, SIGNAL_EMIT SIG_DN NAME;	\
      }					\
      program {				\
        name, NAME"-down2";		\
	signal, "mouse,down,1";		\
	source, NAME;			\
	action, STATE_SET "down" 0.0;	\
	target, NAME;			\
      }					\
      program {				\
        name, NAME"-up1";		\
	signal, "mouse,up,1";		\
	source, NAME;			\
	action, SIGNAL_EMIT SIG_UP NAME;	\
      }					\
      program {				\
        name, NAME"-up2";		\
	signal, "mouse,up,1";		\
	source, NAME;			\
	action, STATE_SET "over" 0.0;	\
	target, NAME;			\
      }					\




#define TOG_BUTTON(NAME, NAME2, IMG, IMG2, IMG3, IMG4, W, H, REL1_TO, REL1_REL, REL1_OFF, REL2_TO, REL2_REL, REL2_OFF) \
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
	  state, "toggled" 0.0;		\
	  min, W H;			\
	  max, W H;			\
	  visible, 0;			\
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
      }					\
      part {				\
        name, NAME2;			\
	description {			\
	  state, "default" 0.0;		\
	  min, W H;			\
	  max, W H;			\
	  visible, 0;			\
	  color, 255 255 255 0;		\
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
	    normal, IMG3;		\
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
      }			


#define TOG_BUTTON_PROG(NAME, NAME2, SIG, SIG2) \
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
        name, NAME"-down1";		\
	signal, "mouse,up,1";		\
	source, NAME;			\
	action, SIGNAL_EMIT SIG NAME;	\
      }					\
      program {				\
        name, NAME"-down2";		\
	signal, "mouse,down,1";		\
	source, NAME;			\
	action, STATE_SET "down" 0.0	\
        transition, LINEAR 0.0;	\
	target, NAME;			\
      }					\
      program {				\
        name, NAME"-up";		\
	signal, "mouse,up,1";		\
	source, NAME;			\
	action, STATE_SET "over" 0.0	\
        transition, LINEAR 0.0;	\
	target, NAME;			\
      }					\
      program {				\
        name, NAME2"-in";		\
        signal, "mouse,in";		\
        source, NAME2;			\
        action, STATE_SET "over" 0.0;	\
        transition, DECELERATE 0.5;	\
        target, NAME2;			\
      }					\
      program {				\
        name, NAME2"-out";		\
        signal, "mouse,out";		\
        source, NAME2;			\
        action, STATE_SET "toggled" 0.0;\
        transition, DECELERATE 0.5;	\
        target, NAME2;			\
      }					\
      program {				\
        name, NAME2"-up1";		\
	signal, "mouse,up,1";		\
	source, NAME2;			\
	action, SIGNAL_EMIT SIG NAME2;	\
      }					\
      program {				\
        name, NAME2"-down2";		\
	signal, "mouse,down,1";		\
	source, NAME2;			\
	action, STATE_SET "down" 0.0	\
        transition, LINEAR 0.0;	\
	target, NAME2;			\
      }					\
      program {				\
        name, NAME2"-up";		\
	signal, "mouse,up,1";		\
	source, NAME2;			\
	action, STATE_SET "over" 0.0	\
        transition, LINEAR 0.0;	\
	target, NAME2;			\
      }					\
      program {				\
        name, NAME"-toggle";		\
        signal, "mouse,clicked,1";		\
        source, NAME;			\
        action, STATE_SET "toggled" 0.0;	\
        transition, LINEAR 0.0;	\
        target, NAME;			\
        target, NAME2;			\
      }					\
      program {				\
        name, NAME2"-toggle";		\
        signal, "mouse,clicked,1";		\
        source, NAME2;			\
        action, STATE_SET "default" 0.0;	\
        transition, LINEAR 0.0;	\
        target, NAME;			\
        target, NAME2;			\
      }					\



#define TEST_BUTTON_PROG(NAME, NAME2, SIG, SIG2) \
      program {				\
        name, NAME"-toggle";		\
        signal, "mouse,clicked,1";		\
        source, NAME;			\
        action, STATE_SET "toggled" 0.0;	\
        transition, LINEAR 0.0;	\
        target, NAME;			\
        target, NAME2;			\
      }					\
      program {				\
        name, NAME2"-toggle";		\
        signal, "mouse,clicked,1";		\
        source, NAME2;			\
        action, STATE_SET "default" 0.0;	\
        transition, LINEAR 0.0;	\
        target, NAME;			\
        target, NAME2;			\
      }	


