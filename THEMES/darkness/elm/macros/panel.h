#define PANEL_PROGRAMS \
   programs \
       { \
          program \
            { \
               name: "show"; \
               signal: "elm,action,show"; \
               source: "elm"; \
               action: STATE_SET "default" 0.0; \
               target: "bg"; \
               target: "button_icon"; \
               transition: LINEAR 0.5; \
            } \
          program \
            { \
               name: "hide"; \
               signal: "elm,action,hide"; \
               source: "elm"; \
               action: STATE_SET "hidden" 0.0; \
               target: "bg"; \
               target: "button_icon"; \
               transition: LINEAR 0.5; \
            } \
          program \
            { \
               name: "button_click"; \
               signal: "mouse,down,1"; \
               source: "button_events"; \
               action: STATE_SET "down" 0.0; \
               target: "button"; \
               BUTTON_GLOW_SUDDEN_STOP("button") \
            } \
          program \
            { \
               name: "button_unclick"; \
               signal: "mouse,up,1"; \
               source: "button_events"; \
               action: STATE_SET "default" 0.0; \
               target: "button"; \
            } \
          program \
            { \
               name: "button_unclick3"; \
               signal: "mouse,clicked,1"; \
               source: "button_events"; \
               action: SIGNAL_EMIT "elm,action,panel,toggle" ""; \
            } \
          program { \
            name: "button_mouseover"; \
            signal: "mouse,in"; \
            source: "button_events"; \
            BUTTON_GLOW_ACTION("button") \
          } \
          program { \
             name: "button_mouseout"; \
             signal: "mouse,out"; \
             source: "button_events"; \
             BUTTON_GLOW_STOP("button") \
          } \
          program { \
             name: "button_focus"; \
             FOCUS_SIGNAL \
             FOCUS_GLOW_ACTION("button") \
          } \
          program { \
             name: "button_unfocus"; \
             UNFOCUS_SIGNAL \
             UNFOCUS_GLOW_ACTION("button") \
          } \
          program { \
             name: "button_drag"; \
             signal: "repeated"; \
             source: "elm"; \
             DRAG_ENTER_ACTION("button") \
          } \
          BUTTON_GLOW_PROGRAM("button") \
          BUTTON_DISABLE_PROGRAM("button") \
       }
