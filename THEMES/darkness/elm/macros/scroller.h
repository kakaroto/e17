#define SCROLLER_PART_DISABLER \
   part { name: "disabler"; \
      type: RECT; \
      description { state: "default" 0.0; \
         rel1.to: "clipper"; \
         rel2.to: "clipper"; \
         color: 0 0 0 0; \
         visible: 0; \
      } \
      description { state: "disabled" 0.0; \
         inherit: "default" 0.0; \
         visible: 1; \
         color: 128 128 128 128; \
      } \
   }

#define SCROLLER_PART_VBAR_CLIP \
   part { \
       name: "sb_vbar_clip_master"; \
       type: RECT; \
       mouse_events: 0; \
       description { \
          state: "default" 0.0; \
       } \
       description { \
           state: "hidden" 0.0; \
           visible: 0; \
           color: 255 255 255 0; \
       } \
   } \
   part { \
       name: "sb_vbar_clip"; \
       type: RECT; \
       mouse_events: 0; \
       clip_to: "sb_vbar_clip_master"; \
       description { \
          state: "default" 0.0; \
       } \
       description { \
           state: "hidden" 0.0; \
           visible: 0; \
           color: 255 255 255 0; \
       } \
   }

#define SCROLLER_PART_VBAR_THUMB \
   part { \
      name: "sb_vbar_over1_thumb"; \
      mouse_events: 0; \
      clip_to: "sb_vbar_clip"; \
      description { \
         state: "default" 0.0; \
         rel1 { \
            relative: 0.5 0.5; \
            offset: -4 -4; \
            to: "elm.dragable.vbar"; \
         } \
         rel2 { \
            relative: 0.5 0.5; \
            offset: 3 3; \
            to: "elm.dragable.vbar"; \
         } \
         image { \
            normal: "scrollbar_vdrag_thumb.png"; \
         } \
         fill { \
            smooth: 0; \
         } \
      } \
   }

#define SCROLLER_PART_HBAR_CLIP \
   part { \
      name: "sb_hbar_clip_master"; \
      type: RECT; \
      mouse_events: 0; \
      description { \
         state: "default" 0.0; \
      } \
      description { \
         state: "hidden" 0.0; \
         visible: 0; \
         color: 255 255 255 0; \
      } \
   } \
   part { \
      name: "sb_hbar_clip"; \
      clip_to: "sb_hbar_clip_master"; \
      type: RECT; \
      mouse_events: 0; \
      description { \
         state: "default" 0.0; \
      } \
      description { \
         state: "hidden" 0.0; \
         visible: 0; \
         color: 255 255 255 0; \
      } \
   }


#define SCROLLER_PART_HBAR_THUMB \
   part { \
      name: "sb_hbar_over1_thumb"; \
      mouse_events: 0; \
      clip_to: "sb_hbar_clip"; \
      description { \
         state: "default" 0.0; \
         rel1 { \
            relative: 0.5 0.5; \
            offset: -4 -4; \
            to: "elm.dragable.hbar"; \
         } \
         rel2 { \
            relative: 0.5 0.5; \
            offset: 3 3; \
            to: "elm.dragable.hbar"; \
         } \
         image { \
            normal: "scrollbar_hdrag_thumb.png"; \
         } \
         fill { \
            smooth: 0; \
         } \
      } \
   }
