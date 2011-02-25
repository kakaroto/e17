#define TREE_HEADER(group_name, image_file, image_file2) \
group { \
   name: group_name; \
   data { \
      item: "inset" "5 5 5 4"; \
   } \
   parts { \
      part { \
         name: "header"; \
         description { \
            state: "default" 0.0; \
            rel1 { \
               relative: 0.0 0.0; \
               offset: 0 0; \
            } \
            rel2 { \
               relative: 1.0 1.0; \
               offset: -1 0; \
            } \
            image { \
               normal: image_file; \
               border: 3 3 3 3; \
            } \
            fill { \
               smooth: 0; \
            } \
         } \
         description { \
            state: "down" 0.0; \
            inherit: "default" 0.0; \
            image { \
               normal: image_file2; \
            } \
         } \
      } \
      part { \
         name: "header_over"; \
         mouse_events: 0; \
         description { \
            state: "default" 0.0; \
            visible: 0; \
            rel1 { \
               relative: 0.0 1.0; \
               offset: 1 -3; \
               to: "header"; \
            } \
            rel2 { \
               relative: 1.0 1.0; \
               offset: -2 -1; \
               to: "header"; \
            } \
            image { \
               normal: "tree_header_over.png"; \
            } \
            fill { \
               smooth: 0; \
            } \
         } \
         description { \
            state: "over" 0.0; \
            inherit: "default" 0.0; \
            visible: 1; \
         } \
      } \
   } \
   programs { \
      program { \
         name: "header_enter"; \
         signal: "etk,state,enter"; \
         source: "etk"; \
         action: STATE_SET "over" 0.0; \
         target: "header_over"; \
      } \
      program { \
         name: "header_leave"; \
         signal: "etk,state,leave"; \
         source: "etk"; \
         action: STATE_SET "default" 0.0; \
         target: "header_over"; \
      } \
      program { \
         name: "header_pressed"; \
         signal: "etk,state,pressed"; \
         source: "etk"; \
         action: STATE_SET "down" 0.0; \
         target: "header"; \
      } \
      program { \
         name: "header_released"; \
         signal: "etk,state,released"; \
         source: "etk"; \
         action: STATE_SET "default" 0.0; \
         target: "header"; \
      } \
   } \
}
