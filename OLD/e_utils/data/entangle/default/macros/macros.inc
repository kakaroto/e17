#define SCROLL_BUTTON(DIR) \
        part { \
            name, DIR"_shadow2"; \
            mouse_events, 0; \
            description { \
                state, "default" 0.0; \
                rel1 { \
                    relative, 0.0 0.0; \
                    offset, 0 0; \
                    to, DIR"_bg"; \
                } \
                rel2 { \
                    relative, 1.0 1.0; \
                    offset, -1 -1; \
                    to, DIR"_bg"; \
                } \
                image { \
                    normal, "e17_mini_button_shadow2.png"; \
                    border, 8 8 8 8; \
                } \
            } \
            description { \
                state, "active" 0.0; \
                inherit, "default" 0.0; \
                visible, 0; \
                color, 255 255 255 0; \
            } \
            description { \
                state, "hidden" 0.0; \
                inherit, "default" 0.0; \
                visible, 0; \
                color, 255 255 255 0; \
            } \
        } \
        part { \
            name, DIR"_shadow1"; \
            mouse_events, 0; \
            description { \
                state, "default" 0.0; \
                rel1 { \
                    relative, 0.0 0.0; \
                    offset, 0 0; \
                    to, DIR"_bg"; \
                } \
                rel2 { \
                    relative, 1.0 1.0; \
                    offset, -1 -1; \
                    to, DIR"_bg"; \
                } \
                image { \
                    normal, "e17_mini_button_shadow1.png"; \
                    border, 8 8 8 8; \
                } \
            } \
            description { \
                state, "active" 0.0; \
                inherit, "default" 0.0; \
                visible, 0; \
                color, 255 255 255 0; \
            } \
            description { \
                state, "hidden" 0.0; \
                inherit, "default" 0.0; \
                visible, 0; \
                color, 255 255 255 0; \
            } \
        } \
        part { \
            name, DIR"_bg"; \
            mouse_events, 1; \
            description { \
                state, "default" 0.0; \
                rel1 { \
                    relative, 0.0 0.0; \
                    to: DIR"_clip"; \
                    offset, 0 0; \
                } \
                rel2 { \
                    relative, 1.0 1.0; \
                    to: DIR"_clip"; \
                    offset, -1 -1; \
                } \
                image { \
                    normal, "e17_mini_button.png"; \
                    border, 8 8 8 8; \
                } \
            } \
            description { \
                state, "active" 0.0; \
                inherit, "default" 0.0; \
                rel1 { \
                    relative, 0.0 0.0; \
                    offset, 1 1; \
                } \
                rel2 { \
                    relative, 1.0 1.0; \
                    offset, 0 0; \
                } \
            } \
            description { \
                state, "hidden" 0.0; \
                inherit, "default" 0.0; \
                visible, 0; \
                color, 255 255 255 0; \
            } \
        } \
        part { \
            name, DIR"_detail"; \
            mouse_events, 0; \
            description { \
                state, "default" 0.0; \
                max, 16 16; \
                rel1 { \
                    relative, 0.0 0.0; \
                    offset, 4 4; \
                    to, DIR"_bg"; \
                } \
                rel2 { \
                    relative, 1.0 1.0; \
                    offset, -5 -5; \
                    to, DIR"_bg"; \
                } \
                image { \
                    normal, "e17_button_detail_"DIR".png"; \
                } \
            } \
            description { \
                state, "hidden" 0.0; \
                inherit, "default" 0.0; \
                visible, 0; \
                color, 255 255 255 0; \
            } \
        }

#define SCROLL_BUTTON_PROG(DIR) \
        program { \
            name, DIR"_active_pre"; \
            signal, "mouse,down,1"; \
            source, DIR"_bg"; \
            action, STATE_SET "default" 0.0; \
            target, DIR"_shadow1"; \
            target, DIR"_shadow2"; \
            target, DIR"_bg"; \
            target, DIR"_detail"; \
            after, DIR"_active"; \
            after, DIR"_active_p"; \
            after, DIR"_signal"; \
        } \
        program { \
            name, DIR"_signal"; \
            action, SIGNAL_EMIT "menu,"DIR",clicked" "*"; \
        } \
        program { \
                name, "menu,"DIR",up"; \
                source, DIR"_bg"; \
                signal, "mouse,up,1"; \
                action, SIGNAL_EMIT "menu,"DIR",release" "*"; \
        } \
        program { \
            name, DIR"_active"; \
            action, STATE_SET "active" 0.0; \
            target, DIR"_shadow2"; \
            transition, LINEAR 0.05; \
            after, DIR"_active2"; \
        } \
        program { \
            name, DIR"_active2"; \
            action, STATE_SET "active" 0.0; \
            target, DIR"_shadow1"; \
            transition, LINEAR 0.05; \
        } \
        program { \
            name, DIR"_active_p"; \
            action, STATE_SET "active" 0.0; \
            target, DIR"_bg"; \
            target, DIR"_detail"; \
            transition, LINEAR 0.1; \
        } \
        program { \
            name, DIR"_passive_pre"; \
            signal, "mouse,up,1"; \
            source, DIR"_bg"; \
            action, STATE_SET "active" 0.0; \
            target, DIR"_shadow1"; \
            target, DIR"_shadow2"; \
            target, DIR"_bg"; \
            target, DIR"_detail"; \
            after, DIR"_passive"; \
            after, DIR"_passive_p"; \
        } \
        program { \
            name, DIR"_passive"; \
            action, STATE_SET "default" 0.0; \
            target, DIR"_shadow2"; \
            transition, LINEAR 0.1; \
            after, DIR"_passive2"; \
        } \
        program { \
            name, DIR"_passive2"; \
            action, STATE_SET "default" 0.0; \
            target, DIR"_shadow1"; \
            transition, LINEAR 0.1; \
        } \
        program { \
            name, DIR"_passive_p"; \
            action, STATE_SET "default" 0.0; \
            target, DIR"_bg"; \
            target, DIR"_detail"; \
            transition, LINEAR 0.2; \
        } 

#define GLINTING_EFFECT_ACTIVE_PROGS(ITEM) \
 program { \
     name, ITEM"_go_active_pre"; \
     signal, "mouse,in"; \
     source, ITEM"_overlay"; \
     action, STATE_SET "default" 0.0; \
     target, ITEM"_title_outline"; \
     target, ITEM"_title_bar"; \
     target, ITEM"_title_shadow1"; \
     target, ITEM"_title_shadow2"; \
     after, ITEM"_go_active"; \
 } \
 program { \
     name, ITEM"_go_active"; \
     action, STATE_SET "active" 0.0; \
     target, ITEM"_title_outline"; \
     target, ITEM"_title_bar"; \
     transition, LINEAR 0.05; \
     after, ITEM"_go_active2"; \
 } \
 program { \
     name, ITEM"_go_active2"; \
     action, STATE_SET "active" 0.0; \
     target, ITEM"_title_shadow1"; \
     transition, LINEAR 0.05; \
     after, ITEM"_go_active3"; \
     after, ITEM"_go_active3b"; \
 } \
 program { \
     name, ITEM"_go_active3"; \
     action, STATE_SET "active" 0.0; \
     target, ITEM"_title_shadow2"; \
     transition, LINEAR 0.05; \
 } 

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
 
#define GLINTING_EFFECT_PASSIVE_PROGS(ITEM) \
program { \
     name, ITEM"_go_passive_pre"; \
     signal, "mouse,out"; \
     source, ITEM"_overlay"; \
     action, STATE_SET "active" 0.0; \
     target, ITEM"_title_outline"; \
     target, ITEM"_title_bar"; \
     target, ITEM"_title_shadow1"; \
     target, ITEM"_title_shadow2"; \
     after, ITEM"_go_passive"; \
 } \
 program { \
     name, ITEM"_go_passive"; \
     action, STATE_SET "default" 0.0; \
     target, ITEM"_title_shadow2"; \
     transition, LINEAR 0.1; \
     after, ITEM"_go_passive2"; \
 } \
 program { \
     name, ITEM"_go_passive2"; \
     action, STATE_SET "default" 0.0; \
     target, ITEM"_title_shadow1"; \
     transition, LINEAR 0.1; \
     after, ITEM"_go_passive3"; \
 } \
 program { \
     name, ITEM"_go_passive3"; \
     action, STATE_SET "default" 0.0; \
     target, ITEM"_title_outline"; \
     target, ITEM"_title_bar"; \
     transition, LINEAR 0.1; \
}

#define  GLINTING_EFFECT_PROGS(ITEM) \
GLINTING_EFFECT_ACTIVE_PROGS(ITEM) \
GLINTING_EFFECT_GLINT_PROGS(ITEM) \
GLINTING_EFFECT_PASSIVE_PROGS(ITEM)

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

#define GLINTING_ITEM_PARTS(ITEM) \
GLINTING_ITEM_SHADOW_PARTS(ITEM)\
GLINTING_ITEM_GLINT_PARTS(ITEM) \
GLINTING_ITEM_OVERLAY_PART(ITEM)

#define ITEM_OVERLAY(PART,DIR) \
part { \
    name: PART"_overlay"; \
    mouse_events:  0; \
    description { \
        state:    "default" 0.0; \
        color, 255 255 255 255; \
        rel1.to, PART; \
        rel2.to, PART; \
        image { \
            normal: "e17_ibar_over_"DIR".png"; \
            border: 13 13 13 13; \
        } \
    } \
}

#define EAPPS_ARROW_PROGS(DIR) \
program { \
    name, DIR"_click"; \
    source, DIR"_arrow"; \
    signal, "mouse,down,*"; \
    action, SIGNAL_EMIT DIR",clicked" "*"; \
} \
program { \
    name, DIR"_click_release"; \
    source, DIR"_arrow"; \
    signal, "mouse,up,*"; \
    action, SIGNAL_EMIT DIR",click,release" "*"; \
} \
 program { \
    name, DIR"_glow"; \
    source, DIR"_arrow"; \
    signal, "mouse,in"; \
    action, STATE_SET "glow" 0.0; \
    target, DIR"_arrow"; \
 } \
program { \
     name, DIR"_unglow"; \
     source, DIR"_arrow"; \
     signal, "mouse,out"; \
     action, STATE_SET "default" 0.0; \
     target, DIR"_arrow"; \
}

#define ARROW(DIR,REL1_H,REL1_V,REL1_OFF_H,REL1_OFF_V,REL2_H,REL2_V,REL2_OFF_H,REL2_OFF_V) \
part { \
    name, DIR"_arrow"; \
    mouse_events, 1; \
    repeat_events, 0; \
    description { \
        state, "default" 0.0; \
        min, 29 21; \
        max, 29 21; \
        color, 255 255 255 180; \
        rel1 { \
            relative, REL1_H REL1_V; \
            offset, REL1_OFF_H REL1_OFF_V; \
            to, "eapps_bar_body"; \
        } \
        rel2 { \
            relative, REL2_H REL2_V; \
            offset, REL2_OFF_H REL2_OFF_V; \
            to, "eapps_bar_body"; \
        } \
        image { \
            normal, DIR".png"; \
        } \
        fill { \
            origin { \
                relative, 0.0 0.0; \
                offset, 0 0; \
            } \
            size { \
                relative, 1.0 1.0; \
                offset, 0 0; \
            } \
        } \
    } \
    description { \
        state, "glow" 0.0; \
        inherit, "default" 0.0; \
        color, 246 207 135 255; \
    } \
    description { \
        state, "hidden" 0.0; \
        inherit, "default" 0.0; \
        visible, 0; \
    } \
}

