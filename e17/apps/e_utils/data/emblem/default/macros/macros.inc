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

#define ARROW(DIR,PART_REL,REL1_H,REL1_V,REL1_OFF_H,REL1_OFF_V,REL2_H,REL2_V,REL2_OFF_H,REL2_OFF_V) \
part { \
    name, DIR"_arrow"; \
    mouse_events, 1; \
    repeat_events, 0; \
    description { \
        state, "default" 0.0; \
        min, 21 29; \
        max, 21 29; \
        color, 255 255 255 180; \
        rel1 { \
            relative, REL1_H REL1_V; \
            offset, REL1_OFF_H REL1_OFF_V; \
            to, PART_REL"_body"; \
        } \
        rel2 { \
            relative, REL2_H REL2_V; \
            offset, REL2_OFF_H REL2_OFF_V; \
            to, PART_REL"_body"; \
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

#define BGS_ARROW_PROGS(DIR) \
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
