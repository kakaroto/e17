#define GENLIST_PART_BASE_IMAGE(IMG) \
         part { \
            name: "base"; \
            mouse_events: 0; \
            description { \
               state: "default" 0.0; \
               image { \
                  normal: IMG; \
                  border: 2 2 2 2; \
               } \
               fill.smooth: 0; \
            } \
         }

#define GENLIST_PART_FG \
         part { \
            name: "fg1"; \
            clip_to: "disclip"; \
            mouse_events: 0; \
            description { \
               state: "default" 0.0; \
               visible: 0; \
               color: DARKNESS_LIST_ROW_ODD 0; \
               rel1.to: "base"; \
               rel2.to: "base"; \
               image { \
                  normal: "menu_item_highlight.png"; \
                  border: 6 6 6 0; \
               } \
            } \
            description { \
               state: "selected" 0.0; \
               inherit: "default" 0.0; \
               visible: 1; \
               color: 255 255 255 255; \
            } \
         }

#define GENLIST_PART_DISCLIP(REL) \
         part { \
            name: "disclip"; \
            type: RECT; \
            description { \
               state: "default" 0.0; \
               rel1.to: REL; \
               rel2.to: REL; \
            } \
            description { \
               state: "disabled" 0.0; \
               inherit: "default" 0.0; \
               color: 255 255 255 64; \
            } \
         }

#define GENLIST_PART_BASE_SHADOW \
         part { \
            name: "base_sh"; \
            mouse_events: 0; \
            description { \
               state: "default" 0.0; \
               align: 0.0 0.0; \
               min: 0 10; \
               fixed: 1 1; \
               rel1 { \
                  to: "base"; \
                  relative: 0.0 1.0; \
               } \
               rel2 { \
                  to: "base"; \
                  offset: -1 0; \
               } \
               color: DARKNESS_BRIGHT_BLUE 255; \
               image.normal: "ilist_item_shadow.png"; \
               fill.smooth: 0; \
            } \
            description { \
               state: "default" 1.0; \
               visible: 0; \
            } \
         }

#define GENLIST_PART_SWALLOW_PAD \
         part { \
            name: "elm.swallow.pad"; \
            type: SWALLOW; \
            description { state: "default" 0.0; \
               fixed: 1 0; \
               align: 0.0 0.5; \
               rel1.offset:   4    4; \
               rel2 { \
                  relative: 0.0  1.0; \
                  offset:   4   -5; \
               } \
            } \
         }

#define GENLIST_PART_SWALLOW_ICON_REL_PAD \
         part { \
            name: "elm.swallow.icon"; \
            clip_to: "disclip"; \
            type: SWALLOW; \
            description { \
               state: "default" 0.0; \
               fixed: 1 0; \
               align: 0.0 0.5; \
               rel1 { \
                  to_x: "elm.swallow.pad"; \
                  relative: 1.0  0.0; \
                  offset:   -1    4; \
               } \
               rel2 { \
                  to_x: "elm.swallow.pad"; \
                  offset:   -1   -5; \
               } \
            } \
         }

#define GENLIST_PART_SWALLOW_ICON_REL_ARROW \
         part { \
            name: "elm.swallow.icon"; \
            clip_to: "disclip"; \
            type: SWALLOW; \
            description { \
               state: "default" 0.0; \
               fixed: 1 0; \
               align: 0.0 0.5; \
               rel1 { \
                  to_x: "arrow"; \
                  relative: 1.0  0.0; \
                  offset:   4    4; \
               } \
               rel2 { \
                  to_x: "arrow"; \
                  relative: 1.0  1.0; \
                  offset:   4   -5; \
               } \
            } \
         }

#define GENLIST_PART_SWALLOW_END \
         part { \
            name: "elm.swallow.end"; \
            clip_to: "disclip"; \
            type: SWALLOW; \
            description { \
               state: "default" 0.0; \
               fixed: 1 0; \
               align: 1.0 0.5; \
               aspect: 1.0 1.0; \
               aspect_preference: VERTICAL; \
               rel1 { \
                  relative: 1.0  0.0; \
                  offset:   -5    4; \
               } \
               rel2.offset:   -5   -5; \
            } \
         }

#define GENLIST_PART_TEXT(REL) \
  GENLIST_PART_TEXT_BASE(REL, \
                  style: "Label"; \
                  min: 1 1; \
                  align: -1.0 0.5;, \
               color: 224 224 224 255; \
               color3: 0 0 0 64; \
  )

#define GENLIST_PART_TEXT_GENLIST(REL) \
  GENLIST_PART_TEXT_BASE(REL, \
                       style: "Label"; \
                       min: 1 1;, \
  )


#define GENLIST_PART_TEXT_BASE(REL, T, SELECTED) \
         part { \
            name: "elm.text"; \
            clip_to: "disclip"; \
            type: TEXTBLOCK; \
            effect: SOFT_SHADOW; \
            mouse_events: 0; \
            scale: 1; \
            description { \
               state: "default" 0.0; \
               REL \
               color: 255 255 255 255; \
               color3: DARKNESS_BLUE 32; \
               text { \
                  T \
               } \
            } \
            description { \
               state: "selected" 0.0; \
               inherit: "default" 0.0; \
               SELECTED \
            } \
         }

#define GENLIST_PART_TEXT_SUB \
         part { \
            name: "elm.text.sub"; \
            clip_to: "disclip"; \
            type:           TEXT; \
            mouse_events:   0; \
            scale: 1; \
            description { \
               state: "default" 0.0; \
               rel1 { \
                  to_x:     "elm.swallow.icon"; \
                  relative: 1.0  0.5; \
                  offset:   0 4; \
               } \
               rel2 { \
                  to_x:     "elm.swallow.end"; \
                  relative: 0.0  1.0; \
                  offset:   -1 -5; \
               } \
               color: 0 0 0 128; \
               color3: DARKNESS_BLUE 32; \
               text { \
                  font: "DejaVu"; \
                  size: 8; \
                  min: 1 1; \
                  align: 0.0 0.5; \
                  text_class: "Label"; \
               } \
            } \
            description { \
               state: "selected" 0.0; \
               inherit: "default" 0.0; \
               color: 128 128 128 255; \
               color3: 0 0 0 32; \
            } \
         }

#define GENLIST_PART_TEXT_TITLE(REL) \
         part { \
            name: "elm.text.title"; \
            clip_to: "disclip"; \
            type:           TEXT; \
            effect:         SOFT_SHADOW; \
            mouse_events:   0; \
            scale: 1; \
            description { \
               state: "default" 0.0; \
               rel1 { \
                  REL \
               } \
               rel2 { \
                  relative: 1.0  0.5; \
                  offset:   -1 -5; \
               } \
               color: 255 255 255 255; \
               color3: 0 0 0 0; \
               text { \
                  text_class: "Label"; \
                  min: 0 1; \
                  align: 0.0 0.5; \
               } \
            } \
            description { \
               state: "selected" 0.0; \
               inherit: "default" 0.0; \
               color: 224 224 224 255; \
               color3: 0 0 0 64; \
            } \
         }


#define GENLIST_PART_ARROW \
         part { \
            name: "arrow"; \
            clip_to: "disclip"; \
            ignore_flags: ON_HOLD; \
            description { \
               state: "default" 0.0; \
               fixed: 1 0; \
               align: 0.0 0.5; \
               aspect: 1.0 1.0; \
               rel1 { \
                  to_x: "elm.swallow.pad"; \
                  relative: 1.0  0.0; \
                  offset:   -1    4; \
               } \
               rel2 { \
                  to_x: "elm.swallow.pad"; \
                  relative: 1.0  1.0; \
                  offset:   -1   -5; \
               } \
               image.normal: "icon_arrow_right.png"; \
            } \
            description { \
               state: "default" 0.1; \
               inherit: "default" 0.0; \
               image.normal: "icon_arrow_left.png"; \
            } \
            description { \
               state: "active" 0.0; \
               inherit: "default" 0.0; \
               image.normal: "icon_arrow_down.png"; \
            } \
            description { \
              state: "active" 0.1; \
               inherit: "default" 0.0; \
               image.normal: "icon_arrow_down.png"; \
            } \
         }
         
#define GENLIST_PROGRAMS(PART, TARGETS) \
   LIST_PROGRAMS(TARGETS) \
   programs { \
      program { \
         DISABLE_PROGRAM_PART \
         target:  "disclip"; \
      } \
      program { \
         ENABLE_PROGRAM_PART \
         target:  "disclip"; \
      } \
      program { \
         name: "focus"; \
         FOCUS_SIGNAL \
         FOCUS_GLOW_ACTION(PART) \
      } \
      program { \
         name: "unfocus"; \
         UNFOCUS_SIGNAL \
         UNFOCUS_GLOW_ACTION(PART) \
      } \
   }

#define GENLIST_PROGRAMS_RTL \
      programs { \
         program { \
            name: "to_rtl"; \
            signal: "edje,state,rtl"; \
            source: "edje"; \
            script { \
               new st[31]; \
               new Float:vl; \
               get_state(PART:"arrow", st, 30, vl); \
               if (vl == 0.0) { \
                  set_state(PART:"arrow", st, 0.1); \
               } \
            } \
         } \
         program { \
            name: "to_ltr"; \
            signal: "edje,state,ltr"; \
            source: "edje"; \
            script { \
               new st[31]; \
               new Float:vl; \
               get_state(PART:"arrow", st, 30, vl); \
               if (vl == 0.1) { \
                  set_state(PART:"arrow", st, 0.0); \
               } \
            } \
         } \
      }
#define GENLIST_PROGRAMS_TREE \
      programs { \
         program { \
            name:    "expand"; \
            signal:  "mouse,up,1"; \
            source:  "arrow"; \
            action:  SIGNAL_EMIT "elm,action,expand,toggle" "elm"; \
         } \
         program { \
            name:    "go_expanded"; \
            signal:  "elm,state,expanded"; \
            source:  "elm"; \
            script { \
               new st[31]; \
               new Float:vl; \
               get_state(PART:"arrow", st, 30, vl); \
               set_state(PART:"arrow", "active", vl); \
            } \
         } \
         program { \
            name:    "go_contracted"; \
            signal:  "elm,state,contracted"; \
            source:  "elm"; \
            script { \
               new st[31]; \
               new Float:vl; \
               get_state(PART:"arrow", st, 30, vl); \
               set_state(PART:"arrow", "default", vl); \
            } \
         } \
      }
#define GENLIST_PROGRAMS_DOT \
      programs { \
         program { \
            name:    "go_active"; \
            signal:  "elm,state,selected"; \
            source:  "elm"; \
            action:  STATE_SET "selected" 0.0; \
            target:  "bg"; \
            target:  "fg1"; \
            target:  "elm.text.title"; \
            target:  "elm.text.trackno"; \
            target:  "elm.text.length"; \
            after:   "dot_active"; \
         } \
         program { \
            name: "dot_active"; \
            script { \
               if (get_int(dot_visible) == 1) \
                  set_state(PART:"dot", "selected", 0.0); \
            } \
         } \
         program { \
            name:    "go_passive"; \
            signal:  "elm,state,unselected"; \
            source:  "elm"; \
            action:  STATE_SET "default" 0.0; \
            target:  "bg"; \
            target:  "fg1"; \
            target:  "elm.text.title"; \
            target:  "elm.text.length"; \
            target:  "elm.text.trackno"; \
            transition: LINEAR 0.1; \
            after:   "dot_passive"; \
         } \
         program { \
            name: "dot_passive"; \
            script { \
               if (get_int(dot_visible) == 1) \
                  set_state(PART:"dot", "visible", 0.0); \
            } \
         } \
         program { \
            signal: "elm,state,elm.state.trackno,active"; \
            source: "elm"; \
            script { \
               set_state(PART:"dot", "visible", 0.0); \
               set_int(dot_visible, 1); \
            } \
         } \
         program { \
            DISABLE_PROGRAM_PART \
            target:  "disclip"; \
         } \
         program { \
            ENABLE_PROGRAM_PART \
            target:  "disclip"; \
         } \
      }
