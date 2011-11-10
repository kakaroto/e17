#define BUTTON_DISABLE_PART(NAME, BORDER) \
      part { \
         name:          NAME"_disabled"; \
         type:          IMAGE; \
         mouse_events:  1; \
         description { \
            state:    "default" 0.0; \
            visible:  0; \
            rel1.to: NAME; \
            rel2.to: NAME; \
            image { \
               normal: "button_disabled.png"; \
               border: BORDER; \
            } \
         } \
         description { \
            state:    "disabled" 0.0; \
            inherit:  "default" 0.0; \
            visible:  1; \
         } \
      }


#define BUTTON_PART(NAME, EXTRA, BORDER) \
      part { \
         name: NAME; \
         description { \
            state: "default" 0.0; \
            visible: 1; \
            EXTRA \
            image { \
               normal: "button_up.png"; \
               border: BORDER; \
            } \
            fill.smooth: 0; \
         } \
         description { \
            state: "down" 0.0; \
            inherit: "default" 0.0; \
            image.normal: "button_down.png"; \
         } \
         description { \
            state: "disabled" 0.0; \
            inherit: "default" 0.0; \
            visible: 0; \
         } \
      } \
      BUTTON_DISABLE_PART(NAME, BORDER)
#define BUTTON_GLOW_PART(rect_part, alpha) \
   part { \
      name: rect_part"_glow"; \
      mouse_events: 0; \
      description { \
         state: "default" 0.0; \
         color: 255 255 255 0; \
         fixed: 1 1; \
         rel1.to: rect_part; \
         rel2.to: rect_part; \
         image { \
            normal: "button_glow.png"; \
            border: 15 15 10 10; \
         } \
         fill { \
            smooth: 0; \
         } \
      } \
      description { \
         state: "active" 0.0; \
         inherit: "default" 0.0; \
         color: 255 255 255 alpha; \
      } \
   }

#define BUTTON_GLOW_DOWN_PART(rect_part, alpha) \
   part { \
      name: rect_part"_glow_down"; \
      mouse_events: 0; \
      description { \
         state: "default" 0.0; \
         color: 255 255 255 0; \
         fixed: 1 1; \
         rel1 { \
            to: rect_part; \
         } \
         rel2 { \
            to: rect_part; \
         } \
         image { \
            normal: "button_glow_down.png"; \
            border: 15 15 10 10; \
         } \
         fill { \
            smooth: 0; \
         } \
      } \
      description { \
         state: "active" 0.0; \
         inherit: "default" 0.0; \
         color: 255 255 255 alpha; \
      } \
   }

#define BUTTON_GLOW_PROGRAM(NAME) \
   program { \
      name: NAME"_glow_go_active"; \
      action: STATE_SET "active" 0.0; \
      target: NAME"_glow"; \
      transition: SINUSOIDAL 0.5; \
   } \
   program { \
      name: NAME"_glow_go_inactive"; \
      action: STATE_SET "default" 0.0; \
      target: NAME"_glow"; \
      transition: SINUSOIDAL 1.0; \
   } \
   program { \
      name: NAME"_glow_go_sudden_inactive"; \
      action: STATE_SET "default" 0.0; \
      target: NAME"_glow"; \
      transition: SINUSOIDAL 0.2; \
   }

#define BUTTON_GLOW_ACTION(NAME) \
   after: NAME"_glow_go_active";

#define BUTTON_GLOW_STOP(NAME) \
   after: NAME"_glow_go_inactive";

#define BUTTON_GLOW_SUDDEN_STOP(NAME) \
   after: NAME"_glow_go_sudden_inactive";

#define BUTTON_GLOW_DOWN_PROGRAM(NAME) \
   program { \
      name: NAME"_glow_go_down_active"; \
      action: STATE_SET "active" 0.0; \
      target: NAME"_glow_down"; \
      transition: SINUSOIDAL 0.5; \
   } \
   program { \
      name: NAME"_glow_go_down_inactive"; \
      action: STATE_SET "default" 0.0; \
      target: NAME"_glow_down"; \
      transition: SINUSOIDAL 1.0; \
   } \
   program { \
      name: NAME"_glow_go_down_sudden_inactive"; \
      action: STATE_SET "default" 0.0; \
      target: NAME"_glow_down"; \
      transition: SINUSOIDAL 0.2; \
   }

#define BUTTON_GLOW_DOWN_ACTION(NAME) \
   after: NAME"_glow_go_down_active";

#define BUTTON_GLOW_DOWN_STOP(NAME) \
   after: NAME"_glow_go_down_inactive";

#define BUTTON_GLOW_DOWN_SUDDEN_STOP(NAME) \
   after: NAME"_glow_go_down_sudden_inactive";

#define BUTTON_DISABLE_PROGRAM(NAME) \
      program { \
         name: NAME"_enable"; \
         signal: "elm,state,enabled"; \
         source: "elm"; \
         action: STATE_SET "default" 0.0; \
         target: NAME; \
         target: NAME"_disabled";  \
      } \
      program { \
         name: NAME"_disable"; \
         signal: "elm,state,disabled"; \
         source: "elm"; \
         action: STATE_SET "disabled" 0.0; \
         target: NAME; \
         target: NAME"_disabled"; \
      }

