#define PAGER_PART_BASE0 \
   part { \
      name: "base0"; \
      type: RECT; \
      mouse_events: 0; \
      clip_to: "clip"; \
      description { \
         state: "default" 0.0; \
         color: DARKNESS_BACKGROUND 255; \
         rel1.to: "over"; \
         rel2.to: "over"; \
      } \
   }


#define PAGER_PART_OVER \
         part { \
            name: "over"; \
            mouse_events:  0; \
            clip_to: "clip"; \
            description { \
               state:    "default" 0.0; \
               rel1 { \
                  to: "base"; \
                  offset: 4 4; \
               } \
               rel2 { \
                  to: "base"; \
                  offset: -5 -5; \
               } \
               image { \
                  normal: "entry_over.png"; \
                  border: 2 2 28 22; \
                  middle: 0; \
               } \
               fill.smooth : 0; \
            } \
         }

#define PAGER_PART_SWALLOW_CONTENT(REL) \
         part { \
            name: "elm.swallow.content"; \
            type: SWALLOW; \
            clip_to: "clip"; \
            description { \
               state: "default" 0.0; \
               REL \
            } \
         }

#define PAGER_PROGRAMS_BASE(STATE, TARGET) \
      programs { \
         program { \
            name: "push_start"; \
            signal: "elm,action,push"; \
            source: "elm"; \
            action:  STATE_SET STATE 0.0; \
            TARGET \
            target: "clip"; \
            after: "show_start2"; \
         } \
         program { \
            name: "show_start"; \
            signal: "elm,action,show"; \
            source: "elm"; \
            action:  STATE_SET "hidden" 0.0; \
            TARGET \
            target: "clip"; \
            after: "show_start2"; \
         } \
         program { \
            name: "show_start2"; \
            action:  STATE_SET "visible" 0.0; \
            transition: DECELERATE 0.5; \
            TARGET \
            target: "clip"; \
            after: "show_end"; \
         } \
         program { \
            name: "show_end"; \
            action: SIGNAL_EMIT "elm,action,show,finished" ""; \
         } \
         program { \
            name: "pop_start"; \
            signal: "elm,action,pop"; \
            source: "elm"; \
            action:  STATE_SET "visible" 0.0; \
            TARGET \
            target: "clip"; \
            after: "hide_start2"; \
         } \
         program { \
            name: "hide_start"; \
            signal: "elm,action,hide"; \
            source: "elm"; \
            action:  STATE_SET "visible" 0.0; \
            TARGET \
            target: "clip"; \
            after: "hide_start2"; \
         } \
         program { \
            name: "hide_start2"; \
            action:  STATE_SET "hidden" 0.0; \
            transition: DECELERATE 0.5; \
            TARGET \
            target: "clip"; \
            after: "hide_end"; \
         } \
         program { \
            name: "hide_end"; \
            action: SIGNAL_EMIT "elm,action,hide,finished" ""; \
         } \
         program { \
            name: "reset"; \
            signal: "elm,action,reset"; \
            source: "elm"; \
            action:  STATE_SET "default" 0.0; \
            TARGET \
            target: "clip"; \
         } \
      }

#define PAGER_PROGRAMS(STATE) \
   PAGER_PROGRAMS_BASE(STATE, target: "base";)
