#define MENU_PART_BASE \
           part { name: "item_image_disabled"; \
               mouse_events: 1; \
               description { state: "default" 0.0; \
                   color: 255 255 255 0; \
                   image { \
                       normal: "bt_dis_base.png"; \
                       border: 4 4 4 4; \
                   } \
                   image.middle: SOLID; \
               } \
               description { state: "disabled" 0.0; \
                   inherit:  "default" 0.0; \
                   color: 255 255 255 255; \
               } \
           }

#define MENU_PART_OVER \
           part {          name: "over1"; \
               mouse_events: 0; \
               description { state: "default" 0.0; \
                   color: 255 255 255 0; \
                   image { \
                       normal: "menu_item_highlight.png"; \
                       border: 7 7 7 0; \
                   } \
               } \
               description { state: "visible" 0.0; \
                   inherit:  "default" 0.0; \
                   color: 255 255 255 255; \
               } \
               description { state: "disabled" 0.0; \
                   inherit:  "default" 0.0; \
               } \
           } \
           part {          name: "over_disabled"; \
               type: RECT; \
               mouse_events: 0; \
               description { state: "default" 0.0; \
                   color: 100 100 100 0; \
               } \
               description { state: "disabled" 0.0; \
                   inherit:  "default" 0.0; \
                   color: 100 100 100 255; \
               } \
           }

#define MENU_PART_SWALLOW_CONTENT \
           part { name: "elm.swallow.content"; \
               type: SWALLOW; \
               description { state: "default" 0.0; \
                   fixed: 1 0; \
                   visible: 1; \
                   align: 0.0 0.5; \
                   rel1.offset: 4 4; \
                   rel2.offset: 3 -5; \
                   rel2.relative: 0.0 1.0; \
                   aspect: 1.0 1.0; \
                   aspect_preference: VERTICAL; \
                   rel2.offset: 4 -5; \
               } \
           }

#define MENU_PART_TEXT \
           part { \
               name:          "elm.text"; \
               type:          TEXT; \
               mouse_events:  0; \
               scale: 1; \
               description { state: "default" 0.0; \
                   visible: 0; \
                   rel1.to_x: "elm.swallow.content"; \
                   rel1.relative: 1.0 0.0; \
                   rel1.offset: 5 7; \
                   rel2.offset: -10 -8; \
                   color: 255 255 255 255; \
                   text { \
                       font:     "DejaVu"; \
                       size:     10; \
                       min:      1 1; \
                       align:    0.0 0.5; \
                   } \
               } \
               description { state: "visible" 0.0; \
                   inherit: "default" 0.0; \
                   visible: 1; \
                   text.min: 1 1; \
               } \
               description { state: "selected" 0.0; \
                   inherit: "default" 0.0; \
                   inherit: "visible" 0.0; \
                   color: 254 254 254 255; \
               } \
               description { state: "disabled" 0.0; \
                   inherit: "default" 0.0; \
                   color: 0 0 0 128; \
               } \
               description { state: "disabled_visible" 0.0; \
                   inherit: "default" 0.0; \
                   inherit: "visible" 0.0; \
                   color: 0 0 0 128; \
               } \
           }

#define MENU_PART_OVER3 \
           part { name: "over3"; \
               mouse_events: 1; \
               repeat_events: 1; \
               description { state: "default" 0.0; \
                   color: 255 255 255 0; \
                   image { \
                       normal: "bt_glow.png"; \
                       border: 12 12 12 12; \
                   } \
                   fill.smooth : 0; \
               } \
               description { state: "clicked" 0.0; \
                   inherit:  "default" 0.0; \
                   visible: 1; \
                   color: 255 255 255 255; \
               } \
           }

#define MENU_PART_DISABLER \
           part { name: "disabler"; \
               type: RECT; \
               description { state: "default" 0.0; \
                   color: 0 0 0 0; \
                   visible: 0; \
               } \
               description { state: "disabled" 0.0; \
                   inherit: "default" 0.0; \
                   visible: 1; \
               } \
           }

#define MENU_PROGRAMS \
       programs { \
           program { \
               name:   "item_mouse_in"; \
               signal: "mouse,in"; \
               source: "over3"; \
               action: SIGNAL_EMIT "elm,action,activate" ""; \
               after: "item_mouse_in_2"; \
               after: "item_mouse_in_3"; \
           } \
           program { \
               name:   "item_mouse_in_2"; \
               transition: DECELERATE 0.5; \
               script { \
                   new v, d; \
                   v = get_int(menu_text_visible); \
                   d = get_int(menu_disable); \
                   if (v==1 && d==0) \
                        run_program(PROGRAM:"selected_text"); \
               } \
           } \
           program { \
               name:   "item_mouse_in_3"; \
               action : STATE_SET "visible" 0.0; \
               target: "over1"; \
               transition: DECELERATE 0.5; \
           } \
           program { \
               name:   "selected_text"; \
               action: STATE_SET "selected" 0.0; \
               target: "elm.text"; \
               transition: DECELERATE 0.5; \
           } \
           program { \
               name:   "item_mouse_out"; \
               signal: "mouse,out"; \
               source: "over3"; \
               after: "item_mouse_out_2"; \
               after: "item_mouse_out_3"; \
           } \
           program { \
               name:   "item_mouse_out_2"; \
               transition: DECELERATE 0.5; \
               script { \
                   new v, d; \
                   v = get_int(menu_text_visible); \
                   d = get_int(menu_disable); \
                   if (v==1 && d==0) \
                        run_program(PROGRAM:"visible_text"); \
               } \
           } \
           program { \
               name:   "item_mouse_out_3"; \
               action: STATE_SET "default" 0.0; \
               target: "over1"; \
               transition: DECELERATE 0.5; \
           } \
           program { \
               name:   "visible_text"; \
               action: STATE_SET "visible" 0.0; \
               target: "elm.text"; \
               transition: DECELERATE 0.5; \
           } \
           program { \
               name:   "item_click2"; \
               signal: "mouse,down,1"; \
               source: "over3"; \
               action: STATE_SET "clicked" 0.0; \
               target: "over3"; \
           } \
           program { \
               name:   "item_unclick2"; \
               signal: "mouse,up,1"; \
               source: "over3"; \
               action: STATE_SET "default" 0.0; \
               transition: DECELERATE 0.5; \
               target: "over3"; \
           } \
           program { \
               name:   "item_unclick3"; \
               signal: "mouse,up,1"; \
               source: "over3"; \
               action: SIGNAL_EMIT "elm,action,click" ""; \
           } \
           program { \
               DISABLE_PROGRAM_PART \
               target: "item_image_disabled"; \
               target: "over1"; \
               target: "over_disabled"; \
               target: "disabler"; \
               after: "disable_text"; \
           } \
           program { \
               ENABLE_PROGRAM_PART \
               target: "item_image_disabled"; \
               target: "over1"; \
               target: "over_disabled"; \
               target: "disabler"; \
               after: "enable_text"; \
           } \
       }
