#define GENGRID_PART_BG(CLIP) \
         part { \
            name: "bg"; \
            CLIP \
            mouse_events: 0; \
            description { \
               state: "default" 0.0; \
               visible: 0; \
               color: 255 255 255 0; \
               rel1.offset: -3 -3; \
               rel2.offset: 2 2; \
               image { \
                  normal: "slider_button.png"; \
                  border: 6 6 6 6; \
                  middle: SOLID; \
               } \
            } \
            description { \
               state: "selected" 0.0; \
               inherit: "default" 0.0; \
               visible: 1; \
               color: 255 255 255 255; \
            } \
         }

#define GENGRID_PART_FG_BASE(CLIP, REL) \
         part { \
            name: "fg1"; \
            CLIP \
            mouse_events: 0; \
            description { \
               state: "default" 0.0; \
               visible: 0; \
               color: 255 255 255 0; \
               REL \
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

#define GENGRID_PART_FG_REL_BG \
   GENGRID_PART_FG_BASE(clip_to: "disclip";, \
               rel1.to: "bg"; \
               rel2.to: "bg"; \
               rel2.offset: -1 -2; \
   )

#define GENGRID_PART_FG \
   GENGRID_PART_FG_BASE(clip_to: "disclip";, \
               rel1.offset: -3 -3; \
               rel2.offset: 2 -1; \
   )
#define GENGRID_PART_SWALLOW_PAD \
         part { \
            name: "elm.swallow.pad"; \
            type: SWALLOW; \
            description { \
               state: "default" 0.0; \
               fixed: 1 0; \
               align: 0.0 0.5; \
               rel1 { \
                  relative: 0.0  1.0; \
                  offset:   0    -10; \
               } \
               rel2 { \
                  to_y: "elm.text"; \
                  relative: 0.0  0.0; \
                  offset:   -1   -1; \
               } \
            } \
         }

#define GENGRID_PART_SWALLOW_ICON \
         part { \
            name: "elm.swallow.icon"; \
            clip_to: "disclip"; \
            type: SWALLOW; \
            description { \
               state: "default" 0.0; \
               fixed: 1 0; \
               align: 0.5 0.5; \
               rel1.offset:   -1    6; \
               rel2 { \
                  to_y: "elm.swallow.pad"; \
                  relative: 1.0  0.0; \
                  offset:   -1   -7; \
               } \
            } \
         }
