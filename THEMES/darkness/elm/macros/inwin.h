#define INWIN_PART_BASE \
         part { \
            name: "base"; \
            type: RECT; \
            description { \
               state: "default" 0.0; \
               color: 0 0 0 0; \
            } \
            description { \
               state: "visible" 0.0; \
               inherit: "default" 1.0; \
               color: 0 0 0 64; \
            } \
         }

#define INWIN_PART_SHADOW \
         part { \
            name: "shad"; \
            mouse_events:  0; \
            description { \
               state: "default" 0.0; \
               color: DARKNESS_BLUE 255; \
               image.normal: "shad_circ.png"; \
               rel1.to: "elm.swallow.content"; \
               rel1.offset: -64 -64; \
               rel2.to: "elm.swallow.content"; \
               rel2.offset: 63 63; \
               fill.smooth: 0; \
            } \
         }

#define INWIN_PART_POP \
         part { \
            name: "pop"; \
            description { \
               state: "default" 0.0; \
               rel1.to: "elm.swallow.content"; \
               rel1.offset: -5 -5; \
               rel2.to: "elm.swallow.content"; \
               rel2.offset: 4 4; \
               image { \
                  normal: "entry_under.png"; \
               } \
               image.middle: SOLID; \
            } \
         } \
         part { \
            name: "pop_bg"; \
            type: RECT; \
            description { \
               state: "default" 0.0; \
               rel1.to: "pop"; \
               rel1.offset: -2 -2; \
               rel2.to: "pop"; \
               rel2.offset: 2 2; \
               color: DARKNESS_BACKGROUND 255; \
            } \
         }

#define INWIN_PROGRAMS \
      programs { \
         program { name: "show"; \
            signal: "elm,action,show"; \
            source: "elm"; \
            action: STATE_SET "visible" 0.0; \
            target: "base"; \
         } \
         program { name: "hide"; \
            signal: "elm,action,hide"; \
            source: "elm"; \
            action: STATE_SET "default" 0.0; \
            target: "base"; \
         } \
      }
