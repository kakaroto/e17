#define TOOL_BUTTON_GROUP(ox1, oy1, ox2, oy2) \
   parts { \
      part { \
         name: "button_rect"; \
         type: RECT; \
         description { \
            state: "default" 0.0; \
            color: 255 255 255 0; \
            min: 48 48; \
            rel1 { \
               relative: 0.0 0.0; \
               offset: ox1 oy1; \
            } \
            rel2 { \
               relative: 1.0 1.0; \
               offset: ox2 oy2; \
            } \
         } \
      } \
      part { \
         name: "etk.swallow.child"; \
         type: SWALLOW; \
         description { \
            state: "active" 0.0; \
            rel1 { \
               relative: 0.0 0.0; \
               offset: 7 7; \
               to: "button_rect"; \
            } \
            rel2 { \
               relative: 1.0 1.0; \
               offset: -7 -7; \
               to: "button_rect"; \
            } \
         } \
      } \
      BUTTON_GLOW_PART("button_rect", 120); \
      BUTTON_GLOW_DOWN_PART("button_rect", 70); \
   } \
   programs { \
      program { \
         name: "button_pressed"; \
         signal: "etk,state,pressed"; \
         source: "etk"; \
         BUTTON_GLOW_DOWN_ACTION \
         BUTTON_GLOW_STOP \
      } \
      program { \
         name: "button_released"; \
         signal: "etk,state,released"; \
         source: "etk"; \
         BUTTON_GLOW_DOWN_STOP \
         BUTTON_GLOW_ACTION \
      } \
      program { \
         name: "button_enter"; \
         signal: "etk,state,enter"; \
         source: "etk"; \
         BUTTON_GLOW_ACTION \
      } \
      program { \
         name: "button_leave"; \
         signal: "etk,state,leave"; \
         source: "etk"; \
         BUTTON_GLOW_STOP \
      } \
       \
      BUTTON_GLOW_PROGRAM \
      BUTTON_GLOW_DOWN_PROGRAM \
   }
