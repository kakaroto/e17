#define GLINTING_ITEM_GLINT_PARTS(ITEM) \
part { \
    name, ITEM"_title_glint_clip"; \
    type, RECT; \
    mouse_events, 0; \
    description { \
	state, "default" 0.0; \
	rel1 { \
	    offset, 3 3; \
	    to, ITEM"_title_bar"; \
	} \
	rel2 { \
	    offset, -4 -4; \
	    to, ITEM"_title_bar"; \
	} \
	color, 255 255 255 255; \
    } \
} \
part { \
    name, ITEM"_title_glint_1"; \
    mouse_events, 0; \
    description { \
	state, "default" 0.0; \
	visible, 0; \
	color, 255 255 255 0; \
	align, 0.0 0.0; \
	max, 34 13; \
	rel1 { \
	    relative, 0.0 0.0; \
	    offset, 2 2; \
	    to, ITEM"_title_bar"; \
	} \
	rel2 { \
	    relative, 1.0 1.0; \
	    offset, -3 -3; \
	    to, ITEM"_title_bar"; \
	} \
	image { \
	    normal, "e17_titlebar_glint1.png"; \
	    border, 5 0 5 0; \
	} \
	fill { \
	    smooth, 0; \
	} \
    } \
    description { \
	state, "active" 0.0; \
	inherit, "default" 0.0; \
	visible, 1; \
	color, 255 255 255 255; \
    } \
} \
part { \
    name, ITEM"_title_glint_2"; \
    mouse_events, 0; \
    clip_to, ITEM"_title_glint_clip"; \
    description { \
	state, "default" 0.0; \
	visible, 0; \
	color, 255 255 255 0; \
	align, 1.0 0.5; \
	max, 0 0; \
	rel1 { \
	    relative, 0.0 0.0; \
	    offset, 3 3; \
	    to, ITEM"_title_bar"; \
	} \
	rel2 { \
	    relative, 0.0 1.0; \
	    offset, 3 -4; \
	    to, ITEM"_title_bar"; \
	} \
	image { \
	    normal, "e17_titlebar_glint2.png"; \
	    border, 0 0 5 5; \
	} \
	fill { \
	    smooth, 0; \
	} \
    } \
    description { \
	state, "active" 0.0; \
	inherit, "default" 0.0; \
	visible, 1; \
	color, 255 255 255 255; \
	max, 169 9999; \
	min, 169 8; \
	align, 0.5 0.5; \
	rel1 { \
	    relative, 0.0 0.0; \
	    offset, 3 3; \
	} \
	rel2 { \
	    relative, 0.0 1.0; \
	    offset, 3 -4; \
	} \
    } \
    description { \
	state, "active2" 0.0; \
	inherit, "default" 0.0; \
	visible, 1; \
	color, 255 255 255 128; \
	max, 169 9999; \
	min, 169 8; \
	align, 0.5 0.5; \
	rel1 { \
	    relative, 1.0 0.0; \
	    offset, -4 3; \
	} \
	rel2 { \
	    relative, 1.0 1.0; \
	    offset, -4 -4; \
	} \
    } \
    description { \
	state, "active3" 0.0; \
	inherit, "default" 0.0; \
	visible, 0; \
	color, 255 255 255 0; \
	max, 169 9999; \
	min, 169 8; \
	align, 0.0 0.5; \
	rel1 { \
	    relative, 1.0 0.0; \
	    offset, -4 3; \
	} \
	rel2 { \
	    relative, 1.0 1.0; \
	    offset, -4 -4; \
	} \
    } \
} \
part { \
    name, ITEM"_title_glint_3"; \
    mouse_events, 0; \
    description { \
	state, "default" 0.0; \
	visible, 0; \
	color, 255 255 255 0; \
	align, 1.0 1.0; \
	max, 34 13; \
	rel1 { \
	    relative, 0.0 0.0; \
	    offset, 2 2; \
	    to, ITEM"_title_bar"; \
	} \
	rel2 { \
	    relative, 1.0 1.0; \
	    offset, -3 -3; \
	    to, ITEM"_title_bar"; \
	} \
	image { \
	    normal, "e17_titlebar_glint3.png"; \
	    border, 0 5 0 5; \
	} \
	fill { \
	    smooth, 0; \
	} \
    } \
    description { \
	state, "active" 0.0; \
	inherit, "default" 0.0; \
	visible, 1; \
	color, 255 255 255 255; \
    } \
} 

#define GLINTING_ITEM_SHADOW_PARTS(ITEM) \
 part { \
     name, ITEM"_title_shadow2"; \
     mouse_events, 0; \
     description { \
	 state, "default" 0.0; \
	 visible, 0; \
	 rel1 { \
	     relative, 0.0 0.0; \
	     offset, 0 0; \
	     to, ITEM"_title_outline"; \
	 } \
	 rel2 { \
	     relative, 1.0 1.0; \
	     offset, -1 -1; \
	     to, ITEM"_title_outline"; \
	 } \
	 image { \
	     normal, "e17_titlebar_shadow2.png"; \
	     border, 8 8 8 8; \
	 } \
	 fill { \
	     smooth, 0; \
	 } \
	 color, 255 255 255 0; \
     } \
     description { \
	 state, "active" 0.0; \
	 visible, 1; \
	 rel1 { \
	     relative, 0.0 0.0; \
	     offset, 0 0; \
	     to, ITEM"_title_outline"; \
	 } \
	 rel2 { \
	     relative, 1.0 1.0; \
	     offset, -1 -1; \
	     to, ITEM"_title_outline"; \
	 } \
	 image { \
	     normal, "e17_titlebar_shadow2.png"; \
	     border, 8 8 8 8; \
	 } \
	 fill { \
	     smooth, 0; \
	 } \
	 color, 255 255 255 255; \
     } \
 } \
part { \
     name, ITEM"_title_shadow1"; \
     mouse_events, 0; \
     description { \
	 state, "default" 0.0; \
	 visible, 0; \
	 rel1 { \
	     relative, 0.0 0.0; \
	     offset, 0 0; \
	     to, ITEM"_title_outline"; \
	 } \
	 rel2 { \
	     relative, 1.0 1.0; \
	     offset, -1 -1; \
	     to, ITEM"_title_outline"; \
	 } \
	 image { \
	     normal, "e17_titlebar_shadow1.png"; \
	     border, 8 8 8 8; \
	 } \
	 fill { \
	     smooth, 0; \
	 } \
	 color, 255 255 255 0; \
     } \
     description { \
	 state, "active" 0.0; \
	 visible, 1; \
	 rel1 { \
	     relative, 0.0 0.0; \
	     offset, 0 0; \
	     to, ITEM"_title_outline"; \
	 } \
	 rel2 { \
	     relative, 1.0 1.0; \
	     offset, -1 -1; \
	     to, ITEM"_title_outline"; \
	 } \
	 image { \
	     normal, "e17_titlebar_shadow1.png"; \
	     border, 8 8 8 8; \
	 } \
	 fill { \
	     smooth, 0; \
	 } \
	 color, 255 255 255 255; \
     } \
} \
part { \
    name, ITEM"_title_bar"; \
    mouse_events, 0; \
    description { \
	state, "default" 0.0; \
	visible, 0; \
	rel1 { \
	    relative, 0.0 0.0; \
	    offset, 0 0; \
	    to, ITEM"_title_outline"; \
	} \
	rel2 { \
	    relative, 1.0 1.0; \
	    offset, -1 -1; \
	    to, ITEM"_title_outline"; \
	} \
	image { \
	    normal, "e17_titlebar.png"; \
	    border, 8 8 8 8; \
	} \
	color, 255 255 255 0; \
    } \
    description { \
	state, "active" 0.0; \
	visible, 1; \
	rel1 { \
	    relative, 0.0 0.0; \
	    offset, 0 0; \
	    to, ITEM"_title_outline"; \
	} \
	rel2 { \
	    relative, 1.0 1.0; \
	    offset, -1 -1; \
	    to, ITEM"_title_outline"; \
	} \
	image { \
	    normal, "e17_titlebar.png"; \
	    border, 8 8 8 8; \
	} \
	color, 255 255 255 255; \
    } \
}

#define GLINTING_ITEM_OVERLAY_PART(ITEM) \
part { \
    name, ITEM"_overlay"; \
    type, RECT; \
    mouse_events, 1; \
    repeat_events, 1; \
    description { \
	state, "default" 0.0; \
	visible, 1; \
	rel1 { \
	    relative, 0.0 0.0; \
	    to, ITEM"_title_outline"; \
	    offset, 0 0; \
	} \
	rel2 { \
	    relative, 1.0 1.0; \
	    to, ITEM"_title_outline"; \
	    offset, -1 -1; \
	} \
	color, 0 0 0 0; \
    } \
    description { \
	state, "hidden" 0.0; \
	inherit: "default" 0.0; \
	visible, 0; \
    } \
}

#define BUTTON_COMMON(BT_TEXT,rel1_h,rel1_v,rel1_off_h,rel1_off_v,rel2_h,rel2_v,rel2_off_h,rel2_off_v,bt_to) \
part { \
    name: BT_TEXT"_title_outline"; \
    mouse_events,  0; \
    type, RECT; \
    description { \
	color: 0 0 0 0; \
	rel1.to, BT_TEXT"_title_bar"; \
	rel2.to, BT_TEXT"_title_bar"; \
    } \
} \
part { \
    name: BT_TEXT"_title_bar"; \
    mouse_events,  0; \
    description { \
	state,    "default" 0.0;\
	rel1 { \
          relative: rel1_h rel1_v; \
          offset: rel1_off_h rel1_off_v; \
	  to: bt_to; \
	} \
	rel2 { \
          relative: rel2_h rel2_v; \
          offset: rel2_off_h rel2_off_v; \
	  to: bt_to; \
	} \
	image { \
	    normal, "e17_button.png"; \
	    border, 8 8 8 8; \
	} \
    } \
    description { \
	state,    "clicked" 0.0; \
	inherit,  "default" 0.0; \
	image { \
	    normal, "e17_button2.png"; \
	} \
    } \
} 

#define LAST_BUTTON(BT_TEXT,rel1_h,rel1_v,rel1_off_h,rel1_off_v,rel2_h,rel2_v,rel2_off_h,rel2_off_v,bt_to) \
BUTTON_COMMON(BT_TEXT,rel1_h,rel1_v,rel1_off_h,rel1_off_v,rel2_h,rel2_v,rel2_off_h,rel2_off_v,bt_to) \
part { \
    name, BT_TEXT"_text"; \
    type, TEXT; \
    effect, SHADOW; \
    mouse_events, 0; \
    description { \
        state, "default" 0.0; \
	rel1.to, BT_TEXT"_title_bar"; \
	rel2.to, BT_TEXT"_title_bar"; \
        color, 0 0 0 255; \
        color3, 255 255 255 128; \
        text { \
            text, BT_TEXT; \
            font, "Edje Vera"; \
            size, 10; \
            min, 1 1; \
            align, 0.5 0.5; \
        } \
    } \
} \
GLINTING_ITEM_GLINT_PARTS(BT_TEXT) \
GLINTING_ITEM_OVERLAY_PART(BT_TEXT)


#define GLINTING_EFFECT_GLINT_PROGS(ITEM)\
 program { \
     name, ITEM"_go_active3b"; \
     action, STATE_SET "active" 0.0; \
     target, ITEM"_title_glint_1"; \
     target, ITEM"_title_glint_2"; \
     transition, LINEAR 0.05; \
     after, ITEM"_go_active4"; \
     after, ITEM"_go_active4b"; \
 } \
 program { \
     name, ITEM"_go_active4"; \
     action, STATE_SET "active2" 0.0; \
     target, ITEM"_title_glint_2"; \
     transition, LINEAR 0.2; \
     after, ITEM"_go_active5"; \
     after, ITEM"_go_active5b"; \
 } \
 program { \
     name, ITEM"_go_active4b"; \
     action, STATE_SET "active" 0.0; \
     target, ITEM"_title_glint_3"; \
     transition, LINEAR 0.2; \
 } \
 program { \
     name, ITEM"_go_active5"; \
     action, STATE_SET "active3" 0.0; \
     target, ITEM"_title_glint_2"; \
     transition, LINEAR 0.05; \
 } \
 program { \
     name, ITEM"_go_active5b"; \
     action, STATE_SET "default" 0.0; \
     target, ITEM"_title_glint_3"; \
     transition, LINEAR 0.1; \
     after, ITEM"_go_active6"; \
 } \
 program { \
     name, ITEM"_go_active6"; \
     action, STATE_SET "default" 0.0; \
     target, ITEM"_title_glint_1"; \
     target, ITEM"_title_glint_2"; \
     target, ITEM"_title_glint_3"; \
 }


#define BUTTONS_PROGS(BT_TEXT) \
program { \
    name, BT_TEXT"_button,clicked"; \
    source, BT_TEXT"_overlay"; \
    signal, "mouse,clicked,1"; \
    action, SIGNAL_EMIT "button,"BT_TEXT",clicked" "*"; \
} \
program \
{ \
    name, BT_TEXT"_button_down"; \
    signal, "mouse,down,1"; \
    source, BT_TEXT"_overlay"; \
    action, STATE_SET "clicked" 0.0; \
    target, BT_TEXT"_title_bar"; \
} \
program \
{ \
    name, BT_TEXT"_button_up"; \
    signal, "mouse,up,1"; \
    source, BT_TEXT"_overlay"; \
    action, STATE_SET "default" 0.0; \
    target, BT_TEXT"_title_bar"; \
} \
program { \
    name, BT_TEXT"_button_go_active_pre"; \
    signal, "mouse,in"; \
    source, BT_TEXT"_overlay"; \
    after, BT_TEXT"_go_active3b"; \
} \
GLINTING_EFFECT_GLINT_PROGS(BT_TEXT)
