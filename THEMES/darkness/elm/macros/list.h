#define LIST_PART_EVENT \
   part { \
      name: "event"; \
      type: RECT; \
      repeat_events: 1; \
      description { \
         state: "default" 0.0; \
         color: 0 0 0 0; \
      } \
   }

#define LIST_PART_BASE \
         part { \
            name: "base"; \
            type: RECT; \
            mouse_events: 0; \
            description { \
               state: "default" 0.0; \
               color: DARKNESS_LIST_ROW 255; \
            } \
            description { \
               state: "default" 1.0; \
               color: DARKNESS_LIST_ROW_ODD 255; \
            } \
         }

#define LIST_PART_BASE_ODD \
         part { \
            name: "base"; \
            type: RECT; \
            mouse_events: 0; \
             description { \
               state: "default" 0.0; \
                color: DARKNESS_LIST_ROW_ODD 255; \
             } \
          }

#define LIST_PART_BG(CLIP) \
         part { \
            name: "bg"; \
            CLIP \
            mouse_events: 0; \
            description { \
               state: "default" 0.0; \
               visible: 0; \
               color: 255 255 255 0; \
               rel1.offset: -5 -5; \
               rel2.offset: 4 4; \
               image { \
                  normal: "slider_button.png"; \
                  border: 6 6 6 6; \
               } \
               image.middle: SOLID; \
            } \
            description { \
               state: "selected" 0.0; \
               inherit: "default" 0.0; \
               visible: 1; \
               color: 255 255 255 255; \
               rel1.offset: -2 -2; \
               rel2.offset: 1 1; \
            } \
         }

#define LIST_PART_SWALLOW_ICON_BASE(REL2) \
          part { \
            name: "elm.swallow.icon"; \
            type: SWALLOW; \
            description { state: "default" 0.0; \
               rel1.offset:   4    4; \
               REL2 \
            } \
         }

#define LIST_PART_SWALLOW_ICON \
   LIST_PART_SWALLOW_ICON_BASE( \
                  fixed: 1 0; \
                  rel2.relative: 0.0  1.0; \
                  rel2.offset:   4   -5; \
                  align: 0.0 0.5; \
   )
   
#define LIST_PART_SWALLOW_ICON_HORIZ \
   LIST_PART_SWALLOW_ICON_BASE( \
                  fixed: 0 1; \
                  rel2.relative: 1.0 0.0; \
                  rel2.offset: -5 4; \
                  align: 0.5 0.0; \
  )

#define LIST_PART_SWALLOW_END_BASE(REL1) \
         part { \
            name: "elm.swallow.end"; \
            type: SWALLOW; \
            description { \
               state: "default" 0.0; \
               fixed: 1 1; \
               REL1 \
               rel2.offset:   -5   -5; \
            } \
         }

#define LIST_PART_SWALLOW_END \
   LIST_PART_SWALLOW_END_BASE( \
                  rel1.relative: 1.0  0.0; \
                  rel1.offset:   -5    4; \
                  align: 1.0 0.5; \
   )
   
#define LIST_PART_SWALLOW_END_HORIZ \
   LIST_PART_SWALLOW_END_BASE( \
                  rel1.relative: 0.0 1.0; \
                  rel1.offset: 4 -5; \
                  align: 0.5 1.0; \
  )

#define LIST_PART_FG(REL) \
         part { \
            name: "fg1"; \
            mouse_events: 0; \
            description { \
               state: "default" 0.0; \
               visible: 0; \
               color: 255 255 255 0; \
               rel1.to: REL; \
               rel2 { \
                  to: REL; \
                  offset: -1 -1; \
               } \
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

#define LIST_PART_TEXT(REL, TEXT_ALIGN) \
   part { \
      name: "elm.text"; \
      type:           TEXT; \
      effect:         SOFT_SHADOW; \
      mouse_events:   0; \
      scale: 1; \
      description { \
         state: "default" 0.0; \
         REL \
         color: 255 255 255 255; \
         color3: DARKNESS_BLUE 32; \
         text { \
            text_class: "Label"; \
            font: "DejaVu"; \
            size: 10; \
            min: 1 1; \
            align: TEXT_ALIGN; \
         } \
      } \
      description { \
         state: "selected" 0.0; \
         inherit: "default" 0.0; \
         color: 224 224 224 255; \
         color3: 0 0 0 64; \
      } \
   }

#define LIST_PROGRAMS(TARGETS) \
   programs { \
      program { \
         name:    "go_active"; \
         signal:  "elm,state,selected"; \
         source:  "elm"; \
         action:  STATE_SET "selected" 0.0; \
         transition: SINUSOIDAL 0.2; \
         TARGETS \
      } \
      program { \
         name:    "go_passive"; \
         signal:  "elm,state,unselected"; \
         source:  "elm"; \
         action:  STATE_SET "default" 0.0; \
         TARGETS \
         transition: SINUSOIDAL 0.1; \
      } \
   }

#define LIST_PROGRAMS_EVEN_ODD(TARGETS) \
   programs { \
      program { \
         name:    "odd"; \
         signal:  "elm,state,odd"; \
         source:  "elm"; \
         action:  STATE_SET "default" 1.0; \
         TARGETS \
      } \
      program { \
         name:    "even"; \
         signal:  "elm,state,even"; \
         source:  "elm"; \
         action:  STATE_SET "default" 0.0; \
         TARGETS \
      } \
   }
